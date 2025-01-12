#include "common.h"
#include "dma.h"
#include "vdp.h"
#include "xgm.h"
#include "resources.h"
#include "bank_data.h"

#include "stage.h"
#include "tables.h"

#include "gba.h"

#include "gba_systemcalls.h"
#include "gba_video.h"
#include "gba_sprites.h"

#include <stdio.h>

extern const uint32_t TILE_BLANK[8];
static const uint16_t BLANK_DATA[0x80];
const uint16_t PAL_FadeOut[64] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xEEE,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
const uint16_t PAL_FadeOutBlue[64] = {
	0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0xEEE,
	0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,
	0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,
	0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200,0x200
};
const uint16_t PAL_FullWhite[64] = {
	0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,
	0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,
	0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,
	0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE,0xEEE
};

static volatile uint16_t* const vdp_data_port = (uint16_t*) 0xC00000;
static volatile uint16_t* const vdp_ctrl_port = (uint16_t*) 0xC00004;
static volatile uint32_t* const vdp_ctrl_wide = (uint32_t*) 0xC00004;

// Palette vars
static uint16_t pal_current[64];
static uint16_t pal_next[64];
static uint8_t pal_fading;
static uint8_t pal_fadespeed;
static uint8_t pal_fadecnt;

// Sprite vars
static uint16_t sprite_count;
static VDPSprite sprite_table[80];
static uint16_t sprite_ymax;

// Font vars
static uint16_t font_pal;

uint8_t pal_mode = 0;

uint16_t tileScrollX = 0;
uint16_t tileScrollY = 0;

//uint8_t SCREEN_HEIGHT = 0;

void vdp_init() {
	//SCREEN_HEIGHT = 160;
	// Store pal_mode and adjust some stuff based on it
    //pal_mode = *vdp_ctrl_port & 1;
    //SCREEN_HEIGHT = pal_mode ? 240 : 224;
	SCREEN_HALF_H = SCREEN_HEIGHT >> 1;
	sprite_ymax = SCREEN_HEIGHT + 32;
	FPS = pal_mode ? 50 : 60;
	// Set the registers
	/**vdp_ctrl_port = 0x8004;
	*vdp_ctrl_port = 0x8174 | (pal_mode ? 8 : 0); // Enable display
	*vdp_ctrl_port = 0x8200 | (VDP_PLAN_A >> 10); // Plane A address
	*vdp_ctrl_port = 0x8300 | (VDP_PLAN_W >> 10); // Window address
	*vdp_ctrl_port = 0x8400 | (VDP_PLAN_B >> 13); // Plane B address
	*vdp_ctrl_port = 0x8500 | (VDP_SPRITE_TABLE >> 9); // Sprite list address
	*vdp_ctrl_port = 0x8600;
	*vdp_ctrl_port = 0x8700; // Background color palette index
	*vdp_ctrl_port = 0x8800;
	*vdp_ctrl_port = 0x8900;
	*vdp_ctrl_port = 0x8A01; // Horizontal interrupt timer
	*vdp_ctrl_port = 0x8B00 | (VSCROLL_PLANE << 2) | HSCROLL_PLANE; // Scroll mode
	*vdp_ctrl_port = 0x8C81; // No interlace or shadow/highlight
	*vdp_ctrl_port = 0x8D00 | (VDP_HSCROLL_TABLE >> 10); // HScroll table address
	*vdp_ctrl_port = 0x8E00;
	*vdp_ctrl_port = 0x8F02; // Auto increment
	*vdp_ctrl_port = 0x9001; // Map size (64x32)
	*vdp_ctrl_port = 0x9100; // Window X
	*vdp_ctrl_port = 0x9200; // Window Y*/
	// Reset the tilemaps
	vdp_map_clear(VDP_PLAN_A);
	vdp_hscroll(VDP_PLAN_A, 0);
	vdp_vscroll(VDP_PLAN_A, 0);
	vdp_map_clear(VDP_PLAN_B);
	vdp_hscroll(VDP_PLAN_B, 0);
	vdp_vscroll(VDP_PLAN_B, 0);
	// Reset sprites
	vdp_sprites_clear();
	vdp_sprites_update();
	// (Re)load the font
	vdp_font_load(TS_SysFont.tiles);
	vdp_colors(0, PAL_FadeOut, 64);
	// Put blank tile in index 0
	vdp_tiles_load(TILE_BLANK, 0, 1);

	//Background
	BGCTRL[0] = BG_PRIORITY(3) | BG_SIZE(0) | CHAR_BASE(1) | SCREEN_BASE(BASE_BACK);
	//Foreground
	BGCTRL[1] = BG_PRIORITY(1) | BG_SIZE(0) | CHAR_BASE(0) | SCREEN_BASE(BASE_STAGE);

	//Stage Back
	BGCTRL[2] = BG_PRIORITY(2) | BG_SIZE(0) | CHAR_BASE(0) | SCREEN_BASE(BASE_STAGE_BACK);

	//Text/ Window
	BGCTRL[3] |= BG_PRIORITY(0);

	// screen mode & background to display
	SetMode( MODE_0 | BG0_ON | BG1_ON | BG2_ON | BG3_ON | OBJ_ON | OBJ_1D_MAP);
	
}

