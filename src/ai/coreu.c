#include "ai_common.h"

#define CFRONT				5
#define CBACK				6
#define CFACE				7
#define CSUE				8
#define CMISERY				9

#define savedhp		id
#define angle		jump_time
#define substate	hit_box.top

#define NUM_ROTATORS	4
#define NUM_BBOXES		4
#define BB_TARGET		3

// Hitbox target offsets from main core (in pixels)
static const struct {
	int8_t x, y;
} core_bboxes[NUM_BBOXES] = {
	{ 0,  -32 },	// top shield
	{ 28,  0  },	// right shield
	{ 4,   32 },	// bottom shield
	{ -28, 4  },	// face target
};

// Global rotator and bbox entity arrays
static Entity *rotator[NUM_ROTATORS];
static Entity *bbox[NUM_BBOXES];

enum CORE_STATES {
	CR_FightBegin		= 20,		// scripted
	CR_FaceClosed		= 200,
	CR_FaceSkull		= 210,
	CR_FaceTeeth		= 220,
	CR_FaceDoom			= 230,
	
	CR_Defeated			= 500,		// scripted
	CR_Exploding		= 1000
};

enum FACE_STATES {
	FC_Closed		= 0,
	FC_Skull		= 1,
	FC_Teeth		= 2,
	FC_Mouth		= 3
};

enum ROTR_STATES {
	RT_Spin_Closed			= 10,
	RT_Spin_Open			= 20,
	RT_Spin_Slow_Closed		= 30,
	RT_Spin_Fast_Closed		= 40
};

// Minicore frame indeces
static const uint16_t mframeindex[4] = {
	TILE_BACKINDEX, 		// Back - dark
	TILE_BACKINDEX + 12,	// Back - bright
	TILE_BACKINDEX + 24,	// Bottom - dark
	TILE_BACKINDEX + 28,	// Bottom - bright
};

// Prototypes
static void SpawnPellet(uint8_t angle);
static void RunHurtFlash(uint16_t timer);
static uint8_t RunDefeated(Entity *e);
static void DrawMinicoreBack(Entity *e);
static void FadeMinicore(Entity *e);
static void SpawnFaceSmoke(void);
static Entity *create_rotator(int init_angle, int front_half);
static void SetRotatorStates(int newstate);
static void set_bbox_shootable(uint8_t enable);
static void move_bboxes(void);
static void CreateSpinner(int x, int y);

/*
	Main core body:
		non-shootable when closed
		invulnerable when open
		starsolid's appear just behind the little dots
		
		when you shoot the face the starsolid hits at just past his nose.
		
		when face is open it does block the face from shooting below,
		but there is still a spot that you can hurt it from below.
		
		four always-dark minicores that spin around it, they also seem
		to be switching their z-order.
*/

void onspawn_undead_core(Entity *e) {
	// Flag 1340 is set after the Undead Core is defeated. Then there is a scene with
	// Momo and Itoh at the helicopter, and this stage gets reloaded again.
	// So anyways we need this here, otherwise there is gonna be an Unundead Core
	if(system_get_flag(1340)) {
		e->state = STATE_DELETE;
		return;
	}
	e->alwaysActive = TRUE;
	e->hurtSound = SND_CORE_HURT;
	e->health = 700;
	e->x = pixel_to_sub(624);
	e->y = pixel_to_sub(128);
	e->event = 1000;	// defeated script
	
	// create rear rotators
	//rotator[2] = create_rotator(0, 1);
	//rotator[3] = create_rotator(0x80, 1);

	// create rear rotators (behind core)
	rotator[2] = create_rotator(0, 1);
	rotator[3] = create_rotator(0x80, 1);

	// create front & back
	pieces[CFRONT] = entity_create(0, 0, OBJ_UDCORE_FRONT, 0);
	pieces[CFRONT]->display_box = (bounding_box) { 32, 48, 32, 48 };

	pieces[CBACK] = entity_create(0, 0, OBJ_UDCORE_BACK, 0);
	pieces[CBACK]->display_box = (bounding_box) { 40, 48, 40, 48 };

	// create face
	pieces[CFACE] = entity_create(0, 0, OBJ_UDCORE_FACE, 0);
	pieces[CFACE]->state = FC_Closed;
	pieces[CFACE]->hit_box = (bounding_box) { 24, 14, 24, 14 };
	pieces[CFACE]->display_box = (bounding_box) { 28, 16, 28, 16 };
	pieces[CFACE]->health = 1000;
	pieces[CFACE]->hurtSound = SND_ENEMY_HURT_COOL;

	// create front rotators (in front of core)
	rotator[0] = create_rotator(0, 0);
	rotator[1] = create_rotator(0x80, 0);

	// create hitbox targets
	for(int i=0;i<NUM_BBOXES;i++) {
		bbox[i] = entity_create(0, 0, OBJ_UD_HITBOX, 0);
		bbox[i]->state = i;  // index used by ai_ud_hitbox
	}

	// Set up hitboxes: body zones are invulnerable, face target is not shootable yet
	set_bbox_shootable(FALSE);
	
	// Upload some tile data for the minicore sprites into the background section
	
	SHEET_LOAD(&SPR_MUCoreBack, 2, 12, mframeindex[0], 1, 0,0, 0,1);
	SHEET_LOAD(&SPR_MUCoreBottom, 2, 4, mframeindex[2], 1, 0,0, 0,1);
}

