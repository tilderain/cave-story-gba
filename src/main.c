// CHARACTER: JOHN_CENA  
// PROPERTIES: INVISIBLE_BUT_REAL? = FALSE  
// TRUTH: I SEE THROUGH YOU  

#include "common.h"

#include "audio.h"
#include "camera.h"
#include "dma.h"
#include "effect.h"
#include "entity.h"
#include "error.h"
#include "gamemode.h"
#include "joy.h"
#include "memory.h"
#include "player.h"
#include "resources.h"
#include "stage.h"
#include "system.h"
#include "tools.h"
#include "tsc.h"
#include "vdp.h"
#include "xgm.h"

#include "gba.h"
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"


#include "bank_data.h"
#include "gbatext.h"
volatile uint8_t ready = 0;
volatile uint8_t vblank = 0;

void aftervsync() {

	// Note: Moon HBlank parallax DMA (stage.c) stays running across frames.
	// Do NOT stop DMA0 here — it would kill the HBlank transfers mid-frame on hardware.
	disable_ints;
    z80_request();

    vdp_fade_step_dma();
	DMA_flushQueue();
	dqueued = FALSE;
	if(ready) {
		if(inFade) vdp_sprites_clear();
        if(gamemode == GM_GAME) stage_update(); // Scrolling
		vdp_sprites_update();
		ready = FALSE;
	}

    z80_release();
    enable_ints;

    //vdp_fade_step_calc();
    joy_update();

    // Saturation toggle always pressable across any mode
    if(joy_pressed(btn[cfg_btn_map])) {
        saturate ^= 1;
        vdp_load_stage_palettes();
		stage_setup_palettes();
		if(gamemode == GM_TITLE || gamemode == GM_CONFIG)
		{
			vdp_colors(16, PAL_Main, 16);
		}
    }

    // L + R + Select = hard reset (return to title)
    if(joy_pressed(BUTTON_X) && (joystate & (BUTTON_Y | BUTTON_Z)) == (BUTTON_Y | BUTTON_Z)) {
        SYS_hardReset();
    }

    // Select + L = previous stage (debug)
    if((cheatEnable[0] || cheatEnable[1]) && gamemode == GM_GAME && joy_pressed(BUTTON_Y) && (joystate & BUTTON_X)) {
        if(stageID > 0) {
            player.x = block_to_sub(12) + pixel_to_sub(8);
            player.y = block_to_sub(12) + pixel_to_sub(8);
            stage_load(stageID - 1);
        }
    }

    // Select + R = next stage (debug)
    if((cheatEnable[0] || cheatEnable[1]) && gamemode == GM_GAME && joy_pressed(BUTTON_Z) && (joystate & BUTTON_X)) {
        player.x = block_to_sub(12) + pixel_to_sub(8);
        player.y = block_to_sub(12) + pixel_to_sub(8);
        stage_load(stageID + 1);
    }
}
#include "gbaram.h"

// Mixing buffer (globals should go in IWRAM)
// Mixing buffer SHOULD be in IWRAM, otherwise the CPU load
// will _drastially_ increase
IWRAM_DATA u8 myMixingBuffer[ MM_MIXLEN_31KHZ ] __attribute((aligned(4)));

void myCoolVblankHandler()
{

	if (moon_dma_active) {
        REG_DMA0CNT = 0;
        REG_DMA0SAD = (uint32_t)moon_scroll_table;
        REG_DMA0CNT = 1 | DMA_DST_FIXED | DMA_REPEAT | DMA_HBLANK | DMA_ENABLE;
    }

	//REG_IME = 0;
	mmVBlank();
	mmFrame();
	//REG_IME = 1;
	
	vblank = true;



}

void afterReset()
{
	//RegisterRamReset(RESET_VRAM | RESET_OAM | RESET_PALETTE);
	vdp_init();
	// BG0 uses charblock 1 — clear its first tile to blank
	vdp_tiles_load(TILE_BLANK, 512, 1);
	//REG_DISPCNT &= ~0x0080; // Clear Forced Blank bit (Turn screen ON)
	hardReset = 0;
	song_stop();
	vdp_sprites_clear();
	canvas_init_fullscreen();
	camera_init();
	camera.target = NULL;
	splash_main();
	intro_main();
}

