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
    { SFX_07 }, // 7 - Run
    { SFX_08 }, // 8 - Step
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
    { SFX_21 }, // 33 - snd_21
    { SFX_22 }, // 34 - Fireball
    { SFX_23 }, // 35 - Explosion 1
    { 0 },
    { SFX_25 }, // 37 - Gun Click
    { SFX_26 }, // 38 - Get Item
    { SFX_27 }, // 39 - EM Fire
    { SFX_28 }, // 40 - Stream 1
    { SFX_29 }, // 41 - Quake
    { SFX_2A }, // 42 - Get Missile
    { SFX_2B }, // 43 - Computer Beep
    { SFX_2C }, // 44 - Missile Hit
    { SFX_2D }, // 45 - Energy Bounce
    { SFX_2E }, // 46 - IronH Shot Fly
    { SFX_2F }, // 47 - Explosion 2
    { SFX_30 }, // 48 - snd_30
    { SFX_31 }, // 49 - Polar star Lv 3
    { SFX_32 }, // 50 - Mimiga Squeak
    { SFX_33 }, // 51 - Enemy Hurt
    { SFX_34 }, // 52 - Enemy Hurt Big
    { SFX_35 }, // 53 - Enemy Hurt Small
    { SFX_36 }, // 54 - Enemy Hurt Cool
    { SFX_37 }, // 55 - Enemy Squeak 2
    { SFX_38 }, // 56 - Splash
    { SFX_39 }, // 57 - Enemy Damage
    { SFX_3A }, // 58 - snd_3a
    { SFX_3B }, // 59 - Spur charge 1
    { SFX_3C }, // 60 - Spur charge 2
    { SFX_3D }, // 61 - Spur charge 3
    { SFX_3E }, // 62 - Spur fire lv2
    { SFX_3F }, // 63 - Spur fire lv3
    { SFX_40 }, // 64 - Spur fire max
    { SFX_41 }, // 65 - Spur full charge
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { SFX_46 }, // 70 - Expl Small
    { SFX_47 }, // 71 - Little Crash
    { SFX_48 }, // 72 - Big Crash
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
    { SFX_96 }, // 0x56 (86) -> 0x96 - snd_96
    { SFX_97 }, // 0x57 (87) -> 0x97 - snd_97
    { SFX_98 }, // 0x58 (88) -> 0x98 - snd_98
    { SFX_99 }, // 0x59 (89) -> 0x99 - snd_99
    { SFX_9A }, // 0x5A (90) -> 0x9A - snd_9a
    { SFX_9B }, // 0x5B (91) -> 0x9B - snd_9b
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    // End of remapped section
    { 0 }, // 0x60 (96)
    { 0 },
    { 0 },
    { 0 },
    { SFX_64 }, // 100 - Bubble Pop
    { SFX_65 }, // 101 - Lightning Strike
    { SFX_66 }, // 102 - Jaws
    { SFX_67 }, // 103 - Curly Charge Gun
    { SFX_68 }, // 104 - snd_68
    { SFX_69 }, // 105 - Puppy Bark
    { SFX_6A }, // 106 - snd_6a
    { SFX_6B }, // 107 - Block Move
    { SFX_6C }, // 108 - snd_6c
    { SFX_6D }, // 109 - Critter Fly
    { SFX_6E }, // 110 - snd_6e
    { SFX_6F }, // 111 - snd_6f
    { SFX_70 }, // 0x70 (112) - snd_70
    { SFX_71 }, // 113 - snd_71
    { SFX_72 }, // 114 - Core Hurt
    { SFX_73 }, // 115 - Core Thrust
    { SFX_74 }, // 116 - snd_74
    { SFX_75 }, // 0x75 (117) - snd_75
};