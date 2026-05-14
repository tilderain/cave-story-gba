#include "common.h"

#include "camera.h"
#include "dma.h"
#include "entity.h"
#include "memory.h"
#include "player.h"
#include "resources.h"
#include "sheet.h"
#include "tables.h"
#include "tools.h"
#include "vdp.h"

#include "effect.h"

#include "gba.h"
#include "hud.h"
#include "gbatext.h"

#include "stage.h"
typedef struct {
	VDPSprite sprite;
	uint8_t type, ttl, timer, timer2;
	int16_t x, y;
	int8_t x_speed, y_speed;
	uint8_t digit_count;
} Effect;

static Effect effDamage[MAX_DAMAGE], effSmoke[MAX_SMOKE], effMisc[MAX_MISC];
static struct {
	Entity *e;
	int16_t xoff, yoff;
} damageFollow[MAX_DAMAGE];

uint8_t dqueued = 0;

// Create a memory buffer of 4 tiles containing a string like "+3" or "-127"
// Then copy to VRAM via DMA transfer
uint32_t dtiles[MAX_DAMAGE][4][8];

// === Pure BG3 Fade System (CSE2-compatible) ===

int8_t fadeSweepTimer;

// BG3 tilemap & tile 0 save buffer for save/restore
static uint16_t bg3_map_save[1024];
static uint32_t bg3_tile0_save[8];

// Fade grid: 15x10 cells, each 16x16 pixels
#define FADE_GRID_W      15
#define FADE_GRID_H      10
#define FADE_FRAMES      16
#define FADE_TILE_BASE   640   // 64 tiles in BG3 charbase 2

typedef struct {
    int mode;       // 0=none, 1=fadein, 2=fadeout
    int bMask;      // 1=full mask active
    int count;      // sweep line position
    int dir;        // 0=right, 2=left, 1=bottom, 3=top, 4=diamond
    int8_t ani_no[FADE_GRID_H][FADE_GRID_W];
    int8_t flag[FADE_GRID_H][FADE_GRID_W];
} FadeState;
static FadeState gFade;

// Load 16 frames of 16x16 fade tiles from SPR_Fade into BG3 charbase 2
// Each frame = 2x2 BG tiles = 4 tiles
// Palette: palette bank 3, index 1 = covered (black)
EWRAM_CODE static void fade_generate_tiles(void) {
    for (int f = 0; f < FADE_FRAMES; f++) {
        vdp_tiles_load(
            SPR_TILES(&SPR_Fade, 0, f),
            1024 + FADE_TILE_BASE + f * 4,
            4
        );
    }
}

// Map all fade cells to the BG3 tilemap
EWRAM_CODE static void put_fade_bg3(void) {
    volatile uint16_t* map = (volatile uint16_t*)0x0600E800;
    for (int y = 0; y < FADE_GRID_H; y++) {
        for (int x = 0; x < FADE_GRID_W; x++) {
            int an = gFade.ani_no[y][x];
            uint16_t base = FADE_TILE_BASE + an * 4;
            int my = y * 2, mx = x * 2;
            map[my * 32 + mx]       = base | CHAR_PALETTE(3);
            map[my * 32 + mx + 1]   = (base + 1) | CHAR_PALETTE(3);
            map[(my + 1) * 32 + mx] = (base + 2) | CHAR_PALETTE(3);
            map[(my + 1) * 32 + mx + 1] = (base + 3) | CHAR_PALETTE(3);
        }
    }
}

