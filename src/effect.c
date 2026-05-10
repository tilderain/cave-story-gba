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

// Add this dedicated memory for the fades:
static VDPSprite fadeSpr[3][8];

uint8_t fadeSweepDir;
#define bval (1<<15) | TILE_FADEINDEX
static const uint16_t winmap[40] = { 
	bval,bval,bval,bval,bval,bval,bval,bval,bval,bval,
	bval,bval,bval,bval,bval,bval,bval,bval,bval,bval,
	bval,bval,bval,bval,bval,bval,bval,bval,bval,bval,
	bval,bval,bval,bval,bval,bval,bval,bval,bval,bval,
};
static const uint32_t tblack[8] = {
	0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,
};

int8_t fadeSweepTimer;
void effects_init() {
	for(uint8_t i = 0; i < MAX_DAMAGE; i++) effDamage[i].ttl = 0;
	for(uint8_t i = 0; i < MAX_SMOKE; i++) effSmoke[i].ttl = 0;
	for(uint8_t i = 0; i < MAX_MISC; i++) effMisc[i].ttl = 0;
	// Load each frame of the small smoke sprite
	uint32_t stiles[7][32]; // [number of frames][tiles per frame * (tile bytes / sizeof(uint32_t))]
	for(uint8_t i = 0; i < 7; i++) {
		memcpy(stiles[i], SPR_TILES(&SPR_Smoke, 0, i), 128);
	}
	// Transfer to VRAM
	vdp_tiles_load_from_rom(stiles[0], TILE_SMOKEINDEX, TILE_SMOKESIZE);
}

void effects_clear() {
	for(uint8_t i = 0; i < MAX_DAMAGE; i++) effDamage[i].ttl = 0;
	for(uint8_t i = 0; i < MAX_MISC; i++) effMisc[i].ttl = 0;
	effects_clear_smoke();
}

void effects_clear_smoke() {
	for(uint8_t i = 0; i < MAX_SMOKE; i++) effSmoke[i].ttl = 0;
}

