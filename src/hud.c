#include "common.h"

#include "dma.h"
#include "entity.h"
#include "gamemode.h"
#include "memory.h"
#include "player.h"
#include "resources.h"
#include "system.h"
#include "tables.h"
#include "vdp.h"
#include "weapon.h"
#include "effect.h"

#include "hud.h"
#include "xgm.h"

#define TSIZE 8
#define SPR_TILE(x, y) (((x)*4)+(y))

#define HP_BAR  0
#define XP_BAR  8
#define WPN     16
#define AMMO    20

VDPSprite sprHUD[4];
VDPSprite sprSwap[3];
uint32_t tileData[28][8];

// Values used to draw parts of the HUD
// If the originator's value changes that part of the HUD will be updated
uint8_t hudMaxHealth, hudHealth;
uint8_t hudWeapon, hudMaxAmmo, hudAmmo;
uint8_t hudLevel, hudMaxEnergy, hudEnergy;
uint8_t hudMaxBlink;

// Used for bar animation
uint8_t hudEnergyPixel, hudEnergyTimer, hudEnergyDest;

// Weapon swap scroll animation
// Matches CSE2 gArmsEnergyX sliding: forward (next) scrolls in from right (+16px),
// backward (prev) scrolls in from left (-16px). Slides at 2px/frame over 8 frames.
static int8_t hudScrollOffset;   // current pixel offset: -16 to 16, 0 = normal
static int8_t hudScrollDir;      // 0 = none, 1 = next (from right), -1 = prev (from left)

// Used for weapon switch display (all weapons shown during swap)
#define SWAP_BEGIN    9
// 4-tile block in reserved space for 2x2-compatible weapon icon repack
// (kept outside TILE_EXWEPINDEX to avoid overlapping with swap weapon tiles)
#define TILE_SWAPICONINDEX (TILE_RESERVEDSPACE)
static uint8_t swapTimer, swapDir, swapWepNum;

uint8_t showing = FALSE;

static const uint8_t EnergyPixel[4][42] = {
        { 0, 4, 8,12,16,20,24,28,32,36,40 },
        { 0, 2, 4, 6, 8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40 },
        { 0, 1, 3, 4, 6, 7, 9,10,11,12,14,15,17,18,19,20,22,23,25,26,
         27,29,30,31,33,34,36,37,38,39,40 },
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,
         20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40 },
};

void hud_refresh_health();
void hud_refresh_energy(uint8_t hard);
void hud_refresh_weapon();
void hud_refresh_ammo();
void hud_refresh_maxammo();

