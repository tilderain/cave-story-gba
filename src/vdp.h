#define VDP_PLAN_W				((uint16_t)2)
#define VDP_PLAN_A              ((uint16_t)1)
#define VDP_PLAN_B              ((uint16_t)0)
#define VDP_SPRITE_TABLE        ((uint16_t)0xF800)
#define VDP_HSCROLL_TABLE       ((uint16_t)0xFC00)

#define BASE_STAGE 30
#define BASE_STAGE_BACK 31
#define BASE_BACK 28
#define BASE_TEXT 29

#define PLAN_WIDTH				32
#define PLAN_HEIGHT				32
#define PLAN_WIDTH_SFT			6
#define PLAN_HEIGHT_SFT			5

#define HSCROLL_PLANE           0
#define HSCROLL_TILE            2
#define HSCROLL_LINE            3
#define VSCROLL_PLANE           0
#define VSCROLL_2TILE           1

#define PAL0					0
#define PAL1					1
#define PAL2					2
#define PAL3					3
#define PAL4					4
#define PAL5					5

#define FADE_NONE               0
#define FADE_INPROGRESS         1
#define FADE_LAST               2

#define TILE_SIZE				32

#define TILE_INDEX_MASK         0x7FF

// ----------------------------------------------------------------------------
// OBJ VRAM OFFSETS (Sprite Graphics)
// GBA OBJ VRAM has room for 1024 tiles (32KB). 
// Allocate the beginning for fixed assets and the rest for dynamic sheets.
// ----------------------------------------------------------------------------

// Fixed Assets (Loaded once or kept resident)
#define TILE_PLAYERINDEX      0
#define TILE_PLAYERSIZE       4

#define TILE_WEAPONINDEX      (TILE_PLAYERINDEX + TILE_PLAYERSIZE)
#define TILE_WEAPONSIZE       6

#define TILE_HUDINDEX         (TILE_WEAPONINDEX + TILE_WEAPONSIZE)
#define TILE_HUDSIZE          108

#define TILE_NUMBERINDEX      (TILE_HUDINDEX + TILE_HUDSIZE)
#define TILE_NUMBERSIZE       24

#define TILE_BRUHINDEX       (TILE_NUMBERINDEX + TILE_NUMBERSIZE) // Map name / weapon icon conflict
#define TILE_BRUHSIZE        0

#define TILE_AIRINDEX         (TILE_BRUHINDEX + TILE_BRUHSIZE)
#define TILE_AIRSIZE          8

#define TILE_QMARKINDEX       (TILE_AIRINDEX + TILE_AIRSIZE)
#define TILE_QMARKSIZE        1


#define TILE_WHIMINDEX        (TILE_QMARKINDEX + TILE_QMARKSIZE)
#define TILE_WHIMSIZE         2

#define TILE_BONKINDEX        (TILE_WHIMINDEX + TILE_WHIMSIZE)
#define TILE_BONKSIZE         1

#define TILE_BOOSTINDEX       (TILE_BONKINDEX + TILE_BONKSIZE)
#define TILE_BOOSTSIZE        4

#define TILE_FADEINDEX        (TILE_BOOSTINDEX + TILE_BOOSTSIZE)      
#define TILE_FADESIZE         2

#define TILE_GIBINDEX         (TILE_FADEINDEX + TILE_FADESIZE)
#define TILE_GIBSIZE          4

#define TILE_DISSIPINDEX      (TILE_GIBINDEX + TILE_GIBSIZE)
#define TILE_DISSIPSIZE       16

#define TILE_BOOMINDEX        (TILE_DISSIPINDEX + TILE_DISSIPSIZE)
#define TILE_BOOMSIZE         32

#define TILE_HITINDEX         (TILE_BOOMINDEX + TILE_BOOMSIZE)
#define TILE_HITSIZE          16

#define TILE_SHOOTINDEX       (TILE_HITINDEX + TILE_HITSIZE)
#define TILE_SHOOTSIZE        16

