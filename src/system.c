#include "common.h"

#include "audio.h"
#include "bank_data.h"
#include "dma.h"
#include "entity.h"
#include "gamemode.h"
#include "joy.h"
#include "memory.h"
#include "player.h"
#include "resources.h"
#include "sram.h"
#include "stage.h"
#include "string.h"
#include "tsc.h"
#include "tools.h"
#include "vdp.h"
#include "weapon.h"
#include "window.h"
#include "xgm.h"

#include "system.h"

#include "gba.h"

// ASCII string "CSMD" used as a sort of checksum to verify save data exists
#define STR_CSMD 0x43534D44
// When save data is not found "TEST" is written to verify SRAM is usable
#define STR_TEST 0x54455354

#define CFG_MAGIC 0x12345678

// Supports 0-4095, official game uses 0-4000
#define FLAGS_LEN 128

typedef struct { uint8_t hour, minute, second, frame; } Time;

static const uint8_t *FileList[] = {
	LS_00, LS_01, LS_02, LS_03, LS_04, LS_05, LS_06, LS_07,
	LS_08, LS_09, LS_10, LS_11, LS_12, LS_13, LS_14, LS_15,
	LS_16, LS_17, LS_18, LS_19, LS_20, LS_21,
};

uint8_t cfg_btn_jump = 5;
uint8_t cfg_btn_shoot = 4;
uint8_t cfg_btn_ffwd = 6;
uint8_t cfg_btn_rswap = 8;
uint8_t cfg_btn_lswap = 9;
uint8_t cfg_btn_map = 10;
uint8_t cfg_btn_pause = 7;

uint8_t cfg_language = 0;
uint8_t cfg_ffwd = TRUE;
uint8_t cfg_updoor = FALSE;
uint8_t cfg_hellquake = TRUE;
uint8_t cfg_iframebug = TRUE;
uint8_t cfg_60fps = FALSE;

uint8_t cfg_force_btn = 0;
uint8_t cfg_msg_blip = TRUE;

uint8_t cfg_music_mute = FALSE;
uint8_t cfg_sfx_mute = FALSE;

uint8_t sram_file = 0;
uint8_t sram_state = SRAM_UNCHECKED;

// Put the counter tiles in a "blank spot" of Hell's tileset
#define TILE_COUNTERINDEX TILE_AIRINDEX
//416
//uint8_t counterShow = FALSE;
//uint8_t counterTick = FALSE;
Time time, counter;
static const VDPSprite counterSpriteNTSC[2] = {
	{ 
		.x = 0x80 + 16, 
		.y = 0x80 + 8, 
		.size = SPRITE_SIZE(4,1),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_COUNTERINDEX),
	}, { 
		.x = 0x80 + 48, 
		.y = 0x80 + 8, 
		.size = SPRITE_SIZE(4,1),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_COUNTERINDEX+4),
	},
};
static const VDPSprite counterSpritePAL[2] = {
	{ 
		.x = 0x80 + 16, 
		.y = 0x80 + 16, 
		.size = SPRITE_SIZE(4,1),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_COUNTERINDEX),
	}, { 
		.x = 0x80 + 48, 
		.y = 0x80 + 16, 
		.size = SPRITE_SIZE(4,1),
		.attr = TILE_ATTR(PAL0,1,0,0,TILE_COUNTERINDEX+4),
	},
};
uint8_t counter_decisec;
static const uint8_t decisec[2][60] = {
	{ // NTSC
		0,0,0,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,3,4,4,4,4,4,4,
		5,5,5,5,5,5,6,6,6,6,6,6,7,7,7,7,7,7,8,8,8,8,8,8,9,9,9,9,9,9 
	},{ // PAL
		0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4,
		5,5,5,5,5,6,6,6,6,6,7,7,7,7,7,8,8,8,8,8,9,9,9,9,9
	},
};

uint32_t flags[FLAGS_LEN];
uint32_t skip_flags = 0;

