#include "common.h"

#include "bank_data.h"
#include "camera.h"
#include "dma.h"
#include "effect.h"
#include "entity.h"
#include "error.h"
#include "hud.h"
#include "joy.h"
#include "memory.h"
#include "npc.h"
#include "player.h"
#include "resources.h"
#include "sheet.h"
#include "string.h"
#include "system.h"
#include "tables.h"
#include "tools.h"
#include "tsc.h"
#include "vdp.h"
#include "xgm.h"

#include "stage.h"

// Could fit under the Oside map (192 tile gap)
#define TILE_MOONINDEX (TILE_TSINDEX + 32*8)
// Another tile gap, fits under both Almond and Cave
#define TILE_WATERINDEX (TILE_TSINDEX + 384)

// Index of background in db/back.c and the effect type
uint8_t stageBackground = 0xFF;

int16_t backScrollTable[32];
//uint8_t *stageBlocks = NULL;
uint16_t *stageTable = NULL;
const uint8_t *stagePXA = NULL;

typedef struct {
	uint8_t index;
	uint8_t dir;
} Current;
Current currents[4];
uint8_t currentsCount = 0;
uint8_t currentsTimer = 0;

static void stage_load_tileset();
static void stage_load_blocks();

static void stage_draw_block(uint16_t x, uint16_t y);
static void stage_draw_screen();
static void stage_draw_screen_credits();
static void stage_draw_background();
static void stage_draw_moonback();

#include "gba.h"

void stage_load(uint16_t id) {

	iprintf("Loading stage %d\n", id);
	vdp_set_display(FALSE);
	oldstate = ~0;
	// Prevents an issue where a column of the previous map would get drawn over the new one
	DMA_clearQueue();
	stageID = id;
	// Clear out or deactivate stuff from the old stage
	effects_clear();
	entities_clear();
	if(stageTable) {
		free(stageTable);
		stageTable = NULL;
	}
	vdp_sprites_clear();
	water_entity = NULL;
	bossEntity = NULL;

	// Load the tileset
	if(stageTileset != stage_info[id].tileset) {
		stageTileset = stage_info[id].tileset;
        disable_ints;
        z80_request();
		stage_load_tileset();
        z80_release();
        enable_ints;
	}
	// Load sprite sheets
    disable_ints;
    z80_request();
	sheets_load_stage(id, FALSE, TRUE);
	// Load backgrounds
	if(background_info[stage_info[id].background].type == 4 || stageBackground != stage_info[id].background) {
		stageBackground = stage_info[id].background;
		stageBackgroundType = background_info[stageBackground].type;
		vdp_set_backcolor(0); // Color index 0 for everything except fog
		if(stageBackgroundType == 0 || stageBackgroundType == 3) { // Tiled image
			vdp_set_scrollmode(HSCROLL_PLANE, VSCROLL_PLANE);
			vdp_tiles_load_from_rom(background_info[stageBackground].tileset->tiles, TILE_BACKINDEX, 
						background_info[stageBackground].tileset->numTile);
			stage_draw_background();
		} else if(stageBackgroundType == 1) { // Moon
			vdp_set_scrollmode(HSCROLL_TILE, VSCROLL_PLANE);
			stage_draw_moonback();
		} else if(stageBackgroundType == 2) { // Solid Color
			vdp_set_scrollmode(HSCROLL_PLANE, VSCROLL_PLANE);
			vdp_map_clear(VDP_PLAN_B);
		} else if(stageBackgroundType == 4) { // Almond Water
			vdp_set_scrollmode(HSCROLL_PLANE, VSCROLL_PLANE);
			vdp_map_clear(VDP_PLAN_B);
            backScrollTable[0] = (SCREEN_HEIGHT >> 3) + 1;
			vdp_tiles_load_from_rom(BG_Water.tiles, TILE_WATERINDEX, BG_Water.numTile);
		} else if(stageBackgroundType == 5) { // Fog
			vdp_set_scrollmode(HSCROLL_TILE, VSCROLL_PLANE);
			// Use background color from tileset
			vdp_set_backcolor(32);
			stage_draw_moonback();
		}
	}
    z80_release();
    enable_ints;

	// Load stage PXM into RAM
	stage_load_blocks();
	// Move camera to player's new position
	camera_set_position(player.x, player.y - (stageBackgroundType == 3 ? 8<<CSF : 0));
	camera.target = &player;
	camera.x_offset = 0;
	camera.y_offset = 0;

    disable_ints;
    z80_request();
	stage_draw_screen(); // Draw 64x32 foreground PXM area at camera's position
    z80_release();
    enable_ints;

	//GBATODO
	//stage_load_entities(); // Create entities defined in the stage's PXE
	// For rooms where the boss is always loaded
	if(stageID == STAGE_WATERWAY_BOSS) {
		bossEntity = entity_create(0, 0, 360 + BOSS_IRONHEAD, 0);
	}
	if(stageID == STAGE_BLACK_SPACE) {
		bossEntity = entity_create(0, 0, 360 + BOSS_UNDEADCORE, 0);
	}
	if(stageID == STAGE_HELL_B3 || stageID == STAGE_HELL_PASSAGEWAY_2) {
		bossEntity = entity_create(0, 0, 360 + BOSS_HEAVYPRESS, 0);
	}

    disable_ints;
    z80_request();
	DMA_flushQueue();
    z80_release();
    enable_ints;

	if((playerEquipment & EQUIP_CLOCK) || stageID == STAGE_HELL_B1) system_draw_counter();
	tsc_load_stage(id);
	vdp_set_display(TRUE);
}