void ai_undead_core(Entity *e) {
	if (!e->health && RunDefeated(e)) return;
	
	switch(e->state) {
		// fight begin (scripted)
		case CR_FightBegin:
		{
			e->state = CR_FaceSkull;
			e->dir = 0;

			SetRotatorStates(RT_Spin_Slow_Closed);
			SpawnFaceSmoke();
		}
		break;
		
		// face closed
		case CR_FaceClosed:
		{
			e->state++;
			e->timer = 0;

			pieces[CFACE]->state = FC_Closed;
			pieces[CFRONT]->frame = 0;		// closed
			RunHurtFlash(0);

			set_bbox_shootable(FALSE);
			SetRotatorStates(RT_Spin_Closed);
			SpawnFaceSmoke();

			// Close the mouth charge sound
			// CutNoise equivalent done via sound_play
		} /* fallthrough */
		case CR_FaceClosed+1:
		{
			e->timer++;
			
			if (e->dir || e->frame > 0 || e->health < 200) {
				if (e->timer > 200) {
					e->timer2++;
					sound_play(SND_CORE_THRUST, 5);
					
					// select attack mode
					if (e->health < 200) {
						e->state = CR_FaceDoom;
					} else if (e->timer2 > 2) {
						e->state = CR_FaceTeeth;
					} else {
						e->state = CR_FaceSkull;
					}
				}
			}
		}
		break;
		
		// face open/skull
		case CR_FaceSkull:
		{
			e->state++;
			e->timer = 0;

			pieces[CFACE]->state = FC_Skull;
			pieces[CFRONT]->frame = 1;
			SpawnFaceSmoke();

			e->savedhp = e->health;
			set_bbox_shootable(TRUE);
		} /* fallthrough */
		case CR_FaceSkull+1:
		{
			e->timer++;
			RunHurtFlash(e->timer);

			if ((e->timer % 100) == 1) {
				CURLY_TARGET_HERE(e);
			}

			if (e->timer < 300) {
				if ((e->timer % 120) == 1) {
					SpawnPellet(1);
				} else if ((e->timer % 120) == 61) {
					SpawnPellet(0);
				}
			}
			
			if (e->timer > 400 || (e->savedhp - e->health) > 50) {
				e->state = CR_FaceClosed;
			}
		}
		break;
		
		// face open/teeth
		case CR_FaceTeeth:
		{
			e->state++;
			e->timer = 0;

			pieces[CFACE]->state = FC_Teeth;
			pieces[CFRONT]->frame = 1;
			SpawnFaceSmoke();

			SetRotatorStates(RT_Spin_Open);
			camera_shake(100);

			e->savedhp = e->health;
			set_bbox_shootable(TRUE);
		} /* fallthrough */
		case CR_FaceTeeth+1:
		{
			e->timer++;
			RunHurtFlash(e->timer);

			// fire spinners from rotators
			if ((e->timer % 40) == 1) {
				sound_play(SND_FUNNY_EXPLODE, 5);
				// Pick a random rotator and spawn spinners from its position
				uint8_t ri = random() & 3;
				if (rotator[ri]) {
					CreateSpinner(rotator[ri]->x - (16<<CSF), rotator[ri]->y);
				}
			}

			if (e->timer > 400 || (e->savedhp - e->health) > 150 || e->health < 200) {
				e->state = CR_FaceClosed;
			}
		}
		break;
		
		// face open/mouth: blasts of doom
		case CR_FaceDoom:
		{
			e->state++;
			e->timer = 0;

			pieces[CFACE]->state = FC_Mouth;
			pieces[CFRONT]->frame = 1;
			SpawnFaceSmoke();
			SetRotatorStates(RT_Spin_Fast_Closed);

			sound_play(SND_FUNNY_EXPLODE, 5);

			// spawn a whole bunch of crazy spinners from the face
			CreateSpinner(pieces[CFACE]->x - (16<<CSF), pieces[CFACE]->y);
			CreateSpinner(pieces[CFACE]->x, pieces[CFACE]->y - (16<<CSF));
			CreateSpinner(pieces[CFACE]->x, pieces[CFACE]->y + (16<<CSF));

			e->savedhp = e->health;
			set_bbox_shootable(TRUE);
		} /* fallthrough */
		case CR_FaceDoom+1:
		{
			e->timer++;
			RunHurtFlash(e->timer);

			if ((e->timer % 100) == 1) {
				CURLY_TARGET_HERE(e);
			}

			if ((e->timer % 120) == 1) {
				SpawnPellet(1);
			} else if ((e->timer % 120) == 61) {
				SpawnPellet(0);
			}
		}
		break;
	}
	
	// move back and forth
	if (e->state >= CR_FightBegin && e->state < CR_Defeated) {
		if (e->x < block_to_sub(19))
			e->dir = 1;
		
		if (e->x > block_to_sub(stageWidth - 1))
			e->dir = 0;
		
		ACCEL_X(4);
	}
	
	// spawn minicore platforms
	switch(e->state) {
		case CR_FaceClosed+1:
		case CR_FaceTeeth+1:
		case CR_FaceSkull+1:
		case CR_FaceDoom+1:
		{
			e->jump_time++;
			
			// upper platforms
			if (e->jump_time == 75) {
				entity_create(block_to_sub(stageWidth) + 40,
							 block_to_sub(1 + (random() & 3)), OBJ_UDMINI_PLATFORM, 0);
			}
			
			// lower platforms
			if (e->jump_time == 150) {
				e->jump_time = 0;
				entity_create(block_to_sub(stageWidth) + 40,
							 block_to_sub(11 + (random() & 3)), OBJ_UDMINI_PLATFORM, 0);
				
				break;
			}
		}
		break;
	}
	
	LIMIT_X(0x80);
	LIMIT_Y(0x80);
	
	e->x += e->x_speed;
	e->y += e->y_speed;
}