static uint8_t LS_readByte(uint8_t file, uint32_t addr);
static uint16_t LS_readWord(uint8_t file, uint32_t addr);
static uint32_t LS_readLong(uint8_t file, uint32_t addr);

void system_set_flag(uint16_t flag, uint8_t value) {
	if(value) flags[flag>>5] |= ((uint32_t)1)<<(flag&31);
	else flags[flag>>5] &= ~(((uint32_t)1)<<(flag&31));
}

uint8_t system_get_flag(uint16_t flag) {
	return (flags[flag>>5] & (((uint32_t)1)<<(flag&31))) > 0;
}

void system_set_skip_flag(uint16_t flag, uint8_t value) {
	if(value) skip_flags |= (((uint32_t)1)<<flag);
	else skip_flags &= ~(((uint32_t)1)<<flag);
}

uint8_t system_get_skip_flag(uint16_t flag) {
	return (skip_flags & (((uint32_t)1)<<flag)) > 0;
}

static void counter_draw_minute() {
	DMA_queueDma(DMA_VRAM, (uint32_t) &TS_Numbers.tiles[div10[counter.minute] << 3], 
			(TILE_COUNTERINDEX+1) << 5, 16, 2);
	DMA_queueDma(DMA_VRAM, (uint32_t) &TS_Numbers.tiles[mod10[counter.minute] << 3], 
			(TILE_COUNTERINDEX+2) << 5, 16, 2);
}

static void counter_draw_second() {
	DMA_queueDma(DMA_VRAM, (uint32_t) &TS_Clock.tiles[(counter.second & 1) << 3], 
			(TILE_COUNTERINDEX) << 5, 16, 2);
	DMA_queueDma(DMA_VRAM, (uint32_t) &TS_Numbers.tiles[div10[counter.second] << 3], 
			(TILE_COUNTERINDEX+4) << 5, 16, 2);
	DMA_queueDma(DMA_VRAM, (uint32_t) &TS_Numbers.tiles[mod10[counter.second] << 3], 
			(TILE_COUNTERINDEX+5) << 5, 16, 2);
}

static void counter_draw_decisecond() {
	DMA_queueDma(DMA_VRAM, (uint32_t) &TS_Numbers.tiles[counter_decisec << 3], 
			(TILE_COUNTERINDEX+7) << 5, 16, 2);
}

void system_draw_counter() {
	counter_draw_minute();
	DMA_queueDma(DMA_VRAM, (uint32_t) &TS_Clock.tiles[2 << 3], 
			(TILE_COUNTERINDEX+3) << 5, 16, 2);
	counter_draw_second();
	DMA_queueDma(DMA_VRAM, (uint32_t) &TS_Clock.tiles[3 << 3], 
			(TILE_COUNTERINDEX+6) << 5, 16, 2);
	counter_draw_decisecond();
	const VDPSprite *spr = pal_mode ? counterSpritePAL : counterSpriteNTSC;
	vdp_sprites_add(spr, 2);
}

void system_update() {
	if(++time.frame >= FPS) {
		time.frame = 0;
		if(++time.second >= 60) {
			time.second = 0;
			if(++time.minute >= 60) {
				// Limit 255:59:59.9
				if(!(++time.hour)) {
					time.hour = 255;
					time.minute = 59;
					time.second = 59;
					time.frame = FPS - 1;
				} else {
					time.minute = 0;
				}
			}
		}
	}
	if(playerEquipment & EQUIP_CLOCK) {
		// Stop the counter during scripts that lock controls or freeze the gameplay.
		// Notice how the counter stops when talking to Ballos and the ghost puppy,
		// but not in the scene when Ballos transforms (since you can move).
		if(!gameFrozen && !controlsLocked && !paused) {
			if(++counter.frame >= FPS) {
				if(++counter.second >= 60) {
					// Limit 99'59"9
					if(++counter.minute > 99) {
						counter.minute = 99;
						counter.second = 59;
						counter.frame = FPS - 1;
					} else {
						counter.second = 0;
						counter.frame = 0;
						counter_draw_minute();
						counter_draw_second();
					}
				} else {
					counter.frame = 0;
					counter_draw_second();
				}
			}
			if(counter_decisec != decisec[pal_mode][counter.frame]) {
				counter_decisec = decisec[pal_mode][counter.frame];
				counter_draw_decisecond();
			}
		}
		// Don't display the counter when a message window is open on top
		// because it overlaps the text making it hard to read
		if(!(window_is_open() && windowOnTop)) {
			const VDPSprite *spr = pal_mode ? counterSpritePAL : counterSpriteNTSC;
		vdp_sprites_add(spr, 2);
		}
	}
}

