#include "common.h"
#include "bank_data.h"
#include "tables.h"

#include "soundbank.h"

const song_info_def song_info[SONG_COUNT] = {

};

const song_info_def song_info_xm[SONG_COUNT] = {
	{ NULL, 			"Silence" },
	{ MOD_MISCHIEVOUS_ROBOT, 	"Mischievous Robot" },
	{ MOD_SAFETY, 		"Safety" },
	{ MOD_GAMEOVER, 	"Game Over" },
	{ MOD_GRAVITY, 		"Gravity" },
	{ MOD_ON_TO_GRASSTOWN, 	"On To Grasstown" },
	{ MOD_MELTDOWN_2, 	"Meltdown 2" },
	{ MOD_EYES_OF_FLAME, 	"Eyes Of Flame" },
	{ MOD_GESTATION, 	"Gestation" },
	{ MOD_MIMIGA_TOWN, 		"Mimiga Village" },
	{ MOD_GOT_ITEM, 	"Fanfare 1" },
	{ MOD_BALROG, 		"Balrog's Theme" },
	{ MOD_CEMETERY, 	"Cemetery" },
	{ MOD_PLANT, 		"Yamashita Farm" },
	{ MOD_PULSE, 		"Pulse" },
	{ MOD_GET_HEART_TANK, 	"Fanfare 2" },
	{ MOD_VICTORY_, 	"Fanfare 3" }, // 0x10
	{ MOD_TYRANT, 		"Tyrant" },
	{ MOD_RUN_, 			"Run!" },
	{ MOD_JENKA_1, 		"Jenka 1" },
	{ MOD_LABYRINTH_FIGHT, 	"Labyrinth Fight" },
	{ MOD_ACCESS, 		"Access" },
	{ MOD_OPPRESSION, 	"Oppression" },
	{ MOD_GEOTHERMAL, 	"Geothermal" },
	{ MOD_CAVE_STORY, 	"Cave Story" },
	{ MOD_MOONSONG, 	"Moon Song" },
	{ NULL, 	"Hero's End" },
	{ MOD_SCORCHING_BACK, 	"Scorching Back" },
	{ MOD_QUIET, 		"Quiet" },
	{ MOD_LAST_CAVE, 	"Last Cave" },
	{ MOD_BALCONY, 		"Balcony" },
	{ NULL, 		"Charge" },
	{ MOD_LAST_BATTLE, 	"Last Battle" }, // 0x20
	{ MOD_THE_WAY_BACK_HOME, 	"The Way Back Home" },
	{ MOD_ZOMBIE, 		"Zombie" },
	{ MOD_BREAK_DOWN, 	"Break Down" },
	{ MOD_RUNNING_HELL, 	"Running Hell" },
	{ MOD_JENKA_2, 		"Jenka 2" },
	{ MOD_LIVING_WATERWAY, 	"Living Waterway" },
	{ MOD_SEAL_CHAMBER, 	"Seal Chamber" },
	{ NULL, 		"Toroko's Theme" },
	{ MOD_WHITE, 		"White" },
    { NULL,    "Time Table" },
};