#define TILE_EXWEPINDEX       (TILE_SHOOTINDEX + TILE_SHOOTSIZE)
#define TILE_EXWEPSIZE        16
// 12 tiles at the end for nemesis vertical frames
#define TILE_NEMINDEX         (TILE_EXWEPINDEX + TILE_EXWEPSIZE)
#define TILE_NEMSIZE          12
// 8 tiles after window plane for blade L3
#define TILE_SLASHINDEX       (TILE_NEMINDEX + TILE_NEMSIZE)
#define TILE_SLASHSIZE        8

#define TILE_PROMPTINDEX      (TILE_SLASHINDEX + TILE_SLASHSIZE)
#define TILE_PROMPTSIZE       38

#define TILE_AIRTANKINDEX     (TILE_PROMPTINDEX + TILE_PROMPTSIZE)
#define TILE_AIRTANKSIZE      9

#define TILE_SMOKEINDEX       (TILE_AIRTANKINDEX + TILE_AIRTANKSIZE + 16)
#define TILE_SMOKESIZE        28

#define TILE_RESERVEDSPACE    (TILE_SMOKEINDEX + TILE_SMOKESIZE)
#define TILE_RESERVEDSIZE     8 // Deep padding buffer to prevent graphic overruns

// Dynamic / Contextual Sprite Assets (loaded per stage/menu)
#define TILE_FACEINDEX        (TILE_RESERVEDSPACE + TILE_RESERVEDSIZE)
#define TILE_FACESIZE         36

#define TILE_NAMEINDEX        (TILE_FACEINDEX + TILE_FACESIZE)
#define TILE_NAMESIZE         32

// Space for shared sprite sheets (Enemies, Bosses, Items, etc)
#define TILE_SHEETINDEX       (TILE_NAMEINDEX + TILE_NAMESIZE)
#define TILE_SHEETSIZE        (1024 - TILE_SHEETINDEX)

// Specific fixed indices for cutscene sprites that override standard sheets
#define TILE_CLOUDINDEX       TILE_SHEETINDEX
#define TILE_CLOUD2INDEX      (TILE_CLOUDINDEX + (16*12))
#define TILE_CLOUD3INDEX      (TILE_CLOUD2INDEX + (16*3))
#define TILE_CLOUD4INDEX      (TILE_CLOUD3INDEX + (9*3))


// ----------------------------------------------------------------------------
// BG VRAM OFFSETS (Background Graphics)
// ----------------------------------------------------------------------------
#define TILE_SYSTEMINDEX        0
#define TILE_USERINDEX          0
#define TILE_FONTINDEX          96

// Legacy Genesis defines used in background mappings
#define TILE_EXTRA1INDEX        (128*128)
#define TILE_EXTRA2INDEX        64

// Tileset width/height
#define TS_WIDTH 32
#define TS_HEIGHT 16
// Stage tileset is first in USERINDEX
#define TILE_TSINDEX            TILE_USERINDEX
#define TILE_TSSIZE             (TS_WIDTH * TS_HEIGHT)

// Allocation of EXTRA1 - background
#define TILE_BACKINDEX          512
#define TILE_BACKSIZE           96

#define TILE_WINDOWINDEX        108
#define TILE_WINDOWSIZE         9




// 16 tiles for weapon swap display (4 weapons x 4 tiles each)
#define TILE_EXWEPINDEX     (TILE_SHOOTINDEX + TILE_SHOOTSIZE)
#define TILE_EXWEPSIZE      16




// Unused palette color tiles area
#define TILE_WEAPONINDEX (TILE_PLAYERINDEX + TILE_PLAYERSIZE)
#define TILE_WEAPONSIZE 6

#define TILE_ATTR(pal, prio, flipV, flipH, index)                               \
	((((uint16_t)flipH) << 11) | (((uint16_t)flipV) << 12) |                    \
	(((uint16_t)pal) << 13) | (((uint16_t)prio) << 15) | ((uint16_t)index))

#define SPRITE_SIZE(w, h)   ((((w) - 1) << 2) | ((h) - 1))

