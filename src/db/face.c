#include "common.h"

#include "resources.h"
#include "vdp.h"

#include "tables.h"

#include "bank_data.h"


const face_info_def face_info[30] = {
	{ NULL,        NULL,         PAL0 }, // 00
	{ &TS_Face01,  PAL_face01,   PAL0 }, // 01
	{ &TS_Face02,  PAL_face02,   PAL0 }, // 02
	{ &TS_Face03,  PAL_face03,   PAL0 }, // 03
	{ &TS_Face04,  PAL_face04,   PAL0 }, // 04
	{ &TS_Face05,  PAL_face05,   PAL3 }, // 05
	{ &TS_Face06,  PAL_face06,   PAL3 }, // 06
	{ &TS_Face07,  PAL_face07,   PAL3 }, // 07
	{ &TS_Face08,  PAL_face08,   PAL3 }, // 08
	{ &TS_Face09,  PAL_face09,   PAL3 }, // 09
	{ &TS_Face10,  PAL_face10,   PAL3 }, // 10
	{ &TS_Face11,  PAL_face11,   PAL3 }, // 11
	{ &TS_Face12,  PAL_face12,   PAL3 }, // 12
	{ &TS_Face13,  PAL_face13,   PAL3 }, // 13
	{ &TS_Face14,  PAL_face14,   PAL3 }, // 14
	{ &TS_Face15,  PAL_face15,   PAL0 }, // 15
	{ &TS_Face16,  PAL_face16,   PAL0 }, // 16
	{ &TS_Face17,  PAL_face17,   PAL3 }, // 17
	{ &TS_Face18,  PAL_face18,   PAL3 }, // 18
	{ &TS_Face19,  PAL_face19,   PAL3 }, // 19
	{ &TS_Face20,  PAL_face20,   PAL3 }, // 20
	{ &TS_Face21,  PAL_face21,   PAL3 }, // 21
	{ &TS_Face22,  PAL_face22,   PAL3 }, // 22
	{ &TS_Face23,  PAL_face23,   PAL3 }, // 23
	{ &TS_Face24,  PAL_face24,   PAL3 }, // 24
	{ &TS_Face25,  PAL_face25,   PAL3 }, // 25
	{ &TS_Face26,  PAL_face26,   PAL0 }, // 26
	{ &TS_Face01,  PAL_face27,   PAL3 }, // 27
	{ &TS_Face28,  PAL_face28,   PAL3 }, // 28
	{ &TS_Face29,  PAL_face29,   PAL0 }, // 29
};