void system_new() {
	time.hour = time.minute = time.second = time.frame = 0;
	counter.hour = counter.minute = counter.second = counter.frame = 0;
	for(uint16_t i = 0; i < FLAGS_LEN; i++) flags[i] = 0;
	if(sram_state == SRAM_INVALID) system_set_flag(FLAG_DISABLESAVE, TRUE);
	stage_load(13);
	tsc_call_event(GAME_START_EVENT);
}

static uint8_t checksum_verify(uint8_t file_num, uint8_t is_backup) {
	uint16_t save_loc = SRAM_FILE_START + (SRAM_FILE_LEN * file_num);
	uint16_t checksum_loc = SRAM_CHECKSUM_POS + (file_num * 8);
	if(is_backup) {
		save_loc += SRAM_BACKUP_OFFSET;
		checksum_loc += 4;
	}

	uint32_t checksum = SRAM_readLong(checksum_loc);
	uint32_t recheck = 0x10101010;
	for(uint16_t i = save_loc; i < save_loc + SRAM_BACKUP_OFFSET; i += 4) {
		recheck += SRAM_readLong(i);
	}
	return checksum == recheck;
}

static void checksum_write(uint8_t file_num, uint8_t is_backup) {
	uint16_t save_loc = SRAM_FILE_START + (SRAM_FILE_LEN * file_num);
	uint16_t checksum_loc = SRAM_CHECKSUM_POS + (file_num * 8);
	if(is_backup) {
		save_loc += SRAM_BACKUP_OFFSET;
		checksum_loc += 4;
	}

	uint32_t checksum = 0x10101010;
	for(uint16_t i = save_loc; i < save_loc + SRAM_BACKUP_OFFSET; i += 4) {
		checksum += SRAM_readLong(i);
	}
	SRAM_writeLong(SRAM_CHECKSUM_POS + (file_num * 8), checksum);
}

void system_save() {
	if(sram_file >= SRAM_FILE_MAX) return;
	if(sram_state == SRAM_INVALID) return;
	
	// Start of save data in SRAM
	uint16_t loc_start = SRAM_FILE_START + SRAM_FILE_LEN * sram_file;
	// Counters to increment while reading/writing
	uint16_t loc = loc_start;

	disable_ints;
	z80_request();
	SRAM_enable();
	
	SRAM_writeLong(loc, STR_CSMD);					loc += 4;
	SRAM_writeWord(loc, stageID); 					loc += 2;
	SRAM_writeWord(loc, song_get_playing()); 		loc += 2;
	SRAM_writeWord(loc, sub_to_block(player.x)); 	loc += 2;
	SRAM_writeWord(loc, sub_to_block(player.y)); 	loc += 2;
	SRAM_writeWord(loc, playerMaxHealth);			loc += 2;
	SRAM_writeWord(loc, player.health);				loc += 2;
	SRAM_writeWord(loc, currentWeapon);				loc += 2;
	SRAM_writeWord(loc, playerEquipment);			loc += 2;
	SRAM_writeByte(loc, time.hour);					loc++;
	SRAM_writeByte(loc, time.minute);				loc++;
	SRAM_writeByte(loc, time.second);				loc++;
	SRAM_writeByte(loc, time.frame);				loc++;
	// Weapons
	for(uint8_t i = 0; i < MAX_WEAPONS; i++) {
		SRAM_writeByte(loc, playerWeapon[i].type);		loc++;
		SRAM_writeByte(loc, playerWeapon[i].level);		loc++;
		SRAM_writeWord(loc, playerWeapon[i].energy);	loc += 2;
		SRAM_writeWord(loc, playerWeapon[i].maxammo);	loc += 2;
		SRAM_writeWord(loc, playerWeapon[i].ammo);		loc += 2;
	}
	// Inventory
	for(uint8_t i = 0; i < MAX_ITEMS; i++) {
		SRAM_writeByte(loc, playerInventory[i]); loc++;
	}
	// Teleporter locations
	for(uint8_t i = 0; i < 8; i++) {
		SRAM_writeWord(loc, teleportEvent[i]); loc += 2;
	}
	// Flags
	for (uint16_t i = 0; i < FLAGS_LEN; i++) {
		SRAM_writeLong(loc, flags[i]); loc += 4;
	}
	// Checksum
	checksum_write(sram_file, FALSE);

	// Backup
	loc = loc_start;
	while(loc < loc_start + SRAM_FILE_LEN / 2) {
		uint32_t dat = SRAM_readLong(loc);
		SRAM_writeLong(loc + SRAM_BACKUP_OFFSET, dat);
		loc += 4;
	}
	checksum_write(sram_file, TRUE);
	
	SRAM_disable();
	z80_release();
	enable_ints;
}