#define sprite_pos(s, px, py) { (s).x = 0x80 + (px); (s).y = 0x80 + (py); }
#define sprite_size(s, w, h) { (s).size = ((((w) - 1) << 2) | ((h) - 1)); }
#define sprite_pri(s, pri)   { (s).attr &= ~(1<<15); (s).attr |= ((pri)&1) << 15; }
#define sprite_pal(s, pal)   { (s).attr &= ~(3<<13); (s).attr |= ((pal)&3) << 13; }
#define sprite_vflip(s, flp) { (s).attr &= ~(1<<12); (s).attr |= ((flp)&1) << 12; }
#define sprite_hflip(s, flp) { (s).attr &= ~(1<<11); (s).attr |= ((flp)&1) << 11; }
#define sprite_index(s, ind) { (s).attr &= ~0x7FF;   (s).attr |= (ind)&0x7FF; }

extern u8 saturate;
// 32 bytes of zero, can be sent to VDP to clear any tile
extern const uint32_t TILE_BLANK[8];
// FadeOut is almost completely black, except index 15 which is white
// This allows text to still be displayed after the screen fades to black
extern const uint16_t PAL_FadeOut[64];
extern const uint16_t PAL_FadeOutBlue[64];
// FullWhite is used for a TSC instruction that flashes the screen white
extern const uint16_t PAL_FullWhite[64];
// Remember the pal mode flag so we don't have to read the control port every time
extern uint8_t pal_mode;

extern uint16_t tileScrollX;
extern uint16_t tileScrollY;

// Set defaults, clear everything
void vdp_init();
// Wait until next vblank
void vdp_vsync();

// Register stuff
void vdp_set_display(uint8_t enabled);
void vdp_set_autoinc(uint8_t val);
void vdp_set_scrollmode(uint8_t hoz, uint8_t vert);
void vdp_set_highlight(uint8_t enabled);
void vdp_set_backcolor(uint8_t index);
void vdp_set_window(uint8_t x, uint8_t y);

// Status
uint16_t vdp_get_palmode();
uint16_t vdp_get_vblank();

// DMA stuff
void vdp_dma_vram(uint32_t from, uint16_t to, uint16_t len);
void vdp_dma_cram(uint32_t from, uint16_t to, uint16_t len);
void vdp_dma_vsram(uint32_t from, uint16_t to, uint16_t len);

// Tile patterns
void vdp_tiles_load(volatile const uint32_t *data, uint16_t index, uint16_t num);
void vdp_tiles_load_from_rom(volatile const uint32_t *data, uint16_t index, uint16_t num);

// Tile maps
void vdp_map_xy(uint16_t plan, uint16_t tile, uint16_t x, uint16_t y);
void vdp_map_hline(uint16_t plan, const uint16_t *tiles, uint16_t x, uint16_t y, uint16_t len);
void vdp_map_vline(uint16_t plan, const uint16_t *tiles, uint16_t x, uint16_t y, uint16_t len);
void vdp_map_fill_rect(uint16_t plan, uint16_t index, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t inc);
void vdp_map_clear(uint16_t plan);

// Palettes
void vdp_colors(uint16_t index, const uint16_t *values, uint16_t count);
void vdp_colors_bg(uint16_t index, const uint16_t *values, uint16_t count);

void vdp_color(uint16_t index, uint16_t color);
void vdp_colors_next(uint16_t index, const uint16_t *values, uint16_t count);
void vdp_color_next(uint16_t index, uint16_t color);
uint16_t vdp_fade_step_calc();
void vdp_fade_step_dma();
void vdp_fade(const uint16_t *src, const uint16_t *dst, uint16_t speed, uint8_t async);

// Scrolling
void vdp_hscroll(uint16_t plan, int16_t hscroll);
void vdp_hscroll_tile(uint16_t plan, int16_t *hscroll);
void vdp_vscroll(uint16_t plan, int16_t vscroll);

// Sprites
void vdp_sprite_add(const VDPSprite *spr);
void vdp_sprites_add(const VDPSprite *spr, uint16_t num);
void vdp_sprites_clear();
void vdp_sprites_update();
void vdp_load_stage_palettes();

// Text
void vdp_font_load(const uint32_t *tiles);
void vdp_font_pal(uint16_t pal);
void vdp_puts(uint16_t plan, const char *str, uint16_t x, uint16_t y);
void vdp_puts_shadow(uint16_t plan, const char *str, uint16_t x, uint16_t y);
void vdp_text_clear(uint16_t plan, uint16_t x, uint16_t y, uint16_t len);

uint16_t saturate_color(uint16_t color);