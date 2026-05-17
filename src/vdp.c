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

#include "gbaram.h"

#include "gamemode.h" 

u8 saturate = false;
// Fast integer soft-clip curve
// - 0 to 22: Untouched (Linear)
// - 23 to 30: Compressed 2:1 (Gently eases into the brights)
// - 31 to 38: Compressed 4:1 (Heavily compresses overblown colors)
// - 39+: Caps at 28
IWRAM_CODE static inline int apply_soft_clip(int val, int orig) {
    if (orig == 31) return 31; // Always preserve pure max-intensity channels
    if (val < 0) return 0;
    
    if (val <= 22) {
        return val;
    } else if (val <= 30) {
        return 22 + ((val - 22) >> 1); // Maps 23-30 down to 22-26
    } else {
        val = 26 + ((val - 30) >> 2);  // Maps 31+ starting at 26
        return (val > 30) ? 30 : val;  // Finally cap at 28
    }
}

IWRAM_CODE uint16_t saturate_color(uint16_t color) {
    if(!saturate) return color;

    // 1. Unpack 5-bit RGB
    int r = color & 0x1F;
    int g = (color >> 5) & 0x1F;
    int b = (color >> 10) & 0x1F;

    // Store original values to check for max intensity later
    int orig_r = r;
    int orig_g = g;
    int orig_b = b;

    // 2. Calculate average brightness (Grayscale)
    int gray = (r + g + b) / 3;

    // 3. Apply Intense Saturation (Difference * 2)
    r = gray + ((r - gray) << 1);
    g = gray + ((g - gray) << 1);
    b = gray + ((b - gray) << 1);

    // 4. Apply 1.375x Brightness
    r = (r * 21) >> 4;
    g = (g * 21) >> 4;
    b = (b * 21) >> 4;

    // 5. Apply Soft Clipping Curve to preserve color differentiation
    r = apply_soft_clip(r, orig_r);
    g = apply_soft_clip(g, orig_g);
    b = apply_soft_clip(b, orig_b);

    // 6. Repack
    return (uint16_t)(r | (g << 5) | (b << 10));
}
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

#include "gbatext.h"

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
	BGCTRL[3] = BG_PRIORITY(0) | BG_SIZE(0) | CHAR_BASE(2) | SCREEN_BASE(BASE_TEXT);

	// screen mode & background to display
	SetMode( MODE_0 | BG0_ON | BG1_ON | BG2_ON | BG3_ON | OBJ_ON | OBJ_1D_MAP);
	    REG_BLDCNT = (1 << 3) |          // 1st Target BG3
                 (1 << 8) |          // 2nd Target BG0
                 (1 << 9) |          // 2nd Target BG1
                 (1 << 10)|          // 2nd Target BG2
                 (1 << 12)|          // 2nd Target Sprites (OBJ)
                 (1 << 6);           // Mode: Alpha Blending

    // Set transparency levels
    // EVA = Source weight (3/16), EVB = Destination weight (13/16)
    REG_BLDALPHA = (1 << 8) | 16; 
	canvas_init();
}

#include "maxmod.h"

void vdp_vsync() {
	VBlankIntrWait();

		//mmFrame();
		vblank = false;
	

}

// Register stuff

void vdp_set_display(uint8_t enabled) {
	return;
    if (enabled) {
        REG_DISPCNT &= ~0x0080; // Clear Forced Blank bit (Turn screen ON)
    } else {
        REG_DISPCNT |= 0x0080;  // Set Forced Blank bit (Turn screen OFF)
    }
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
    uint16_t* src = (uint16_t*)from;
    
    // 'to' is likely a byte offset (0xC000 based in Genesis), 
    // so we divide by 2 to get the 16-bit index for GBA palette RAM.
    volatile uint16_t* dst = OBJ_COLORS + (to / 2);

    // Loop through each color
    for (int i = 0; i < len; i++) {
        uint16_t color = src[i];

        //color = saturate_color(color);
        dst[i] = saturate_color(color);
    }
}

/*void vdp_dma_vsram(uint32_t from, uint16_t to, uint16_t len) {
		return;
	dma_do(from, len, ((0x4000 + (((uint32_t)to) & 0x3FFF)) << 16) + ((((uint32_t)to) >> 14)));
}*/

// Tile patterns