#include "maxmod.h"

void vdp_vsync() {
	VBlankIntrWait();

		mmFrame();
		vblank = false;
	

}

// Register stuff

void vdp_set_display(uint8_t enabled) {
		return;
	*vdp_ctrl_port = 0x8134 | (enabled ? 0x40 : 0) | (pal_mode ? 0x08 : 0);
}

void vdp_set_autoinc(uint8_t val) {
		return;
	*vdp_ctrl_port = 0x8F00 | val;
}

void vdp_set_scrollmode(uint8_t hoz, uint8_t vert) {
		return;
	*vdp_ctrl_port = 0x8B00 | (vert << 2) | hoz;
}

void vdp_set_highlight(uint8_t enabled) {
		return;
	*vdp_ctrl_port = 0x8C81 | (enabled << 3);
}

void vdp_set_backcolor(uint8_t index) {
		return;
    *vdp_ctrl_port = 0x8700 | index;
}

void vdp_set_window(uint8_t x, uint8_t y) {
		return;
    *vdp_ctrl_port = 0x9100 | x;
    *vdp_ctrl_port = 0x9200 | y;
}

// DMA stuff

static void dma_do(uint32_t from, uint16_t len, uint32_t cmd) {
	//CpuFastSet(from, (u16*)VRAM + cmd, len | COPY32);
	//CpuFastSet(from, (u16*)SPRITE_GFX + cmd, len | COPY32);
	return;
	// Setup DMA length (in word here)
    *vdp_ctrl_port = 0x9300 + (len & 0xff);
    *vdp_ctrl_port = 0x9400 + ((len >> 8) & 0xff);
    // Setup DMA address
    from >>= 1;
    *vdp_ctrl_port = 0x9500 + (from & 0xff);
    from >>= 8;
    *vdp_ctrl_port = 0x9600 + (from & 0xff);
    from >>= 8;
    *vdp_ctrl_port = 0x9700 + (from & 0x7f);
    // Enable DMA transfer
    *vdp_ctrl_wide = cmd;
}

void vdp_dma_vram(uint32_t from, uint16_t to, uint16_t len) {
	dma_do(from, len, (uint32_t)to);
}

void vdp_dma_cram(uint32_t from, uint16_t to, uint16_t len) {
	DMA3COPY(from, OBJ_COLORS + (to/2), len | COPY32);
		return;
	dma_do(from, len, ((0xC000 + (((uint32_t)to) & 0x3FFF)) << 16) + ((((uint32_t)to) >> 14)));
}

/*void vdp_dma_vsram(uint32_t from, uint16_t to, uint16_t len) {
		return;
	dma_do(from, len, ((0x4000 + (((uint32_t)to) & 0x3FFF)) << 16) + ((((uint32_t)to) >> 14)));
}*/

// Tile patterns

void vdp_tiles_load(volatile const uint32_t *data, uint16_t index, uint16_t num) {
	DMA3COPY(data, VRAM + 0 + (index), num | COPY32);
	//vdp_dma_vram((uint32_t) data, index, num);
}

