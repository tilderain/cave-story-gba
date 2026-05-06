#include "common.h"

#include "audio.h"
#include "dma.h"
#include "entity.h"
#include "gamemode.h"
#include "hud.h"
#include "joy.h"
#include "kanji.h"
#include "memory.h"
#include "player.h"
#include "resources.h"
#include "sheet.h"
#include "system.h"
#include "tables.h"
#include "tsc.h"
#include "vdp.h"
#include "xgm.h"

#include "window.h"

#include <stdio.h>

// Window location (GBA is 30 columns wide and 20 rows tall)
#define WINDOW_X1 1
#define WINDOW_X2 28                  // Leaves a 1-tile margin on the right (GBA column 28)
#define WINDOW_Y1 13                  // Placed at row 13 (so bottom box occupies rows 13-19)
#define WINDOW_Y2 19                  // Placed at row 19 (the very bottom of GBA screen)

// Text area location within window
#define TEXT_X1 (WINDOW_X1 + 1)       // Col 2
#define TEXT_X2 (WINDOW_X2 - 1)       // Col 27
#define TEXT_Y1 (WINDOW_Y1 + 1)       // Row 14 (where bottom text begins)
#define TEXT_Y2 (WINDOW_Y2 - 1)       // Row 18
#define TEXT_X1_FACE (WINDOW_X1 + 8)  // Col 9 (leaves room for face on GBA width)

// On top
#define WINDOW_Y1_TOP 0
#define WINDOW_Y2_TOP 6               // Top window occupies rows 0-6
#define TEXT_Y1_TOP (WINDOW_Y1_TOP + 1) // Row 1 (where top text begins)
#define TEXT_Y2_TOP (WINDOW_Y2_TOP - 1) // Row 5

// Prompt window location (Yes/No dialog)
#define PROMPT_X 18                   // Centered for GBA width
#define PROMPT_Y 11                   // Placed perfectly above the bottom window

#define ITEM_Y_START	(SCREEN_HALF_H + 128)
#define ITEM_Y_END		(SCREEN_HALF_H + 12 + 128)

static int s_scroll_debt = 0; // Tracks how many pixels we still need to shift up


const uint8_t ITEM_PAL[40] = {
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 1, 1, 0, 1, 1, 1,
	0, 1, 0, 1, 0, 0, 0, 0,
	1, 0, 0, 1, 0, 1, 1, 1,
};

uint8_t windowOpen = FALSE;
uint16_t showingFace = 0;

uint8_t textMode = TM_NORMAL;

uint8_t windowText[3][36];
uint8_t textRow, textColumn;
uint8_t windowTextTick = 0;
uint8_t spaceCounter = 0, spaceOffset = 0;

uint8_t promptAnswer = TRUE;
VDPSprite promptSpr[2];
VDPSprite handSpr, handSprR;

uint16_t showingItem = 0;

uint8_t blinkTime = 0;

uint8_t windowOnTop = 0;

#include "gbatext.h"

static int16_t faceXOffset = 0; // Current horizontal sliding offset

void window_clear_text();
void window_draw_face();