// Expected to happen while the screen is off
void hud_create() {
	// Invalidate all values, forces a redraw
	hudMaxHealth = hudHealth = hudWeapon = hudLevel = hudMaxAmmo = hudAmmo = 255;
    hudEnergy = hudMaxEnergy = 10;
	hudEnergyPixel = hudEnergyTimer = hudEnergyDest = 0;
	hudScrollOffset = 0;
	hudScrollDir = 0;
	swapTimer = swapDir = swapWepNum = 0;
	// Create the sprites
	sprHUD[0] = (VDPSprite) {
		.x = 16 + 128,
		.y = (pal_mode ? 24 : 16) + 128,
		.size = SPRITE_SIZE(4, 4),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_HUDINDEX*2)
	};
	sprHUD[1] = (VDPSprite) {
		.x = 16 + 32 + 128,
		.y = (pal_mode ? 24 : 16) + 128,
		.size = SPRITE_SIZE(4, 4),
		.attr = TILE_ATTR(PAL0,1,0,0,(TILE_HUDINDEX*2)+16)
	};
	// Health bar sprites (packed at DMA 96-107, right after scrolling HUD)
	// Heart icon + digit at (32, Y+8), 16x16
	sprHUD[2] = (VDPSprite) {
		.x = (16 + 16) + 128 - 16,
		.y = (16) + 8 + 128 + 16,
		.size = SPRITE_SIZE(4, 2),
		.attr = TILE_ATTR(PAL0,1,0,0,(TILE_HUDINDEX + 32)*2)
	};
	// HP bar at (16, Y+16), 32x16
	sprHUD[3] = (VDPSprite) {
		.x = 16 + 128 + 32,
		.y = (16) + 16 + 128 + 8,
		.size = SPRITE_SIZE(4, 2),
		.attr = TILE_ATTR(PAL0,1,0,0,(TILE_HUDINDEX + 42)*2)
	};
	// Weapon swap display sprites (offscreen until swap begins)
	sprSwap[0] = (VDPSprite) {
		.x = 16 + 64 + 128,
		.y = (pal_mode ? 24 : 16) + 128,
		.size = SPRITE_SIZE(4, 2) | (5 << 4),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_EXWEPINDEX)
	};
	sprSwap[1] = (VDPSprite) {
		.x = 16 + 96 + 128,
		.y = (pal_mode ? 24 : 16) + 128,
		.size = SPRITE_SIZE(4, 2) | (5 << 4),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_EXWEPINDEX+8)
	};
	// Weapon icon overlay — replaces the HUD weapon icon (always shown as separate sprite)
	sprSwap[2] = (VDPSprite) {
		.x = 16 + 128,
		.y = (pal_mode ? 24 : 16) + 128,
		.size = SPRITE_SIZE(2, 2) | (5 << 4),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_SWAPICONINDEX)
	};
	hud_refresh_swap(TRUE);
	// Blank weapon icon in HUD sprite (replaced by sprSwap[2] overlay)
	DMA_doDma(DMA_VRAM, (uint32_t)TILE_BLANK, (TILE_HUDINDEX)*TILE_SIZE, 16, 2);
	DMA_doDma(DMA_VRAM, (uint32_t)TILE_BLANK, (TILE_HUDINDEX+2)*TILE_SIZE, 16, 2);
	// Draw blank tiles next to weapon area (columns 2-3)
	DMA_doDma(DMA_VRAM, (uint32_t)TILE_BLANK, (TILE_HUDINDEX+1)*TILE_SIZE, 16, 2);
	DMA_doDma(DMA_VRAM, (uint32_t)TILE_BLANK, (TILE_HUDINDEX+3)*TILE_SIZE, 16, 2);
	// Fill health sprites' blank tile slots: heart sprite bottom (+34,+35)
	// and 32x16 sprite non-HP positions (+36..+41)
	for(int i = 34; i < 42; i++)
		DMA_doDma(DMA_VRAM, (uint32_t)TILE_BLANK, (TILE_HUDINDEX+i)*TILE_SIZE, 16, 2);
	// Blank old health bar tile positions in the scrolling sprite area
	DMA_doDma(DMA_VRAM, (uint32_t)TILE_BLANK, (TILE_HUDINDEX+6)*TILE_SIZE, 32, 2);
	DMA_doDma(DMA_VRAM, (uint32_t)TILE_BLANK, (TILE_HUDINDEX+14)*TILE_SIZE, 32, 2);
}


void hud_force_redraw(void) {
	if(fadeSweepTimer > 0) return;
	hud_refresh_health();
    hud_refresh_weapon();
    hud_refresh_swap(TRUE);
    hud_refresh_energy(TRUE);
	hud_refresh_maxammo();
	hud_refresh_ammo();
    // Draw blank tiles next to weapon
	DMA_doDma(DMA_VRAM, (uint32_t)TILE_BLANK, (TILE_HUDINDEX+1)*TILE_SIZE, 16, 2);
	DMA_doDma(DMA_VRAM, (uint32_t)TILE_BLANK, (TILE_HUDINDEX+3)*TILE_SIZE, 16, 2);
	// Blank old health bar tile positions in the scrolling sprite area
	DMA_doDma(DMA_VRAM, (uint32_t)TILE_BLANK, (TILE_HUDINDEX+6)*TILE_SIZE, 32, 2);
	DMA_doDma(DMA_VRAM, (uint32_t)TILE_BLANK, (TILE_HUDINDEX+14)*TILE_SIZE, 32, 2);

    disable_ints;
    z80_request();
	DMA_flushQueue();
    z80_release();
    enable_ints;
}