void stage_load_credits(uint8_t id) {
	stageID = id;
	
	entities_clear();
	vdp_sprites_clear();
	if(stageTable) {
		free(stageTable);
		stageTable = NULL;
	}

    disable_ints;
    z80_request();
	vdp_set_display(FALSE);

	stageTileset = stage_info[id].tileset;
	stage_load_tileset();
	sheets_load_stage(id, FALSE, TRUE);
	stage_load_blocks();
	stage_draw_screen_credits();
	stage_load_entities();
	DMA_flushQueue();
	tsc_load_stage(id);

	vdp_set_display(TRUE);
    z80_release();
    enable_ints;
}

void stage_load_tileset() {
    uint32_t *buf = (uint32_t*) 0xFF0100;
    uint16_t numtile = tileset_info[stageTileset].size << 2;
    for(uint16_t i = 0; i < numtile; i += 128) {
        uint16_t num = min(numtile - i, 128);
        uftc_unpack(tileset_info[stageTileset].pat, buf, i, num);
        vdp_tiles_load(buf, TILE_TSINDEX + i, num);
    }
	// Inject the breakable block sprite into the tileset
	stagePXA = tileset_info[stageTileset].PXA;
	for(uint16_t i = 0; i < numtile >> 2; i++) {
		if(stagePXA[i] == 0x43) vdp_tiles_load_from_rom(TS_Break.tiles, TILE_TSINDEX + (i << 2), 4);
	}
	// Search for any "wind" tiles and note their index to animate later
	currentsCount = 0;
	for(uint16_t i = 0; i < numtile >> 2; i++) {
		if(!(stagePXA[i] & 0x80)) continue;
		currents[currentsCount] = (Current) { .index = i, .dir = stagePXA[i] & 0x3 };
		if(++currentsCount == 4) break;
	}
}

