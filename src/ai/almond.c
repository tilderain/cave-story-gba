#include "ai_common.h"

void onspawn_waterlevel(Entity *e) {
	e->alwaysActive = TRUE;
	water_entity = e;
	e->state = WL_CALM;
	e->y += pixel_to_sub(4);
	if(stageID == STAGE_CORE) e->y += pixel_to_sub(8);
	e->y_mark = e->y;
	e->y_speed = 0xFF;
}

void ai_waterlevel(Entity *e) {
	switch(e->state) {
		case WL_CALM:	// calm waves around set point
		{
			e->y_speed += (e->y < e->y_mark) ? 4 : -4;
			LIMIT_Y(0xFF);
		}
		break;
		case WL_CYCLE:	// wait 1000 ticks, then rise all the way to top come down and repeat
		{
			e->state = WL_DOWN;
			e->timer = 0;
		}
		/* fallthrough */
		case WL_DOWN:
		{
			e->y_speed += (e->y < e->y_mark) ? 4 : -4;
			LIMIT_Y(0x200);
			if (++e->timer > 1000) {
				e->state = WL_UP;
			}
		}
		break;
		case WL_UP:			// rise all the way to top then come back down
		{
			e->y_speed += (e->y > 0) ? -4 : 4;
			LIMIT_Y(0x200);
			// when we reach the top return to normal level
			if (e->y < (64<<9)) {
				e->state = WL_CYCLE;
			}
		}
		break;
		case WL_STAY_UP:	// rise quickly all the way to top and stay there
		{
			e->y_speed += (e->y > 0) ? -4 : 4;
			// Asymmetric limit: rise fast (-0x200), settle slow (+0x100)
			if (e->y_speed < -0x200) e->y_speed = -0x200;
			if (e->y_speed > 0x100) e->y_speed = 0x100;
		}
		break;
	}
	e->y += e->y_speed;
}

void onspawn_shutter(Entity *e) {
	e->flags |= NPC_SHOOTABLE | NPC_INVINCIBLE;
	e->alwaysActive = TRUE;
	e->y += pixel_to_sub(8);
	if(e->type == OBJ_SHUTTER_BIG) {
		e->x += pixel_to_sub(8);
	}
}

// common code to both Shutter AND Lift
void ai_shutter(Entity *e) {
	switch(e->state) {
		case 10:
		{
			// allow hitting the stuck shutter no. 4
			//e->eflags &= ~(NPC_SHOOTABLE | NPC_INVINCIBLE);
			e->flags &= ~(NPC_SHOOTABLE | NPC_INVINCIBLE);
			// Store direction in timer2 instead of flipping the sprite
			e->timer2 = e->dir;
			e->dir = 0;
			e->state++;
		} /* fallthrough */
		case 11:
		{
			e->x_next = e->x;
			e->y_next = e->y;
			switch(e->timer2) {
				case 0: e->x_next = e->x - 0x80; break;
				case 1: e->x_next = e->x + 0x80; break;
				case 2: e->y_next = e->y - 0x80; break;
				case 3: e->y_next = e->y + 0x80; break;
			}
			if (!e->timer) {
				camera_shake(20);
				sound_play(SND_QUAKE, 5);
				e->timer = 8;
			} else e->timer--;
			e->x = e->x_next;
			e->y = e->y_next;
		}
		break;
		case 20: // tripped by script when Shutter_Big closes fully
		{
			SMOKE_AREA((e->x>>CSF) - 16, (e->y>>CSF) + 12, 32, 8, 4);
			e->state = 21;
		}
		break;
	}
}

void ai_shutter_stuck(Entity *e) {
	if(!e->state) {
		e->state++;
		e->x -= pixel_to_sub(4);
		e->y += pixel_to_sub(2);
		e->flags |= NPC_SHOOTABLE;
	}
	// when you shoot shutter 4, you're actually shooting us, but we want them
	// to think they're shooting the regular shutter object, so go invisible
	e->hidden = TRUE;
}

// the damaged robot which wakes up right before the Almond battle
void ai_almond_robot(Entity *e) {
	switch(e->state) {
		case 0:
		{
			e->frame = 0;
			e->state++;
		}
		break;
		case 10:	// blows up
		{
			sound_play(SND_BIG_CRASH, 5);
			SMOKE_AREA((e->x>>CSF) - 12, (e->y>>CSF) - 12, 24, 24, 8);
			e->state = STATE_DELETE;
		}
		break;
		case 20:	// flashes
		{
			e->frame = 1;
			e->state++;
		}
		break;
	}
}