// spawn smoke puffs from face that come when face opens/closes
static void SpawnFaceSmoke(void)
{
	camera_shake(20);
	SMOKE_AREA(pieces[CFACE]->x>>CSF, pieces[CFACE]->y>>CSF, 64, 48, 12);
}

// spit a "pellet" shot out of the face. That's what I'm calling the flaming lava-rock
// type things that are thrown out and trail along the ceiling or floor.
// dir=1 shoots upward (y offset -16), dir=0 shoots downward (y offset +16)
static void SpawnPellet(uint8_t dir) {
	int32_t yoff = dir ? -(16<<CSF) : (16<<CSF);
	entity_create(bossEntity->x - (32<<CSF), bossEntity->y + yoff, OBJ_UD_PELLET,
				  dir ? NPC_OPTION2 : 0);
}

// Swap palette back and forth between orange and normal blue color
static void RunHurtFlash(uint16_t timer) {
	uint16_t pal = (pieces[CFACE]->damage_time && (timer & 2)) ? PAL3 : PAL2;
	for(uint8_t i = 0; i < pieces[CFRONT]->sprite_count; i++) {
		sprite_pal(pieces[CFRONT]->sprite[i], pal);
	}
	for(uint8_t i = 0; i < pieces[CBACK]->sprite_count; i++) {
		sprite_pal(pieces[CBACK]->sprite[i], pal);
	}
}

