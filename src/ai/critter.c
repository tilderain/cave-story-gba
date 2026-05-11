#include "ai_common.h"

enum CritterState {
	STATE_WAITING = 0, 
	STATE_ATTENTION = 10, 
	STATE_HOPPING = 20, 
	STATE_FLYING = 30
};

void ai_critter(Entity *e) {
	// Set shootable during hover/fall for power critter (cleared otherwise)
	if(e->type == OBJ_POWER_CRITTER) {
		if(e->state == STATE_FLYING || e->state == STATE_FLYING+1)
			e->flags |= NPC_SHOOTABLE;
		else
			e->flags &= ~NPC_SHOOTABLE;
	}

	if(e->x_speed < 0) {
		uint8_t block = blk(e->x, -e->hit_box.left, e->y, 0);
		if(block == 0x41 || block == 0x43 || block == 0x44) {
			e->x_speed = 0;
			e->x += 0xFF;
		}
	} else if(e->x_speed > 0) {
		uint8_t block = blk(e->x, e->hit_box.right, e->y, 0);
		if(block == 0x41 || block == 0x43 || block == 0x44) {
			e->x_speed = 0;
			e->x -= 0xFF;
		}
	}
	if(e->y_speed < 0) {
		e->x_next = e->x;
		e->y_next = e->y;
		if(collide_stage_ceiling(e)) {
			e->y = e->y_next;
			e->y_speed = 0xFF;
		}
	} else if(e->y_speed > 0) {
		e->x_next = e->x;
		e->y_next = e->y;
		e->grounded = collide_stage_floor(e);
		e->y = e->y_next;
	}

	switch(e->state) {
		case STATE_WAITING:
		{
			if(e->type == OBJ_CRITTER_SHOOTING_PURPLE) {
                e->flags |= NPC_SOLID;
            } else {
                e->flags &= ~NPC_SOLID;
            }
			if(e->type == OBJ_POWER_CRITTER) e->attack = 2;
			e->frame = 0;
			e->timer = 0;
			e->x_speed = e->y_speed = 0;
			e->state++;
		} /* fallthrough */
		case STATE_WAITING+1:
		{
			if(e->damage_time || (PLAYER_DIST_X(e, pixel_to_sub(0x60)) && PLAYER_DIST_Y(e, pixel_to_sub(0x40)))) {
				FACE_PLAYER(e);
				e->state = STATE_ATTENTION;
				e->frame = 1;
			}
		}
		break;
		case STATE_ATTENTION:
		{
			if(++e->timer > 8) {
				e->state = STATE_HOPPING;
				e->frame = 2;
				e->timer = 0;
				e->grounded = FALSE;
				e->y_speed = -0x5FF;
				if(e->type == OBJ_CRITTER_FLYING) e->y_speed = -0x4CC;
				sound_play(SND_ENEMY_JUMP, 5);
			}
		}
		break;
		case STATE_HOPPING:
		{
			// Keep trying to move after hitting a wall
			if(e->type != OBJ_CRITTER_SHOOTING_PURPLE) MOVE_X(0xFF);
			
			// Handle critters that can hover
			int hover_y = (e->type == OBJ_POWER_CRITTER) ? 0x200 : 0x100; // Power Critter waits longer
			if(e->y_speed > hover_y && (e->type == OBJ_CRITTER_FLYING ||
					e->type == OBJ_POWER_CRITTER || e->type == OBJ_CRITTER_SHOOTING_PURPLE)) {
				e->state = STATE_FLYING;
				e->frame = 3;
				e->timer = 0;
				e->y_mark = e->y;
			} else if(e->grounded) {
				e->state = STATE_WAITING;
			}
		}
		break;

		case STATE_FLYING:
		{
			e->timer++;
			if(e->type == OBJ_POWER_CRITTER) {
				// CSE2 ActNpc024 state 4: altitude-hold hover toward player
				FACE_PLAYER(e);
				++e->animtime;
				if(++e->frame > 5) e->frame = 3;
				
				// CSE2 technically uses sound 110 here instead of 109, but SND_CRITTER_FLY is close enough
				if((e->timer & 3) == 1) sound_play(SND_CRITTER_FLY, 2);
				
				// Exit: horizontal wall collision or 100 frames timeout
				if((e->x_speed == 0 && e->timer > 4) || e->timer > 100) {
					e->state = STATE_FLYING + 1;
					e->attack = 12;
					e->frame = 2;
					e->x_speed /= 2; // Was missing! Causes it to violently slide down walls.
				} else {
					// Accelerate toward player
					if (e->x < player.x) e->x_speed += 0x20;
					else e->x_speed -= 0x20;
					
					// Altitude hold
					if(e->y > e->y_mark) e->y_speed -= 0x10;
					else e->y_speed += 0x10;
					
					// Caps
					if(e->x_speed > 0x200) e->x_speed = 0x200;
					if(e->x_speed < -0x200) e->x_speed = -0x200;
					if(e->y_speed > 0x200) e->y_speed = 0x200;
					if(e->y_speed < -0x200) e->y_speed = -0x200;
				}
			} else {

				// CSE2 ActNpc028 / ActNpc147 state 4: hover toward player
				FACE_PLAYER(e);

				// Exit: wall collision or timeout
				// CSE2: flag&7 || act_wait > 100 (flying) / > 60 (purple)
				// GBA AI zeroes x_speed on wall hit, check after ramp-up
				uint16_t hover_timeout = (e->type == OBJ_CRITTER_SHOOTING_PURPLE) ? 60 : 100;
				
				// Purple critter stays still, so x_speed == 0 doesn't mean it hit a wall.
				// Only use the x_speed == 0 check for the moving green flying critter.
				int hit_wall = (e->type == OBJ_CRITTER_FLYING && e->x_speed == 0 && e->timer > 4);

				if (hit_wall || e->timer > hover_timeout) {
					e->attack = 3;
					e->frame = 2;
					e->x_speed /= 2;
					e->state++;
					break;
				}
				// Sound every 4 frames (CSE2: act_wait % 4 == 1)
				if ((e->timer & 3) == 1)
					sound_play(SND_CRITTER_FLY, 2);

				// Bounce off floor (CSE2: flag&8 → ym = -0x200)
				if (e->grounded)
					e->y_speed = -0x200;

				// Animate every frame, frames 3-5 (CSE2: ++ani_wait > 0 → always)
				if (++e->frame > 5) e->frame = 3;

				// Shooting for purple critter (CSE2 ActNpc147: act_wait % 30 == 6)
				if (e->type == OBJ_CRITTER_SHOOTING_PURPLE && e->timer % 30 == 6) {
					sound_play(SND_EM_FIRE, 3);
					Entity *shot = entity_create(e->x, e->y, OBJ_CRITTER_SHOT, 0);
					THROW_AT_TARGET(shot, player.x, player.y, 0x200);
				}
			}
		}
		break;
		case STATE_FLYING+1:
		{
			if(e->type == OBJ_POWER_CRITTER) {
				// CSE2 ActNpc024 case 5: fall, land with quake
				if(e->grounded) {
					e->attack = 2;
					e->state = STATE_WAITING;
					e->x_speed = 0;
					sound_play(SND_QUAKE, 5);
					camera_shake(30);
				}
			} else {
				if(e->grounded) {
					e->state = STATE_WAITING;
				}
			}
		}
		break;
	}
	
	// CSE2: gravity when not in hover state for flying/power/purple types
	int hover = (e->state == STATE_FLYING && (e->type == OBJ_POWER_CRITTER ||
	             e->type == OBJ_CRITTER_FLYING || e->type == OBJ_CRITTER_SHOOTING_PURPLE));
	
	if (!hover) {
		if(!e->grounded && e->y_speed < 0x5FF) {
			if(e->type == OBJ_CRITTER_HOPPING_RED)
				e->y_speed += 0x55;
			else if(e->type == OBJ_POWER_CRITTER)
				e->y_speed += 0x20; // Power critter uses floaty gravity!
			else
				e->y_speed += 0x40;
		}
	} else if (e->type != OBJ_POWER_CRITTER) { // Skip Power Critter so we don't apply altitude hold twice!
		// CSE2 hover physics: altitude hold + horizontal accel toward player
		if (e->type == OBJ_CRITTER_FLYING) {
			// Horizontal accel toward player
			if (e->x < player.x) e->x_speed += 0x20;
			else e->x_speed -= 0x20;
		}
		
		if (e->x_speed > 0x200) e->x_speed = 0x200;
		if (e->x_speed < -0x200) e->x_speed = -0x200;

		// Altitude hold for all hover types (CSE2: ym ±= 0x10 toward tgt_y)
		if (e->y > e->y_mark) e->y_speed -= 0x10;
		else e->y_speed += 0x10;
		
		if (e->y_speed > 0x200) e->y_speed = 0x200;
		if (e->y_speed < -0x200) e->y_speed = -0x200;
	}
	
	e->x += e->x_speed;
	e->y += e->y_speed;
}