void hud_force_energy() {
	hudLevel = 9;
}

void hud_show() {
	showing = TRUE;
}

void hud_hide() {
	showing = FALSE;
}

void hud_update() {
	uint8_t weaponChange = FALSE;
	//if(paused) return;
	if(!showing || fadeSweepTimer > 0) return;

	// Handle weapon swap scroll animation (matches CSE2 gArmsEnergyX sliding)
	if(hudScrollDir != 0) {
		// Apply scroll offset to both weapon panel sprites and icon
		sprHUD[0].x = (16 + 128) + hudScrollOffset;
		sprHUD[1].x = (16 + 32 + 128) + hudScrollOffset;
		sprSwap[2].x = (16 + 128) + hudScrollOffset;

		// Move offset toward 0 (2 px/frame, matching CSE2's rate)
		if(hudScrollOffset > 0) {
			hudScrollOffset -= 2;
			if(hudScrollOffset < 0) hudScrollOffset = 0;
		} else {
			hudScrollOffset += 2;
			if(hudScrollOffset > 0) hudScrollOffset = 0;
		}

		// Scroll complete
		if(hudScrollOffset == 0) {
			hudScrollDir = 0;
			sprHUD[0].x = 16 + 128;
			sprHUD[1].x = 16 + 32 + 128;
			sprSwap[2].x = 16 + 128;
		}
	}

	vdp_sprites_add(sprHUD, 4);
	vdp_sprite_add(&sprSwap[2]); // Always shown as weapon icon overlay
	// Handle weapon swap display animation
	if(swapTimer) {
		hud_refresh_swap(FALSE);
	}
	// Add swap display sprites if any weapons to show
	if(swapWepNum > 0) {
		vdp_sprites_add(sprSwap, swapWepNum > 2 ? 2 : 1);
	}
	// Only refresh one part of the HUD in a single frame, at most 8 tiles will be sent
	if(hudMaxHealth != playerMaxHealth || hudHealth != player.health) {
		hud_refresh_health();
	}
    if(hudScrollDir == 0 && hudWeapon != playerWeapon[currentWeapon].type) {
		hud_refresh_weapon();
		weaponChange = TRUE;
	}
    if(hudLevel != playerWeapon[currentWeapon].level || weaponChange) {
		hud_refresh_energy(TRUE);
	}
    if(hudEnergy != playerWeapon[currentWeapon].energy) {
		hud_refresh_energy(FALSE);
	}
    if(hudMaxAmmo != playerWeapon[currentWeapon].maxammo) {
		// Max ammo changed refresh both
		hud_refresh_maxammo();
	//	hud_refresh_ammo();
	}
    //if(hudAmmo != playerWeapon[currentWeapon].ammo ) {
		hud_refresh_ammo();
	//}
}