// Temporary solution until I get the tilesets aligned, SGDK takes into account 128K unalignment
void vdp_tiles_load_from_rom(volatile const uint32_t *data, uint16_t index, uint16_t num) {
	//CpuFastSet(data, VRAM + 0 + (index), num | COPY32);
	//CpuFastSet(data, VRAM + (index), num | COPY32);
	//iprintf("index %d num %d\n", index, num);
	DMA3COPY(data, SPRITE_GFX + (index*16), (num*8) | COPY32);
		return;
	DMA_doDma(DMA_VRAM, (uint32_t) data, index << 5, num << 4, 2);
}

// Tile maps

void vdp_map_xy(uint16_t plan, uint16_t tile, uint16_t x, uint16_t y) {
	//iprintf("\x1b[%hu;%huH%s\n", (y>>2)*3, (x>>2)*3, tile);
	//*((u16 *)MAP_BASE_ADR(BASE_STAGE) + x + (y*x/64)) = tile;
	return;
    uint32_t addr = plan + ((x + (y << PLAN_WIDTH_SFT)) << 1);
    *vdp_ctrl_wide = ((0x4000 + ((addr) & 0x3FFF)) << 16) + (((addr) >> 14) | 0x00);
	*vdp_data_port = tile;
}

void vdp_map_hline(uint16_t plan, const uint16_t *tiles, uint16_t x, uint16_t y, uint16_t len) {
	return;
	vdp_dma_vram((uint32_t) tiles, plan + ((x + (y << PLAN_WIDTH_SFT)) << 1), len);
}

void vdp_map_vline(uint16_t plan, const uint16_t *tiles, uint16_t x, uint16_t y, uint16_t len) {
	return;
	vdp_set_autoinc(128);
	vdp_dma_vram((uint32_t) tiles, plan + ((x + (y << PLAN_WIDTH_SFT)) << 1), len);
	vdp_set_autoinc(2);
}

void vdp_map_fill_rect(uint16_t plan, uint16_t index, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t inc) {
	return;
	volatile uint16_t tiles[64]; // Garbled graphics on -Ofast without this volatile here
    for(uint16_t yy = 0; yy < h; yy++) {
        for(uint16_t xx = 0; xx < w; xx++) {
            tiles[xx] = index | CHAR_PALETTE(1);
            index += inc;
        }
		DMA3COPY(tiles, MAP_BASE_ADR(plan) + (x + ((y+yy) << PLAN_WIDTH_SFT)), w | COPY32);
		//vdp_dma_vram((uint32_t) tiles, plan + ((x + ((y+yy) << PLAN_WIDTH_SFT)) << 1), w);
    }
}

void vdp_map_clear(uint16_t plan) {
	iprintf("\x1b[2J");

	return;
	uint16_t addr = plan;
	while(addr < plan + 0x1000) {
		vdp_dma_vram((uint32_t) BLANK_DATA, addr, 0x80);
		addr += 0x100;
	}
}

// Palettes

void vdp_colors(uint16_t index, const uint16_t *values, uint16_t count) {
	vdp_dma_cram((uint32_t) values, index << 1, count);
    for(uint16_t i = count; i--;) pal_current[index+i] = values[i];
}

void vdp_color(uint16_t index, uint16_t color) {
	//BG_PALETTE[index] = color;
	pal_current[index] = color;
	return;
	uint16_t ind = index << 1;
    *vdp_ctrl_wide = ((0xC000 + (((uint32_t)ind) & 0x3FFF)) << 16) + ((((uint32_t)ind) >> 14) | 0x00);
    *vdp_data_port = color;

}

void vdp_colors_next(uint16_t index, const uint16_t *values, uint16_t count) {
    for(uint16_t i = count; i--;) pal_next[index+i] = values[i];
	vdp_dma_cram((uint32_t) values, index << 1, count);
}

void vdp_color_next(uint16_t index, uint16_t color) {
    pal_next[index] = color;
}