void window_open(uint8_t mode) {
    mapNameTTL = 0; // Hide map name to avoid tile conflict
    if(cfg_language >= LANG_JA && cfg_language <= LANG_KO) {
        const uint32_t *data = &TS_MsgFont.tiles[('_' - 0x20) << 3];
        vdp_tiles_load_from_rom(data, (0xB000 >> 5) + 3 + (29 << 2), 1);
    }
    textRow = textColumn = 0;
    
    windowOnTop = mode;
    hud_hide();

    uint16_t wy1 = mode ? WINDOW_Y1_TOP : WINDOW_Y1,
        wy2 = mode ? WINDOW_Y2_TOP : WINDOW_Y2,
        ty1 = mode ? TEXT_Y1_TOP : TEXT_Y1,
        ty2 = mode ? TEXT_Y2_TOP : TEXT_Y2;
    
    vdp_map_xy(VDP_PLAN_W, WINDOW_ATTR(0), WINDOW_X1, wy1);
    // Draw horizontal borders based on actual GBA window width (26 tiles)
    vdp_map_fill_rect(VDP_PLAN_W, WINDOW_ATTR(1), TEXT_X1, wy1, WINDOW_X2 - WINDOW_X1 - 1, 1, 0);
    vdp_map_xy(VDP_PLAN_W, WINDOW_ATTR(2), WINDOW_X2, wy1);
    for(uint8_t y = ty1; y <= ty2; y++) {
        vdp_map_xy(VDP_PLAN_W, 0, 0, y);
        vdp_map_xy(VDP_PLAN_W, WINDOW_ATTR(3), WINDOW_X1, y);
        vdp_map_xy(VDP_PLAN_W, WINDOW_ATTR(5), WINDOW_X2, y);
        vdp_map_xy(VDP_PLAN_W, 0, 29, y); // Row width is 30 columns (index 29 is the rightmost)
    }
    vdp_map_xy(VDP_PLAN_W, WINDOW_ATTR(6), WINDOW_X1, wy2);
    vdp_map_fill_rect(VDP_PLAN_W, WINDOW_ATTR(7), TEXT_X1, wy2, WINDOW_X2 - WINDOW_X1 - 1, 1, 0);
    vdp_map_xy(VDP_PLAN_W, WINDOW_ATTR(8), WINDOW_X2, wy2);

    window_clear();
    
    if(!paused) {
        if(showingFace > 0) window_draw_face();
        vdp_set_window(0, mode ? 8 : 104); // Sets vertical GBA window split (104px = Row 13)
    } else showingFace = 0;

    if(textMode == TM_MSG) textMode = TM_NORMAL;

    windowOpen = TRUE;

    canvas_setup_tilemap(mode);  // clears canvas, resets scroll, fixes tilemap
}


uint8_t window_is_open() {
	return windowOpen;
}

void window_clear() {
    iprintf("\x1b[2J");
    uint8_t x = showingFace ? TEXT_X1_FACE : TEXT_X1;
    uint8_t y = windowOnTop ? TEXT_Y1_TOP : TEXT_Y1;
    uint8_t w = showingFace ? 19 : 26; // GBA friendly text widths

    disable_ints;
    z80_request();
    vdp_map_fill_rect(VDP_PLAN_W, WINDOW_ATTR(4), x, y, w, 6, 0);
    z80_release();
    enable_ints;

    window_clear_text();
}


void window_clear_text() {
    textRow = textColumn = spaceCounter = spaceOffset = 0;
    
    // Reset X position!
    textPixelX = showingFace ? 56 : 0; 
    s_scroll_debt = 0; // Reset scroll debt

    memset(windowText, ' ', 36*3);
    cjk_reset(CJK_MESSAGE);
    
    // Wipe the canvas graphics in VRAM
    canvas_clear(); 
}

void window_close() {
	if(!paused) {
	    vdp_set_window(0, 0);
        hud_force_redraw();
	}
	showingItem = 0;
	windowOpen = FALSE;
    //if(textMode == TM_MSG) textMode = TM_NORMAL;
	canvas_clear();
}

#include "vdp.h"

void window_set_face(uint16_t face, uint8_t open) {
    if(paused) return;
    if(open && !windowOpen) window_open(windowOnTop);
    
    // Only reset the slide if the face is actually changing
    if(showingFace != face) {
        showingFace = face;
        if(face > 0) {
            faceXOffset = -48; // Start 12 pixels to the left (Original CS style)
        }
    }
    
    if(face > 0) {
        vdp_tiles_load_from_rom(face_info[face].tiles->tiles, TILE_FACEINDEX, 36);
        textPixelX = 56; 
    } else {
        textPixelX = 0;
    }
}
int textPixelX = 0;

void window_draw_char(uint8_t c) {
    if (c == '\n') {
        textRow++;
        if (textRow > 2) {
            // Trigger a 16-pixel scroll animation
            s_scroll_debt += 16; 
            
            textRow = 2; // Stay on the bottom row
            textColumn = 0;
            spaceCounter = spaceOffset = 0;
            textPixelX = showingFace ? 56 : 0;
        } else {
            textColumn = 0;
            spaceCounter = spaceOffset = 0;
            textPixelX = showingFace ? 56 : 0; 
        }
    } else {
        // THE FIX: Offset the Y position by the remaining scroll debt
        // If we are at Row 2 (Y=32) and debt is 16, we draw at Y=48.
        // On the next frame, debt is 14 and the buffer shifted up by 2,
        // so the new character drawn at 32+14=46 aligns perfectly with 
        // the previous one which moved from 48 to 46.
        int py = (textRow * 16) + s_scroll_debt; 
        
        textPixelX += canvas_put_glyph(textPixelX, py, c, 1);
        textColumn++;
    }
}


