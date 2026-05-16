/*
 * Number & Smoke effects, will be more than that eventually
 */

#define MAX_DAMAGE 4
#define MAX_SMOKE 8
#define MAX_MISC 12

enum {
	EFF_BONKL, EFF_BONKR, EFF_ZZZ, EFF_BOOST8, EFF_BOOST2, EFF_QMARK,
	EFF_FANL, EFF_FANU, EFF_FANR, EFF_FAND, EFF_SPLASH,
	EFF_PSTAR_HIT, EFF_MGUN_HIT, EFF_BUBB_POP, EFF_FIRETRAIL, EFF_SNAKETRAIL,
	EFF_DISSIPATE, EFF_GIB,
	EFF_BOOMFLASH,
	EFF_SHOOT, EFF_HIT,
};

// Only send tiles for 1 damage string per frame
extern uint8_t dqueued;

// Initialize default (0, NULL) values to avoid weird glitches
void effects_init();
// Clear all effects
void effects_clear();
// Just clear the smoke
void effects_clear_smoke();
// Per frame update of effects
void effects_update();

// Creates a damage string (maximum of 4)
// Positive values will be white, negative will be red, zero is ignored
// Values must be limited to 3 digits
void effect_create_damage(int16_t num, Entity *follow, int16_t xoff, int16_t yoff);
// Creates a single puff of smoke
void effect_create_smoke(int16_t x, int16_t y);
// Creates count smoke puffs scattered within a w×w area (CSE2 SetDestroyNpChar equivalent)
void effect_smoke_burst(int16_t x, int16_t y, uint16_t w, uint8_t count);


void effect_create_misc(uint8_t type, int16_t x, int16_t y, uint8_t only_one);

// Fading

extern int8_t fadeSweepTimer;

// Fade the screen out using diamond sweep pattern (blocking)
void do_fadeout_sweep(uint8_t dir);

// Begin fading the screen in using diamond sweep patter (non-blocking)
void start_fadein_sweep(uint8_t dir);

// Frame step for fade in effect
void update_fadein_sweep(void);

// Fade grid: 15x10 cells, each 16x16 pixels
#define FADE_GRID_W      15
#define FADE_GRID_H      10
#define FADE_FRAMES      16
#define FADE_TILE_BASE   640   // 64 tiles in BG3 charbase 2

typedef struct {
    int mode;       // 0=none, 1=fadein, 2=fadeout
    int bMask;      // 1=full mask active
    int count;      // sweep line position
    int dir;        // 0=right, 2=left, 1=bottom, 3=top, 4=diamond
    int8_t ani_no[FADE_GRID_H][FADE_GRID_W];
    int8_t flag[FADE_GRID_H][FADE_GRID_W];
} FadeState;
extern FadeState gFade;