void decompress_slz(uint8_t *in, uint8_t *out) {
   // Retrieve uncompressed size
   uint16_t size = in[0] << 8 | in[1];
   in += 2;
   
   // To store the tokens
   uint8_t num_tokens = 1;
   uint8_t tokens;
   
   // Go through all compressed data until we're done decompressing
   while (size != 0) {
      // Need more tokens?
      num_tokens--;
      if (num_tokens == 0) {
         tokens = *in++;
         num_tokens = 8;
      }
      
      // Compressed string?
      if (tokens & 0x80) {
         // Get distance and length
         uint16_t dist = in[0] << 8 | in[1];
         uint8_t len = dist & 0x0F;
         dist = dist >> 4;
         in += 2;
         
         // Discount string length from size
         size -= len + 3;
         
         // Copy string using Duff's device
         // Code looks crazy, doesn't it? :)
         uint8_t *ptr = out - dist - 3;
         switch (len) {
            case 15: *out++ = *ptr++;
            case 14: *out++ = *ptr++;
            case 13: *out++ = *ptr++;
            case 12: *out++ = *ptr++;
            case 11: *out++ = *ptr++;
            case 10: *out++ = *ptr++;
            case  9: *out++ = *ptr++;
            case  8: *out++ = *ptr++;
            case  7: *out++ = *ptr++;
            case  6: *out++ = *ptr++;
            case  5: *out++ = *ptr++;
            case  4: *out++ = *ptr++;
            case  3: *out++ = *ptr++;
            case  2: *out++ = *ptr++;
            case  1: *out++ = *ptr++;
            case  0: *out++ = *ptr++;
                     *out++ = *ptr++;
                     *out++ = *ptr++;
         }
      }
      
      // Uncompressed byte?
      else {
         // Store byte as-is
         *out++ = *in++;
         size--;
      }
      
      // Go for next token
      tokens += tokens;
   }
}

void stage_load_blocks() {
    decompress_slz(stage_info[stageID].PXM, stagePXM);
    stageWidth = stagePXM[4] | (stagePXM[5] << 8);
    stageHeight = stagePXM[6] | (stagePXM[7] << 8);
	// Multiplication table for stage rows
	stageTable = malloc(stageHeight << 1);
	if(!stageTable) error_oom();
	uint16_t blockTotal = 0;
	for(uint16_t y = 0; y < stageHeight; y++) {
		stageTable[y] = blockTotal;
		blockTotal += stageWidth;
	}
}

void stage_load_entities() {
	const uint8_t *PXE = stage_info[stageID].PXE;
	// PXE[4] is the number of entities to load. It's word length but never more than 255
	for(uint16_t i = 0; i < PXE[4]; i++) {
		uint16_t x, y, id, event, type, flags;
		// Like all of cave story's data files PXEs are little endian
		x     = PXE[8  + i * 12] + (PXE[9  + i * 12]<<8);
		y     = PXE[10 + i * 12] + (PXE[11 + i * 12]<<8);
		id    = PXE[12 + i * 12] + (PXE[13 + i * 12]<<8);
		event = PXE[14 + i * 12] + (PXE[15 + i * 12]<<8);
		type  = PXE[16 + i * 12] + (PXE[17 + i * 12]<<8);
		flags = PXE[18 + i * 12] + (PXE[19 + i * 12]<<8);
		// There are some unused entities that have all these values as 0, as well as
		// entities that should only exist when specific flags are on/off
		// Loading these would be a waste of memory, just skip them
		if(!id && !event && !type && !flags) continue;
		if((flags&NPC_DISABLEONFLAG) && system_get_flag(id)) continue;
		if((flags&NPC_ENABLEONFLAG) && !system_get_flag(id)) continue;
		// Special case to not load save points if SRAM is not found
		if(type == OBJ_SAVE_POINT && system_get_flag(FLAG_DISABLESAVE)) continue;
		// I'll probably need this code block again in the future.
		// When an NPC is assigned the improper number of sprites for their metasprite
		// loading it will crash BlastEm and possibly hardware too. This steps through
		// each entity as it is loaded so the problematic NPC can be found
	#if DEBUG
		if(joy_down(BUTTON_A)) {
			vdp_set_display(TRUE);
			vdp_color(0, 0x444);
			vdp_color(15, 0xEEE);
			char str[40];
			sprintf(str, "Debug Entity # %03hu", i);
			vdp_puts(VDP_PLAN_A, str, 2, 2);
			sprintf(str, "X:%04hu Y:%04hu I:%04hu", x, y, id);
			vdp_puts(VDP_PLAN_A, str, 2, 5);
			sprintf(str, "E:%04hu T:%04hu F:%04hX", event, type, flags);
			vdp_puts(VDP_PLAN_A, str, 2, 7);
			
			while(!joy_pressed(BUTTON_C)) {
				vdp_vsync();
				//xgm_vblank();
				joy_update();
				vdp_hscroll(VDP_PLAN_A, 0);
				vdp_vscroll(VDP_PLAN_A, 0);
			}
			vdp_color(0, 0);
			vdp_set_display(FALSE);
		}
	#endif
		entity_create_ext(block_to_sub(x) + 0x1000, block_to_sub(y) + 0x1000, type, flags, id, event);
	}
}