void window_draw_jchar(uint8_t iskanji, uint16_t c) {
	if(!iskanji && c == '\n') {
        //vdp_map_xy(VDP_PLAN_W, WINDOW_ATTR(4), x, y);
        //vdp_map_xy(VDP_PLAN_W, WINDOW_ATTR(4), x, y+1);
		textRow++;
		textColumn = 0;
        cjk_newline();
		if(textRow > 2) {
            uint16_t msgTextX = showingFace ? TEXT_X1_FACE : TEXT_X1;
            uint16_t msgTextY = windowOnTop ? TEXT_Y1_TOP : TEXT_Y1;
            uint16_t msgTextW = showingFace ? 29 : 36;
            cjk_winscroll(msgTextX, msgTextY);
            vdp_map_fill_rect(VDP_PLAN_W, WINDOW_ATTR(4), msgTextX, msgTextY+4, msgTextW, 2, 0);
            textRow = 2;
		}
		return;
	}
	if(iskanji) c += 0x100;
	// Figure out where this char is gonna go
	uint8_t msgTextX = showingFace ? TEXT_X1_FACE : TEXT_X1;
	msgTextX += textColumn + (textColumn >> 1); // * 1.5
	uint8_t msgTextY = (windowOnTop ? TEXT_Y1_TOP : TEXT_Y1) + textRow * 2;
	// And draw it
    cjk_draw(VDP_PLAN_W, c, msgTextX, msgTextY, 2, FALSE);
    textColumn++;
}

void window_scroll_text() {
    // Push bottom 2 rows to top
    for(uint8_t row = 0; row < 2; row++) {
        if(vblank) aftervsync();
        vblank = 0;
        
        uint8_t msgTextX = showingFace ? TEXT_X1_FACE : TEXT_X1;
        uint8_t msgTextY = (windowOnTop ? TEXT_Y1_TOP:TEXT_Y1) + row * 2;
        for(uint8_t col = 0; col < 26 - (showingFace > 0) * 7; col++) { // GBA bounds
            windowText[row][col] = windowText[row + 1][col];
            uint8_t c = windowText[row][col];
            if(c >= 0x80) {
                uint16_t index = (VDP_PLAN_W >> 5) + 3;
                index += (c - 0x80) << 2;
                vdp_map_xy(VDP_PLAN_W, TILE_ATTR(PAL0, 1, 0, 0, index-4), msgTextX, msgTextY);
            } else {
                vdp_map_xy(VDP_PLAN_W, TILE_ATTR(PAL0, 1, 0, 0,
                        TILE_FONTINDEX + c - 0x20), msgTextX, msgTextY);
            }
            msgTextX++;
        }
    }
    // Clear third row
    uint8_t msgTextX = showingFace ? TEXT_X1_FACE : TEXT_X1;
    uint8_t msgTextY = (windowOnTop ? TEXT_Y1_TOP:TEXT_Y1) + 4;
    uint8_t msgTextW = showingFace ? 19 : 26; // GBA bounds
    memset(windowText[2], ' ', 36);
    vdp_map_fill_rect(VDP_PLAN_W, WINDOW_ATTR(4), msgTextX, msgTextY, msgTextW, 1, 0);
    // Reset to beginning of third row
    textRow = 2;
    textColumn = 0;
    spaceCounter = spaceOffset = 0;

    // Reset X position for the new line!
    textPixelX = showingFace ? 56 : 0;

    // Scroll the actual canvas graphics up!
    canvas_scroll_up();
}
uint8_t window_get_textmode() {
	return textMode;
}

void window_set_textmode(uint8_t mode) {
	textMode = mode;
}

uint8_t window_tick() {
	if(textMode > 0) return TRUE;
	windowTextTick++;
	if(windowTextTick > 2 || (windowTextTick > 1 && (joystate&btn[cfg_btn_jump]))) {
		windowTextTick = 0;
		return TRUE;
	} else {
		return FALSE;
	}
}