void hud_refresh_health() {
	// Redraw health if it changed
	hudMaxHealth = max(playerMaxHealth, 1); // Just so it's impossible to divide zero
	hudHealth = player.health;
	// 40 * hudHealth / hudMaxHealth
	int16_t fillHP = ((uint16_t)((hudHealth<<5) + (hudHealth<<3))) / hudMaxHealth;
	for(uint8_t i = 0; i < 5; i++) {
		// The TS_HudBar tileset has two rows of 8 tiles, where the section of the
		// bar is empty at tile 0 and full at tile 7
		int16_t addrHP = min(fillHP*TSIZE, 7*TSIZE);
		if(addrHP < 0) addrHP = 0;
		// Fill in the bar
		memcpy(tileData[i+3], &TS_HudBar.tiles[addrHP], TILE_SIZE);
		fillHP -= 8;
	}
	// Heart icon and two digits displaying current health
	memcpy(tileData[0], &SPR_TILES(&SPR_Hud2, 0, 0)[12*TSIZE], TILE_SIZE*3);
	uint8_t digit = div10[hudHealth];
	if(digit) {
		// Overlay tens digit on top of tileData[1] (palette index 0 = transparent)
		const uint8_t *src = (const uint8_t*)&TS_Numbers.tiles[(digit)*TSIZE];
		uint8_t *dst = (uint8_t*)tileData[1];
		for(uint32_t i = 0; i < TILE_SIZE; i++) {
			uint8_t s = src[i];
			uint8_t d = dst[i];
			uint8_t lo = (s & 0x0F) ? (s & 0x0F) : (d & 0x0F);
			uint8_t hi = (s & 0xF0) ? (s & 0xF0) : (d & 0xF0);
			dst[i] = lo | hi;
		}
	}
	// Overlay ones digit on top of tileData[2]
	{
		const uint8_t *src = (const uint8_t*)&TS_Numbers.tiles[mod10[hudHealth]*TSIZE];
		uint8_t *dst = (uint8_t*)tileData[2];
		for(uint32_t i = 0; i < TILE_SIZE; i++) {
			uint8_t s = src[i];
			uint8_t d = dst[i];
			uint8_t lo = (s & 0x0F) ? (s & 0x0F) : (d & 0x0F);
			uint8_t hi = (s & 0xF0) ? (s & 0xF0) : (d & 0xF0);
			dst[i] = lo | hi;
		}
	}
	// Queue DMA transfer for health display
	// Heart+digit at TILE_HUDINDEX+32 (sprite tile 160, DMA 96-97)
	// HP bar at TILE_HUDINDEX+42 (sprite tile 164, DMA 106-107, internal pos 2,1)
	DMA_queueDma(DMA_VRAM, (uint32_t)tileData[0], (TILE_HUDINDEX+32)*TILE_SIZE, 32, 2);
	DMA_queueDma(DMA_VRAM, (uint32_t)tileData[3], (TILE_HUDINDEX+42)*TILE_SIZE, 32, 2);
}

