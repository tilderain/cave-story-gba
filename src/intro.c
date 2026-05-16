#include "common.h"

#include "audio.h"
#include "camera.h"
#include "dma.h"
#include "effect.h"
#include "entity.h"
#include "hud.h"
#include "joy.h"
#include "npc.h"
#include "player.h"
#include "resources.h"
#include "sheet.h"
#include "stage.h"
#include "string.h"
#include "system.h"
#include "tables.h"
#include "tsc.h"
#include "vdp.h"
#include "weapon.h"
#include "window.h"

#include "gamemode.h"

#include "gba.h"

#include "gbatext.h"

#include "bank_data.h"

void intro_main() {
	gamemode = GM_INTRO;
	vdp_colors(16, PAL_Intro, 16);
	start_fadein_sweep(1);
	// Init some subsystems used
	sheets_load_intro();
	vdp_sprites_clear();
	effects_init();
	camera_init();
    camera.target = NULL;

	int32_t cam_locked_x = player.x + pixel_to_sub(150);
	int32_t cam_locked_y = player.y + pixel_to_sub(150);
	camera_set_position(cam_locked_x, cam_locked_y);

	tsc_init();
	stage_load(STAGE_INTRO);
	//vdp_colors_next(16, PAL_Intro.data, 16);
	tsc_call_event(100);
	
	// Create "Studio Pixel Presents" text
	//vdp_puts(VDP_PLAN_A, "Studio Pixel Presents", 10, 8);

	uint16_t timer = 0;
	//oldstate = 0;
	bool canvas_init = false;
	while(++timer <= 400 && !joy_pressed(BUTTON_C) && !joy_pressed(BUTTON_START)) {
		if(timer == 150) {
            //vdp_text_clear(VDP_PLAN_A, 10, 6, 20);
            //vdp_text_clear(VDP_PLAN_A, 10, 8, 20);
			canvas_clear();
		}

    	vdp_puts(VDP_PLAN_A, "Based on the Work of", 10, 6);
   		vdp_puts(VDP_PLAN_A, "    Studio Pixel    ", 10, 8);

		update_fadein_sweep();
		// Fade completion zeroes BG3 tilemap; re-init canvas to restore text visibility
		if(gFade.mode == 0 && canvas_init == false) 
		{
			canvas_init_fullscreen();
			canvas_init = true;
		}

		camera.target = NULL;
		int32_t cam_locked_x = player.x + pixel_to_sub(165);
		int32_t cam_locked_y = player.y + pixel_to_sub(140);
		
		camera_set_position(cam_locked_x, cam_locked_y);

		vdp_hscroll(VDP_PLAN_A, -sub_to_pixel(camera.x) + SCREEN_HALF_W + 8);
		vdp_vscroll(VDP_PLAN_A, sub_to_pixel(camera.y) - SCREEN_HALF_H - 8);
		vdp_hscroll(VDP_PLAN_B, -sub_to_pixel(camera.x) / 4 + SCREEN_HALF_W);
		vdp_vscroll(VDP_PLAN_B, sub_to_pixel(camera.y) / 4 - SCREEN_HALF_H);
	
		camera_update();
		tsc_update();
		entities_update(TRUE);
		effects_update(); // Draw Smoke
		ready = TRUE;
		vdp_vsync();
		aftervsync();
	}

	if(!joy_pressed(BUTTON_C) && !joy_pressed(BUTTON_START))
	{
		do_fadeout_sweep(1);
		gFade.bMask = 0; 
		gFade.mode = 0;
	}



	vdp_text_clear(VDP_PLAN_A, 10, 8, 20);
	vdp_fade(NULL, PAL_FadeOut, 4, FALSE);
	entities_clear();
	effects_clear();


}