// CSE2 ProcFade: advance sweep line, update cell animation frames
// Returns 1 when the fade transition is complete
EWRAM_CODE static int proc_fade(void) {
    if (gFade.mode == 0) return 1;

    int x, y;

    switch (gFade.mode) {
        case 2: // Fade out
            switch (gFade.dir) {
                case 0:
                    for (y = 0; y < FADE_GRID_H; ++y)
                        for (x = 0; x < FADE_GRID_W; ++x)
                            if ((FADE_GRID_W - 1) - gFade.count == x)
                                gFade.flag[y][x] = 1;
                    break;
                case 2:
                    for (y = 0; y < FADE_GRID_H; ++y)
                        for (x = 0; x < FADE_GRID_W; ++x)
                            if (gFade.count == x)
                                gFade.flag[y][x] = 1;
                    break;
                case 1:
                    for (y = 0; y < FADE_GRID_H; ++y)
                        for (x = 0; x < FADE_GRID_W; ++x)
                            if ((FADE_GRID_H - 1) - gFade.count == y)
                                gFade.flag[y][x] = 1;
                    break;
                case 3:
                    for (y = 0; y < FADE_GRID_H; ++y)
                        for (x = 0; x < FADE_GRID_W; ++x)
                            if (gFade.count == y)
                                gFade.flag[y][x] = 1;
                    break;
                case 4:
                    for (y = 0; y < (FADE_GRID_H / 2); ++y)
                        for (x = 0; x < (FADE_GRID_W / 2); ++x)
                            if (gFade.count == x + y)
                                gFade.flag[y][x] = 1;

                    for (y = 0; y < (FADE_GRID_H / 2); ++y)
                        for (x = (FADE_GRID_W / 2); x < FADE_GRID_W; ++x)
                            if (gFade.count == y + ((FADE_GRID_W - 1) - x))
                                gFade.flag[y][x] = 1;

                    for (y = (FADE_GRID_H / 2); y < FADE_GRID_H; ++y)
                        for (x = 0; x < (FADE_GRID_W / 2); ++x)
                            if (gFade.count == x + ((FADE_GRID_H - 1) - y))
                                gFade.flag[y][x] = 1;

                    for (y = (FADE_GRID_H / 2); y < FADE_GRID_H; ++y)
                        for (x = (FADE_GRID_W / 2); x < FADE_GRID_W; ++x)
                            if (gFade.count == ((FADE_GRID_W - 1) - x) + ((FADE_GRID_H - 1) - y))
                                gFade.flag[y][x] = 1;
                    break;
            }

            for (y = 0; y < FADE_GRID_H; ++y)
                for (x = 0; x < FADE_GRID_W; ++x)
                    if (gFade.ani_no[y][x] < 15 && gFade.flag[y][x])
                        ++gFade.ani_no[y][x];

            if (++gFade.count > ((FADE_GRID_W > FADE_GRID_H) ? FADE_GRID_W : FADE_GRID_H) + 16) {
                gFade.bMask = 1;
                gFade.mode = 0;
            }
            break;

        case 1: // Fade in
            gFade.bMask = 0;

            switch (gFade.dir) {
                case 0:
                    for (y = 0; y < FADE_GRID_H; ++y)
                        for (x = 0; x < FADE_GRID_W; ++x)
                            if ((FADE_GRID_W - 1) - gFade.count == x)
                                gFade.flag[y][x] = 1;
                    break;
                case 2:
                    for (y = 0; y < FADE_GRID_H; ++y)
                        for (x = 0; x < FADE_GRID_W; ++x)
                            if (gFade.count == x)
                                gFade.flag[y][x] = 1;
                    break;
                case 1:
                    for (y = 0; y < FADE_GRID_H; ++y)
                        for (x = 0; x < FADE_GRID_W; ++x)
                            if ((FADE_GRID_H - 1) - gFade.count == y)
                                gFade.flag[y][x] = 1;
                    break;
                case 3:
                    for (y = 0; y < FADE_GRID_H; ++y)
                        for (x = 0; x < FADE_GRID_W; ++x)
                            if (gFade.count == y)
                                gFade.flag[y][x] = 1;
                    break;
                case 4:
                    for (y = 0; y < (FADE_GRID_H / 2); ++y)
                        for (x = 0; x < (FADE_GRID_W / 2); ++x)
                            if ((FADE_GRID_W - 1) - gFade.count == x + y)
                                gFade.flag[y][x] = 1;

                    for (y = 0; y < (FADE_GRID_H / 2); ++y)
                        for (x = (FADE_GRID_W / 2); x < FADE_GRID_W; ++x)
                            if ((FADE_GRID_W - 1) - gFade.count == y + ((FADE_GRID_W - 1) - x))
                                gFade.flag[y][x] = 1;

                    for (y = (FADE_GRID_H / 2); y < FADE_GRID_H; ++y)
                        for (x = 0; x < (FADE_GRID_W / 2); ++x)
                            if ((FADE_GRID_W - 1) - gFade.count == x + ((FADE_GRID_H - 1) - y))
                                gFade.flag[y][x] = 1;

                    for (y = (FADE_GRID_H / 2); y < FADE_GRID_H; ++y)
                        for (x = (FADE_GRID_W / 2); x < FADE_GRID_W; ++x)
                            if ((FADE_GRID_W - 1) - gFade.count == ((FADE_GRID_W - 1) - x) + ((FADE_GRID_H - 1) - y))
                                gFade.flag[y][x] = 1;
                    break;
            }

            for (y = 0; y < FADE_GRID_H; ++y)
                for (x = 0; x < FADE_GRID_W; ++x)
                    if (gFade.ani_no[y][x] > 0 && gFade.flag[y][x])
                        --gFade.ani_no[y][x];

            if (++gFade.count > ((FADE_GRID_W > FADE_GRID_H) ? FADE_GRID_W : FADE_GRID_H) + 16)
                gFade.mode = 0;
            break;
    }

    return (gFade.mode == 0) ? 1 : 0;
}