void window_prompt_open() {
	promptAnswer = TRUE; // Yes is default
	sound_play(SND_MENU_PROMPT, 5);
	// Load hand sprite and move next to yes
	handSpr = (VDPSprite) {
		//.x = tile_to_pixel(PROMPT_X) - 4 + 128,
		//.y = tile_to_pixel(PROMPT_Y + 1) - 4 + 128,
		.size = SPRITE_SIZE(2, 2),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_PROMPTINDEX)
	};
	int16_t cursor_x = (PROMPT_X << 3) - 9;
	if(!promptAnswer) cursor_x += cfg_language == LANG_JA ? 26 : 34; // "いいえ" starts more left than "No"
	int16_t cursor_y = (PROMPT_Y << 3) + 4;
	sprite_pos(handSpr, cursor_x, cursor_y);
	promptSpr[0] = (VDPSprite) {
		.x = tile_to_pixel(PROMPT_X) + 128,
		.y = tile_to_pixel(PROMPT_Y) + 128,
		.size = SPRITE_SIZE(4, 3),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_PROMPTINDEX+4)
	};
	promptSpr[1] = (VDPSprite) {
		.x = tile_to_pixel(PROMPT_X) + 32 + 128,
		.y = tile_to_pixel(PROMPT_Y) + 128,
		.size = SPRITE_SIZE(4, 3),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_PROMPTINDEX+16)
	};
	TILES_QUEUE(SPR_TILES(&SPR_Pointer,0,0), TILE_PROMPTINDEX, 4);
	const SpriteDefinition *spr = cfg_language == LANG_JA ? &SPR_J_Prompt : &SPR_Prompt;
	TILES_QUEUE(SPR_TILES(spr,0,0), TILE_PROMPTINDEX+4, 24);
}

void window_prompt_close() {
	window_clear();
}

uint8_t window_prompt_answer() {
	return promptAnswer;
}

uint8_t window_prompt_update() {
	if(joy_pressed(btn[cfg_btn_jump])) {
		sound_play(SND_MENU_SELECT, 5);
		window_prompt_close();
		return TRUE;
	} else if(joy_pressed(BUTTON_LEFT) || joy_pressed(BUTTON_RIGHT)) {
		promptAnswer = !promptAnswer;
		sound_play(SND_MENU_MOVE, 5);
		int16_t cursor_x = (PROMPT_X << 3) - 9;
		if(!promptAnswer) cursor_x += cfg_language == LANG_JA ? 26 : 34; // "いいえ" starts more left than "No"
		int16_t cursor_y = (PROMPT_Y << 3) + 4;
		sprite_pos(handSpr, cursor_x, cursor_y);
	}
    vdp_sprite_add(&handSpr);
    vdp_sprites_add(promptSpr, 2);
	return FALSE;
}

void window_draw_face() {
    // 1. Get current face definition
    const face_info_def *f = &face_info[showingFace];
    
    // Safety check: Don't try to draw if there are no tiles or palette data
    if (f->tiles == NULL || f->pal_ptr == NULL) return;

    // 2. Load the 16 colors for this face into GBA Palette Line 4
    vdp_colors(64, f->pal_ptr, 16);

    // 3. Load the 36 tiles (6x6) into VRAM
    vdp_tiles_load_from_rom(f->tiles->tiles, TILE_FACEINDEX, 36);

    // 4. Draw the 6x6 face into the window plane
    // We pass '4' to TILE_ATTR so the hardware knows to use the colors we just loaded
    vdp_map_fill_rect(VDP_PLAN_W, 
        TILE_ATTR(4, 1, 0, 0, TILE_FACEINDEX), 
        TEXT_X1, (windowOnTop ? TEXT_Y1_TOP : TEXT_Y1), 6, 6, 1);
}