uint16_t vdp_fade_step() {
	return 1;
	if(!pal_fading) return 0;
	if(++pal_fadecnt >= pal_fadespeed) {
		pal_fadecnt = 0;
		uint16_t colors_changed = 0;
		for(uint16_t i = 64; i--;) {
			uint16_t cR = pal_current[i] & 0x00E;
			uint16_t nR = pal_next[i]    & 0x00E;
			uint16_t cG = pal_current[i] & 0x0E0;
			uint16_t nG = pal_next[i]    & 0x0E0;
			uint16_t cB = pal_current[i] & 0xE00;
			uint16_t nB = pal_next[i]    & 0xE00;
			if(cR != nR) { pal_current[i] += cR < nR ? 0x002 : -0x002; colors_changed++; }
			if(cG != nG) { pal_current[i] += cG < nG ? 0x020 : -0x020; colors_changed++; }
			if(cB != nB) { pal_current[i] += cB < nB ? 0x200 : -0x200; colors_changed++; }
		}
		if(!colors_changed) {
			pal_fading = FALSE;
			return 0;
		}
		vdp_dma_cram((uint32_t) pal_current, 0, 64);
	}
    return 1;
}

void vdp_fade(const uint16_t *src, const uint16_t *dst, uint16_t speed, uint8_t async) {
	return;
    if(src) vdp_colors(0, src, 64);
    if(dst) vdp_colors_next(0, dst, 64);
	pal_fading = TRUE;
	pal_fadespeed = speed;
	pal_fadecnt = 0;
    if(!async) {
        while(vdp_fade_step()) {
			vdp_vsync();
			//xgm_vblank();
		}
    }
}

// Scroll

void vdp_hscroll(uint16_t plan, int16_t hscroll) {
	BG_OFFSET[plan].x = -hscroll;
	if(plan == VDP_PLAN_A)
	{
		tileScrollX = -hscroll;
		//scroll stage back too
		BG_OFFSET[2].x = -hscroll;
	}

	return;
	uint32_t addr = (plan == VDP_PLAN_A) ? VDP_HSCROLL_TABLE : VDP_HSCROLL_TABLE + 2;
	*vdp_ctrl_wide = ((0x4000 + ((addr) & 0x3FFF)) << 16) + (((addr) >> 14) | 0x00);
	*vdp_data_port = hscroll;
}

void vdp_hscroll_tile(uint16_t plan, int16_t *hscroll) {
	BG_OFFSET[plan].x = hscroll;
		return;
    vdp_set_autoinc(32);
    vdp_dma_vram((uint32_t) hscroll, VDP_HSCROLL_TABLE + (plan == VDP_PLAN_A ? 0 : 2), 32);
    vdp_set_autoinc(2);
}

void vdp_vscroll(uint16_t plan, int16_t vscroll) {
	BG_OFFSET[plan].y = vscroll;
	if(plan == VDP_PLAN_A)
	{
		tileScrollX = vscroll;
		//scroll stage back too
		BG_OFFSET[2].y = vscroll;
	}

	return;	
	uint32_t addr = (plan == VDP_PLAN_A) ? 0 : 2;
	*vdp_ctrl_wide = ((0x4000 + ((addr) & 0x3FFF)) << 16) + (((addr) >> 14) | 0x10);
	*vdp_data_port = vscroll;
}

// Sprites

IWRAM_CODE void vdp_sprite_add(const VDPSprite *spr) {
    // Exceeded max number of sprites
    if(sprite_count >= 80) return;
    // Prevent drawing off screen sprites
    if((unsigned)(spr->x-96) < 352 && (unsigned)(spr->y-96) < sprite_ymax) {
        sprite_table[sprite_count] = *spr;
        sprite_table[sprite_count].link = sprite_count + 1;
        sprite_count++;
    }
}

IWRAM_CODE void vdp_sprites_add(const VDPSprite *spr, uint16_t num) {
	for(uint16_t i = num; i--;) vdp_sprite_add(&spr[i]);
}

void vdp_sprites_clear() {
	static const VDPSprite NULL_SPRITE = { .x = 0x80, .y = 0x80 };
	sprite_count = 0;
	vdp_sprites_add(&NULL_SPRITE, 1);
}

static OBJATTR obj_buffer[128] = { 0 };
const u16 palette[] = {
	RGB8(0x00,0x00,0x00),
	RGB8(0xFF,0xFF,0xFF),
	RGB8(0xF5,0xFF,0xFF),
	RGB8(0xDF,0xFF,0xF2),
	RGB8(0xCA,0xFF,0xE2),
	RGB8(0xB7,0xFD,0xD8),
	RGB8(0x2C,0x4F,0x8B),
	RGB8(0xF5,0xFF,0xFF),
	RGB8(0xDF,0xFF,0xF2),
	RGB8(0xCA,0xFF,0xE2),
	RGB8(0xB7,0xFD,0xD8),
	RGB8(0x2C,0x4F,0x8B)
};