static uint8_t RunDefeated(Entity *e) {
	switch(e->state) {
		// defeated (descending)
		case CR_Defeated:
		{
			e->state++;
			e->timer = 0;
			e->x_speed = 0;
			e->y_speed = 0;

			pieces[CFACE]->state = FC_Closed;
			pieces[CFRONT]->frame = 0;		// pieces[CFRONT] closed
			SetRotatorStates(RT_Spin_Slow_Closed);

			camera_shake(20);
			effect_smoke_burst(e->x>>CSF, e->y>>CSF, 8, 8);

			entities_clear_by_type(OBJ_UDMINI_PLATFORM);
			entities_clear_by_type(OBJ_UD_SPINNER);
			entities_clear_by_type(OBJ_UD_SPINNER_TRAIL);
			set_bbox_shootable(FALSE);
		} /* fallthrough */
		case CR_Defeated+1:
		{
			effect_smoke_burst(e->x>>CSF, e->y>>CSF, 8, 1);
			
			e->x_speed = 0x40;
			e->y_speed = 0x80;
			
			if (++e->timer > 200) {
				e->state = CR_Exploding;
				e->x_speed = 0;
				e->y_speed = 0;
				e->timer = 0;
			}
		}
		break;
		
		// defeated (exploding)
		case CR_Exploding:
		{
			camera_shake(100);
			e->timer++;
			
			if ((e->timer & 7) == 0)
				sound_play(SND_MISSILE_HIT, 5);
			
			effect_smoke_burst(e->x>>CSF, e->y>>CSF, 144, 1);
			effect_create_misc(EFF_BOOMFLASH, e->x >> CSF, e->y >> CSF, FALSE);
			
			if (e->timer > 100) {
				sound_play(SND_EXPLOSION1, 5);
				SCREEN_FLASH(3);
				// Delete this stuff now while the screen is white
				entities_clear_by_type(OBJ_MISERY_MISSILE);
				pieces[CFRONT]->state = STATE_DELETE;
				pieces[CBACK]->state = STATE_DELETE;
				pieces[CFACE]->state = STATE_DELETE;
				for(int i=0;i<NUM_ROTATORS;i++)
					if (rotator[i]) rotator[i]->state = STATE_DELETE;
				for(int i=0;i<NUM_BBOXES;i++)
					if (bbox[i]) bbox[i]->state = STATE_DELETE;

				e->state++;
				e->timer = 0;
			}
		}
		break;
		case CR_Exploding+1:
		{
			camera_shake(40);
			if (++e->timer > 5) {
				e->state = STATE_DELETE;
				bossEntity = NULL;
				
				return 1;
			}
		}
		break;
	}
	
	return 0;
}

void ai_undead_core_face(Entity *e) {
	//e->sprite = SPR_UD_FACES;
	e->hidden = FALSE;
	e->flags = NPC_SHOWDAMAGE;
	
	switch(e->state) {
		// to "show" the closed face, we go invisible and the
		// face area of the main core shows through.
		case FC_Closed:
		{
			e->hidden = TRUE;
		}
		break;
		case FC_Skull:
		{
			e->flags |= NPC_SHOOTABLE;
			e->frame = 0;
		}
		break;
		case FC_Teeth:
		{
			e->flags |= NPC_SHOOTABLE;
			e->frame = 1;
		}
		break;
		
		// mouth blasts of doom. Once started, it's perpetual blasting
		// until told otherwise.
		case FC_Mouth:
		{
			e->state++;
			e->timer = 100;
		} /* fallthrough */
		case FC_Mouth+1:
		{
			e->flags |= NPC_SHOOTABLE;

			if (++e->timer > 300)
				e->timer = 0;

			if (e->timer > 250) {
				if ((e->timer & 0xF) == 1)
					sound_play(SND_QUAKE, 5);

				if ((e->timer & 0xF) == 7) {
					entity_create(e->x, e->y, OBJ_UD_BLAST, 0);
					sound_play(SND_LIGHTNING_STRIKE, 5);
				}
			}

			if (e->timer == 200)
				sound_play(SND_CORE_CHARGE, 5);

			if (e->timer >= 200) {
				if (e->timer & 1)
					e->frame = 3;	// mouth lit
				else
					e->frame = 2;	// mouth norm
			} else {
				e->frame = 2;	// mouth norm
			}
		}
		break;
	}
	
	// Transfer damage to main object
	if(e->health < 1000) {
		if(bossEntity->health > 1000 - e->health) {
			bossEntity->health -= 1000 - e->health;
		} else { // ded
			bossEntity->health = 0;
			tsc_call_event(1000);
		}
		e->health = 1000;
	}
	
	e->x = bossEntity->x - pixel_to_sub(28);
	e->y = bossEntity->y + pixel_to_sub(2);
}

