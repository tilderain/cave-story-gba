#pragma once
#include "gba.h"

// ---------------------------------------------------------------------------
// Canvas text system for GBA
//
// BG3 layout (tilemap space, 32x32 tiles = 256x256 pixels):
//
//   Columns 0..29  = textbox frame tiles  (screen-fixed, BG3HOFS=0 shows col 0)
//   Columns 30..61 = text canvas          (wide canvas, scrolled over frame via BG3HOFS)
//
//   The frame is drawn into the tilemap at fixed positions.
//   The canvas tiles live in VRAM and are pixel-blitted into directly.
//   BG3HOFS shifts the canvas columns into view over the frame area.
//   Since frame columns are to the LEFT and canvas to the RIGHT in tilemap
//   space, scrolling right reveals canvas. We negate: BG3HOFS = -canvasScrollX
//   (hardware wraps at 256px naturally for a 256px-wide map).
//
// VRAM tile layout for BG3 (char base must be free - configure as needed):
//   CANVAS_TILE_BASE   : first tile of the text canvas grid
//   CANVAS_TILES_W * CANVAS_TILES_H tiles reserved
//
// The textbox frame tiles are whatever your existing TILE_WINDOWINDEX tiles are.
// ---------------------------------------------------------------------------

// --- Tunables: adjust to match your VRAM layout ---

// Which tile index in VRAM does the canvas region start at?
// Must not overlap font, window, face, stage tiles etc.
// 3 rows * 32 cols = 96 tiles. Put this somewhere safe.
#define CANVAS_TILE_BASE    0

// Canvas dimensions in tiles
// Width should be >= screen tile width so text can scroll fully
// 3 rows matching your existing textRow 0..2 layout
#define CANVAS_TILES_W 26
#define CANVAS_TILES_H 6

// Canvas pixel dimensions
#define CANVAS_W            (CANVAS_TILES_W * 8)
#define CANVAS_H            (CANVAS_TILES_H * 8)

// BG3 tilemap screen base (must match your BGCTRL[3] SCREEN_BASE value)
#define BG3_MAP_BASE    ((volatile uint16_t*)0x0600E800)

// BG3 scroll registers
#define BG3HOFS             (*(volatile uint16_t*)0x04000016)
#define BG3VOFS             (*(volatile uint16_t*)0x04000018)

// Pixel position of canvas top-left within the tilemap (in pixels)
// Canvas columns start at tile 30 = pixel 240
#define CANVAS_MAP_PIXEL_X  240

// Pixel Y of the canvas top row within the tilemap
// This should match where TEXT_Y1 rows are in BG3 tilemap pixel coords
// e.g. TEXT_Y1 = 21 tiles * 8 = 168px for NTSC bottom window
#define CANVAS_MAP_PIXEL_Y  168    // adjust for top window if needed

// Text color index in BG3 palette (4bpp palette index 1..15)
#define TEXT_COLOR          1

// --- Font ---
// Tonc 1bpp font, 96 glyphs (ASCII 0x20..0x7F), 8 bytes per glyph
extern const unsigned int toncfontTiles[192];

// Per-glyph pixel advance widths (index 0 = ASCII 0x20 = space)
// Default fixed-width 8; override per-glyph for VWF
extern const uint8_t glyph_widths[96];

// --- API ---

// Call once at init (or when BG3 char base changes)
void canvas_init(void);

// Clear the entire canvas to transparent (color 0)
void canvas_clear(void);

// Draw a string into the canvas at pixel position (px, py) relative to canvas top-left
// Returns final X pixel position (useful for cursor placement)
int  canvas_puts(int px, int py, const char *str, uint8_t color);

// Draw a single glyph, return advance width
int  canvas_put_glyph(int px, int py, uint8_t ascii, uint8_t color);

// Set the hardware scroll so the canvas appears over the textbox frame
// call_x is in canvas pixels (0 = left edge of canvas visible)
void canvas_set_scroll(int canvas_x);

// Set up BG3 tilemap: draw frame tiles and point canvas region at CANVAS_TILE_BASE tiles
// Call this when window_open() is called (after your existing frame draw code)
void canvas_setup_tilemap(uint8_t on_top);

// Reset scroll to 0 (frame visible, canvas hidden)
void canvas_reset_scroll(void);

void canvas_scroll_up(void);

extern int textPixelX;