void window_show_item(uint16_t item) {
	showingItem = item;
	if(item == 0) return;
	// Wonky workaround to use either PAL_Sym or PAL_Main
	const SpriteDefinition *sprDef = &SPR_ItemImage;
	uint16_t pal = PAL1;
	if(ITEM_PAL[item]) {
		sprDef = &SPR_ItemImage;
		pal = PAL0;
	}
    int pal_line = 6;
    handSpr = (VDPSprite) {
        .x = SCREEN_HALF_W - 12 + 128,
        .y = ITEM_Y_START,
        .size = SPRITE_SIZE(2, 2) | (pal_line << 4),
        .attr = TILE_ATTR(0,1,0,0,TILE_PROMPTINDEX)
    };
    // Right side (8x16)
    handSprR = (VDPSprite) {
        .x = SCREEN_HALF_W - 12 + 16 + 128,
        .y = ITEM_Y_START,
        .size = SPRITE_SIZE(1, 2) | (pal_line << 4),
        .attr = TILE_ATTR(0,1,0,0,TILE_PROMPTINDEX + 4) // Offset tile by 4
    };
	promptSpr[0] = (VDPSprite) {
		.x = SCREEN_HALF_W - 24 + 128,
		.y = SCREEN_HALF_H + 8 + 128,
		.size = SPRITE_SIZE(3, 3),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_PROMPTINDEX+6)
	};
	promptSpr[1] = (VDPSprite) {
		.x = SCREEN_HALF_W + 128,
		.y = SCREEN_HALF_H + 8 + 128,
		.size = SPRITE_SIZE(3, 3),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_PROMPTINDEX+15)
	};
	TILES_QUEUE(SPR_TILES(sprDef,item,0), TILE_PROMPTINDEX, 6);
	TILES_QUEUE(SPR_TILES(&SPR_ItemWin,0,0), TILE_PROMPTINDEX+6, 18);
}

void window_show_weapon(uint16_t item) {
	showingItem = item;
	if(item == 0) return;
	handSpr = (VDPSprite) {
		.x = SCREEN_HALF_W - 8 + 128,
		.y = ITEM_Y_START,
		.size = SPRITE_SIZE(2, 2),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_PROMPTINDEX)
	};
	promptSpr[0] = (VDPSprite) {
		.x = SCREEN_HALF_W - 24 + 128,
		.y = SCREEN_HALF_H + 8 + 128,
		.size = SPRITE_SIZE(3, 3),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_PROMPTINDEX+6)
	};
	promptSpr[1] = (VDPSprite) {
		.x = SCREEN_HALF_W + 128,
		.y = SCREEN_HALF_H + 8 + 128,
		.size = SPRITE_SIZE(3, 3),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_PROMPTINDEX+15)
	};
	TILES_QUEUE(SPR_TILES(&SPR_ArmsImage,0,item), TILE_PROMPTINDEX, 6);
	TILES_QUEUE(SPR_TILES(&SPR_ItemWin,0,0), TILE_PROMPTINDEX+6, 18);
}