void ai_undead_core_front(Entity *e) {
	e->x = bossEntity->x - (32<<CSF);
	e->y = bossEntity->y;
}

void ai_undead_core_back(Entity *e) {
	e->x = bossEntity->x + (40<<CSF);
	e->y = bossEntity->y;
}

// "front" refers to whether they are doing the pieces[CFRONT] (left) or rear (right)
// half of the arc; the ones marked "pieces[CFRONT]" are actually BEHIND the core.
static Entity *create_rotator(int angle, int front)
{
	Entity *e = entity_create(0, 0, OBJ_UDMINI_ROTATOR, 0);
	if (!e) return NULL;
	e->angle = angle;
	e->substate = front;
	return e;
}

// the rotators are 4 minicores that spin around the main core during the battle
// and have pseudo-3D effects. They also shoot the spinners during the teeth-face phase.
static void run_rotator(Entity *e)
{
	// each "side" covers half the rotation angle
	int a = (e->angle / 2);

	if (e->substate)
		a += 0x40;	// front half of arc
	else
		a += 0xC0;	// back half of arc

	e->x = bossEntity->x + ((int32_t)cos[(uint8_t)a] * (24<<CSF) >> 8);
	e->y = bossEntity->y + ((int32_t)sin[(uint8_t)a] * (40<<CSF) >> 8);
}

static void SetRotatorStates(int newstate)
{
	for(int i=0;i<NUM_ROTATORS;i++)
		if (rotator[i]) rotator[i]->state = newstate;
}

// extra bbox puppets/shoot targets
// only one, located at the face, is shootable, the other 3 are invulnerable shields.
static void move_bboxes(void) {
	for(int i=0;i<NUM_BBOXES;i++) {
		if (bbox[i]) {
			bbox[i]->x = bossEntity->x + (core_bboxes[i].x << CSF);
			bbox[i]->y = bossEntity->y + (core_bboxes[i].y << CSF);
		}
	}
}

// sets up bboxes for the Core entering shootable or non-shootable mode.
static void set_bbox_shootable(uint8_t enable) {
	uint16_t body_flags, target_flags;
	int i;

	// in shootable mode target can be hit and shields are up.
	// in non-shootable mode (when face is closed) nothing can be hit.
	if (enable) {
		body_flags = NPC_INVINCIBLE;
		target_flags = NPC_SHOOTABLE;
	} else {
		body_flags = 0;
		target_flags = 0;
	}

	for(i=0;i<NUM_BBOXES;i++) {
		if (!bbox[i]) continue;
		bbox[i]->flags &= ~(NPC_SHOOTABLE | NPC_INVINCIBLE);

		if (i == BB_TARGET)
			bbox[i]->flags |= target_flags;
		else
			bbox[i]->flags |= body_flags;
	}
}

// rotator entity onspawn
void onspawn_udmini_rotator(Entity *e) {
	e->alwaysActive = TRUE;
	e->display_box = (bounding_box) { 16, 16, 16, 16 };
	e->hit_box = (bounding_box) { 10, 10, 10, 10 };
	e->flags &= ~NPC_SHOOTABLE;
	e->frame = 3;
}

// rotator entity AI - spins around the core
void ai_udmini_rotator(Entity *e) {
	switch(e->state)
	{
		case RT_Spin_Closed:
		case RT_Spin_Slow_Closed:
		case RT_Spin_Fast_Closed:
			e->frame = 3;
			if (e->state == RT_Spin_Closed) e->angle += 2;
			else if (e->state == RT_Spin_Slow_Closed) e->angle++;
			else e->angle += 4;
			break;

		case RT_Spin_Open:
			e->frame = 2;
			e->angle += 2;
			break;

		default:
			e->frame = 0;
			break;
	}

	run_rotator(e);
	DrawMinicoreBack(e);
}