void hud_refresh_energy(uint8_t hard) {
	// Energy or level changed
	if(hudLevel != playerWeapon[currentWeapon].level) {
		hudLevel = playerWeapon[currentWeapon].level;
		hard = TRUE;
	}

    // The Spur's values are rather high, so if it's equipped half it in this temp variable,
    // so that the lookup table can still be used
    uint8_t tempMaxEnergy;
    uint8_t tempEnergy;
	if(playerWeapon[currentWeapon].type == WEAPON_SPUR) {
		hudMaxEnergy = spur_time[pal_mode||cfg_60fps][playerWeapon[currentWeapon].level];
		if(playerWeapon[currentWeapon].level == 3){
			tempMaxEnergy = hudMaxEnergy >> 2;
        	tempEnergy = hudEnergy >> 2;
		} else {
        	tempMaxEnergy = hudMaxEnergy >> 1;
        	tempEnergy = hudEnergy >> 1;
		}

        if(hudEnergy != playerWeapon[currentWeapon].energy) hudMaxBlink = 1;
	} else {
		hudMaxEnergy = max(weapon_info[playerWeapon[currentWeapon].type].experience[hudLevel-1], 10);
	}
	if(!hard) {
		if(hudEnergyTimer == 0) {
            hudEnergyPixel = EnergyPixel[div10[tempMaxEnergy] - 1][tempEnergy];
			if(playerWeapon[currentWeapon].type == WEAPON_SPUR)
            	hudEnergyDest = EnergyPixel[div10[tempMaxEnergy] - 1][tempEnergy];
			else
				hudEnergyDest = EnergyPixel[div10[tempMaxEnergy] - 1][playerWeapon[currentWeapon].energy];
			hudEnergyTimer = 3;
		} else {
			hudEnergyTimer--;
		}
	} else {
		hudEnergy = playerWeapon[currentWeapon].energy;
        hudEnergyPixel = EnergyPixel[div10[tempMaxEnergy] - 1][playerWeapon[currentWeapon].energy];
		hudEnergyDest = hudEnergyPixel;
		hudEnergyTimer = 0;
	}
	if(hudEnergyTimer == 0) {
		// Max energy draws "MAX"
		if(playerWeapon[currentWeapon].energy == hudMaxEnergy) {
			for(uint8_t i = 0; i < 5; i++) {
				memcpy(tileData[XP_BAR+i+3], &TS_HudMax.tiles[i * TSIZE], TILE_SIZE);
			}
			hudEnergy = hudMaxEnergy;
			hudEnergyDest = hudEnergyPixel;
		} else {
			// Even if these values are equal, we need to redraw the bar after it flashes
			if(hudEnergyPixel > hudEnergyDest) {
				// Energy decreasing
				hudEnergyPixel--;
			} else if(hudEnergyPixel < hudEnergyDest) {
				// Energy increasing
				hudEnergyPixel++;
			}
			int16_t fillXP = hudEnergyPixel;
			for(uint8_t i = 0; i < 5; i++) {
				int16_t addrXP = min(fillXP*TSIZE, 7*TSIZE);
				if(addrXP < 0) addrXP = 0;
				memcpy(tileData[XP_BAR+i+3], &TS_HudBar.tiles[addrXP + 8*TSIZE], TILE_SIZE);
				fillXP -= 8;
			}
			if(hudEnergyPixel == hudEnergyDest) {
				// Finished increasing / decreasing energy
				hudEnergy = playerWeapon[currentWeapon].energy;
			} else {
				hudEnergyTimer = 3;
			}
		}
	} else if(hudEnergyTimer == 2) {
		// Flashing while increasing / decreasing
		for(uint8_t i = 0; i < 5; i++) {
			memcpy(tileData[XP_BAR+i+3], &TS_HudFlash.tiles[i * TSIZE], TILE_SIZE);
		}
	}
	// "Lv." and 1 digit for the level
	memcpy(tileData[XP_BAR+0], &SPR_TILES(&SPR_Hud2, 0, 0)[8*TSIZE], TILE_SIZE);
	memcpy(tileData[XP_BAR+1], &SPR_TILES(&SPR_Hud2, 0, 0)[9*TSIZE], TILE_SIZE);
	memcpy(tileData[XP_BAR+2], &TS_Numbers.tiles[hudLevel*TSIZE], TILE_SIZE);
	// Queue DMA transfer for level/energy display
	//for(uint8_t i = 0; i < 8; i++)
	//DMA_queueDma(DMA_VRAM, (uint32_t)tileData[XP_BAR+i], (TILE_HUDINDEX+4+i*4)*TILE_SIZE, 16, 2);
	DMA_queueDma(DMA_VRAM, (uint32_t)tileData[XP_BAR], (TILE_HUDINDEX+4)*TILE_SIZE, 16, 2);
	DMA_queueDma(DMA_VRAM, (uint32_t)tileData[XP_BAR+2], (TILE_HUDINDEX+5)*TILE_SIZE, 16, 2);
	DMA_queueDma(DMA_VRAM, (uint32_t)tileData[XP_BAR+4], (TILE_HUDINDEX+12)*TILE_SIZE, 32, 2);
}

static uint8_t hudLastWeaponIdx = 0xFF; // last known currentWeapon index, for scroll direction. 0xFF = uninitialized