// Replaces a block with another (for <CMP, <SMP, and breakable blocks)
void stage_replace_block(int16_t bx, int16_t by, uint8_t index) {
	stageBlocks[stageTable[by] + bx] = index;
	int16_t cx = sub_to_block(camera.x), cy = sub_to_block(camera.y);
	if(cx - 16 > bx || cx + 16 < bx || cy - 8 > by || cy + 8 < by) return;
	// Only redraw if change was made onscreen
	stage_draw_block(bx, by);
}

// Stage vblank drawing routine
void stage_update() {
    //z80_request();
	// Background Scrolling
	// Type 2 is not included here, that's blank backgrounds which are not scrolled
	if(stageBackgroundType == 0) {
		vdp_hscroll(VDP_PLAN_A, -sub_to_pixel(camera.x) + SCREEN_HALF_W);
		vdp_vscroll(VDP_PLAN_A, sub_to_pixel(camera.y) - SCREEN_HALF_H);
		vdp_hscroll(VDP_PLAN_B, -sub_to_pixel(camera.x) / 4 + SCREEN_HALF_W);
		vdp_vscroll(VDP_PLAN_B, sub_to_pixel(camera.y) / 4 - SCREEN_HALF_H);
	} else if(stageBackgroundType == 1 || stageBackgroundType == 5) {
		// PLAN_A Tile scroll
		int16_t off[32];
		off[0] = -sub_to_pixel(camera.x) + SCREEN_HALF_W;
		for(uint8_t i = 1; i < 32; i++) {
			off[i] = off[0];
		}
		vdp_hscroll_tile(VDP_PLAN_A, off);
		vdp_vscroll(VDP_PLAN_A, sub_to_pixel(camera.y) - SCREEN_HALF_H);
		// Moon background has different spots scrolling horizontally at different speeds
		backScrollTimer--;
		
		if(pal_mode) {
			uint8_t y = 28;
			for(;y >= 22; --y) backScrollTable[y] = backScrollTimer << 1;
			for(;y >= 18; --y) backScrollTable[y] = backScrollTimer;
			for(;y >= 15; --y) backScrollTable[y] = backScrollTimer >> 1;
			for(;y >= 11; --y) backScrollTable[y] = backScrollTimer >> 2;
			vdp_hscroll_tile(VDP_PLAN_B, backScrollTable);
			//VDP_setVerticalScroll(VDP_PLAN_B, -8);
		} else {
			uint8_t y = 27;
			for(;y >= 21; --y) backScrollTable[y] = backScrollTimer << 1;
			for(;y >= 17; --y) backScrollTable[y] = backScrollTimer;
			for(;y >= 14; --y) backScrollTable[y] = backScrollTimer >> 1;
			for(;y >= 10; --y) backScrollTable[y] = backScrollTimer >> 2;
			vdp_hscroll_tile(VDP_PLAN_B, backScrollTable);
			//VDP_setVerticalScroll(VDP_PLAN_B, 0);
		}
	} else if(stageBackgroundType == 3) {
		// Lock camera at specific spot
		camera.target = NULL;
		// Ironhead boss background auto scrolls leftward
		backScrollTable[0] -= 2;
		vdp_hscroll(VDP_PLAN_A, -sub_to_pixel(camera.x) + SCREEN_HALF_W);
		vdp_vscroll(VDP_PLAN_A, sub_to_pixel(camera.y) - SCREEN_HALF_H);
		vdp_hscroll(VDP_PLAN_B, backScrollTable[0]);
	} else if(stageBackgroundType == 4) {
		int16_t rowc = SCREEN_HEIGHT >> 3;
		int16_t rowgap = 31 - rowc;
		// Water surface relative to top of screen
		int16_t scroll = (water_entity->y >> CSF) - ((camera.y >> CSF) - SCREEN_HALF_H);
		int16_t row = scroll >> 3;
		int16_t oldrow = backScrollTable[0];
		while(row < oldrow) { // Water is rising (Y decreasing)
			oldrow--;
			uint8_t rowup = 31 - ((oldrow + rowgap) & 31);// Row that will be updated
			if(oldrow > rowc) { // Below Screen
				uint16_t mapBuffer[64];
				for(uint16_t x = 0; x < 64; x++) mapBuffer[x] = 0;
				DMA_doDma(DMA_VRAM, (uint32_t)mapBuffer, VDP_PLAN_B + (rowup << 7), 64, 2);
			} else { // On screen or above
				uint16_t mapBuffer[64];
				for(uint16_t x = 0; x < 64; x++) {
					mapBuffer[x] = TILE_ATTR(PAL0,1,0,0,
							TILE_WATERINDEX + (oldrow == rowc ? x&3 : 4 + (random()&15)));
				}
				DMA_doDma(DMA_VRAM, (uint32_t)mapBuffer, VDP_PLAN_B + (rowup << 7), 64, 2);
			}
		}
		while(row > oldrow) { // Water is lowering (Y increasing)
			oldrow++;
			uint8_t rowup = 31 - (oldrow & 31); // Row that will be updated
			if(oldrow <= 0) { // Above screen
				uint16_t mapBuffer[64];
				for(uint16_t x = 0; x < 64; x++) {
					mapBuffer[x] = TILE_ATTR(PAL0,1,0,0,
							TILE_WATERINDEX + (oldrow == 0 ? x&3 : 4 + (random()&15)));
				}
				DMA_doDma(DMA_VRAM, (uint32_t)mapBuffer, VDP_PLAN_B + (rowup << 7), 64, 2);
			} else { // On screen or below
				uint16_t mapBuffer[64];
				for(uint16_t x = 0; x < 64; x++) mapBuffer[x] = 0;
				DMA_doDma(DMA_VRAM, (uint32_t)mapBuffer, VDP_PLAN_B + (rowup << 7), 64, 2);
			}
		}
		
		vdp_hscroll(VDP_PLAN_B, -sub_to_pixel(camera.x) + SCREEN_HALF_W - backScrollTimer);
		vdp_vscroll(VDP_PLAN_B, -scroll);
		vdp_hscroll(VDP_PLAN_A, -sub_to_pixel(camera.x) + SCREEN_HALF_W);
		vdp_vscroll(VDP_PLAN_A, sub_to_pixel(camera.y) - SCREEN_HALF_H);
		backScrollTable[0] = row;
	} else {
		// Only scroll foreground
		vdp_hscroll(VDP_PLAN_A, -sub_to_pixel(camera.x) + SCREEN_HALF_W);
		vdp_vscroll(VDP_PLAN_A, sub_to_pixel(camera.y) - SCREEN_HALF_H);
	}
	if(currentsCount) { // Waterway currents
		currentsTimer = (currentsTimer + 1) & 0x1F;
		uint8_t t = currentsTimer & 3;
		if(t < currentsCount) {
			uint16_t from_index = 0;
			uint8_t *from_ts = NULL;
			uint16_t to_index = TILE_TSINDEX + (currents[t].index << 2);
			switch(currents[t].dir) {
				case 0: // Left
					from_ts = (uint8_t*) TS_WindH.tiles;
					from_index = (currentsTimer >> 1) & ~1;
				break;
				case 1: // Up
					from_ts = (uint8_t*) TS_WindV.tiles;
					from_index = (currentsTimer >> 1) & ~1;
				break;
				case 2: // Right
					from_ts = (uint8_t*) TS_WindH.tiles;
					from_index = 14 - ((currentsTimer >> 1) & ~1);
				break;
				case 3: // Down
					from_ts = (uint8_t*) TS_WindV.tiles;
					from_index = 14 - ((currentsTimer >> 1) & ~1);
				break;
				default: return;
			}
			// Replace the tile in the tileset
			DMA_doDma(DMA_VRAM, (uint32_t) (from_ts + (from_index << 5)), to_index << 5, 32, 2);
			from_index += 16;
			to_index += 2;
			DMA_doDma(DMA_VRAM, (uint32_t) (from_ts + (from_index << 5)), to_index << 5, 32, 2);
		}
	}
    //z80_release();
}

