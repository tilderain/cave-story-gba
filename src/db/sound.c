#include "common.h"
#include "bank_data.h"
#include "tables.h"

#include "soundbank.h"

const sound_info_def sound_info[SOUND_COUNT] = {
    { 0 },
    { SFX_01 }, // 1 - Cursor
    { SFX_02 }, // 2 - Message
    { SFX_03 }, // 3 - Head Bonk
    { SFX_04 }, // 4 - Switch Weapon
    { SFX_05 }, // 5 - Prompt
    { SFX_06 }, // 6 - Hop
    { 0 },
    { SFX_08 }, // 8 - ????
    { 0 },
    { 0 },
    { SFX_0B }, // 11 - Door Open
    { SFX_0C }, // 12 - Destroy Block
    { 0 },
    { SFX_0E }, // 14 - Weapon Energy
    { SFX_0F }, // 15 - Jump
    { SFX_10 }, // 16 - Take Damage
    { SFX_11 }, // 17 - Die
    { SFX_12 }, // 18 - Confirm
    { 0 },
    { SFX_14 }, // 20 - Refill
    { SFX_15 }, // 21 - Bubble
    { SFX_16 }, // 22 - Open Chest
    { SFX_17 }, // 23 - Hit Ground
    { SFX_18 }, // 24 - Walking
    { SFX_19 }, // 25 - Funny Explode
    { SFX_1A }, // 26 - Quake
    { SFX_1B }, // 27 - Level Up
    { SFX_1C }, // 28 - Shot Hit
    { SFX_1D }, // 29 - Teleport
    { SFX_1E }, // 30 - Critter Hop
    { SFX_1F }, // 31 - Shot Bounce
    { SFX_20 }, // 32 - Polar Star
    { SFX_21 }, // 33 - ????
    { SFX_22 }, // 34 - Fireball
    { SFX_23 },
    { 0 },
    { SFX_25 },
    { SFX_26 },
    { SFX_27 },
    { SFX_28 },
    { 0 },
    { SFX_2A }, // Get Missile
    { SFX_2B },
    { SFX_2C },
    { SFX_2D }, // 45 - Energy Bounce
    { SFX_2E },
    { SFX_2F },
    { SFX_30 },
    { SFX_31 }, // 49 - Polar star Lv 3
    { SFX_32 },
    { SFX_33 },
    { SFX_34 },
    { SFX_35 },
    { SFX_36 },
    { SFX_37 },
    { SFX_38 }, // Splash
    { SFX_39 },
    { SFX_3A },
    { SFX_3B }, // Spur charge 1
    { SFX_3C }, // 2
    { SFX_3D }, // 3
    { SFX_3E },
    { SFX_3F },
    { SFX_40 }, // 64 - Spur fire max
    { SFX_41 }, // 65 - Spur full charge
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { SFX_46 },
    { SFX_47 },
    { SFX_48 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    // The following range 0x50 - 0x5F are remapped 0x90 - 0x9F
    { 0 }, // 0x50 (80)
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },//{ SFX_96, SFX_96_end },
    { 0 },//{ SFX_97, SFX_97_end },
    { 0 },//{ SFX_98, SFX_98_end },
    { 0 },//{ SFX_99, SFX_99_end },
    { 0 },//{ SFX_9A, SFX_9A_end },
    { 0 },//{ SFX_9B, SFX_9B_end },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    // End of remapped section
    { 0 }, // 0x60 (96)
    { 0 },
    { 0 },
    { 0 },
    { SFX_64 },
    { SFX_65 },
    { SFX_66 },
    { SFX_67 },
    { SFX_68 },
    { SFX_69 },
    { SFX_6A },
    { SFX_6B },
    { SFX_6C },
    { SFX_6D },
    { SFX_6E },
    { SFX_6F },
    { SFX_70 }, // 0x70 (112)
    { SFX_71 },
    { SFX_72 },
    { SFX_73 },
    { SFX_74 },
    { SFX_75 }, // 0x75 (117)
};