u8 sprct = 0;

bool IsBitSet(uint16_t b, int pos)
{
   return (b & (1 << pos)) != 0;
}

int get_sprite_size(uint8_t size)
{
	int w = (size&12) >> 2;
	int h = size&3;
	//printf("sizew %d sizeh %d\n", w, h);

	if (w == 0 && h == 0)
		return Sprite_8x8;
	if (w == 1 && h == 1)
		return Sprite_16x16;
	if (w == 3 && h == 3)
		return Sprite_32x32;
	if (w == 1 && h == 0)
		return Sprite_16x8;
	if (w == 3 && h == 0)
		return Sprite_32x8;
	if (w == 3 && h == 1)
		return Sprite_32x16;	
	if (w == 0 && h == 1)
		return Sprite_8x16;
	if (w == 0 && h == 3)
		return Sprite_8x32;
	if (w == 1 && h == 3)
		return Sprite_16x32;

	return Sprite_16x16;
}

IWRAM_CODE void vdp_sprites_update() {
	if(!sprite_count) return;
	//iprintf("%d %d\n", (&SPR_Quote)->animations[0]->frames[0]->w, (&SPR_Quote)->animations[0]->frames[0]->h);
	sprite_table[sprite_count - 1].link = 0; // Mark end of sprite list

	for(int i=0;i<sprite_count;i++)
	{
		int size = get_sprite_size(sprite_table[i].size);

		char size_bits = 0; char shape_bits = 0;

    	switch (size) {
    	    case Sprite_8x8:   size_bits = 0; shape_bits = 0; break;
    	    case Sprite_16x16: size_bits = 1; shape_bits = 0; break;
    	    case Sprite_32x32: size_bits = 2; shape_bits = 0; break;
    	    case Sprite_16x8:  size_bits = 0; shape_bits = 1; break;
    	    case Sprite_32x8:  size_bits = 1; shape_bits = 1; break;
    	    case Sprite_32x16: size_bits = 2; shape_bits = 1; break;
    	    case Sprite_8x16:  size_bits = 0; shape_bits = 2; break;
    	    case Sprite_8x32:  size_bits = 1; shape_bits = 2; break;
    	    case Sprite_16x32: size_bits = 2; shape_bits = 2; break;
    	}

		obj_buffer[i].attr0 = OBJ_Y(sprite_table[i].y - 128) | OBJ_SHAPE(shape_bits);
		obj_buffer[i].attr1 = OBJ_X(sprite_table[i].x - 128) | OBJ_SIZE(size_bits);
		if(IsBitSet(sprite_table[i].attr, 11))
			obj_buffer[i].attr1 |= OBJ_HFLIP;
		if(IsBitSet(sprite_table[i].attr, 12))
			obj_buffer[i].attr1 |= OBJ_VFLIP;
			
		int prio = sprite_table[i].attr>>15;
		if(prio == 0)
			prio = 2;
		else
			prio = 1;
		obj_buffer[i].attr2 = OBJ_PRIORITY(prio) 
			| OBJ_CHAR((sprite_table[i].attr&0x7FF)+0) | OBJ_PALETTE((sprite_table[i].attr>>13)&3);
	}
	u16 *temppointer;
	u16 *temppointer2;
	// load the palette for the background, 7 colors
	temppointer = BG_COLORS;
	*temppointer = palette[0];
	temppointer2 = OBJ_COLORS + 33;
	*temppointer2 = palette[0];
	temppointer = BG_COLORS + 1;
	if(stage_info[stageID].tileset != NULL)
		for(int i=1; i<16; i++) { //OBJ Pal 0

			*temppointer++ = tileset_info[stage_info[stageID].tileset].palette[i];
		}
	if(background_info[stageBackground].palette != NULL)
		for(int i=0; i<16; i++) { //OBJ Pal 1

			*temppointer++ = background_info[stageBackground].palette[i];
		}
	if(stage_info[stageID].tileset != NULL)	
		for(int i=1; i<16; i++) { //OBJ Pal 2
			*temppointer2++ = tileset_info[stage_info[stageID].tileset].palette[i];
		}
	if(stage_info[stageID].npcPalette != NULL)
		for(int i=0; i<16; i++) { //OBJ Pal 3
			*temppointer2++ = stage_info[stageID].npcPalette[i];
		}
	if(stage_info[stageID].npcPalette2 != NULL)
		for(int i=0; i<16; i++) { //OBJ Pal 3
			*temppointer2++ = stage_info[stageID].npcPalette2[i];
		}

	BG_COLORS[241]=RGB5(17,31,31);

	DMA3COPY(obj_buffer, OAM, ((sizeof(OBJATTR)*128)/2));

	for (u8 i = 0; i < 128; i++)
		obj_buffer[i].attr0 = OBJ_DISABLE;

	//vdp_dma_vram((uint32_t) sprite_table, VDP_SPRITE_TABLE, sprite_count << 2);
	sprite_count = 0;
	/*for(int i=0;i<SPR_Balrog.animations[0]->frames[0]->numSprite;i++)
	{
		printf("Balrog %d, x %d y %d s %d n %d\n", i, SPR_Balrog.animations[0]->frames[0]->vdpSpritesInf[i]->x,
		SPR_Balrog.animations[0]->frames[0]->vdpSpritesInf[i]->y, SPR_Balrog.animations[0]->frames[0]->vdpSpritesInf[i]->size, 
		SPR_Balrog.animations[0]->frames[0]->vdpSpritesInf[i]->numTile);
	}*/
}

