#include "common.h"

#include "dma.h"
#include "entity.h"
#include "joy.h"
#include "player.h"
#include "stage.h"
#include "tables.h"
#include "tools.h"
#include "vdp.h"

#include "camera.h"
#include "gba_video.h"

#include "gba.h"

// Since only one row or column of tiles is drawn at a time
#define CAMERA_MAX_SPEED 	0xFFF
#define FOCUS_SPEED 		5
// While following the player focus on a variable point left/right of the player
// The idea is to move slowly along this line relative to the player, but not globally
// limit the speed that way or we will fall behind
#define PAN_SPEED	0x200
#define PAN_SIZE	0x6000
// When cameraShake is nonzero the camera will shake, and decrement this value
// each frame until it becomes zero again
uint16_t cameraShake;
// Tile attr buffer to draw offscreen during map scroll
uint16_t mapbuf[64];
// Alternates between drawing row and column each frame when moving diagonally
uint8_t diag_tick;

int32_t camera_xmin, camera_ymin = 0;
uint32_t camera_xsize, camera_ysize = 0;

cameraStruct camera = {NULL};

void camera_init() {
	camera.target = &player;
    camera.x_offset = 0;
    cameraShake = 0;
    camera_set_position(0, 0);
}

void camera_set_position(int32_t x, int32_t y) {
	// Don't let the camera leave the stage
	if(x > block_to_sub(stageWidth) - pixel_to_sub(SCREEN_HALF_W + 8))
		x = block_to_sub(stageWidth) - pixel_to_sub(SCREEN_HALF_W + 8);
	if(y > block_to_sub(stageHeight) - pixel_to_sub(SCREEN_HALF_H + 8))
		y = block_to_sub(stageHeight) - pixel_to_sub(SCREEN_HALF_H + 8);
	if(x < pixel_to_sub(SCREEN_HALF_W + 8)) x = pixel_to_sub(SCREEN_HALF_W + 8);
	if(y < pixel_to_sub(SCREEN_HALF_H + 8)) y = pixel_to_sub(SCREEN_HALF_H + 8);
	// Apply
	camera.x = camera.x_mark = x;
	camera.y = camera.y_mark = y;
	camera.x_shifted = (x >> CSF) - SCREEN_HALF_W;
	camera.y_shifted = (y >> CSF) - SCREEN_HALF_H;
	// Update quick fetch cutoff values
	camera_xmin = camera.x - pixel_to_sub(SCREEN_HALF_W + 32);
	camera_xsize = pixel_to_sub(SCREEN_WIDTH + 64);
	camera_ymin = camera.y - pixel_to_sub(SCREEN_HALF_H + 32);
	camera_ysize = pixel_to_sub(SCREEN_HEIGHT + 64);
}

void camera_shake(uint16_t time) {
	cameraShake = time;
}

int x_start = 0;
int y_start = 0;
int stage_x = 0;
int stage_y = 0;

void setStageColumn(int iterations)
{
	const uint8_t *pxa = tileset_info[stageTileset].PXA;
	for(uint16_t i = iterations; i--; ) {
		// It's actually faster to just draw garbage than have these checks
		//if(y >= stageHeight << 1) break;
		//if(y >= 0) {
			// Fuck math tbh
			int16_t add = (morphingColumn == 1 ? -16 : 16);
			uint16_t b = stage_get_block(((stage_x+add)>>1), stage_y>>1);
			uint16_t t = b << 2; //((b&15) << 1) + ((b>>4) << 6);
			uint16_t ta = pxa[b]; //stage_get_block_type(x>>1, y>>1);
			uint16_t pal = (ta == 0x43 || ta & 0x80) ? PAL1 : PAL2;
			//mapbuf[y&31] = TILE_ATTR(pal, (ta&0x40) > 0, 
					//0, 0, TILE_TSINDEX + t + (x&1) + ((y&1)<<1));
				
			//mapbuf[y&31] = 8 + t + (x&1) + ((y&1)<<1);
			u16* adr = MAP_BASE_ADR(BASE_STAGE) + ((y_start&31)<<6) + (((x_start&31)<<1));
			*adr = TILE_TSINDEX + t + (stage_x&1) + ((stage_y&1)<<1);
		//}
		stage_y++;
		y_start++;
	}
}