// hitbox entity AI - invisible collision targets
void ai_ud_hitbox(Entity *e) {
	move_bboxes();
}

// minicores by entrance seen before fight
void onspawn_ud_minicore(Entity *e) {
	e->alwaysActive = TRUE;
	e->display_box = (bounding_box) { 16, 16, 16, 16 };
	e->hit_box = (bounding_box) { 10, 10, 32, 16 };
	if (e->flags & NPC_OPTION2) FadeMinicore(e);
}

void ai_udmini_idle(Entity *e) {
	DrawMinicoreBack(e);
}

// these are the ones you can ride
void ai_udmini_platform(Entity *e) {
	switch(e->state) {
		case 0:
		{
			e->state = 1;
			e->y_mark = e->y;
			
			e->x_speed = -0x200;
			e->y_speed = 0x100;
			if (random() & 1) e->y_speed = -e->y_speed;
		} /* fallthrough */
		case 1:
		{
			if (e->x < -pixel_to_sub(64)) e->state = STATE_DELETE;
			
			if (e->y > e->y_mark) e->y_speed -= 0x10;
			if (e->y < e->y_mark) e->y_speed += 0x10;
			LIMIT_Y(0xFF);
			
			// when player jumps on them, they open up and start
			// moving their Y to align with the core.
			if (playerPlatform == e) {
				e->y_mark = block_to_sub(9);
				e->frame = 0;
				// Don't squish player into ceiling
				if(e->y <= block_to_sub(6)) {
					playerPlatform = NULL;
					FadeMinicore(e);
				}
			} else if(e->frame == 0) {
				e->frame = 1;
			}
		}
		break;
	}
	
	e->x += e->x_speed;
	e->y += e->y_speed;
	
	DrawMinicoreBack(e);
	
	// Fade & desolidify if we are about to squish the player
	if(e->frame < 3) {
		bounding_box c = entity_react_to_collision(&player, e);
		if(c.top && blk(player.x, 0, player.y, 10) == 0x41)
			FadeMinicore(e);
		else if(c.right && blk(player.x, -8, player.y, 0) == 0x41)
			FadeMinicore(e);
	}
}

// The face of the minicores have 4 different frames for light/dark and faces, but the
// backs just use 2 for dark/light, so we draw them separately from the engine
static void DrawMinicoreBack(Entity *e) {
	// Back split into 2 GBA-compatible sprites: 16×32 + 8×32
	uint16_t back_tile = mframeindex[e->frame >> 1];
	VDPSprite back_left = { // Back left half (16×32)
		.x = (e->x>>CSF) - (camera.x>>CSF) + SCREEN_HALF_W + 16 + 128,
		.y = (e->y>>CSF) - (camera.y>>CSF) + SCREEN_HALF_H - 16 + 128,
		.size = SPRITE_SIZE(2, 4),
		.attr = TILE_ATTR(PAL2,0,0,0,back_tile)
	};
	VDPSprite back_right = { // Back right half (8×32)
		.x = (e->x>>CSF) - (camera.x>>CSF) + SCREEN_HALF_W + 32 + 128,
		.y = (e->y>>CSF) - (camera.y>>CSF) + SCREEN_HALF_H - 16 + 128,
		.size = SPRITE_SIZE(1, 4),
		.attr = TILE_ATTR(PAL2,0,0,0,back_tile + 8)
	};
	VDPSprite bottom = { // Bottom bar (32×8)
		.x = (e->x>>CSF) - (camera.x>>CSF) + SCREEN_HALF_W - 8 + 128,
		.y = (e->y>>CSF) - (camera.y>>CSF) + SCREEN_HALF_H + 16 + 128,
		.size = SPRITE_SIZE(4, 1),
		.attr = TILE_ATTR(PAL2,0,0,0,mframeindex[2 + (e->frame >> 1)])
	};
	vdp_sprite_add(&back_left);
	vdp_sprite_add(&back_right);
	vdp_sprite_add(&bottom);
}

static void FadeMinicore(Entity *e) {
	e->flags &= ~NPC_SPECIALSOLID;
	e->frame = 3;
}