// Blocking fade out (with palette fade at end)
EWRAM_CODE void do_fadeout_sweep(uint8_t dir) {
    // Save BG3 state
    volatile uint16_t* map = (volatile uint16_t*)0x0600E800;
    for (int i = 0; i < 1024; i++) bg3_map_save[i] = map[i];
    volatile uint32_t* tile0 = (volatile uint32_t*)0x06008000;
    for (int i = 0; i < 8; i++) bg3_tile0_save[i] = tile0[i];

    // Generate tiles and set palette (using palette bank 3, NOT overwritten by vdp_sprites_update)
    fade_generate_tiles();
    BG_COLORS[49] = 0x0000; // Palette bank 3, index 1 = black for fade pixels

    // Init fade-out state
    memset(&gFade, 0, sizeof(gFade));
    gFade.mode = 2;
    gFade.dir = dir;
    gFade.bMask = 0;

    // Run animation
    for (;;) {
        if (proc_fade()) break;
        put_fade_bg3();

        player_draw();
        entities_draw();
		stage_update();
        vdp_sprites_update();

        vdp_vsync();
        ready = TRUE;
        aftervsync();
    }

    // Palette fade to full black
    vdp_colors(0, PAL_FadeOut, 64);
    ready = TRUE;
    aftervsync();

    // Restore BG3 state
    for (int i = 0; i < 1024; i++) map[i] = bg3_map_save[i];
    for (int i = 0; i < 8; i++) tile0[i] = bg3_tile0_save[i];

    gFade.mode = 0;
}

// Non-blocking fade in (timer-driven from game loop)
EWRAM_CODE void start_fadein_sweep(uint8_t dir) {
    // Save BG3 state
    volatile uint16_t* map = (volatile uint16_t*)0x0600E800;
    for (int i = 0; i < 1024; i++) bg3_map_save[i] = map[i];
    volatile uint32_t* tile0 = (volatile uint32_t*)0x06008000;
    for (int i = 0; i < 8; i++) bg3_tile0_save[i] = tile0[i];

    // Generate tiles and set palette
    fade_generate_tiles();
    BG_COLORS[49] = 0x0000; // Palette bank 3, index 1 = black for fade

    // Init fade-in state: all cells start at max (fully black)
    memset(&gFade, 0, sizeof(gFade));
    gFade.mode = 1;
    gFade.dir = dir;
    gFade.bMask = 1;
    for (int y = 0; y < FADE_GRID_H; y++)
        for (int x = 0; x < FADE_GRID_W; x++)
            gFade.ani_no[y][x] = FADE_FRAMES - 1;

    // Render initial full-black frame
    put_fade_bg3();

    fadeSweepTimer = 1; // Mark as active
}

// Per-frame step for non-blocking fade-in
EWRAM_CODE void update_fadein_sweep(void) {
    if (gFade.mode != 0) {
        proc_fade();
        put_fade_bg3();
        
        if (gFade.mode == 0) {
            fadeSweepTimer = -1; // Fade finished!
            // Fade complete: restore BG3 state
            volatile uint16_t* map = (volatile uint16_t*)0x0600E800;
            for (int i = 0; i < 1024; i++) map[i] = bg3_map_save[i];
            volatile uint32_t* tile0 = (volatile uint32_t*)0x06008000;
            for (int i = 0; i < 8; i++) tile0[i] = bg3_tile0_save[i];
            
            hud_force_redraw();
        }
    }
}
void effects_init() {
	for(uint8_t i = 0; i < MAX_DAMAGE; i++) effDamage[i].ttl = 0;
	for(uint8_t i = 0; i < MAX_SMOKE; i++) effSmoke[i].ttl = 0;
	for(uint8_t i = 0; i < MAX_MISC; i++) effMisc[i].ttl = 0;
	
	// Load each frame of the small smoke sprite directly to VRAM
	for(uint8_t i = 0; i < 7; i++) {
		vdp_tiles_load_from_rom(SPR_TILES(&SPR_Smoke, 0, i), TILE_SMOKEINDEX + i * 4, 4);
	}
	
	// Load dissipation effect tiles (4 frames, 4 tiles each)
	for(uint8_t i = 0; i < 4; i++) {
		vdp_tiles_load_from_rom(SPR_TILES(&SPR_Dissipate, 0, i), TILE_DISSIPINDEX + i * 4, 4);
	}
	
	// Load gib effect tiles (4 frames, 1 tiles each)
	for(uint8_t i = 0; i < 4; i++) {
		vdp_tiles_load_from_rom(SPR_TILES(&SPR_Gib, 0, i), TILE_GIBINDEX + i, 1);
	}
	
	// Load boomflash effect tiles (2 frames, 16 tiles each)
	for(uint8_t i = 0; i < 2; i++) {
		vdp_tiles_load_from_rom(SPR_TILES(&SPR_Boomflash, 0, i), TILE_BOOMINDEX + i * 16, 16);
	}

	// Load caret hit tiles (4 frames, 4 tiles each)
	for(uint8_t i = 0; i < 4; i++) {
		vdp_tiles_load_from_rom(SPR_TILES(&SPR_CaretHit, 0, i), TILE_HITINDEX + i * 4, 4);
	}

	// Load caret shoot tiles (4 frames, 4 tiles each)
	for(uint8_t i = 0; i < 4; i++) {
		vdp_tiles_load_from_rom(SPR_TILES(&SPR_CaretShoot, 0, i), TILE_SHOOTINDEX + i * 4, 4);
	}
}
void effects_clear() {
	for(uint8_t i = 0; i < MAX_DAMAGE; i++) effDamage[i].ttl = 0;
	for(uint8_t i = 0; i < MAX_MISC; i++) effMisc[i].ttl = 0;
	effects_clear_smoke();
}

