#include "common.h"

#include "audio.h"
#include "bank_data.h"
#include "dma.h"
#include "joy.h"
#include "resources.h"
#include "string.h"
#include "system.h"
#include "tables.h"
#include "vdp.h"
#include "gamemode.h"

#include "gba.h"
#include "gbatext.h"

#define DRAW_BYTE(b, x, y) { \
	char byte[4]; \
	sprintf(byte, "%02hX", b); \
	vdp_puts_shadow(VDP_PLAN_A, byte, x, y); \
}

enum { STOPPED, PLAYING, PAUSED };

// Music 0x00 - 0x7F, Sound Effects 0x80 - 0xFF
#define FIRST_SOUND 0x80

// Track # and name
void draw_track_info(uint8_t track) {
	DRAW_BYTE(track, 9, 6); // Positioned right after "Track: "
	vdp_text_clear(VDP_PLAN_A, 2, 8, 28);
	vdp_puts_shadow(VDP_PLAN_A, song_info_xm[track].name, 2, 8);
}

void draw_status(uint8_t status) {
	vdp_text_clear(VDP_PLAN_A, 2, 4, 10);
	switch(status) {
		case STOPPED: vdp_puts_shadow(VDP_PLAN_A, "Stopped", 2, 4); break;
		case PLAYING: vdp_puts_shadow(VDP_PLAN_A, "Playing", 2, 4); break;
		case PAUSED:  vdp_puts_shadow(VDP_PLAN_A, "Paused ", 2, 4); break;
	}
}

extern const uint8_t PAT_SndTest[];
extern const uint8_t PAT_SndTest_END[];
#define PAT_SndTest_SIZE ((uint32_t)(PAT_SndTest_END - PAT_SndTest))

void soundtest_main() {
    gamemode = GM_SOUNDTEST;
    
    uint8_t track = 0;
    uint8_t status = STOPPED, oldstatus = STOPPED;
    uint8_t old_snes_ost = 255; // force first draw
    
    vdp_set_display(FALSE);
    
    vdp_sprites_clear();
    vdp_map_clear(VDP_PLAN_A);
    vdp_map_clear(VDP_PLAN_B);
    vdp_map_clear(VDP_PLAN_W);

    canvas_init_fullscreen(); 

    uint32_t *dst_tiles = (uint32_t *)(0x06000000 + (1 * 16384));
    DMA3COPY(PAT_SndTest, dst_tiles, (5696 / 4) | COPY32);

    uint16_t *gba_map = (uint16_t*)MAP_BASE_ADR(BASE_BACK);
    DMA3COPY(MAP_SndTest, gba_map, (1024 / 2) | COPY32);

    BG_OFFSET[0].x = 0;
    BG_OFFSET[0].y = 0;

    draw_status(status);
    vdp_puts_shadow(VDP_PLAN_A, "Sound Test", 10, 2);
    vdp_puts_shadow(VDP_PLAN_A, "Track: ", 2, 6);
    {
        char str[32];
        sprintf(str, "%s-Play %s-Stop %s-Quit",
            btnName[cfg_btn_jump], btnName[cfg_btn_shoot], btnName[cfg_btn_pause]);
        vdp_puts_shadow(VDP_PLAN_A, str, 2, 17);
    }
    draw_track_info(track);
    
    vdp_colors_bg(0, PAL_SndTest, 16);
    vdp_set_display(TRUE);

    song_stop();
    oldstate = 65535;
    while(TRUE) {
        vdp_colors_bg(0, PAL_SndTest, 16);

        if(joy_pressed(BUTTON_LEFT)) {
            if(track == 0) track = FIRST_SOUND + SOUND_COUNT - 1;
            else if(track == FIRST_SOUND) track = SONG_COUNT - 1;
            else track--;
            draw_track_info(track);
        } else if(joy_pressed(BUTTON_RIGHT)) {
            if(track == SONG_COUNT - 1) track = FIRST_SOUND;
            else if(track == FIRST_SOUND + SOUND_COUNT - 1) track = 0;
            else track++;
            draw_track_info(track);
        }
        
        // Toggle SNES OST with up/down
        if(joy_pressed(BUTTON_UP) || joy_pressed(BUTTON_DOWN)) {
            snes_ost_enabled = !snes_ost_enabled;
            // If a track is playing, restart it with new setting
            if(status == PLAYING && track < SONG_COUNT)
                song_play(track);
        }

        if(joy_pressed(btn[cfg_btn_jump])) {
            if(track < SONG_COUNT) {
                song_play(track);
                status = PLAYING;
            } else if(track >= FIRST_SOUND && track < FIRST_SOUND + SOUND_COUNT){
                sound_play(track - FIRST_SOUND, 5);
            }
        } else if(joy_pressed(btn[cfg_btn_shoot])) {
            song_stop();
            status = STOPPED;
        }
        
        if(joy_pressed(btn[cfg_btn_pause])) return;
        
        if(status != oldstatus) {
            draw_status(status);
            oldstatus = status;
        }

        // Redraw OST label only when it changes
        if(snes_ost_enabled != old_snes_ost) {
            vdp_puts_shadow(VDP_PLAN_A, snes_ost_enabled ? "OST: SNES   " : "OST: Original", 2, 10);
            old_snes_ost = snes_ost_enabled;
        }
        
        ready = TRUE;
        vdp_vsync();
        aftervsync();
    }
}