void system_peekdata(uint8_t index, SaveEntry *file) {
	uint16_t loc = SRAM_FILE_START + SRAM_FILE_LEN * index;

	disable_ints;
	z80_request();
	SRAM_enableRO();
	
	// Save exists
	uint32_t magic = SRAM_readLong(loc); loc += 4;
	if(magic != STR_CSMD) {
		file->used = FALSE;
		SRAM_disable();
		z80_release();
        enable_ints;
		return;
	}
	// Checksum failed?
	if(!checksum_verify(index, FALSE)) {
		// Checksum for backup failed too?
		if(!checksum_verify(index, TRUE)) {
			file->used = FALSE;
			SRAM_disable();
			z80_release();
            enable_ints;
			return;
		}
		// Load backup
		loc += SRAM_BACKUP_OFFSET;
	}
	
	file->used = TRUE;
	file->stage_id = SRAM_readWord(loc); 	loc += 8;
	file->max_health = SRAM_readWord(loc); 	loc += 2;
	file->health = SRAM_readWord(loc); 		loc += 6;
	file->hour = SRAM_readByte(loc);		loc++;
	file->minute = SRAM_readByte(loc);		loc++;
	file->second = SRAM_readByte(loc);		loc += 2;
	file->weapon[0] = SRAM_readByte(loc);	loc += 8;
	file->weapon[1] = SRAM_readByte(loc);	loc += 8;
	file->weapon[2] = SRAM_readByte(loc);	loc += 8;
	file->weapon[3] = SRAM_readByte(loc);	loc += 8;
	file->weapon[4] = SRAM_readByte(loc);	loc += 8;
	
	SRAM_disable();
	z80_release();
	enable_ints;
}