void stage_setup_palettes() {
	// Stage palette and shared NPC palette
	vdp_colors_next(0, PAL_Main.data, 16);
	if(stageID == STAGE_INTRO) {
		vdp_colors_next(16, PAL_Intro.data, 16);
	} else {
		vdp_colors_next(16, PAL_Sym.data, 16);
	}
	if(stageID == STAGE_WATERWAY) {
		vdp_colors_next(32, PAL_RiverAlt.data, 16); // For Waterway green background
	} else {
		vdp_colors_next(32, tileset_info[stage_info[stageID].tileset].palette->data, 16);
	}
	vdp_colors_next(48, stage_info[stageID].npcPalette->data, 16);
}

void stage_draw_screen() {
    const uint8_t *pxa = tileset_info[stageTileset].PXA;
	uint16_t maprow[64];
	uint16_t y = sub_to_tile(camera.y) - 16;
	for(uint16_t i = 32; i--; ) {
		if(vblank) aftervsync(); // So we don't lag the music
		vblank = 0;
		
		if(y < stageHeight << 1) {
			uint16_t x = sub_to_tile(camera.x) - 32;
			for(uint16_t j = 64; j--; ) {
				//if(x >= stageWidth << 1) break;
				//if(x >= 0) {
					uint16_t b = stage_get_block(x>>1, y>>1);
					uint16_t t = b << 2; //((b&15) << 1) + ((b>>4) << 6);
					uint16_t ta = pxa[b];
					uint16_t pal = (ta == 0x43 || ta & 0x80) ? PAL1 : PAL2;
					maprow[x&63] = TILE_ATTR(pal, (ta&0x40) > 0, 
							0, 0, TILE_TSINDEX + t + (x&1) + ((y&1)<<1));
				//}
				x++;
			}
			DMA_doDma(DMA_VRAM, (uint32_t)maprow, VDP_PLAN_A + ((y&31)<<7), 64, 2);
		}
		y++;
	}
}