// Modify window_update to perform the shift if there is debt
void window_update() {
    // Handle the scrolling animation independently of the script state
    if (s_scroll_debt > 0) {
        canvas_shift_pixels_up_2(); // Shift 2 pixels per frame
        s_scroll_debt -= 2;
    }

    if(showingItem) {
        if(handSpr.y < ITEM_Y_END) {
            handSpr.y++;
            handSprR.y++;
        }
        if(handSprR.y) vdp_sprite_add(&handSprR); 
        vdp_sprite_add(&handSpr);
        vdp_sprites_add(promptSpr, 2);
    }


    // 3. Handle Animated Face Slide
    
    if(windowOpen && showingFace > 0) {
        static int16_t last_drawn_offset = -999;
        static uint16_t last_drawn_face = 0;

        const face_info_def *f = &face_info[showingFace];

        // 2. Load the 16 colors for this face into GBA Palette Line 4
        vdp_colors(64, f->pal_ptr, 16);

        // Force a VRAM update when the face graphic changes
        if (showingFace != last_drawn_face) {
            last_drawn_face = showingFace;
            last_drawn_offset = -999; 
        }

        // Increment the slide offset until it hits 0
        if(faceXOffset < 0) faceXOffset += 6; 

        // If the slide offset changed, we shift the pixels in VRAM
        if(last_drawn_offset != faceXOffset) {
            last_drawn_offset = faceXOffset;
            
            // Calculate how many pixels to cut off from the left
            int x_offset = -faceXOffset;
            const uint32_t *src = face_info[showingFace].tiles->tiles;
            
            // Pointer directly to the Face Tiles in Sprite VRAM
            uint32_t *dst = (uint32_t *)(0x06010000 + TILE_FACEINDEX * 32);

            int bit_shift = x_offset * 4; // 4 bits per pixel (4bpp)
            int word_shift = bit_shift / 32; 
            int bit_rem = bit_shift % 32;    

            // Shift the 48x48 image leftward in VRAM
            for(int ty = 0; ty < 6; ty++) { 
                for(int py = 0; py < 8; py++) { 
                    uint32_t row_words[6];
                    for(int tx = 0; tx < 6; tx++) {
                        row_words[tx] = src[(ty * 6 + tx) * 8 + py];
                    }

                    uint32_t out_words[6] = {0};
                    
                    if(bit_rem == 0) {
                        for(int tx = 0; tx < 6 - word_shift; tx++) {
                            out_words[tx] = row_words[tx + word_shift];
                        }
                    } else {
                        // Bitwise shift across the 32-bit boundaries
                        for(int tx = 0; tx < 6 - word_shift; tx++) {
                            uint32_t curr = row_words[tx + word_shift];
                            uint32_t next = (tx + word_shift + 1 < 6) ? row_words[tx + word_shift + 1] : 0;
                            out_words[tx] = (curr >> bit_rem) | (next << (32 - bit_rem));
                        }
                    }

                    // Write the shifted row to VRAM
                    for(int tx = 0; tx < 6; tx++) {
                        dst[(ty * 6 + tx) * 8 + py] = out_words[tx];
                    }
                }
            }
        }

        // Resting X: (WINDOW_X1 * 8) + 4 = 12
        // Notice we DO NOT add faceXOffset here anymore! 
        // The sprite stays anchored while the VRAM shift creates the sliding mask effect.
        int16_t fx = (WINDOW_X1 * 8) + 4 + 128;
        int16_t fy = (windowOnTop ? (WINDOW_Y1_TOP * 8) : (WINDOW_Y1 * 8)) + 4 + 128;

        uint16_t tileOffset = TILE_FACEINDEX;

        // Draw the 48x48 face as 6 horizontal strips (32x8 + 16x8)
        for (int i = 0; i < 6; i++) {
            VDPSprite faceLeft = {
                .x = fx,
                .y = fy + (i * 8),
                .size = SPRITE_SIZE(4, 1)  | (4 << 4),
                .attr = TILE_ATTR(3, 1, 0, 0, tileOffset)
            };
            vdp_sprite_add(&faceLeft);
            tileOffset += 4;

            VDPSprite faceRight = {
                .x = fx + 32,
                .y = fy + (i * 8),
                .size = SPRITE_SIZE(2, 1)  | (4 << 4),
                .attr = TILE_ATTR(3, 1, 0, 0, tileOffset)
            };
            vdp_sprite_add(&faceRight);
            tileOffset += 2;
        }
    }
	if(tscState == TSC_WAITINPUT && textMode == TM_NORMAL) {
		uint8_t x = showingFace ? TEXT_X1_FACE : TEXT_X1;
		uint8_t y = windowOnTop ? TEXT_Y1_TOP : TEXT_Y1;
		uint16_t index;
		if(cfg_language >= LANG_JA && cfg_language <= LANG_KO) {
			x += textColumn + ((textColumn + 1) >> 1);
			y += textRow * 2;
			index = (0xB000 >> 5) + 3 + (29 << 2);
		} else {
			x += textColumn - spaceOffset;
			y += textRow * 2;
			index = TILE_FONTINDEX - 0x20 + '_';
		}
        if(++blinkTime == 8) {
            vdp_map_xy(VDP_PLAN_W, TILE_ATTR(PAL0,1,0,0,index), x, y);
            if(cfg_language >= LANG_JA && cfg_language <= LANG_KO) {
                vdp_map_xy(VDP_PLAN_W, TILE_ATTR(PAL0,1,0,0,index), x, y + 1);
            }
        } else if(blinkTime == 16) {
            vdp_map_xy(VDP_PLAN_W, WINDOW_ATTR(4), x, y);
            if(cfg_language >= LANG_JA && cfg_language <= LANG_KO) {
                vdp_map_xy(VDP_PLAN_W, WINDOW_ATTR(4), x, y + 1);
            }
            blinkTime = 0;
        }
	}
}