void system_load(uint8_t index) {
	if(index >= SRAM_FILE_CHEAT) {
		system_load_levelselect(index - SRAM_FILE_CHEAT);
		return;
	}
	player_init();
	sram_file = index;

	disable_ints;
	z80_request();
	SRAM_enableRO();
	// Start of save data in SRAM
	uint16_t loc_start = SRAM_FILE_START + SRAM_FILE_LEN * sram_file;
	// Checksum failed?
	if(!checksum_verify(index, FALSE)) {
		// Load backup
		loc_start += SRAM_BACKUP_OFFSET;
	}
	// Counters to increment while reading/writing
	uint16_t loc = loc_start;
	
	// Test magic
	uint32_t magic = SRAM_readLong(loc); loc += 4;
	if(magic != STR_CSMD) {
		// Empty
		SRAM_disable();
		z80_release();
        enable_ints;
		system_new();
		return;
	}
	
	uint16_t rid = SRAM_readWord(loc);			loc += 2;
	uint8_t song = SRAM_readWord(loc);			loc += 2;
	player.x = block_to_sub(SRAM_readWord(loc)) + (8<<CSF); loc += 2;
	player.y = block_to_sub(SRAM_readWord(loc)) + (8<<CSF); loc += 2;
	playerMaxHealth = SRAM_readWord(loc); 		loc += 2;
	player.health = SRAM_readWord(loc); 		loc += 2;
	currentWeapon = SRAM_readWord(loc); 		loc += 2;
	playerEquipment = SRAM_readWord(loc); 		loc += 2;
	time.hour = SRAM_readByte(loc); 			loc++;
	time.minute = SRAM_readByte(loc);			loc++;
	time.second = SRAM_readByte(loc);			loc++;
	time.frame = SRAM_readByte(loc);			loc++;
	counter.hour = counter.minute = counter.second = counter.frame = 0;
	// Weapons
	for(uint8_t i = 0; i < MAX_WEAPONS; i++) {
		playerWeapon[i].type = SRAM_readByte(loc);		loc++;
		playerWeapon[i].level = SRAM_readByte(loc);		loc++;
		playerWeapon[i].energy = SRAM_readWord(loc);	loc += 2;
		playerWeapon[i].maxammo = SRAM_readWord(loc);	loc += 2;
		playerWeapon[i].ammo = SRAM_readWord(loc);		loc += 2;
	}
	// Inventory
	for(uint8_t i = 0; i < MAX_ITEMS; i++) {
		playerInventory[i] = SRAM_readByte(loc); loc++;
	}
	// Teleporter locations
	for(uint8_t i = 0; i < 8; i++) {
		teleportEvent[i] = SRAM_readWord(loc); loc += 2;
	}
	// Flags
	for (uint16_t i = 0; i < FLAGS_LEN; i++) {
		flags[i] = SRAM_readLong(loc); loc += 4;
	}
	SRAM_disable();
	z80_release();
	enable_ints;
	
	stage_load(rid);
	song_play(song);
}

void system_copy(uint8_t from, uint8_t to) {
	uint16_t loc_from     = SRAM_FILE_START + SRAM_FILE_LEN * from;
	uint16_t loc_from_end = loc_from + SRAM_FILE_LEN;
	uint16_t loc_to       = SRAM_FILE_START + SRAM_FILE_LEN * to;

	disable_ints;
	z80_request();
	SRAM_enable();

	// Copy data
	while(loc_from < loc_from_end) {
		uint32_t data = SRAM_readLong(loc_from);
		SRAM_writeLong(loc_to, data);
		loc_from += 4; loc_to += 4;
	}
	// Copy checksum
	loc_from = SRAM_CHECKSUM_POS + from * 8;
	loc_to = SRAM_CHECKSUM_POS + to * 8;
	uint32_t checksum = SRAM_readLong(loc_from);
	SRAM_writeLong(loc_to, checksum);
	checksum = SRAM_readLong(loc_from + 4);
	SRAM_writeLong(loc_to + 4, checksum);

	SRAM_disable();
	z80_release();
	enable_ints;
}

void system_delete(uint8_t index) {
	uint16_t loc = SRAM_FILE_START + SRAM_FILE_LEN * index;

	disable_ints;
	z80_request();
	SRAM_enable();
	
	SRAM_writeLong(loc, 0); // Erase the "CSMD" magic to invalidate file
	SRAM_writeLong(loc + SRAM_BACKUP_OFFSET, 0); // the backup too
	
	SRAM_disable();
	z80_release();
	enable_ints;
}