void stage_draw_screen_credits() {
	uint16_t maprow[20];
	for(uint16_t y = 0; y < 30; y++) {
		for(uint16_t x = 20; x < 40; x++) {
			uint16_t b = stage_get_block(x>>1, y>>1);
			uint16_t t = b << 2; //((b&15) << 1) + ((b>>4) << 6);
			maprow[x-20] = TILE_ATTR(PAL2,0,0,0, TILE_TSINDEX + t + (x&1) + ((y&1)<<1));
		}
		DMA_doDma(DMA_VRAM, (uint32_t)maprow, VDP_PLAN_A + y*0x80 + 40, 20, 2);
	}
}

// Draws just one block
void stage_draw_block(uint16_t x, uint16_t y) {
	if(x >= stageWidth || y >= stageHeight) return;
	uint16_t b, xx, yy; uint8_t p;
	p = (stage_get_block_type(x, y) & 0x40) > 0;
	b = TILE_TSINDEX + (stage_get_block(x, y) << 2);
	xx = block_to_tile(x) % 64;
	yy = block_to_tile(y) % 32;

	vdp_map_xy(VDP_PLAN_A, TILE_ATTR(2, p, 0, 0, b), xx, yy);
	vdp_map_xy(VDP_PLAN_A, TILE_ATTR(2, p, 0, 0, b+1), xx+1, yy);
	vdp_map_xy(VDP_PLAN_A, TILE_ATTR(2, p, 0, 0, b+2), xx, yy+1);
	vdp_map_xy(VDP_PLAN_A, TILE_ATTR(2, p, 0, 0, b+3), xx+1, yy+1);
	
}