IWRAM_CODE void effects_update() {
	for(uint8_t i = 0; i < MAX_DAMAGE; i++) {
    	if(!effDamage[i].ttl) continue;
    	effDamage[i].ttl--;
    	if(effDamage[i].ttl < 8) {
    	    uint8_t start = 3 - effDamage[i].digit_count;
    	    for(uint8_t x = start; x < 4; x++) {
    	        for(uint8_t y = 0; y < 7; y++) {
    	            dtiles[i][x][y] = dtiles[i][x][y + 1];
    	        }
    	        dtiles[i][x][7] = 0;
    	    }
    			DMA_doDma(DMA_VRAM,
    			          (uint32_t) dtiles[i][3 - effDamage[i].digit_count],
    			          (TILE_NUMBERINDEX + (i << 2)) * 16,
    			          (effDamage[i].digit_count + 1) * 8,
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
		effSmoke[i].x += effSmoke[i].x_speed;
		effSmoke[i].y += effSmoke[i].y_speed;
		// Half assed animation
		sprite_index(effSmoke[i].sprite,
			TILE_SMOKEINDEX + 24 - ((effSmoke[i].ttl >> 2) << 2));
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
			{
				if(effMisc[i].ttl&1) {
					if(effMisc[i].ttl > 15) {
						effMisc[i].x--;
						effMisc[i].y--;
					}
					sprite_pos(effMisc[i].sprite,
						effMisc[i].x - sub_to_pixel(camera.x) + SCREEN_HALF_W - 4,
						effMisc[i].y - sub_to_pixel(camera.y) + SCREEN_HALF_H - 4);
					vdp_sprite_add(&effMisc[i].sprite);
				}
			}
			break;
			case EFF_BONKR:
			{
				if(!(effMisc[i].ttl&1)) {
					if(effMisc[i].ttl > 15) {
						effMisc[i].x++;
						effMisc[i].y--;
					}
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
			.size = SPRITE_SIZE(digitCount+1, 1),
			.attr = TILE_ATTR(PAL0, 1, 0, 0, TILE_NUMBERINDEX + (i<<2))
		};
		TILES_QUEUE(dtiles[i][3-digitCount], TILE_NUMBERINDEX + (i<<2), digitCount+1);
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
		switch(random() & 7) {
			case 0: effSmoke[i].x_speed = 0; break;
			case 1:	effSmoke[i].y_speed = 0; break;
			case 2: effSmoke[i].x_speed = 1; break;
			case 3: effSmoke[i].y_speed = 1; break;
			case 4: effSmoke[i].x_speed = -1; break;
			case 5:	effSmoke[i].y_speed = -1; break;
			case 6: effSmoke[i].x_speed ^= 1; break;
			case 7: effSmoke[i].y_speed ^= 1; break;
		}
		effSmoke[i].ttl = 24;
		effSmoke[i].sprite = (VDPSprite) {
			.size = SPRITE_SIZE(2, 2),
			.attr = TILE_ATTR(PAL1, 1, 0, 0, TILE_SMOKEINDEX)
		};
		break;
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
				effMisc[i].ttl = 30;
				effMisc[i].sprite = (VDPSprite) {
					.size = SPRITE_SIZE(1, 1),
					.attr = TILE_ATTR(PAL0,1,0,0,1)
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
					.attr = TILE_ATTR(PAL0,1,0,0,12)
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
					.attr = TILE_ATTR(PAL0,1,0,0,1)
				};
			}
			break;
			case EFF_FANU:
			{
				effMisc[i].y_speed = -(random() & 3) - 1;
				effMisc[i].ttl = 20;
				effMisc[i].sprite = (VDPSprite) {
					.size = SPRITE_SIZE(1, 1),
					.attr = TILE_ATTR(PAL0,1,0,0,1)
				};
			}
			break;
			case EFF_FANR:
			{
				effMisc[i].x_speed = (random() & 3) + 1;
				effMisc[i].ttl = 20;
				effMisc[i].sprite = (VDPSprite) {
					.size = SPRITE_SIZE(1, 1),
					.attr = TILE_ATTR(PAL0,1,0,0,1)
				};
			}
			break;
			case EFF_FAND:
			{
				effMisc[i].y_speed = (random() & 3) + 1;
				effMisc[i].ttl = 20;
				effMisc[i].sprite = (VDPSprite) {
					.size = SPRITE_SIZE(1, 1),
					.attr = TILE_ATTR(PAL0,1,0,0,1)
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
					.attr = TILE_ATTR(PAL0,1,0,0,1)
				};
			}
			break;
		    case EFF_PSTAR_HIT:
            {
                effMisc[i].ttl = 12;
                uint8_t loc = 0;
                SHEET_FIND(loc, SHEET_PSTAR);
                effMisc[i].sprite = (VDPSprite) {
                    .size = SPRITE_SIZE(2, 2),
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
		}
		break;
	}
}

static void fade_setup(VDPSprite *spr0, VDPSprite *spr1, VDPSprite *spr2, uint8_t dir, uint8_t fadein) {
	// Fill window plane
	for(uint16_t y = 0; y < SCREEN_HEIGHT / 8; y++) {
	//	DMA_doDma(DMA_VRAM, (uint32_t) winmap, VDP_PLAN_W + y*128, 40, 2);
	}

    // Load all 3 frames (4x4 tiles each) sequentially starting at TILE_HUDINDEX
    // Frame 0: TILE_HUDINDEX (64)
    // Frame 1: TILE_HUDINDEX + 16 (80)
    // Frame 2: TILE_HUDINDEX + 32 (96)
    SHEET_LOAD(&SPR_Fade, 3, 4*4, TILE_HUDINDEX, TRUE, 0, 1, 2);

    // This is the solid black tile used for the window/background
    DMA_doDma(DMA_VRAM, (uint32_t) tblack, TILE_FADEINDEX*TILE_SIZE, TILE_SIZE/2, 2);
	// Setup sprites
	for(uint16_t i = 0; i < (pal_mode ? 8 : 7); i++) {
		spr0[i].y = 0x80 + i * 32;
		spr1[i].y = 0x80 + i * 32;
		spr2[i].y = 0x80 + i * 32;
		if(fadein) {
			if(dir) { // Start from right
				spr0[i].x = 0x80 + SCREEN_WIDTH + 64;
				spr1[i].x = 0x80 + SCREEN_WIDTH + 32;
				spr2[i].x = 0x80 + SCREEN_WIDTH + 0;
			} else { // Start from left
				spr0[i].x = 0x80 - 96;
				spr1[i].x = 0x80 - 64;
				spr2[i].x = 0x80 - 32;
			}
		} else {
			if(dir) { // Start from right
				spr0[i].x = 0x80 + SCREEN_WIDTH + 0;
				spr1[i].x = 0x80 + SCREEN_WIDTH + 32;
				spr2[i].x = 0x80 + SCREEN_WIDTH + 64;
			} else { // Start from left
				spr0[i].x = 0x80 - 32;
				spr1[i].x = 0x80 - 64;
				spr2[i].x = 0x80 - 96;
			}
		}
		if(i == 7) {
			spr0[i].size = SPRITE_SIZE(4,2);
			spr1[i].size = SPRITE_SIZE(4,2);
			spr2[i].size = SPRITE_SIZE(4,2);
		} else {
			spr0[i].size = SPRITE_SIZE(4,4);
			spr1[i].size = SPRITE_SIZE(4,4);
			spr2[i].size = SPRITE_SIZE(4,4);
		}

        spr0[i].attr = TILE_ATTR(PAL0, 1, 0, (fadein?dir:!dir), TILE_HUDINDEX + 32);
        spr1[i].attr = TILE_ATTR(PAL0, 1, 0, (fadein?dir:!dir), TILE_HUDINDEX );
        spr2[i].attr = TILE_ATTR(PAL0, 1, 0, (fadein?dir:!dir), TILE_HUDINDEX + 16);
	}
}

void do_fadeout_sweep(uint8_t dir) {
	fade_setup(fadeSpr[0], fadeSpr[1], fadeSpr[2], dir, FALSE);
	// Fade loop
	for(uint16_t f = 0; f < SCREEN_WIDTH / 16 + 6; f++) {
		// Update Sprites
		for(uint16_t i = 0; i < (pal_mode ? 8 : 7); i++) {
			fadeSpr[0][i].x += dir ? -16 : 16;
			fadeSpr[1][i].x += dir ? -16 : 16;
			fadeSpr[2][i].x += dir ? -16 : 16;
		}
		vdp_sprites_add(fadeSpr[0], pal_mode ? 8 : 7);
		vdp_sprites_add(fadeSpr[1], pal_mode ? 8 : 7);
		vdp_sprites_add(fadeSpr[2], pal_mode ? 8 : 7);
		player_draw();
		entities_draw();
		//sys_wait_vblank();
		// Update window plane
		if(f > 5) {
			const uint8_t x = dir ? (20 - (f-5)) | 0x80 : f-5;
			vdp_set_window(x, 0);
		}
		ready = TRUE;
		aftervsync();
	}
	vdp_colors(0, PAL_FadeOut, 64);
	vdp_set_window(0, 0);
}
void start_fadein_sweep(uint8_t dir) {
	fade_setup(fadeSpr[0], fadeSpr[1], fadeSpr[2], dir, TRUE);
	fadeSweepTimer = SCREEN_WIDTH / 16 + 6;
	fadeSweepDir = dir;
	// Cover screen with window (black), and load target palette immediately
	vdp_set_window(20, 0);
	//vdp_colors_apply_next_now();
}
void update_fadein_sweep(void) {
	fadeSweepTimer--;
	if(fadeSweepTimer >= 0) {
		// Update VDPSprites
		for(uint16_t i = 0; i < (pal_mode ? 8 : 7); i++) {
			fadeSpr[0][i].x += fadeSweepDir ? -16 : 16;
			fadeSpr[1][i].x += fadeSweepDir ? -16 : 16;
			fadeSpr[2][i].x += fadeSweepDir ? -16 : 16;
		}
		vdp_sprites_add(fadeSpr[0], pal_mode ? 8 : 7);
		vdp_sprites_add(fadeSpr[1], pal_mode ? 8 : 7);
		vdp_sprites_add(fadeSpr[2], pal_mode ? 8 : 7);
		// Update window plane
		if(fadeSweepTimer > 5) {
			const uint8_t x = fadeSweepDir ? (fadeSweepTimer-5) : (20 - (fadeSweepTimer-5)) | 0x80;
			vdp_set_window(x, 0);
		} else {
			vdp_set_window(0, 0);
		}
	} else {
		// After the fade is done, need to restore HUD tiles we clobbered
		hud_force_redraw();
	}
}