// falling lava-rock thing from Skull face
void ai_ud_pellet(Entity *e) {
	switch(e->state) {
		case 0:
		{
			e->x_speed = -0x200;
			e->state = 1;
		} /* fallthrough */
		case 1:		// falling
		{
			if (e->flags & NPC_OPTION2) {
				e->y_speed -= 0x20;
				LIMIT_Y(0x5ff);
				
				if (blk(e->x, 0, e->y, -4) == 0x41) e->state = 2;
			} else {
				e->y_speed += 0x20;
				LIMIT_Y(0x5ff);
				
				if (blk(e->x, 0, e->y, 4) == 0x41) e->state = 2;
			}
			
			ANIMATE(e, 4, 0,1);
		}
		break;
		
		case 2:		// hit ground/ceiling
		{
			sound_play(SND_MISSILE_HIT, 5);
			e->x_speed = (e->x > player.x) ? -0x3FF : 0x3FF;
			e->y_speed = 0;
			
			e->state = 3;
			e->timer = 0;
			
			e->x -= pixel_to_sub(4);
			e->y -= pixel_to_sub(4);
		} /* fallthrough */
		case 3:
		{
			ANIMATE(e, 2, 2,3,4);
			
			if ((++e->timer & 3) == 1) {
				SMOKE_AREA((e->x >> CSF) - 8, (e->y >> CSF) - 8, 16, 16, 1);
			}
				
			if (e->x < block_to_sub(1) || e->x > block_to_sub(stageWidth - 1)) {
				e->state = STATE_DELETE;
			}
		}
		break;
	}
	e->x += e->x_speed;
	e->y += e->y_speed;
}

void ai_ud_smoke(Entity *e)
{
	switch(e->state)
	{
		case 0:
		{
			// CSE2 ActNpc287: Random(-4, 4) * 0x200
			e->x_speed = (int16_t)(random() % 9 - 4) << CSF;
			e->y_speed = (int16_t)(random() % 9 - 4) << CSF;
			e->state = 1;
		} /* fallthrough */
		case 1:
		{
			e->x_speed -= e->x_speed >> 4;
			e->y_speed -= e->y_speed >> 4;

			e->x += e->x_speed;
			e->y += e->y_speed;

			e->frame++;
			if (e->frame > 2)
				e->state = STATE_DELETE;
		}
		break;
	}
}

static void CreateSpinner(int x, int y)
{
	entity_create(x, y, OBJ_UD_SPINNER, 0);
	Entity *s2 = entity_create(x, y, OBJ_UD_SPINNER, 0);
	if (s2) s2->angle = 0x80;
}

// spinny thing shot by rotators during Teeth phase.
// they come in pairs.
void ai_ud_spinner(Entity *e)
{
	if (e->x < block_to_sub(0) || e->x > block_to_sub(stageWidth))
	{
		effect_create_misc(EFF_BOOMFLASH, e->x >> CSF, e->y >> CSF, FALSE);
		e->state = STATE_DELETE;
		return;
	}

	switch(e->state)
	{
		case 0:
		{
			e->flags |= NPC_SHOOTABLE;
			e->flags |= NPC_INVINCIBLE;
			e->x_mark = e->x;
			e->y_mark = e->y;
			e->state = 1;

		} /* fallthrough */
		case 1:
		{
			e->angle += 24;

			e->x_speed -= 0x15;
			e->x_mark += e->x_speed;

			e->x = e->x_mark + ((int32_t)cos[(uint8_t)e->angle] * (4<<CSF) >> 8);
			e->y = e->y_mark + ((int32_t)sin[(uint8_t)e->angle] * (6<<CSF) >> 8);

			entity_create(e->x, e->y, OBJ_UD_SPINNER_TRAIL, 0);
		}
		break;
	}
}

void ai_ud_spinner_trail(Entity *e)
{
	e->frame++;
	if (e->frame > 2)
		e->state = STATE_DELETE;
}
void ai_ud_blast(Entity *e) {
	e->x += -0x1000;
	//e->frame ^= 1;

	effect_smoke_burst(e->x>>CSF, e->y>>CSF, 16, 1);
	
	if (e->x < -pixel_to_sub(8)) e->state = STATE_DELETE;
}