// Fills VDP_PLAN_B with a tiled background
void stage_draw_background() {
	uint16_t w = background_info[stageBackground].width;
	uint16_t h = background_info[stageBackground].height;
	uint16_t pal = background_info[stageBackground].palette;
	for(uint16_t y = 0; y < 32; y += h) {
		for(uint16_t x = 0; x < 64; x += w) {
			vdp_map_fill_rect(VDP_PLAN_B, TILE_ATTR(pal,0,0,0,TILE_BACKINDEX), x, y, w, h, 1);
			//uint16_t tile = TILE_ATTR(pal,0,0,0,TILE_BACKINDEX);
			//for(uint16_t yy = 0; yy < h; yy++) {
			//	for(uint16_t xx = 0; xx < w; xx++) {
			//		vdp_map_xy(VDP_PLAN_B, tile++, x+xx, y+yy);
			//	}
			//}
		}
	}
}

static void stage_draw_moonback() {
	const uint32_t *topTiles, *btmTiles;
	const uint16_t *topMap, *btmMap;
	if(stageBackgroundType == 1) {
		// Moon
		topTiles = (uint32_t*) PAT_MoonTop;
		btmTiles = (uint32_t*) PAT_MoonBtm;
		topMap = (uint16_t*) MAP_MoonTop;
		btmMap = (uint16_t*) MAP_MoonBtm;
	} else {
		// Fog
		topTiles = (uint32_t*) PAT_FogTop;
		btmTiles = (uint32_t*) PAT_FogBtm;
		topMap = (uint16_t*) MAP_FogTop;
		btmMap = (uint16_t*) MAP_FogBtm;
	}
	// Load the top section in the designated background area
	vdp_tiles_load_from_rom(topTiles, TILE_BACKINDEX, 12);
	// Load the clouds under the map, it just fits
	vdp_tiles_load_from_rom(btmTiles, TILE_MOONINDEX, 188);
	for(uint8_t y = 0; y < 32; y++) backScrollTable[y] = 0;
	vdp_vscroll(VDP_PLAN_B, 0);
	// Top part
	uint16_t index = pal_mode ? 0 : 40;
	for(uint16_t y = 0; y < (pal_mode ? 11 : 10); y++) {
		DMA_doDma(DMA_VRAM, (uint32_t) &topMap[index], VDP_PLAN_B + (y << 7), 40, 2);
		index += 40;
	}
	
	if(vblank) aftervsync(); // So we don't lag the music
	vblank = 0;
	
	// Bottom part
	index = 0;
	for(uint16_t y = (pal_mode ? 11 : 10); y < (pal_mode ? 32 : 28); y++) {
		DMA_doDma(DMA_VRAM, (uint32_t) &btmMap[index], VDP_PLAN_B + (y << 7),             32, 2);
		DMA_doDma(DMA_VRAM, (uint32_t) &btmMap[index], VDP_PLAN_B + (y << 7) + (32 << 1), 32, 2);
		index += 32;
	}
	backScrollTimer = 0;
}