void hud_refresh_weapon() {
	// Determine scroll direction from weapon index change
	// CSE2: RotationArms (next) sets gArmsEnergyX=32 (scroll from right)
	//       RotationArmsRev (prev) sets gArmsEnergyX=0  (scroll from left)
	//       GBA offset: normal=0, from right=+16, from left=-16
	// Handle wrap-around with MAX_WEAPONS=5
	uint8_t newIdx = currentWeapon;
	int8_t diff = newIdx - hudLastWeaponIdx;
	if(diff > 2) diff -= MAX_WEAPONS;      // wrap backward: 0→4 → diff=-1
	else if(diff < -2) diff += MAX_WEAPONS; // wrap forward:  4→0 → diff=1

	if(hudLastWeaponIdx == 0xFF) {
		// First load — no scroll animation
		hudScrollDir = 0;
		hudScrollOffset = 0;
	} else if(diff > 0) {
		hudScrollDir = 1;      // forward → scroll in from right
		hudScrollOffset = 16;
	} else {
		hudScrollDir = -1;     // backward → scroll in from left
		hudScrollOffset = -16;
	}
	hudLastWeaponIdx = newIdx;

	// Weapon switched — update tiles immediately (scroll anim handles positioning)
	hudWeapon = playerWeapon[currentWeapon].type;
	memcpy(tileData[WPN+0], SPR_TILES(&SPR_ArmsImage, 0, hudWeapon), TILE_SIZE*2);
	memcpy(tileData[WPN+2], &SPR_TILES(&SPR_ArmsImage, 0, hudWeapon)[TSIZE*2], TILE_SIZE*2);
	// Queue DMA transfer for icon overlay (repacked as 2x2-compatible)
	DMA_queueDma(DMA_VRAM, (uint32_t)tileData[WPN+0], TILE_SWAPICONINDEX * 16, 16, 2);
	DMA_queueDma(DMA_VRAM, (uint32_t)tileData[WPN+2], (TILE_SWAPICONINDEX + 1) * 16, 16, 2);
}

void hud_swap_weapon(uint8_t dir) {
	swapTimer = SWAP_BEGIN;
	swapDir = dir;
	sprSwap[0].x += swapDir ? 18 : -18;
	sprSwap[1].x += swapDir ? 18 : -18;
}

void hud_refresh_swap(uint8_t force) {
	if(force) swapTimer = 0;
	if(force || swapTimer == SWAP_BEGIN) {
		swapWepNum = 0;
		for(uint8_t wep = currentWeapon + 1; ; wep++) {
			if(wep >= MAX_WEAPONS) wep = 0;
			if(wep == currentWeapon) break;

			uint8_t type = playerWeapon[wep].type;
			if(type == WEAPON_NONE) continue;

			// 32x16 sprite holds 2 weapons side-by-side (16x16 each)
			// Sprite 0: cols 0-1 (weapon 0), cols 2-3 (weapon 1)
			// Sprite 1: cols 0-1 (weapon 2), cols 2-3 (weapon 3)
			// In 1D OBJ mapping with sprite width 4:
			//   TL@+0, TR@+1, BL@+4, BR@+5  (weapon at cols 0-1)
			//   TL@+2, TR@+3, BL@+6, BR@+7  (weapon at cols 2-3)
			uint16_t baseTile = TILE_EXWEPINDEX + (swapWepNum >> 1) * 8;
			uint16_t colOff = (swapWepNum & 1) * 2; // 0 for cols 0-1, 2 for cols 2-3

			const uint32_t *src = SPR_TILES(&SPR_ArmsImage, 0, type);
			// Top-left tile
			DMA_queueDma(DMA_VRAM, (uint32_t)(src + 0),  (baseTile + colOff + 0) * 16, 8, 2);
			// Top-right tile
			DMA_queueDma(DMA_VRAM, (uint32_t)(src + 8),  (baseTile + colOff + 1) * 16, 8, 2);
			// Bottom-left tile
			DMA_queueDma(DMA_VRAM, (uint32_t)(src + 16), (baseTile + colOff + 4) * 16, 8, 2);
			// Bottom-right tile
			DMA_queueDma(DMA_VRAM, (uint32_t)(src + 24), (baseTile + colOff + 5) * 16, 8, 2);

			swapWepNum++;
		}
	}
	if(swapTimer) { // Animation in progress
		sprSwap[0].x += swapDir ? -2 : 2;
		sprSwap[1].x += swapDir ? -2 : 2;
		swapTimer--;
	} else { // End of animation, reset positions
		sprSwap[0].x = 16 + 64 + 128;
		sprSwap[1].x = 16 + 96 + 128;
		sprSwap[0].size = SPRITE_SIZE(swapWepNum > 1 ? 4 : 2, 2) | (5 << 4);
		sprSwap[1].size = SPRITE_SIZE(swapWepNum > 3 ? 4 : 2, 2) | (5 << 4);
	}
}