static void get_language() {
    if     (LANGUAGE[0] == 'E' && LANGUAGE[1] == 'N') cfg_language = LANG_EN;
    else if(LANGUAGE[0] == 'E' && LANGUAGE[1] == 'S') cfg_language = LANG_ES;
    else if(LANGUAGE[0] == 'F' && LANGUAGE[1] == 'R') cfg_language = LANG_FR;
    else if(LANGUAGE[0] == 'D' && LANGUAGE[1] == 'E') cfg_language = LANG_DE;
    else if(LANGUAGE[0] == 'I' && LANGUAGE[1] == 'T') cfg_language = LANG_IT;
    else if(LANGUAGE[0] == 'P' && LANGUAGE[1] == 'T') cfg_language = LANG_PT;
    else if(LANGUAGE[0] == 'B' && LANGUAGE[1] == 'R') cfg_language = LANG_BR;
    else if(LANGUAGE[0] == 'J' && LANGUAGE[1] == 'A') cfg_language = LANG_JA;
    else if(LANGUAGE[0] == 'Z' && LANGUAGE[1] == 'H') cfg_language = LANG_ZH;
    else if(LANGUAGE[0] == 'K' && LANGUAGE[1] == 'O') cfg_language = LANG_KO;
}

void system_load_config() {
	get_language();
	
	uint16_t loc = SRAM_CONFIG_POS;

	disable_ints;
	z80_request();
	SRAM_enableRO();
	
	uint32_t magic = SRAM_readLong(loc); loc += 4;
	if(magic != CFG_MAGIC) {
		// No settings saved, keep defaults
		SRAM_disable();
		z80_release();
        enable_ints;
		return;
	}
	
	cfg_btn_jump  = SRAM_readByte(loc++);
	cfg_btn_shoot = SRAM_readByte(loc++);
	cfg_btn_ffwd  = SRAM_readByte(loc++);
	cfg_btn_rswap = SRAM_readByte(loc++);
	cfg_btn_lswap = SRAM_readByte(loc++);
	cfg_btn_map   = SRAM_readByte(loc++);
	cfg_btn_pause = SRAM_readByte(loc++);
	loc++; //cfg_language  = SRAM_readByte(loc++);
	cfg_ffwd      = SRAM_readByte(loc++);
	cfg_updoor    = SRAM_readByte(loc++);
	cfg_hellquake = SRAM_readByte(loc++);
	cfg_iframebug = SRAM_readByte(loc++);
	cfg_force_btn = SRAM_readByte(loc++);
	cfg_msg_blip  = SRAM_readByte(loc++);
	cfg_music_mute= SRAM_readByte(loc++);
	cfg_sfx_mute  = SRAM_readByte(loc++);
	cfg_60fps     = SRAM_readByte(loc++);
	// Just in case
	if(cfg_force_btn > 2) cfg_force_btn = 0;
	if(cfg_music_mute > 1) cfg_music_mute = 0;
	if(cfg_sfx_mute > 1) cfg_sfx_mute = 0;
	if(cfg_60fps > 1) {
		cfg_60fps = 0;
		// In 0.6.0 Japanese was 1 but now it's Spanish
		//if(cfg_language == 1) cfg_language = LANG_JA;
	}

    /*if(pal_mode || cfg_60fps) {
        time_tab = time_tab_pal;
        speed_tab = speed_tab_pal;
    } else {
        time_tab = time_tab_ntsc;
        speed_tab = speed_tab_ntsc;
    }*/

	SRAM_disable();
	z80_release();
	enable_ints;
}

void system_save_config() {
	uint16_t loc = SRAM_CONFIG_POS;

	disable_ints;
	z80_request();
	SRAM_enable();

	SRAM_writeLong(loc, CFG_MAGIC); loc += 4;
	SRAM_writeByte(loc++, cfg_btn_jump);
	SRAM_writeByte(loc++, cfg_btn_shoot);
	SRAM_writeByte(loc++, cfg_btn_ffwd);
	SRAM_writeByte(loc++, cfg_btn_rswap);
	SRAM_writeByte(loc++, cfg_btn_lswap);
	SRAM_writeByte(loc++, cfg_btn_map);
	SRAM_writeByte(loc++, cfg_btn_pause);
	SRAM_writeByte(loc++, cfg_language);
	SRAM_writeByte(loc++, cfg_ffwd);
	SRAM_writeByte(loc++, cfg_updoor);
	SRAM_writeByte(loc++, cfg_hellquake);
	SRAM_writeByte(loc++, cfg_iframebug);
	SRAM_writeByte(loc++, cfg_force_btn);
	SRAM_writeByte(loc++, cfg_msg_blip);
	SRAM_writeByte(loc++, cfg_music_mute);
	SRAM_writeByte(loc++, cfg_sfx_mute);
	SRAM_writeByte(loc++, cfg_60fps);

	SRAM_disable();
	z80_release();
	enable_ints;
}

