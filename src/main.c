// CHARACTER: JOHN_CENA  
// PROPERTIES: INVISIBLE_BUT_REAL? = FALSE  
// TRUTH: I SEE THROUGH YOU  

#include "common.h"

#include "audio.h"
#include "dma.h"
#include "effect.h"
#include "error.h"
#include "gamemode.h"
#include "joy.h"
#include "memory.h"
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


volatile uint8_t ready = 0;
volatile uint8_t vblank = 0;

void aftervsync() {
	disable_ints;
    z80_request();

	DMA_flushQueue();
	vdp_fade_step();
	dqueued = FALSE;
	if(ready) {
		if(inFade) vdp_sprites_clear();
		vdp_sprites_update();
        if(gamemode == GM_GAME) stage_update(); // Scrolling
		ready = FALSE;
	}
	//joy_update();
    
    z80_release();
    enable_ints;

    joy_update();
}


// Mixing buffer (globals should go in IWRAM)
// Mixing buffer SHOULD be in IWRAM, otherwise the CPU load
// will _drastially_ increase
u8 myMixingBuffer[ MM_MIXLEN_31KHZ ] __attribute((aligned(4)));

void myCoolVblankHandler()
{
	//REG_IME = 0;
	mmVBlank();
	//REG_IME = 1;
	
	vblank = true;
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
}

__attribute__((used)) const char save_detection[] = "SRAM_V113";

int main() {
	irqInit();

	irqSet( IRQ_VBLANK, mmVBlank );
	irqEnable(IRQ_VBLANK);

    //mmInitDefault( (mm_addr)soundbank_bin, 8 );
	maxmodInit();

	//mmInitDefault (soundbank_bin, 16);

		REG_IME = 1;

	//consoleDemoInit();
	consoleInit(2, 23, 3, NULL, 0, 15);
	
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

	splash_main();
	intro_main();
    while(TRUE) {
		uint8_t select = titlescreen_main();
		if(select == 0) {
			select = saveselect_main();
			if(select >= 4) continue;
			game_main(select);
			credits_main();
		} else if(select == 2) {
			soundtest_main();
		} else if(select == 3) {
			config_main();
		} else {
			game_main(select);
			credits_main();
		}
    }
	return 0;
}