void hud_refresh_ammo() {
	// Top half of ammo display
	hudAmmo = playerWeapon[currentWeapon].ammo;
	if(hudMaxAmmo > 0) {
		memcpy(tileData[AMMO+0], TILE_BLANK, TILE_SIZE);
		uint8_t ammoTemp = hudAmmo;
		if(ammoTemp >= 100) {
			ammoTemp -= 100;
			memcpy(tileData[AMMO+1], &TS_Numbers.tiles[1*TSIZE], TILE_SIZE);
		} else {
			memcpy(tileData[AMMO+1], TILE_BLANK, TILE_SIZE);
		}
		memcpy(tileData[AMMO+2], &TS_Numbers.tiles[div10[ammoTemp]*TSIZE], TILE_SIZE);
		memcpy(tileData[AMMO+3], &TS_Numbers.tiles[mod10[ammoTemp]*TSIZE], TILE_SIZE);
	} else { // Weapon doesn't use ammo
		memcpy(tileData[AMMO+0], TILE_BLANK, TILE_SIZE);
		memcpy(tileData[AMMO+1], TILE_BLANK, TILE_SIZE);
		memcpy(tileData[AMMO+2], &SPR_TILES(&SPR_Hud2,0,0)[18*TSIZE], TILE_SIZE*2);
	}
	// Queue DMA transfer for ammo
	//for(uint8_t i = 0; i < 4; i++)
	//	DMA_queueDma(DMA_VRAM, (uint32_t)tileData[AMMO+i], (TILE_HUDINDEX+16+i*4)*TILE_SIZE, 16, 2);
	DMA_queueDma(DMA_VRAM, (uint32_t)tileData[AMMO], (TILE_HUDINDEX+8)*TILE_SIZE, 32, 2);
	DMA_queueDma(DMA_VRAM, (uint32_t)tileData[AMMO+2], (TILE_HUDINDEX+11)*TILE_SIZE, 16, 2);
	DMA_queueDma(DMA_VRAM, (uint32_t)tileData[AMMO+4], (TILE_HUDINDEX+10)*TILE_SIZE, 32, 2);
}

void hud_refresh_maxammo() {
	// Bottom half of ammo display
	hudMaxAmmo = playerWeapon[currentWeapon].maxammo;
	memcpy(tileData[AMMO+4], &SPR_TILES(&SPR_Hud2,0,0)[20*TSIZE], TILE_SIZE);
	if(hudMaxAmmo > 0) {
		uint8_t ammoTemp = hudMaxAmmo;
		if(ammoTemp >= 100) {
			ammoTemp -= 100;
			memcpy(tileData[AMMO+5], &TS_Numbers.tiles[1*TSIZE], TILE_SIZE);
		} else {
			memcpy(tileData[AMMO+5], TILE_BLANK, TILE_SIZE);
		}
		memcpy(tileData[AMMO+6], &TS_Numbers.tiles[div10[ammoTemp]*TSIZE], TILE_SIZE);
		memcpy(tileData[AMMO+7], &TS_Numbers.tiles[mod10[ammoTemp]*TSIZE], TILE_SIZE);
	} else { // Weapon doesn't use ammo
		memcpy(tileData[AMMO+5], TILE_BLANK, TILE_SIZE);
		memcpy(tileData[AMMO+6], &SPR_TILES(&SPR_Hud2,0,0)[18*TSIZE], TILE_SIZE*2);
	}
	// Queue DMA transfer for max ammo
	for(uint8_t i = 0; i < 4; i++)
		DMA_queueDma(DMA_VRAM, (uint32_t)tileData[AMMO+i+4], (TILE_HUDINDEX+17+i*4)*TILE_SIZE, 16, 2);
}