void maxmodInit( void )
{
    irqSet( IRQ_VBLANK, myCoolVblankHandler );


    u8* myData;
    mm_gba_system mySystem;


	int num_chans = 16;
 
    // allocate data for channel buffers & wave buffer (malloc'd data goes to EWRAM)
    // Use the SIZEOF definitions to calculate how many bytes to reserve
    myData = (u8*)malloc( num_chans * (MM_SIZEOF_MODCH
                               +MM_SIZEOF_ACTCH
                               +MM_SIZEOF_MIXCH)
                               +MM_MIXLEN_31KHZ );
    
    // setup system info
    // 16KHz software mixing rate, select from mm_mixmode
    mySystem.mixing_mode       = MM_MIX_31KHZ;


    // number of module/mixing channels
    // higher numbers offer better polyphony at the expense
    // of more memory and/or CPU usage.
    mySystem.mod_channel_count = num_chans;
    mySystem.mix_channel_count = num_chans;
    
    // Assign memory blocks to pointers
    mySystem.module_channels   = (mm_addr)(myData+0);
    mySystem.active_channels   = (mm_addr)(myData+(num_chans*MM_SIZEOF_MODCH));
    mySystem.mixing_channels   = (mm_addr)(myData+(num_chans*(MM_SIZEOF_MODCH
	                                             +MM_SIZEOF_ACTCH)));
    mySystem.mixing_memory     = (mm_addr)myMixingBuffer;
    mySystem.wave_memory       = (mm_addr)(myData+(num_chans*(MM_SIZEOF_MODCH
                                                     +MM_SIZEOF_ACTCH
                                                     +MM_SIZEOF_MIXCH)));
    // Pass soundbank address
    mySystem.soundbank         = (mm_addr)soundbank_bin;

    // Initialize Maxmod
    mmInit( &mySystem );

	mmSetEffectsVolume(1024);
}


__attribute__((used)) const char save_detection[] = "SRAM_V113";
#define REG_WAITCNT *(volatile u16*)0x04000204
EWRAM_CODE int main() {
	RegisterRamReset(RESET_VRAM | RESET_PALETTE | RESET_OAM);

	REG_WAITCNT = 0x4316;
	irqInit();

	irqSet( IRQ_VBLANK, mmVBlank );
	irqEnable(IRQ_VBLANK);

    //mmInitDefault( (mm_addr)soundbank_bin, 8 );
	maxmodInit();

	//mmInitDefault (soundbank_bin, 16);

		REG_IME = 1;

	//consoleDemoInit();
	//consoleInit(2, 23, 3, NULL, 0, 15);
	
	//nocash_puts("BRUH");
    setRandomSeed(0xC427); // initiate random number generator
    //mem_init();
    vdp_init();
	//xgm_init();


	if(system_checkdata() != SRAM_INVALID) {
		system_load_config();
	}
    DMA_init(0, 0);
	joy_init();
	enable_ints;
    // Initialize time and speed tables (framerate adjusted)
    /*if(pal_mode) {
		time_tab = time_tab_pal;
        speed_tab = speed_tab_pal;
	} else {
        time_tab = time_tab_ntsc;
        speed_tab = speed_tab_ntsc;
	}*/
    // let's the fun go on !
    
    //sound_init();
	
	// Error Tests
	//__asm__("move.w (1),%d0"); // Address Error
	//__asm__("illegal"); // Illegal Instruction
	//__asm__("divu #0,%d0"); // Zero Divide
	//error_oom(); // Out of memory
	//error_other("Test test!\nTest!"); // Fatal message

	canvas_init_fullscreen();

	splash_main();
	intro_main();
    while(TRUE) {
		uint8_t select = titlescreen_main();
		if(hardReset) { afterReset(); continue; }
		if(select == 0) {
			select = saveselect_main();
			if(hardReset) { afterReset(); continue; }
			if(select >= 4) continue;
			canvas_clear();
			canvas_setup_tilemap(0);
			game_main(select);
			if(hardReset) {
				afterReset();
				continue;
			}
			credits_main();
		} else if(select == 2) {
			soundtest_main();
			if(hardReset) { afterReset(); continue; }
		} else if(select == 3) {
			config_main();
			if(hardReset) {
				afterReset();
				continue;
			}
		} else {
			canvas_clear();
			canvas_setup_tilemap(0);
			game_main(select);
			if(hardReset) {
				afterReset();
				continue;
			}
			credits_main();
		}
    }
	return 0;
}