void vdp_tiles_load(volatile const uint32_t *data, uint16_t index, uint16_t num) {
	DMA3COPY(data, VRAM + (index * TILE_SIZE), (num * 8) | COPY32);
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
    uint16_t gba_base;
    switch(plan) {
        case VDP_PLAN_A: gba_base = BASE_STAGE; break;
        case VDP_PLAN_B: gba_base = BASE_BACK;  break;
        case VDP_PLAN_W: gba_base = BASE_TEXT;  break;
        default: return;
    }
    uint16_t *map = (uint16_t*)MAP_BASE_ADR(gba_base);
    // GBA maps are 32x32. Offset = (y * 32) + x
    map[(y & 31) * 32 + (x & 31)] = tile;
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
    uint32_t fill_val = 0;
    // 512 words = 2048 bytes (32x32 tiles * 2 bytes each)
    uint32_t count = 512 | (1 << 24); 

    if (plan == VDP_PLAN_A) {
        // Clear BOTH Stage layers used for the Genesis Plane A mapping
        CpuFastSet(&fill_val, (void*)MAP_BASE_ADR(BASE_STAGE), count);      // BG1
        CpuFastSet(&fill_val, (void*)MAP_BASE_ADR(BASE_STAGE_BACK), count); // BG2
    } 
    else if (plan == VDP_PLAN_B) {
        // Clear the Tiled Background layer
        CpuFastSet(&fill_val, (void*)MAP_BASE_ADR(BASE_BACK), count);      // BG0
    } 
    else if (plan == VDP_PLAN_W) {
        // Clear the Text/Window layer
        CpuFastSet(&fill_val, (void*)MAP_BASE_ADR(BASE_TEXT), count);      // BG3
    }
    else {
        // Fallback: If a raw GBA base index was passed (like 25, 30, etc.)
        CpuFastSet(&fill_val, (void*)MAP_BASE_ADR(plan), count);
    }
}

// Palettes

void vdp_colors(uint16_t index, const uint16_t *values, uint16_t count) {
	vdp_dma_cram((uint32_t) values, index << 1, count);
    for(uint16_t i = count; i--;) pal_current[index+i] = values[i];
}

void vdp_colors_bg(uint16_t index, const uint16_t *values, uint16_t count) {
    // 0x05000000 is the start of GBA Background Palette RAM
    volatile uint16_t* bg_pal_ram = (volatile uint16_t*)0x05000000;
    
    for (uint16_t i = 0; i < count; i++) {
        // Use the existing saturate_color helper to convert/enhance the color
        uint16_t processed_color = saturate_color(values[i]);
        
        // Write to hardware
        bg_pal_ram[index + i] = processed_color;
        
        // Update the engine's internal tracking array if it's within the first 64 entries
        // (Used by the fade system)
        if ((index + i) < 64) {
            pal_current[index + i] = values[i];
        }
    }
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
			pal_fading = FADE_LAST;
			return 2;
		}
	}
    return 1;
}

void vdp_fade_step_dma() {
    if(pal_fading != FADE_NONE) {
        vdp_dma_cram((uint32_t) pal_current, 0, 64);
        if(pal_fading == FADE_LAST) pal_fading = FADE_NONE;
    }
}