void effects_clear_smoke() {
	for(uint8_t i = 0; i < MAX_SMOKE; i++) effSmoke[i].ttl = 0;
}

EWRAM_CODE void effects_update() {
	for(uint8_t i = 0; i < MAX_DAMAGE; i++) {
    	if(!effDamage[i].ttl) continue;
    	effDamage[i].ttl--;
    	if(effDamage[i].ttl < 8) {
    	    uint8_t start = 3 - effDamage[i].digit_count;
    	    uint8_t tcount = effDamage[i].digit_count + 1;
    	    // GBA has no 24×8 sprite mode — data was shifted left and padded to 4
    	    if (tcount == 3) { start = 0; tcount = 4; }
    	    for(uint8_t x = start; x < 4; x++) {
    	        for(uint8_t y = 0; y < 7; y++) {
    	            dtiles[i][x][y] = dtiles[i][x][y + 1];
    	        }
    	        dtiles[i][x][7] = 0;
    	    }
    			DMA_doDma(DMA_VRAM,
    			          (uint32_t) dtiles[i][start],
    			          (TILE_NUMBERINDEX + (i << 2)) * 16,
    			          tcount * 8,
    			          2);
    	} else {
    	        if(damageFollow[i].e) {
    	            // Only move upward while ttl > 30
    	            if(effDamage[i].ttl > 30) damageFollow[i].yoff -= effDamage[i].ttl & 1;
	
    	            effDamage[i].x = (damageFollow[i].e->x >> CSF) + (damageFollow[i].xoff - 8);
    	            effDamage[i].y = (damageFollow[i].e->y >> CSF) + (damageFollow[i].yoff - 8);
    	        } else {
    	            // Only move upward while ttl > 30
    	            if(effDamage[i].ttl > 30) effDamage[i].y -= effDamage[i].ttl & 1;
    	        }
    	    }
    	sprite_pos(effDamage[i].sprite,
    	    effDamage[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W,
    	    effDamage[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H);
    	vdp_sprite_add(&effDamage[i].sprite);
	}
	for(uint8_t i = 0; i < MAX_SMOKE; i++) {
		if(!effSmoke[i].ttl) continue;
		effSmoke[i].ttl--;
		// CSE2 ActNpc004 drag: xm = xm * 20 / 21
		effSmoke[i].x_speed = (effSmoke[i].x_speed * 20) / 21;
		effSmoke[i].y_speed = (effSmoke[i].y_speed * 20) / 21;
		effSmoke[i].x += effSmoke[i].x_speed;
		effSmoke[i].y += effSmoke[i].y_speed;
		// CSE2 ActNpc004 animation: ++ani_wait > 4 → advance frame
		if(++effSmoke[i].timer > 4) {
			effSmoke[i].timer = 0;
			effSmoke[i].sprite.attr += 4;  // next 16x16 tile
		}
		sprite_pos(effSmoke[i].sprite,
			effSmoke[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 8,
			effSmoke[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 8);
	vdp_sprite_add(&effSmoke[i].sprite);
	}
	for(uint8_t i = 0; i < MAX_MISC; i++) {
		if(!effMisc[i].ttl) continue;
		effMisc[i].ttl--;
		switch(effMisc[i].type) {
			case EFF_BONKL:
			case EFF_BONKR:
			{
				// CSE2 CARET_TINY_PARTICLES: friction (xm*=4/5), blink rcLeft[ani_wait/2%2]
				effMisc[i].x_speed = (effMisc[i].x_speed * 4) / 5;
				effMisc[i].y_speed = (effMisc[i].y_speed * 4) / 5;
				effMisc[i].x += effMisc[i].x_speed;
				effMisc[i].y += effMisc[i].y_speed;
				effMisc[i].timer++;
				// CSE2 ActCaret13 blink: rcLeft[ani_wait/2%2] toggles visibility every 2 frames
				if((effMisc[i].timer / 2) % 2 == 0) {
					sprite_pos(effMisc[i].sprite,
						effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 4,
						effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 4);
					vdp_sprite_add(&effMisc[i].sprite);
				}
			}
			break;
			case EFF_ZZZ:
			{
				if(++effMisc[i].timer > 25) {
					effMisc[i].timer = 0;
					effMisc[i].sprite.attr++;
				}
				sprite_pos(effMisc[i].sprite,
					effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 4,
					effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 4);
				vdp_sprite_add(&effMisc[i].sprite);
			}
			break;
			case EFF_BOOST8:
			{
				effMisc[i].y++;
			} /* fallthrough */
			case EFF_BOOST2:
			{
				if(++effMisc[i].timer >= 5) {
					effMisc[i].timer = 0;
					effMisc[i].sprite.attr++;
				}
				sprite_pos(effMisc[i].sprite,
					effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 4,
					effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 4);
				vdp_sprite_add(&effMisc[i].sprite);
			}
			break;
			case EFF_QMARK:
			{
				if(effMisc[i].ttl > 20 && (effMisc[i].ttl & 1) == 0) {
					effMisc[i].y -= 2;
				}
				sprite_pos(effMisc[i].sprite,
					effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 4,
					effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 4);
				vdp_sprite_add(&effMisc[i].sprite);
			}
			break;
			case EFF_FANL:
			case EFF_FANR:
			{
				effMisc[i].x += effMisc[i].x_speed;
				sprite_pos(effMisc[i].sprite,
					effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 4,
					effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 4);
				vdp_sprite_add(&effMisc[i].sprite);
			}
			break;
			case EFF_FANU:
			case EFF_FAND:
			{
				effMisc[i].y += effMisc[i].y_speed;
				sprite_pos(effMisc[i].sprite,
					effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 4,
					effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 4);
				vdp_sprite_add(&effMisc[i].sprite);
			}
			break;
			case EFF_SPLASH:
			{
				if(++effMisc[i].timer >= 5) {
					effMisc[i].timer = 0;
					effMisc[i].y_speed++;
				}
				effMisc[i].x += effMisc[i].x_speed;
				effMisc[i].y += effMisc[i].y_speed;
				if(effMisc[i].ttl & 1) {
					sprite_pos(effMisc[i].sprite,
						effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 4,
						effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 4);
					vdp_sprite_add(&effMisc[i].sprite);
				}
			}
			break;
            case EFF_PSTAR_HIT:
            case EFF_MGUN_HIT:
            {
                if(effMisc[i].ttl && (effMisc[i].ttl & 3) == 0) {
                    effMisc[i].sprite.attr += 4;
                }
                sprite_pos(effMisc[i].sprite,
                   effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 8,
                   effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 8);
                vdp_sprite_add(&effMisc[i].sprite);
            }
            break;
		    case EFF_BUBB_POP:
            {
                if(effMisc[i].ttl == 4) effMisc[i].sprite.attr++;
                sprite_pos(effMisc[i].sprite,
                   effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 4,
                   effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 4);
                vdp_sprite_add(&effMisc[i].sprite);
            }
            break;
            case EFF_FIRETRAIL:
            case EFF_SNAKETRAIL:
            {
                if((effMisc[i].ttl & 3) == 0) effMisc[i].sprite.attr++;
                sprite_pos(effMisc[i].sprite,
                           effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 4,
                           effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 4);
                vdp_sprite_add(&effMisc[i].sprite);
            }
            break;
			case EFF_DISSIPATE:
			{
				if((effMisc[i].ttl % 7) == 0) effMisc[i].sprite.attr += 4;
				effMisc[i].y--;
				sprite_pos(effMisc[i].sprite,
						   effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 8,
						   effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 8);
				vdp_sprite_add(&effMisc[i].sprite);
			}
			break;
			case EFF_GIB:
			{
				effMisc[i].x += effMisc[i].x_speed;
				effMisc[i].y += effMisc[i].y_speed;
				if((effMisc[i].ttl & 3) == 0) effMisc[i].sprite.attr += 1;
				sprite_pos(effMisc[i].sprite,
						   effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 4,
						   effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 4);
				vdp_sprite_add(&effMisc[i].sprite);
			}
			break;
            case EFF_BOOMFLASH:
            {
                if(effMisc[i].ttl == 3) effMisc[i].sprite.attr += 16;
                sprite_pos(effMisc[i].sprite,
                           effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 16,
                           effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 16);
                vdp_sprite_add(&effMisc[i].sprite);
            }
            break;
            case EFF_SHOOT:
            {
                if((effMisc[i].ttl & 3) == 0) effMisc[i].sprite.attr += 4;
                sprite_pos(effMisc[i].sprite,
                           effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 8,
                           effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 8);
                vdp_sprite_add(&effMisc[i].sprite);
            }
            break;
            case EFF_HIT:
            {
                if((effMisc[i].ttl & 3) == 0) effMisc[i].sprite.attr += 4;
                sprite_pos(effMisc[i].sprite,
                           effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 8,
                           effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 8);
                vdp_sprite_add(&effMisc[i].sprite);
            }
            break;
		}
	}
}

void effect_create_damage(int16_t num, Entity *follow, int16_t xoff, int16_t yoff) {
	if(dqueued) return;
	for(uint8_t i = 0; i < MAX_DAMAGE; i++) {
		if(effDamage[i].ttl) continue;
		uint8_t negative = (num < 0);
		num = abs(num);
		uint8_t digitCount = 0;
		uint16_t tileIndex;
		// Clear the slot first so unused tiles don't have leftover data
		memset(dtiles[i], 0, sizeof(dtiles[i]));
		for(; num; digitCount++) {
			tileIndex = ((negative ? 11 : 0) + mod10[num]) << 3;
			memcpy(dtiles[i][3 - digitCount], &TS_Numbers.tiles[tileIndex], 32);
			num = div10[num];
		}
		tileIndex = ((negative ? 11 : 0) + 10) * 8;
		memcpy(dtiles[i][3 - digitCount], &TS_Numbers.tiles[tileIndex], 32); // - or +

		uint8_t num_tiles = digitCount + 1;
		uint8_t tile_start = 3 - digitCount;
		// GBA has no 24×8 sprite mode (3 tiles wide). Pad to 4 and shift data left.
		if (num_tiles == 3) {
			for (uint8_t j = 0; j < 3; j++)
				memcpy(dtiles[i][j], dtiles[i][j + 1], 32);
			memset(dtiles[i][3], 0, 32);
			num_tiles = 4;
			tile_start = 0;
		}

		if(follow) {
			damageFollow[i].e = follow;
			damageFollow[i].xoff = xoff;
			damageFollow[i].yoff = yoff;
			effDamage[i].x = (damageFollow[i].e->x >> CSF) + (xoff - 8);
			effDamage[i].y = (damageFollow[i].e->y >> CSF) + (yoff);
		} else {
			damageFollow[i].e = NULL;
			effDamage[i].x = xoff - 8;
			effDamage[i].y = yoff;
		}
		effDamage[i].ttl = 60; // 1 second
		effDamage[i].sprite = (VDPSprite) {
			.size = SPRITE_SIZE(num_tiles, 1),
			.attr = TILE_ATTR(PAL0, 1, 0, 0, TILE_NUMBERINDEX + (i<<2))
		};
		TILES_QUEUE(dtiles[i][tile_start], TILE_NUMBERINDEX + (i<<2), num_tiles);
		dqueued = TRUE;

		effDamage[i].digit_count = digitCount;
		break;
	}
}

void effect_create_smoke(int16_t x, int16_t y) {
	for(uint8_t i = 0; i < MAX_SMOKE; i++) {
		if(effSmoke[i].ttl) continue;
		effSmoke[i].x = x;
		effSmoke[i].y = y;
		// CSE2 ActNpc004: random angle direction, speed 1-3 pixels/frame
		{
			int8_t spd = 1 + (random() % 3);
			switch(random() & 7) {
				case 0: effSmoke[i].x_speed = spd;  effSmoke[i].y_speed = 0;    break;
				case 1: effSmoke[i].x_speed = spd;  effSmoke[i].y_speed = -spd; break;
				case 2: effSmoke[i].x_speed = 0;    effSmoke[i].y_speed = -spd; break;
				case 3: effSmoke[i].x_speed = -spd; effSmoke[i].y_speed = -spd; break;
				case 4: effSmoke[i].x_speed = -spd; effSmoke[i].y_speed = 0;    break;
				case 5: effSmoke[i].x_speed = -spd; effSmoke[i].y_speed = spd;  break;
				case 6: effSmoke[i].x_speed = 0;    effSmoke[i].y_speed = spd;  break;
				case 7: effSmoke[i].x_speed = spd;  effSmoke[i].y_speed = spd;  break;
			}
		}
		// CSE2 ActNpc004: 8 frames × ~5 ticks = 40 frames total
		effSmoke[i].ttl = 40;
		effSmoke[i].timer = 0;  // animation timer
		// Animation frame cycles every 5 ticks (40/8=5), matching CSE2 ani_wait > 4
		effSmoke[i].sprite = (VDPSprite) {
			.size = SPRITE_SIZE(2, 2),
			.attr = TILE_ATTR(PAL1, 1, 0, 0, TILE_SMOKEINDEX)
		};
		break;
	}
}

// CSE2 SetDestroyNpChar / SetDestroyNpCharUp equivalent
// Scatters 'count' smoke particles within a w×w pixel area centered at (x, y)
void effect_smoke_burst(int16_t x, int16_t y, uint16_t w, uint8_t count) {
	int16_t half = (int16_t)w / 2;
	if (half < 1) half = 0;
	for(uint8_t i = 0; i < count; i++) {
		int16_t ox = half ? (int16_t)(random() % (half * 2)) - half : 0;
		int16_t oy = half ? (int16_t)(random() % (half * 2)) - half : 0;
		effect_create_smoke(x + ox, y + oy);
	}
}

void effect_create_misc(uint8_t type, int16_t x, int16_t y, uint8_t only_one) {
	for(uint8_t i = 0; i < MAX_MISC; i++) {
		if(effMisc[i].ttl) {
			if(only_one && effMisc[i].type == type) break;
			continue;
		}
		effMisc[i].type = type;
		effMisc[i].x = x;
		effMisc[i].y = y;
		switch(type) {
		case EFF_BONKL: // Dots that appear when player bonks their head on the ceiling
			case EFF_BONKR:
			{
				// CSE2 CARET_TINY_PARTICLES: xm=Random(-0x600,0x600), ym=Random(-0x200,0x200), friction *=4/5
				// Convert sub-pixel range to pixel: x = -3..+3, y = -1..+1
				effMisc[i].ttl = 21;
				effMisc[i].x_speed = (int8_t)((random() % 7) - 3);
				effMisc[i].y_speed = (int8_t)((random() % 3) - 1);
				effMisc[i].timer = 0;
				effMisc[i].sprite = (VDPSprite) {
					.size = SPRITE_SIZE(1, 1),
					.attr = TILE_ATTR(PAL0,1,0,0,TILE_BONKINDEX)
				};
			}
			break;
			case EFF_ZZZ: // Zzz shown above sleeping NPCs like gunsmith, mimiga, etc
			{
				uint8_t sheet = NOSHEET;
				SHEET_FIND(sheet, SHEET_ZZZ);
				if(sheet == NOSHEET) break;
				effMisc[i].ttl = 100;
				effMisc[i].sprite = (VDPSprite) {
					.size = SPRITE_SIZE(1, 1),
					.attr = TILE_ATTR(PAL0,1,0,0,sheets[sheet].index)
				};
			}
			break;
			case EFF_BOOST8: // Smoke that emits while using the booster
			case EFF_BOOST2:
			{
				effMisc[i].ttl = 20;
				effMisc[i].sprite = (VDPSprite) {
					.size = SPRITE_SIZE(1, 1),
					.attr = TILE_ATTR(PAL0,1,0,0,TILE_BOOSTINDEX)
				};
			}
			break;
			case EFF_QMARK:
			{
				effMisc[i].ttl = 30;
				effMisc[i].sprite = (VDPSprite) {
					.size = SPRITE_SIZE(1, 1),
					.attr = TILE_ATTR(PAL0,1,0,0,TILE_QMARKINDEX)
				};
			} 
			break;

			case EFF_FANL:
			{
				effMisc[i].x_speed = -(random() & 3) - 1;
				effMisc[i].ttl = 20;
				effMisc[i].sprite = (VDPSprite) {
					.size = SPRITE_SIZE(1, 1),
					.attr = TILE_ATTR(PAL0,1,0,0,TILE_BONKINDEX)
				};
			}
			break;
			case EFF_FANU:
			{
				effMisc[i].y_speed = -(random() & 3) - 1;
				effMisc[i].ttl = 20;
				effMisc[i].sprite = (VDPSprite) {
					.size = SPRITE_SIZE(1, 1),
					.attr = TILE_ATTR(PAL0,1,0,0,TILE_BONKINDEX)
				};
			}
			break;
			case EFF_FANR:
			{
				effMisc[i].x_speed = (random() & 3) + 1;
				effMisc[i].ttl = 20;
				effMisc[i].sprite = (VDPSprite) {
					.size = SPRITE_SIZE(1, 1),
					.attr = TILE_ATTR(PAL0,1,0,0,TILE_BONKINDEX)
				};
			}
			break;
			case EFF_FAND:
			{
				effMisc[i].y_speed = (random() & 3) + 1;
				effMisc[i].ttl = 20;
				effMisc[i].sprite = (VDPSprite) {
					.size = SPRITE_SIZE(1, 1),
					.attr = TILE_ATTR(PAL0,1,0,0,TILE_BONKINDEX)
				};
			}
			break;
			case EFF_SPLASH:
			{
				if(random() & 1) {
					effMisc[i].y_speed = -2;
					effMisc[i].ttl = 18;
				} else {
					effMisc[i].y_speed = -3;
					effMisc[i].ttl = 29;
				}
				effMisc[i].x_speed = (player.x_speed >> CSF) - 1 + (random() & 3);
				effMisc[i].sprite = (VDPSprite) {
					.size = SPRITE_SIZE(1, 1),
					.attr = TILE_ATTR(PAL0,1,0,0,TILE_BONKINDEX)
				};
			}
			break;
		    case EFF_PSTAR_HIT:
            {
                effMisc[i].ttl = 12;
                uint8_t loc = 0;
                SHEET_FIND(loc, SHEET_PSTAR);
                effMisc[i].sprite = (VDPSprite) {
                    .size = SPRITE_SIZE(2, 2) | (7 << 4),
                    .attr = TILE_ATTR(PAL0,1,0,0,sheets[loc].index+8)
                };
            }
            break;
            case EFF_MGUN_HIT:
            {
                effMisc[i].ttl = 12;
                uint8_t loc = 0;
                SHEET_FIND(loc, SHEET_MGUN);
                effMisc[i].sprite = (VDPSprite) {
                    .size = SPRITE_SIZE(2, 2),
                    .attr = TILE_ATTR(PAL0,1,0,0,sheets[loc].index+8)
                };
            }
            break;
            case EFF_BUBB_POP:
            {
                effMisc[i].ttl = 8;
                uint8_t loc = 0;
                SHEET_FIND(loc, SHEET_BUBB);
                effMisc[i].sprite = (VDPSprite) {
                        .size = SPRITE_SIZE(1, 1),
                        .attr = TILE_ATTR(PAL0,1,0,0,sheets[loc].index+4)
                };
            }
            break;
            case EFF_FIRETRAIL:
            {
                effMisc[i].ttl = 12;
                uint8_t loc = 0;
                SHEET_FIND(loc, SHEET_FBALL);
                effMisc[i].sprite = (VDPSprite) {
                        .size = SPRITE_SIZE(1, 1),
                        .attr = TILE_ATTR(PAL1,1,0,0,sheets[loc].index+12)
                };
            }
            break;
            case EFF_SNAKETRAIL:
            {
                effMisc[i].ttl = 12;
                uint8_t loc = 0;
                SHEET_FIND(loc, SHEET_SNAKE);
                effMisc[i].sprite = (VDPSprite) {
                    .size = SPRITE_SIZE(1, 1),
                    .attr = TILE_ATTR(PAL1,1,0,0,sheets[loc].index+12)
                };
            }
            break;
            case EFF_DISSIPATE:
            {
                effMisc[i].ttl = 20;
                effMisc[i].sprite = (VDPSprite) {
                    .size = SPRITE_SIZE(2, 2) | (7 << 4),
                    .attr = TILE_ATTR(PAL0,1,0,0,TILE_DISSIPINDEX)
                };
            }
            break;
            case EFF_GIB:
            {
                effMisc[i].x_speed = (random() & 3) - 1;
                effMisc[i].y_speed = (random() & 3) - 1;
                effMisc[i].ttl = 12;
                effMisc[i].sprite = (VDPSprite) {
                    .size = SPRITE_SIZE(1, 1),
                    .attr = TILE_ATTR(PAL1,1,0,0,TILE_GIBINDEX)
                };
            }
            break;
            case EFF_BOOMFLASH:
            {
                effMisc[i].ttl = 6;
                effMisc[i].sprite = (VDPSprite) {
                    .size = SPRITE_SIZE(4, 4) | (7 << 4),
                    .attr = TILE_ATTR(PAL0,1,0,0,TILE_BOOMINDEX)
                };
            }
            break;
            case EFF_SHOOT:
            {
                effMisc[i].ttl = 12;
                effMisc[i].sprite = (VDPSprite) {
                    .size = SPRITE_SIZE(2, 2) | (7 << 4),
                    .attr = TILE_ATTR(PAL0,1,0,0,TILE_SHOOTINDEX)
                };
            }
            break;
            case EFF_HIT:
            {
                effMisc[i].ttl = 12;
                effMisc[i].sprite = (VDPSprite) {
                    .size = SPRITE_SIZE(2, 2) | (7 << 4),
                    .attr = TILE_ATTR(PAL0,1,0,0,TILE_HITINDEX)
                };
            }
            break;
		}
		break;
	}
}

// Old sprite-based fade functions removed; replaced by pure BG3 fade above.