// Level select is still the old style format... don't care enough to fix it
void system_load_levelselect(uint8_t file) {
	//ssf_setbank(7, 7); // Level select data is in chunk 7
	player_init();
	uint16_t rid = LS_readWord(file, 0x00);
	uint8_t song = LS_readWord(file, 0x02);
	player.x = block_to_sub(LS_readWord(file, 0x04)) + pixel_to_sub(8);
	player.y = block_to_sub(LS_readWord(file, 0x06)) + pixel_to_sub(8);
	playerMaxHealth = LS_readWord(file, 0x08);
	player.health = LS_readWord(file, 0x0A);
	currentWeapon = LS_readWord(file, 0x0C);
	playerEquipment = LS_readWord(file, 0x0E);
	time.hour = LS_readByte(file, 0x10);
	time.minute = LS_readByte(file, 0x11);
	time.second = LS_readByte(file, 0x12);
	time.frame = LS_readByte(file, 0x13);
	counter.hour = counter.minute = counter.second = counter.frame = 0;
	// Weapons
	for(uint8_t i = 0; i < MAX_WEAPONS; i++) {
		playerWeapon[i].type = LS_readByte(file, 0x20 + i*8);
		playerWeapon[i].level = LS_readByte(file, 0x21 + i*8);
		playerWeapon[i].energy = LS_readWord(file, 0x22 + i*8);
		playerWeapon[i].maxammo = LS_readWord(file, 0x24 + i*8);
		playerWeapon[i].ammo = LS_readWord(file, 0x26 + i*8);
	}
	// Inventory (0x20)
	for(uint8_t i = 0; i < MAX_ITEMS; i++) {
		playerInventory[i] = LS_readByte(file, 0x60 + i);
	}
	// Teleporter locations
	for(uint8_t i = 0; i < 8; i++) {
		teleportEvent[i] = LS_readWord(file, 0x80 + i*2);
	}
	// Flags
	for (uint16_t i = 0; i < FLAGS_LEN; i++) {
		flags[i] = LS_readLong(file, 0x100 + i * 4);
	}
	stage_load(rid);
	song_play(song);
	sram_file = SRAM_FILE_CHEAT + file;
}

uint8_t system_checkdata() {
	sram_state = SRAM_INVALID; // Default invalid
	// Read a specific spot in SRAM
	SRAM_enableRO();
	uint32_t test = SRAM_readLong(SRAM_TEST_POS);
	// Anything there?
	if(test == STR_TEST) {
		// Read first file pos for CSMD magic
		test = SRAM_readLong(SRAM_FILE_START); 
		if(test == STR_CSMD) {
			// Save data exists, this is the only state that should allow selecting "continue"
			sram_state = SRAM_VALID_SAVE;
		} else {
			// No save data, but SRAM was validated before
			sram_state = SRAM_VALID_EMPTY;
		}
	}
	SRAM_disable();
	if(sram_state == SRAM_INVALID) {
		// Nothing is there, try to write "TEST" and re-read
		SRAM_enable();
		SRAM_writeLong(SRAM_TEST_POS, STR_TEST);
		SRAM_disable();
		SRAM_enableRO();
		test = SRAM_readLong(SRAM_TEST_POS);
		SRAM_disable();
		if(test == STR_TEST) {
			// Test passed, game can be saved but not loaded
			sram_state = SRAM_VALID_EMPTY;
			// Clear the hell timer just in case (thanks Fusion)
			SRAM_enable();
			for(uint16_t i = 0; i < 5; i++) SRAM_writeLong(SRAM_COUNTER_POS + i*4, 0);
			SRAM_disable();
		} else {
			// Test failed, SRAM is unusable
			sram_state = SRAM_INVALID;
		}
	}
	return sram_state;
}