void vdp_fade(const uint16_t *src, const uint16_t *dst, uint16_t speed, uint8_t async) {
	return;
    if(src) vdp_colors(0, src, 64);
    if(dst) vdp_colors_next(0, dst, 64);
	pal_fading = FADE_INPROGRESS;
	pal_fadespeed = speed;
	pal_fadecnt = 0;
    if(!async) {
        //while(vdp_fade_step_calc()) {
			vdp_vsync();
			vdp_fade_step_dma();
		//}
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
	BG_OFFSET[plan].x = (int)hscroll;
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
	RGB8(0,0,0),
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

static const uint8_t GBA_SPRITE_SIZES[16] = {
    (0 << 2) | 0, // 0x0: 8x8   -> Square 8x8
    (2 << 2) | 0, // 0x1: 8x16  -> Vertical 8x16
    (2 << 2) | 1, // 0x2: 8x24  -> Vertical 8x32
    (2 << 2) | 1, // 0x3: 8x32  -> Vertical 8x32

    (1 << 2) | 0, // 0x4: 16x8  -> Horizontal 16x8
    (0 << 2) | 1, // 0x5: 16x16 -> Square 16x16
    (2 << 2) | 2, // 0x6: 16x24 -> Vertical 16x32
    (2 << 2) | 2, // 0x7: 16x32 -> Vertical 16x32

    (1 << 2) | 1, // 0x8: 24x8  -> Horizontal 32x8
    (1 << 2) | 2, // 0x9: 24x16 -> Horizontal 32x16
    (0 << 2) | 2, // 0xA: 24x24 -> Square 32x32
    (0 << 2) | 2, // 0xB: 24x32 -> Square 32x32

    (1 << 2) | 1, // 0xC: 32x8  -> Horizontal 32x8
    (1 << 2) | 2, // 0xD: 32x16 -> Horizontal 32x16
    (0 << 2) | 2, // 0xE: 32x24 -> Square 32x32
    (0 << 2) | 2  // 0xF: 32x32 -> Square 32x32
};

IWRAM_CODE void vdp_sprites_update() {
	if(!sprite_count) return;
	//iprintf("%d %d\n", (&SPR_Quote)->animations[0]->frames[0]->w, (&SPR_Quote)->animations[0]->frames[0]->h);
	sprite_table[sprite_count - 1].link = 0; // Mark end of sprite list

	for(int i=0;i<sprite_count;i++)
	{
		uint8_t gen_size = sprite_table[i].size & 0x0F;
		uint8_t gba_size = GBA_SPRITE_SIZES[gen_size];

		char shape_bits = gba_size >> 2;
		char size_bits = gba_size & 3;

		obj_buffer[i].attr0 = OBJ_Y(sprite_table[i].y - 128) | OBJ_SHAPE(shape_bits);
		obj_buffer[i].attr1 = OBJ_X(sprite_table[i].x - 128) | OBJ_SIZE(size_bits);
		if(IsBitSet(sprite_table[i].attr, 11))
			obj_buffer[i].attr1 |= OBJ_HFLIP;
		if(IsBitSet(sprite_table[i].attr, 12))
			obj_buffer[i].attr1 |= OBJ_VFLIP;
			
		int prio = (sprite_table[i].attr & 0x8000) ? 0 : 2; 
		
    	int pal = sprite_table[i].size >> 4; 
    	if (pal == 0) {
    	    // Fallback to the standard Genesis 2-bit palette if none was provided
    	    pal = (sprite_table[i].attr >> 13) & 3; 
    	}

    	obj_buffer[i].attr2 = OBJ_PRIORITY(prio) 
    	    | OBJ_CHAR((sprite_table[i].attr&0x7FF)+0) 
    	    | OBJ_PALETTE(pal);
	}
	u16 *temppointer;
	u16 *temppointer2;
	// load the palette for the background, 7 colors
    temppointer = BG_COLORS;

    if (gamemode == GM_TITLE || gamemode == GM_SAVESEL || gamemode == GM_CONFIG) {
        *temppointer = (4 | (4 << 5) | (4 << 10));
	} else if (gamemode == GM_GAME) {
		*temppointer = (0 | (0 << 5) | (4 << 10));
	} else {
        *temppointer = saturate_color(palette[0]); // Default back to black
    }
    temppointer2 = OBJ_COLORS + 33;
    *temppointer2 = saturate_color(palette[0]);

	temppointer = OBJ_COLORS + (16*8) + 1; // fade color
    *temppointer = (0 | (0 << 5) | (4 << 10));

    temppointer = BG_COLORS + 1;
    if(stage_info[stageID].tileset != NULL)
        for(int i=1; i<16; i++) { // OBJ Pal 0

            *temppointer++ = saturate_color(tileset_info[stage_info[stageID].tileset].palette[i]);
        }
    if(background_info[stageBackground].palette != NULL && gamemode != GM_SOUNDTEST)
        for(int i=0; i<16; i++) { // OBJ Pal 1
            *temppointer++ = saturate_color(background_info[stageBackground].palette[i]);
        }
    if(stage_info[stageID].tileset != NULL)    
        for(int i=1; i<16; i++) { // OBJ Pal 2
            *temppointer2++ = saturate_color(tileset_info[stage_info[stageID].tileset].palette[i]);
        }
    if(stage_info[stageID].npcPalette != NULL)
        for(int i=0; i<16; i++) { // OBJ Pal 3
            *temppointer2++ = saturate_color(stage_info[stageID].npcPalette[i]);
        }
    if(stage_info[stageID].npcPalette2 != NULL)
        for(int i=0; i<16; i++) { // OBJ Pal 3
            *temppointer2++ = saturate_color(stage_info[stageID].npcPalette2[i]);
        }

    // Load balrog palette into OBJ Pal 10 for cutscene NPCs
    temppointer = OBJ_COLORS + (16*10);
    for(int i=0; i<16; i++) {
        temppointer[i] = saturate_color(PAL_bllg[i]);
    }

	// Upload npcsym palette (PAL_Sym) to BG Bank 4 for breakable blocks
	if(stage_info[stageID].tileset != NULL) {
		temppointer = BG_COLORS + 64;
		for(int i=0; i<16; i++) {
			*temppointer++ = saturate_color(PAL_Sym[i]);
		}
	}

	//temp text color
	//BG_COLORS[16+16+0]=RGB5(31,31,31);
	BG_COLORS[16+16+1]=RGB5(31,31,31);
	//BG_COLORS[16+16+2]=RGB5(31,31,31);
	BG_COLORS[241]=RGB5(17,31,31);



    uint16_t blueBG = (8 << 10) | (4 << 5) | 3; 

    // Assuming BG3 (Window) uses Palette Bank 2 (index 32-47)
    // Index 0 of the bank is usually transparency, 
    // but the window "fill" tile usually uses a specific index.
    // Based on your code, we'll set index 0 of palette 2:
    BG_COLORS[32+2] = blueBG; 

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
#include "window.h"
#include "gba.h"
#include "gbatext.h"
void vdp_puts(uint16_t plan, const char *str, uint16_t x, uint16_t y) {
    int cur_px = x * 8;
    int cur_py = y * 8;
    extern uint8_t s_canvas_is_fullscreen;
    int stride = s_canvas_is_fullscreen ? CANVAS_TILES_W_FULL : CANVAS_TILES_W;

    while (*str) {
        uint8_t ascii = (uint8_t)*str++;
        if (ascii < 0x20 || ascii > 0x7F) continue;

        uint8_t glyph_idx = ascii - 0x20;
        const uint8_t *glyph = &thinfontTiles[glyph_idx * 12];
        int advance = thinfont_widths[glyph_idx];

        for (int row = 0; row < 12; row++) {
            uint8_t bits = glyph[row];
            if (!bits) continue;

            for (int col = 0; col < 8; col++) {
                if (bits & (0x80 >> col)) {
                    int final_x = cur_px + col;
                    int final_y = cur_py + row;

                    if (final_x >= 0 && final_x < 240 && final_y >= 0 && final_y < 160) {
                        int tile_col = final_x >> 3;
                        int tile_row = final_y >> 3;

                        int tile_idx = CANVAS_TILE_BASE + (tile_row * stride) + tile_col;

                        write_tile_pixel(tile_idx, final_x & 7, final_y & 7, 1);
                    }
                }
            }
        }
        cur_px += advance;
    }
}
void vdp_puts_shadow(uint16_t plan, const char *str, uint16_t x, uint16_t y) {
    int px = x * 8;
    int py = y * 8;

    // Measure string width
    int w = 0;
    for (const char *s = str; *s; s++)
        if ((uint8_t)*s >= 32 && (uint8_t)*s <= 127)
            w += thinfont_widths[(uint8_t)*s - 32];

    // Clear region (+1 for shadow overhang on right/bottom)
    canvas_clear_region(px, py, 240 - px, 14); // clear to end of screen width


    canvas_puts(px + 1, py + 1, str, 2); // shadow
    canvas_puts(px,     py,     str, 1); // text
}
void vdp_text_clear(uint16_t plan, uint16_t x, uint16_t y, uint16_t len) {
    int px = x * 8;
    int py = y * 8;
    int pixel_len = len * 8;
    
    // Choose 0 (transparent) for fullscreen, or 2 (blue textbox) for windowed.
    // Ensure s_canvas_is_fullscreen is accessible here, otherwise just use 0.
    extern uint8_t s_canvas_is_fullscreen; 
    uint8_t bg_color = s_canvas_is_fullscreen ? 0 : 2;

    // Erase the 12-pixel height of our font
    for (int row = 0; row < 12; row++) {
        for (int col = 0; col < pixel_len; col++) {
            int final_x = px + col;
            int final_y = py + row;

            if (final_x >= 0 && final_x < 240 && final_y >= 0 && final_y < 160) {
                int tile_col = final_x >> 3;
                int tile_row = final_y >> 3;
                
                int tile_idx = s_canvas_is_fullscreen 
                    ? CANVAS_TILE_BASE + (tile_row * CANVAS_TILES_W_FULL) + tile_col 
                    : CANVAS_TILE_BASE + (tile_row * CANVAS_TILES_W) + tile_col;
                
                write_tile_pixel(tile_idx, final_x & 7, final_y & 7, bg_color);
            }
        }
    }
}