// Font / Text

void vdp_font_load(const uint32_t *tiles) {
		return;
	font_pal = 0;
	// ASCII 32-127
	vdp_tiles_load_from_rom(tiles, TILE_FONTINDEX, 0x60);
	// Extended charset
    const uint32_t *ext = tiles + (0x60 << 3);
	uint16_t index = (VDP_PLAN_W >> 5) + 3;
	for(uint16_t i = 0; i < 30; i++) {
	    vdp_tiles_load(ext, index, 1);
	    index += 4;
	    ext += 8;
	}
}

void vdp_font_pal(uint16_t pal) {
		return;
	font_pal = pal;
}

#include "gba.h"

void vdp_puts(uint16_t plan, const char *str, uint16_t x, uint16_t y) {
	//GBATODO
	//char text[128];
	iprintf("\x1b[%hu;%huH%s\n", (y>>2)*3, (x>>2)*3, str);
	//sprintf(text, "\x1b[%hu;%huH%s\n", y, x, str);
	//iprintf(text);
	return;
	uint32_t addr = plan + ((x + (y << PLAN_WIDTH_SFT)) << 1);
	*vdp_ctrl_wide = ((0x4000 + ((addr) & 0x3FFF)) << 16) + (((addr) >> 14) | 0x00);
	for(uint16_t i = 0; i < 64 && *str; ++i) {
		// Wrap around the plane, don't fall to next line
		if(i + x == 64) {
			addr -= x << 1;
			*vdp_ctrl_wide = ((0x4000 + ((addr) & 0x3FFF)) << 16) + (((addr) >> 14) | 0x00);
		}
		uint16_t c = *str++;
		if(c == 1) { // Accent chars
			c = (VDP_PLAN_W >> 5) - 1 + ((*str++) << 2);
		} else {
			c = TILE_FONTINDEX + c - 0x20;
		}
		uint16_t attr = TILE_ATTR(font_pal,1,0,0,c);
		*vdp_data_port = attr;
	}
}

void vdp_text_clear(uint16_t plan, uint16_t x, uint16_t y, uint16_t len) {
	//GBATODO
	char space[256];
	for(int i = 0;i<len;i++)
	{
		space[i] = ' ';
	}
	space[len] = '\0';
	iprintf("\x1b[%hu;%huH%s", (y>>2)*3, (x>>2)*3, space);
	return;
    uint32_t addr = plan + ((x + (y << PLAN_WIDTH_SFT)) << 1);
	*vdp_ctrl_wide = ((0x4000 + ((addr) & 0x3FFF)) << 16) + (((addr) >> 14) | 0x00);
	while(len--) *vdp_data_port = 0;
}