uint32_t system_counter_ticks() {
	// Counter considers a second 50 frames, even for NTSC
	return SPEED_8(counter.frame) + counter.second*50 + counter.minute*60*50;
}

uint32_t system_load_counter() {
	uint8_t buffer[20];
	uint32_t *result = (uint32_t*)buffer;
	// Read 20 bytes of 290.rec from SRAM
	SRAM_enableRO();
	for(uint16_t i = 0; i < 20; i++) {
		buffer[i] = SRAM_readByte(SRAM_COUNTER_POS + i);
	}
	SRAM_disable();
	// Apply key
	for(uint16_t i = 0; i < 4; i++) {
		uint8_t key = buffer[16 + i];
		uint16_t j = i * 4;
		buffer[j] -= key;
		buffer[j+1] -= key;
		buffer[j+2] -= key;
		buffer[j+3] -= (key / 2);
	}
	// Ticks should be nonzero
	if(!result[0]) {
		return 0xFFFFFFFF;
	}
	// Make sure tick values match
	if((result[0] != result[1]) || (result[0] != result[2]) || (result[0] != result[3])) {
		return 0xFFFFFFFF;
	}
	// Convert LE -> BE
	uint32_t rtn = (((uint32_t)(buffer[0]))<<24) + (((uint32_t)(buffer[1]))<<16) 
				 + (((uint32_t)(buffer[2]))<<8) + buffer[3];
	// Split out so the time can be displayed on the title screen
	counter.frame = rtn % 50;
	counter.second = (rtn / 50) % 60;
	counter.minute = rtn / 50 / 60;

    return rtn;
}

void system_save_counter(uint32_t ticks) {
	// Don't save counter if cheating
	if(iSuckAtThisGameSHIT || sram_file >= SRAM_FILE_CHEAT) return;

	uint8_t buffer[20];
	uint32_t *result = (uint32_t*)buffer;
	uint8_t *tickbuf = (uint8_t*)&ticks;
	// Generate random key
	result[4] = random();
	// Write to buffer BE -> LE 4 times
	for(uint16_t i = 0; i < 4; i++) {
		result[i] = (((uint32_t)(tickbuf[0]))<<24) + (((uint32_t)(tickbuf[1]))<<16) 
				  + (((uint32_t)(tickbuf[2]))<<8) + tickbuf[3];
	}
	// Apply the key to each
	for(uint16_t i = 0; i < 4; i++) {
		uint8_t key = buffer[16 + i];
		uint16_t j = i * 4;
		buffer[j] += key;
		buffer[j+1] += key;
		buffer[j+2] += key;
		buffer[j+3] += (key / 2);
	}
	// Write to SRAM
	SRAM_enable();
	for(uint16_t i = 0; i < 20; i++) {
		SRAM_writeByte(SRAM_COUNTER_POS + i, buffer[i]);
	}
	SRAM_disable();
}

void system_format_sram() {
	SRAM_enable();
	for(uint16_t i = 0; i < 0x2000; i += 4) {
		SRAM_writeLong(i, 0);
	}
	SRAM_disable();
}

static uint8_t LS_readByte(uint8_t file, uint32_t addr) {
	return FileList[file][addr];
}

static uint16_t LS_readWord(uint8_t file, uint32_t addr) {
	return (LS_readByte(file, addr) << 8) + LS_readByte(file, addr+1);
}

static uint32_t LS_readLong(uint8_t file, uint32_t addr) {
	return (((uint32_t)(LS_readWord(file, addr))) << 16) + LS_readWord(file, addr+2);
}