void setStageRow(int iterations)
{
	const uint8_t *pxa = tileset_info[stageTileset].PXA;
	for(uint16_t i = iterations; i--; ) {
		//if(x >= stageWidth << 1) break;
		//if(x >= 0) {
			uint16_t b = stage_get_block(stage_x>>1, stage_y>>1);
			uint16_t t = b << 2; //((b&15) << 1) + ((b>>4) << 6);
			uint16_t ta = pxa[b]; //stage_get_block_type(x>>1, y>>1);
			uint16_t pal = (ta == 0x43 || ta & 0x80) ? PAL1 : PAL2;
			//mapbuf[x&63] = TILE_ATTR(pal, (ta&0x40) > 0, 
			//		0, 0, TILE_TSINDEX + t + (x&1) + ((y&1)<<1));
			//mapbuf[x&63] = TILE_TSINDEX + t + (x&1) + ((y&1)<<1);
			u16* adr = MAP_BASE_ADR(BASE_STAGE) + ((y_start&31)<<6) + (((x_start&31)<<1));
			*adr = TILE_TSINDEX + t + (stage_x&1) + ((stage_y&1)<<1);
		//}
		stage_x++;
		x_start++;
	}
}

void camera_update() {
	PF_BGCOLOR(0x08E);
	int32_t x_next, y_next;
	if(camera.target) {
		// If following the player focus on where they are walking/looking
		if(camera.target == &player) {
			if(player.dir == LEFT) {
				//camera.x_offset = -pixel_to_sub(48);
				camera.x_offset -= PAN_SPEED;
				if(camera.x_offset < -PAN_SIZE) camera.x_offset = -PAN_SIZE;
			} else {
				//camera.x_offset = pixel_to_sub(48);
				camera.x_offset += PAN_SPEED;
				if(camera.x_offset > PAN_SIZE) camera.x_offset = PAN_SIZE;
			}
			if(!controlsLocked && joy_down(BUTTON_UP)) {
				camera.y_offset -= PAN_SPEED;
				if(camera.y_offset < -PAN_SIZE) camera.y_offset = -PAN_SIZE;
			} else if(!controlsLocked && joy_down(BUTTON_DOWN)) {
				camera.y_offset += PAN_SPEED;
				if(camera.y_offset > PAN_SIZE) camera.y_offset = PAN_SIZE;
			} else {
				if(camera.y_offset > 0)
					camera.y_offset -= PAN_SPEED;
				else if(camera.y_offset < 0)
					camera.y_offset += PAN_SPEED;
			}
			// Calculate where camera will be next frame
			x_next = camera.x +
					(((floor(camera.target->x) + camera.x_offset) - camera.x) >> 4);
			y_next = camera.y +
					(((floor(camera.target->y) + camera.y_offset) - camera.y) >> 4);
		} else {
			// Just stick to the target object without any offset
			camera.x_offset = 0;
			camera.y_offset = 0;
			x_next = camera.x +
					(((floor(camera.target->x) + camera.x_offset) - camera.x) >> 5);
			y_next = camera.y +
					(((floor(camera.target->y) + camera.y_offset) - camera.y) >> 5);
		}
	} else { // Camera isn't following anything
		return;
	}
	// Enforce max camera speed
	if(x_next - camera.x < -CAMERA_MAX_SPEED) x_next = camera.x - CAMERA_MAX_SPEED;
	if(y_next - camera.y < -CAMERA_MAX_SPEED) y_next = camera.y - CAMERA_MAX_SPEED;
	if(x_next - camera.x > CAMERA_MAX_SPEED) x_next = camera.x + CAMERA_MAX_SPEED;
	if(y_next - camera.y > CAMERA_MAX_SPEED) y_next = camera.y + CAMERA_MAX_SPEED;
	// Don't let the camera leave the stage
	uint16_t bounds = cameraShake ? 2 : 8;
	if(stageID == 18 && !pal_mode) { // Special case for shelter
		x_next = pixel_to_sub(SCREEN_HALF_W + 8);
		y_next = pixel_to_sub(SCREEN_HALF_H + 16);
	} else {
		if(x_next < pixel_to_sub(SCREEN_HALF_W + bounds)) {
			x_next = pixel_to_sub(SCREEN_HALF_W + bounds);
		} else if(x_next > block_to_sub(stageWidth) - pixel_to_sub(SCREEN_HALF_W + bounds)) {
			x_next = block_to_sub(stageWidth) - pixel_to_sub(SCREEN_HALF_W + bounds);
		}
		if(y_next < pixel_to_sub(SCREEN_HALF_H + bounds)) {
			y_next = pixel_to_sub(SCREEN_HALF_H + bounds);
		} else if(y_next > block_to_sub(stageHeight) - pixel_to_sub(SCREEN_HALF_H + bounds)) {
			y_next = block_to_sub(stageHeight) - pixel_to_sub(SCREEN_HALF_H + bounds);
		}
	}
	// Camera shaking
	// stay within the same (pre-shake) 8x8 area to avoid pointless map redraw
	if(cameraShake && (--cameraShake & 1)) {
		int16_t x_shake = (random() & 0x7FF) - 0x400;
		int16_t y_shake = (random() & 0x7FF) - 0x400;
		if((x_next & 0xF000) == ((x_next + x_shake) & 0xF000)) x_next += x_shake;
		if((y_next & 0xF000) == ((y_next + y_shake) & 0xF000)) y_next += y_shake;
	}
	// Shifted values
	camera.x_shifted = (x_next >> CSF) - SCREEN_HALF_W;
	camera.y_shifted = (y_next >> CSF) - SCREEN_HALF_H;
	// Update quick fetch cutoff values
	camera_xmin = camera.x - pixel_to_sub(SCREEN_HALF_W + 32);
	camera_xsize = pixel_to_sub(SCREEN_WIDTH + 64);
	camera_ymin = camera.y - pixel_to_sub(SCREEN_HALF_H + 32);
	camera_ysize = pixel_to_sub(SCREEN_HEIGHT + 64);
	// Morph the stage if the camera is moving
	morphingColumn = sub_to_tile(x_next) - sub_to_tile(camera.x);
	morphingRow = sub_to_tile(y_next) - sub_to_tile(camera.y);

	int scroll_x = tileScrollX%256;
	int scroll_y = tileScrollY%256;

	if(morphingColumn | morphingRow) {
		const uint8_t *pxa = tileset_info[stageTileset].PXA;
		// Queue row and/or column mapping
		if(morphingColumn) {
			// Draw row OR column, and when going diagonally, alternate
			//if(morphingRow && (++diag_tick & 1)) {
			//	morphingColumn = 0;
			//	x_next = camera.x;
			//} else {
				stage_x = sub_to_tile(x_next) + (morphingColumn == 1 ? 32 : -32);
				stage_y = sub_to_tile(y_next) - 10 /*+ morphingRow*/;

				if(morphingColumn != 1) 
					x_start = ((scroll_x / 8)-2) % 32;
				else 
					x_start = ((scroll_x / 8) + 32) % 32;
				if(stage_x >= -32 && stage_x < (int16_t)(stageWidth+32) << 1) {
					y_start = scroll_y / 8;
					setStageColumn(32 - y_start);
					y_start = 0;
					setStageColumn(scroll_y / 8);


					//CpuFastSet(mapbuf, MAP_BASE_ADR(BASE_STAGE) + ((x & 63) << 1), 32 | COPY32);
					//DMA_queueDma(DMA_VRAM, (uint32_t) mapbuf, VDP_PLAN_A + ((x & 63) << 1), 32, 128);
				}
			//}
		}
		if(morphingRow) {
			//if(morphingColumn && !(diag_tick & 1)) {
			//	morphingRow = 0;
			//	y_next = camera.y;
			//} else {
				stage_y = sub_to_tile(y_next) + (morphingRow == 1 ? 10 : -10);;
				stage_x = sub_to_tile(x_next) - 15 /*+ morphingColumn*/;

				if(morphingRow != 1) 
					y_start = ((scroll_y / 8) - 1) % 32;
				else 
					y_start = ((scroll_y / 8) + 21) % 32;
				if(stage_y >= -32 && stage_y < (int16_t)(stageHeight+32) << 1) {
					x_start = scroll_x / 8;
					setStageRow(32 - x_start);
					x_start = 0;
					setStageRow(scroll_x / 8);
					//DMA3COPY(mapbuf, MAP_BASE_ADR(BASE_STAGE) + ((y&31)<<6), 16 | COPY32);
					//DMA_queueDma(DMA_VRAM, (uint32_t) mapbuf, VDP_PLAN_A + ((y & 31) << 7), 64, 2);
				}
			//}
		}
	}
	if(!morphingColumn && (abs(camera.x_mark - x_next) > 0x1FFF || abs(camera.y_mark - y_next) > 0x1FFF)) {
		camera.x_mark = x_next;
		camera.y_mark = y_next;
		entities_update_inactive();
	}
	// Apply camera position
	camera.x = x_next;
	camera.y = y_next;

	camera.x_shifted = (x_next >> CSF) - SCREEN_HALF_W;
	camera.y_shifted = (y_next >> CSF) - SCREEN_HALF_H;

	PF_BGCOLOR(0x000);
}
