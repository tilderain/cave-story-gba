#include "common.h"

#include "resources.h"
#include "vdp.h"

#include "tables.h"

#include "bank_data.h"

const background_info_def background_info[BACKGROUND_COUNT] = {
	{ &BG_bk0, 	&PAL_bk0, 0, 8, 8 },
	{ &BG_Black,     &PAL_bkBlack, 0, 8, 8 },
	{ &BG_Blue, &PAL_bkBlue, 0, 8, 8 }, // Mimiga Village, Grasstown, Waterway, Plantation
	{ &BG_Blue, &PAL_bkBlue,3, 8, 8 }, // Main Artery
	{ NULL, 	&PAL_bkFog, 5, 0, 0 }, // Balcony
	{ &BG_Gard, &PAL_bkGard, 0, 6, 8 }, // Sand Zone Storehouse
	{ &BG_Gray, &PAL_bkGray, 0, 8, 8 }, // Boulder Chamber
	{ NULL,		&PAL_bkMoon, 1, 0, 0 }, // Outer Wall
	{ &BG_Maze, &PAL_bkMaze, 0, 8, 8 }, // Labyrinth W - PAL_X
	{ &BG_Maze2,&PAL_bkMaze, 0, 8, 8 }, // Labyrinth I, O - PAL_XB
	{ &BG_Red, 	&PAL_bkRed, 0, 4, 4 }, // Labyrinth M
	{ NULL,		&PAL_bkWater, 4, 0, 0 }, // Almond, Dark Place
	{ &BG_Red,  &PAL_bkRed, 0, 4, 4 }, // Hell B1 / B2 / B3
	{ &BG_Green, &PAL_bkGreen, 0, 8, 8 }, // Egg Corridor
	{ &BG_Blue, &PAL_bkBlue, 0, 8, 8 }, // Arthur's House, Labyrinth B, Itoh, Seal/Statue Chamber
	{ &BG_Fall, &PAL_bkFall, 0, 8, 8 }, // Fall
	{ &BG_Green, &PAL_bkGreen, 0, 8, 8 }, // Sand Zone
};
