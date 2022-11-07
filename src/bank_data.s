.macro BIN _name, _file, _align=2
    .globl \_name
    .align \_align
\_name:
    .incbin "\_file"
.endm

.macro PCM _name, _file, _align=2
    .globl \_name
    .globl \_name\()_end
    .align \_align
\_name:
    .incbin "\_file"
\_name\()_end:
.endm

.section .text

# Compressed tileset patterns
BIN UFTC_Almond "../res/Stage/PrtAlmond_vert.img.bin"
BIN UFTC_Barr   "../res/Stage/PrtBarr_vert.img.bin"
BIN UFTC_Cave   "../res/Stage/PrtCave_vert.img.bin"
BIN UFTC_Cent   "../res/Stage/PrtCent_vert.img.bin"
BIN UFTC_EggIn  "../res/Stage/PrtEggIn_vert.img.bin"
BIN UFTC_Eggs   "../res/Stage/Eggs/PrtEggs_vert.img.bin"
BIN UFTC_EggX   "../res/Stage/PrtEggX1_vert.img.bin"
BIN UFTC_Fall   "../res/Stage/PrtFall_vert.img.bin"
BIN UFTC_Gard   "../res/Stage/PrtGard_vert.img.bin"
BIN UFTC_Hell   "../res/Stage/Hell/PrtHell_vert.img.bin"
BIN UFTC_Jail   "../res/Stage/PrtJail_vert.img.bin"
BIN UFTC_Labo   "../res/Stage/PrtLabo_vert.img.bin"
BIN UFTC_Maze   "../res/Stage/Maze/PrtMaze_vert.img.bin"
BIN UFTC_Mimi   "../res/Stage/Mimi/PrtMimi_vert.img.bin"
BIN UFTC_Oside  "../res/Stage/PrtOside_vert.img.bin"
BIN UFTC_Pens   "../res/Stage/PrtPens_vert.img.bin"
BIN UFTC_River  "../res/Stage/PrtRiver_vert.img.bin"
BIN UFTC_Sand   "../res/Stage/PrtSand_vert.img.bin"
BIN UFTC_Store  "../res/Stage/PrtStore_vert.img.bin"
BIN UFTC_Weed   "../res/Stage/PrtWeed_vert.img.bin"
BIN UFTC_Blcny  "../res/Stage/White/PrtBlcny_vert.img.bin"

BIN UFTC_EggX2  "../res/Stage/PrtEggX2_vert.img.bin"
BIN UFTC_Eggs2  "../res/Stage/Eggs/PrtEggs2_vert.img.bin"
BIN UFTC_MazeM  "../res/Stage/Maze/PrtMazeM_vert.img.bin"
BIN UFTC_Kings  "../res/Stage/White/PrtKings_vert.img.bin"
BIN UFTC_Statue "../res/Stage/Hell/PrtStatue_vert.img.bin"
BIN UFTC_Ring2  "../res/Stage/White/PrtRing2_vert.img.bin"
BIN UFTC_Ring3  "../res/Stage/White/PrtRing3_vert.img.bin"

BIN PAL_Mimi "../res/Stage/Mimi/PrtMimi_vert.pal.bin"
BIN PAL_Eggs "../res/Stage/Eggs/PrtEggs_vert.pal.bin"
BIN PAL_Maze "../res/Stage/Maze/PrtMaze_vert.pal.bin"
BIN PAL_Blcny "../res/Stage/White/PrtBlcny_vert.pal.bin"
BIN PAL_Ring3 "../res/Stage/White/PrtRing3_vert.pal.bin"
BIN PAL_Hell "../res/Stage/Hell/PrtHell_vert.pal.bin"

BIN PAL_Almond	"../res/Stage/PrtAlmond_vert.pal.bin"
BIN PAL_Barr	"../res/Stage/PrtBarr_vert.pal.bin"
BIN PAL_Cave	"../res/Stage/PrtCave_vert.pal.bin"
BIN PAL_Cent	"../res/Stage/PrtCent_vert.pal.bin"
BIN PAL_EggIn	"../res/Stage/PrtEggIn_vert.pal.bin"
BIN PAL_EggX	"../res/Stage/PrtEggX1_vert.pal.bin"
BIN PAL_Fall	"../res/Stage/PrtFall_vert.pal.bin"
BIN PAL_Gard	"../res/Stage/PrtGard_vert.pal.bin"
BIN PAL_Jail	"../res/Stage/PrtJail_vert.pal.bin"
BIN PAL_Labo	"../res/Stage/PrtLabo_vert.pal.bin"
BIN PAL_Oside	"../res/Stage/PrtOside_vert.pal.bin"
BIN PAL_Pens	"../res/Stage/PrtPens_vert.pal.bin"
BIN PAL_River	"../res/Stage/PrtRiver_vert.pal.bin"
BIN PAL_Sand	"../res/Stage/PrtSand_vert.pal.bin"
BIN PAL_Store	"../res/Stage/PrtStore_vert.pal.bin"
BIN PAL_Weed	"../res/Stage/PrtWeed_vert.pal.bin"

BIN PAL_Sega		"../res/sprite/sega.pal.bin"
BIN PAL_Main		"../res/sprite/quote.pal.bin"
BIN PAL_Sym			"../res/sprite/door.pal.bin"
BIN PAL_Regu		"../res/sprite/kazuma.pal.bin"
BIN PAL_Gunsmith	"../res/sprite/gunsmith.pal.bin"
BIN PAL_Plant		"../res/sprite/flower.pal.bin"
BIN PAL_Frog		"../res/sprite/balfrog1.pal.bin"
BIN PAL_Chaco		"../res/sprite/chaco.pal.bin"
BIN PAL_Jenka		"../res/sprite/jenka.pal.bin"
BIN PAL_Dark		"../res/sprite/dark.pal.bin"
BIN PAL_X			"../res/back/bkMaze.pal.bin"
BIN PAL_XB			"../res/back/bkMaze.pal.bin"
BIN PAL_LabB		"../res/back/bkMimiRegu.pal.bin"
BIN PAL_Boulder		"../res/sprite/boulder.pal.bin"
BIN PAL_MazeM		"../res/back/bkRed.pal.bin"
BIN PAL_Eggs2		"../res/sprite/babydragon.pal.bin"
BIN PAL_Sisters		"../res/sprite/sishead.pal.bin"
BIN PAL_Red			"../res/sprite/dripred.pal.bin"
BIN PAL_Mapi		"../res/sprite/mapignon.pal.bin"
BIN PAL_Miza		"../res/sprite/misery2.pal.bin"
BIN PAL_Intro		"../res/sprite/bubbleintro.pal.bin"

BIN SPR_Sega_data "../res/sprite/sega.img.bin"
BIN SPR_Sega2_data "../res/sprite/sega2.img.bin"
BIN SPR_Quote_data "../res/sprite/quote.img.bin"
BIN SPR_Air_data "../res/sprite/air.img.bin"
BIN SPR_J_Air_data "../res/sprite/air_ja.img.bin"
BIN SPR_Hud2_data "../res/sprite/hud2.img.bin"
BIN SPR_Pointer_data "../res/sprite/pointer.img.bin"
BIN SPR_ArmsImage_data "../res/sprite/armsimage.img.bin"
BIN SPR_ArmsImageM_data "../res/sprite/armsimage_menu.img.bin"
BIN SPR_Misery_data "../res/sprite/misery.img.bin"
BIN SPR_TeleMenu_data "../res/sprite/telemenu.img.bin"
BIN SPR_Fish_data "../res/sprite/fish.img.bin"
BIN SPR_CritBig_data "../res/sprite/critbig.img.bin"
BIN SPR_Jelly_data "../res/sprite/jelly.img.bin"
BIN SPR_Kulala_data "../res/sprite/kulala.img.bin"
BIN SPR_MisslB1_data "../res/sprite/missileb1.img.bin"
BIN SPR_MisslB2_data "../res/sprite/missileb2.img.bin"
BIN SPR_BladeB1_data "../res/sprite/bladeb1.img.bin"
BIN SPR_BladeB2_data "../res/sprite/bladeb2.img.bin"
BIN SPR_BladeB3k_data "../res/sprite/bladeb3king.img.bin"
BIN SPR_BladeB3s_data "../res/sprite/bladeb3slash.img.bin"
BIN SPR_Quotele_data "../res/sprite/quotele.img.bin"
BIN SPR_Bubble_data "../res/sprite/bubble.img.bin"
BIN SPR_MGun_data "../res/sprite/mgun.img.bin"
BIN SPR_MGunB1_data "../res/sprite/mgunb1.img.bin"
BIN SPR_MGunB2_data "../res/sprite/mgunb2.img.bin"
BIN SPR_MGunB3_data "../res/sprite/mgunb3.img.bin"
BIN SPR_Bonk_data "../res/sprite/headbump.img.bin"
BIN SPR_RedFl_data "../res/sprite/redflowers.img.bin"
BIN SPR_BasuShot_data "../res/sprite/basushot.img.bin"
BIN SPR_IkaChan_data "../res/sprite/ikachan.img.bin"
BIN SPR_Droll2_data "../res/sprite/droll2.img.bin"
BIN SPR_Gunfish_data "../res/sprite/gunfish.img.bin"
BIN SPR_Boost_data "../res/sprite/boost.img.bin"
BIN SPR_Midorin_data "../res/sprite/happything.img.bin"
BIN SPR_Stumpy_data "../res/sprite/dragonfly.img.bin"
BIN SPR_Zzz_data "../res/sprite/z.img.bin"
BIN SPR_RedFl3_data "../res/sprite/redflowers3.img.bin"
BIN SPR_MimiSleep_data "../res/sprite/mimisleep.img.bin"
BIN SPR_BubB1_data "../res/sprite/bubb1.img.bin"
BIN SPR_BubB2_data "../res/sprite/bubb2.img.bin"
BIN SPR_BubB3_data "../res/sprite/bubb3.img.bin"
BIN SPR_NemB1h_data "../res/sprite/nemb1h.img.bin"
BIN SPR_NemB1v_data "../res/sprite/nemb1v.img.bin"
BIN SPR_NemB2h_data "../res/sprite/nemb2h.img.bin"
BIN SPR_NemB2v_data "../res/sprite/nemb2v.img.bin"
BIN SPR_NemB3h_data "../res/sprite/nemb3h.img.bin"
BIN SPR_NemB3v_data "../res/sprite/nemb3v.img.bin"
BIN SPR_QMark_data "../res/sprite/qmark.img.bin"
BIN SPR_Ballos_data "../res/sprite/ballos.img.bin"
BIN SPR_BallosP_data "../res/sprite/ballosp.img.bin"
BIN SPR_BallosEye_data "../res/sprite/ballos_eye.img.bin"
BIN SPR_BallosPlat_data "../res/sprite/ballos_plat.img.bin"
BIN SPR_BallosRot_data "../res/sprite/ballos_ball.img.bin"
BIN SPR_BallosSm_data "../res/sprite/ballos_smile.img.bin"
BIN SPR_ButeRed_data "../res/sprite/butered.img.bin"
BIN SPR_ButeRed2_data "../res/sprite/butered2.img.bin"
BIN SPR_SpurB1_data "../res/sprite/spurb1.img.bin"
BIN SPR_SpurB2_data "../res/sprite/spurb2.img.bin"
BIN SPR_SpurB3_data "../res/sprite/spurb3.img.bin"
BIN SPR_Polar_data "../res/sprite/polar.img.bin"
BIN SPR_PolarB1_data "../res/sprite/polarb1.img.bin"
BIN SPR_PolarB2_data "../res/sprite/polarb2.img.bin"
BIN SPR_PolarB3_data "../res/sprite/polarb3.img.bin"
BIN SPR_Fireb_data "../res/sprite/fireball.img.bin"
BIN SPR_FirebB1_data "../res/sprite/fireballb1.img.bin"
BIN SPR_FirebB2_data "../res/sprite/fireballb2.img.bin"
BIN SPR_FirebB3_data "../res/sprite/fireballb3.img.bin"
BIN SPR_Missl_data "../res/sprite/missiles.img.bin"
BIN SPR_Missl2_data "../res/sprite/supermissl.img.bin"
BIN SPR_Snake_data "../res/sprite/snake.img.bin"
BIN SPR_Bubbl_data "../res/sprite/bubbler.img.bin"
BIN SPR_Nemes_data "../res/sprite/nemesis.img.bin"
BIN SPR_Chest_data "../res/sprite/chest.img.bin"
BIN SPR_Save_data "../res/sprite/save.img.bin"
BIN SPR_Refill_data "../res/sprite/refill.img.bin"
BIN SPR_Door_data "../res/sprite/door.img.bin"
BIN SPR_Tele_data "../res/sprite/tele.img.bin"
BIN SPR_Trap_data "../res/sprite/trap.img.bin"
BIN SPR_LifeUp_data "../res/sprite/lifeup.img.bin"
BIN SPR_Bed_data "../res/sprite/bed.img.bin"
BIN SPR_Fire_data "../res/sprite/fire.img.bin"
BIN SPR_Sign_data "../res/sprite/sign.img.bin"
BIN SPR_Smoke_data "../res/sprite/smoke.img.bin"
BIN SPR_Sparkle_data "../res/sprite/sparkle.img.bin"
BIN SPR_EnergyS_data "../res/sprite/energysm.img.bin"
BIN SPR_EnergyL_data "../res/sprite/energylg.img.bin"
BIN SPR_DoorE_data "../res/sprite/doore.img.bin"
BIN SPR_Bat_data "../res/sprite/bat.img.bin"
BIN SPR_Spikes_data "../res/sprite/spikes.img.bin"
BIN SPR_Behem_data "../res/sprite/behem.img.bin"
BIN SPR_Basil_data "../res/sprite/basil.img.bin"
BIN SPR_Balrog_data "../res/sprite/balrog.img.bin"
BIN SPR_Heart_data "../res/sprite/heart.img.bin"
BIN SPR_ForceField_data "../res/sprite/forcefield.img.bin"
BIN SPR_Computer_data "../res/sprite/computer.img.bin"
BIN SPR_Board_data "../res/sprite/blackboard.img.bin"
BIN SPR_SaveSign_data "../res/sprite/savesign.img.bin"
BIN SPR_Wave_data "../res/sprite/wave.img.bin"
BIN SPR_Platform_data "../res/sprite/platform.img.bin"
BIN SPR_Pignon_data "../res/sprite/pignon.img.bin"
BIN SPR_PignonB_data "../res/sprite/pignonb.img.bin"
BIN SPR_Chest2_data "../res/sprite/chestopen.img.bin"
BIN SPR_Key_data "../res/sprite/key.img.bin"
BIN SPR_Term_data "../res/sprite/terminal.img.bin"
BIN SPR_Keeper_data "../res/sprite/gkeeper.img.bin"
BIN SPR_Fan_data "../res/sprite/fan.img.bin"
BIN SPR_Malco_data "../res/sprite/malco.img.bin"
BIN SPR_Pot_data "../res/sprite/pot.img.bin"
BIN SPR_Press_data "../res/sprite/press.img.bin"
BIN SPR_PressH_data "../res/sprite/pressh.img.bin"
BIN SPR_Cage_data "../res/sprite/cage.img.bin"
BIN SPR_EnergyShot_data "../res/sprite/energyshot.img.bin"
BIN SPR_MisslP_data "../res/sprite/missilep.img.bin"
BIN SPR_TeleLight_data "../res/sprite/telelight.img.bin"
BIN SPR_Puppy_data "../res/sprite/puppy.img.bin"
BIN SPR_Table_data "../res/sprite/table.img.bin"
BIN SPR_ManShot_data "../res/sprite/manshot.img.bin"
BIN SPR_Skullhead_data "../res/sprite/skullstep.img.bin"
BIN SPR_FrogShot_data "../res/sprite/frogshot.img.bin"
BIN SPR_ToroBlock_data "../res/sprite/toroblock.img.bin"
BIN SPR_BigDoorFrame_data "../res/sprite/bigdoorframe.img.bin"
BIN SPR_BigDoor_data "../res/sprite/bigdoor.img.bin"
BIN SPR_PowerScreen_data "../res/sprite/powerscreen.img.bin"
BIN SPR_Hey_data "../res/sprite/hey.img.bin"
BIN SPR_Sprinkler_data "../res/sprite/sprinkler.img.bin"
BIN SPR_Sword_data "../res/sprite/kingsword.img.bin"
BIN SPR_Drop_data "../res/sprite/drop.img.bin"
BIN SPR_RedFl2_data "../res/sprite/redflowers2.img.bin"
BIN SPR_Hoppy_data "../res/sprite/jumpthing.img.bin"
BIN SPR_Robot3_data "../res/sprite/surfacerobot.img.bin"
BIN SPR_Grate_data "../res/sprite/grate.img.bin"
BIN SPR_EnCap_data "../res/sprite/energycapsule.img.bin"
BIN SPR_DoctorM_data "../res/sprite/musldr.img.bin"
BIN SPR_DoctorB_data "../res/sprite/docboss.img.bin"
BIN SPR_DocBat_data "../res/sprite/docbat.img.bin"
BIN SPR_RedDot_data "../res/sprite/reddot.img.bin"
BIN SPR_DocShot_data "../res/sprite/docshot.img.bin"
BIN SPR_JailBars_data "../res/sprite/jailbars.img.bin"
BIN SPR_DraShot_data "../res/sprite/dragonshot.img.bin"
BIN SPR_Lightning_data "../res/sprite/lightning.img.bin"
BIN SPR_Heli_data "../res/sprite/heli.img.bin"
BIN SPR_HeliBlade_data "../res/sprite/heliblade.img.bin"
BIN SPR_HeliBlade2_data "../res/sprite/heliblade2.img.bin"
BIN SPR_RedCrystal_data "../res/sprite/redcrystal.img.bin"
BIN SPR_LevelUp_data "../res/sprite/levelup.img.bin"
BIN SPR_J_LevelUp_data "../res/sprite/levelup_ja.img.bin"
BIN SPR_LevelDown_data "../res/sprite/leveldown.img.bin"
BIN SPR_J_LevelDown_data "../res/sprite/leveldown_ja.img.bin"
BIN SPR_HeavyPress_data "../res/sprite/heavypress.img.bin"
BIN SPR_HPLightning_data "../res/sprite/hp_lightning.img.bin"
BIN SPR_HPCharge_data "../res/sprite/hp_charge.img.bin"
BIN SPR_ButeArw_data "../res/sprite/butearw.img.bin"
BIN SPR_Rolling_data "../res/sprite/rolling.img.bin"
BIN SPR_Deleet_data "../res/sprite/deleet.img.bin"
BIN SPR_Target_data "../res/sprite/target.img.bin"
BIN SPR_PuppyGhost_data "../res/sprite/ghostdog.img.bin"
BIN SPR_HeavyPressL_data "../res/sprite/hp_lit.img.bin"
BIN SPR_BalrogFly_data "../res/sprite/balrog_fly.img.bin"
BIN SPR_BalrogMedic_data "../res/sprite/balrog_medic.img.bin"
BIN SPR_BabyPuppy_data "../res/sprite/puppy_baby.img.bin"
BIN SPR_Empty_data "../res/sprite/empty.img.bin"
BIN SPR_J_Empty_data "../res/sprite/empty_ja.img.bin"
BIN SPR_DoctorBlood_data "../res/sprite/blood.img.bin"
BIN SPR_DocDie_data "../res/sprite/docdie.img.bin"
BIN SPR_Drop2_data "../res/sprite/drop2.img.bin"
BIN SPR_Transmg_data "../res/sprite/transmg.img.bin"
BIN SPR_CritHB_data "../res/sprite/critcave.img.bin"
BIN SPR_MazeBlock_data "../res/sprite/mazeblock.img.bin"
BIN SPR_CritterP_data "../res/sprite/critterp.img.bin"
BIN SPR_Gaudi_data "../res/sprite/gaudi.img.bin"
BIN SPR_LabShot_data "../res/sprite/labshot.img.bin"
BIN SPR_GaudiShot_data "../res/sprite/gaudishot.img.bin"
BIN SPR_GaudiArmor_data "../res/sprite/gaudiarmor.img.bin"
BIN SPR_FuzzCore_data "../res/sprite/fuzzcore.img.bin"
BIN SPR_Fuzz_data "../res/sprite/fuzz.img.bin"
BIN SPR_FireWhir_data "../res/sprite/corething.img.bin"
BIN SPR_FireWShot_data "../res/sprite/corethingshot.img.bin"
BIN SPR_Buggy_data "../res/sprite/buggy.img.bin"
BIN SPR_Buggy2_data "../res/sprite/buggy2.img.bin"
BIN SPR_GaudiMerch_data "../res/sprite/gaudimerch.img.bin"
BIN SPR_BuyoBase_data "../res/sprite/buyobase.img.bin"
BIN SPR_Buyo_data "../res/sprite/buyo.img.bin"
BIN SPR_GaudiEgg_data "../res/sprite/gaudiegg.img.bin"
BIN SPR_Shutter_data "../res/sprite/coreshutter.img.bin"
BIN SPR_BigShutter_data "../res/sprite/corebigshutter.img.bin"
BIN SPR_CoreLift_data "../res/sprite/corelift.img.bin"
BIN SPR_GrateMouth_data "../res/sprite/gratemouth.img.bin"
BIN SPR_CoreFront_data "../res/sprite/core.img.bin"
BIN SPR_CoreBack_data "../res/sprite/coreback.img.bin"
BIN SPR_MiniCore1_data "../res/sprite/coremini1.img.bin"
BIN SPR_MiniCore2_data "../res/sprite/coremini2.img.bin"
BIN SPR_MiniCore3_data "../res/sprite/coremini3.img.bin"
BIN SPR_MiniCore4_data "../res/sprite/coremini4.img.bin"
BIN SPR_MiniCore5_data "../res/sprite/coremini5.img.bin"
BIN SPR_CoreShot1_data "../res/sprite/coreshot1.img.bin"
BIN SPR_CoreShot2_data "../res/sprite/coreshot2.img.bin"
BIN SPR_CoreShot3_data "../res/sprite/coreshot3.img.bin"
BIN SPR_CoreShot4_data "../res/sprite/coreshot4.img.bin"
BIN SPR_IronhBlk_data "../res/sprite/ironhblk.img.bin"
BIN SPR_SmStal_data "../res/sprite/smstal.img.bin"
BIN SPR_LgStal_data "../res/sprite/lgstal.img.bin"
BIN SPR_NightSpirit_data "../res/sprite/nightshade.img.bin"
BIN SPR_NightShot_data "../res/sprite/nightshot.img.bin"
BIN SPR_Croc2_data "../res/sprite/sandcroc2.img.bin"
BIN SPR_UCoreFront_data "../res/sprite/ucore.img.bin"
BIN SPR_UCoreBack_data "../res/sprite/ucoreback.img.bin"
BIN SPR_MUCoreFront_data "../res/sprite/ucoreminif.img.bin"
BIN SPR_MUCoreBack_data "../res/sprite/ucoreminib.img.bin"
BIN SPR_MUCoreBottom_data "../res/sprite/ucoreminit.img.bin"
BIN SPR_MUCoreEnd_data "../res/sprite/ucoreminie.img.bin"
BIN SPR_UCoreMouth_data "../res/sprite/ucoremouth.img.bin"
BIN SPR_JailBars2_data "../res/sprite/jailbars2.img.bin"
BIN SPR_JailBars3_data "../res/sprite/cage2.img.bin"
BIN SPR_Block_data "../res/sprite/block.img.bin"
BIN SPR_BlockM_data "../res/sprite/blockm.img.bin"
BIN SPR_Rock_data "../res/sprite/rock.img.bin"
BIN SPR_Statue_data "../res/sprite/statue.img.bin"
BIN SPR_Cloud1_data "../res/sprite/cloud1.img.bin"
BIN SPR_Cloud2_data "../res/sprite/cloud2.img.bin"
BIN SPR_Cloud3_data "../res/sprite/cloud3.img.bin"
BIN SPR_Cloud4_data "../res/sprite/cloud4.img.bin"
BIN SPR_GaudiEnd_data "../res/sprite/gaudi_end.img.bin"
BIN SPR_Kazuma_data "../res/sprite/kazuma.img.bin"
BIN SPR_Cthu_data "../res/sprite/cthu.img.bin"
BIN SPR_CritHG_data "../res/sprite/crithg.img.bin"
BIN SPR_Basu_data "../res/sprite/basu.img.bin"
BIN SPR_BtlHG_data "../res/sprite/btlhg.img.bin"
BIN SPR_BtlHB_data "../res/sprite/btlhb.img.bin"
BIN SPR_Toroko_data "../res/sprite/toroko.img.bin"
BIN SPR_Sue_data "../res/sprite/sue.img.bin"
BIN SPR_King_data "../res/sprite/king.img.bin"
BIN SPR_Igor_data "../res/sprite/igor.img.bin"
BIN SPR_Robot_data "../res/sprite/robot.img.bin"
BIN SPR_KazuCom_data "../res/sprite/kazucom.img.bin"
BIN SPR_Kanpachi_data "../res/sprite/kanpachi.img.bin"
BIN SPR_Jack_data "../res/sprite/jack.img.bin"
BIN SPR_Mahin_data "../res/sprite/mahin.img.bin"
BIN SPR_Santa_data "../res/sprite/santa.img.bin"
BIN SPR_Mannan_data "../res/sprite/mannan.img.bin"
BIN SPR_Curly_data "../res/sprite/curly.img.bin"
BIN SPR_CurlyB_data "../res/sprite/curlyb.img.bin"
BIN SPR_Sunstone_data "../res/sprite/sandstone.img.bin"
BIN SPR_Sandcroc_data "../res/sprite/sandcroc.img.bin"
BIN SPR_Polish_data "../res/sprite/polish.img.bin"
BIN SPR_Baby_data "../res/sprite/baby.img.bin"
BIN SPR_Crow_data "../res/sprite/crow.img.bin"
BIN SPR_Armadl_data "../res/sprite/armadillo.img.bin"
BIN SPR_Booster_data "../res/sprite/booster.img.bin"
BIN SPR_Doctor_data "../res/sprite/doctor.img.bin"
BIN SPR_ToroBoss_data "../res/sprite/torokoboss.img.bin"
BIN SPR_ToroFlower_data "../res/sprite/toroflower.img.bin"
BIN SPR_Frog_data "../res/sprite/frog.img.bin"
BIN SPR_FrogSm_data "../res/sprite/frogsm.img.bin"
BIN SPR_CurlyMimi_data "../res/sprite/cirlymimi.img.bin"
BIN SPR_SueCom_data "../res/sprite/suecom.img.bin"
BIN SPR_DrGero_data "../res/sprite/drgero.img.bin"
BIN SPR_Nurse_data "../res/sprite/nurse.img.bin"
BIN SPR_Omega_data "../res/sprite/omega.img.bin"
BIN SPR_OmegaLeg_data "../res/sprite/omegaleg.img.bin"
BIN SPR_OmgShot_data "../res/sprite/omegashot.img.bin"
BIN SPR_OmgStrut_data "../res/sprite/omgstrut.img.bin"
BIN SPR_SkelFeet_data "../res/sprite/skelfeet.img.bin"
BIN SPR_Skeleton_data "../res/sprite/skeleton.img.bin"
BIN SPR_Bone_data "../res/sprite/bone.img.bin"
BIN SPR_Ironh_data "../res/sprite/ironh.img.bin"
BIN SPR_Blowfish_data "../res/sprite/blowfish.img.bin"
BIN SPR_Momo_data "../res/sprite/momo.img.bin"
BIN SPR_Itoh_data "../res/sprite/itoh.img.bin"
BIN SPR_Droll_data "../res/sprite/droll.img.bin"
BIN SPR_Droll3_data "../res/sprite/droll3.img.bin"
BIN SPR_BigBat_data "../res/sprite/bigbat.img.bin"
BIN SPR_BlackBat_data "../res/sprite/blackbat.img.bin"
BIN SPR_Rocket_data "../res/sprite/rocket.img.bin"
BIN SPR_DrollShot_data "../res/sprite/drollshot.img.bin"
BIN SPR_Cat_data "../res/sprite/cat.img.bin"
BIN SPR_SkyDragon_data "../res/sprite/skydragon.img.bin"
BIN SPR_Kanpachi2_data "../res/sprite/kanpachi2.img.bin"
BIN SPR_Chie_data "../res/sprite/chie.img.bin"
BIN SPR_JailedMimi_data "../res/sprite/jailmimi.img.bin"
BIN SPR_ShovelMimi_data "../res/sprite/shovelmimi.img.bin"
BIN SPR_Megane_data "../res/sprite/megane.img.bin"
BIN SPR_OldMimi_data "../res/sprite/oldmimi.img.bin"
BIN SPR_Bucket_data "../res/sprite/bucket.img.bin"
BIN SPR_Chaco2_data "../res/sprite/chako2.img.bin"
BIN SPR_Ravil_data "../res/sprite/ravil.img.bin"
BIN SPR_Littles_data "../res/sprite/littles.img.bin"
BIN SPR_Bute_data "../res/sprite/bute.img.bin"
BIN SPR_Mesa_data "../res/sprite/mesa.img.bin"
BIN SPR_Casts_data "../res/credits/casts.img.bin"
BIN SPR_GrnDevil_data "../res/sprite/grndevil.img.bin"
BIN SPR_TurnHuman_data "../res/sprite/turning_human.img.bin"
BIN SPR_AhchooE_data "../res/sprite/ahchoo_en.img.bin"
BIN SPR_AhchooJ_data "../res/sprite/ahchoo_ja.img.bin"
BIN SPR_BlgPassngr_data "../res/sprite/balrog_passngr.img.bin"
BIN SPR_DoctorIntro_data "../res/sprite/doctorintro.img.bin"
BIN SPR_CrownIntro_data "../res/sprite/crownintro.img.bin"
BIN SPR_MiseryWind_data "../res/sprite/misery_wind.img.bin"
BIN SPR_Gunsmith_data "../res/sprite/gunsmith.img.bin"
BIN SPR_Sanda_data "../res/sprite/sanda.img.bin"
BIN SPR_Flower_data "../res/sprite/flower.img.bin"
BIN SPR_Balfrog1_data "../res/sprite/balfrog1.img.bin"
BIN SPR_Balfrog2_data "../res/sprite/balfrog2.img.bin"
BIN SPR_Chaco_data "../res/sprite/chaco.img.bin"
BIN SPR_Jenka_data "../res/sprite/jenka.img.bin"
BIN SPR_Dark_data "../res/sprite/dark.img.bin"
BIN SPR_DarkBub_data "../res/sprite/darkbubble.img.bin"
BIN SPR_DarkDie_data "../res/sprite/darkdie.img.bin"
BIN SPR_XFishy_data "../res/sprite/xfishy.img.bin"
BIN SPR_XTarget_data "../res/sprite/xtarget.img.bin"
BIN SPR_XTread_data "../res/sprite/xtread.img.bin"
BIN SPR_XDoor_data "../res/sprite/xdoor.img.bin"
BIN SPR_XInts_data "../res/sprite/xinternals.img.bin"
BIN SPR_XBody_data "../res/sprite/xbody.img.bin"
BIN SPR_XCat_data "../res/sprite/xcat.img.bin"
BIN SPR_Boulder_data "../res/sprite/boulder.img.bin"
BIN SPR_BabyDragon_data "../res/sprite/babydragon.img.bin"
BIN SPR_Basu2_data "../res/sprite/basu2.img.bin"
BIN SPR_Btl2_data "../res/sprite/btl2.img.bin"
BIN SPR_Cloud_data "../res/sprite/wisp.img.bin"
BIN SPR_Countdown_data "../res/sprite/numbubble.img.bin"
BIN SPR_CritAqua_data "../res/sprite/critaqua.img.bin"
BIN SPR_SisBody_data "../res/sprite/sisbody.img.bin"
BIN SPR_SisHead_data "../res/sprite/sishead.img.bin"
BIN SPR_DripRed_data "../res/sprite/dripred.img.bin"
BIN SPR_CritRed_data "../res/sprite/critterred.img.bin"
BIN SPR_BatRed_data "../res/sprite/batred.img.bin"
BIN SPR_BubRed_data "../res/sprite/bubred.img.bin"
BIN SPR_DrollRed_data "../res/sprite/drollred.img.bin"
BIN SPR_DrollShotR_data "../res/sprite/drollshotred.img.bin"
BIN SPR_MaPignon_data "../res/sprite/mapignon.img.bin"
BIN SPR_Misery2_data "../res/sprite/misery2.img.bin"
BIN SPR_Sue2_data "../res/sprite/sue2.img.bin"
BIN SPR_MizaMisery_data "../res/sprite/mizamisery.img.bin"
BIN SPR_MizaSue_data "../res/sprite/mizasue.img.bin"
BIN SPR_MizaBat_data "../res/sprite/mizabat.img.bin"
BIN SPR_MizaCritter_data "../res/sprite/mizacritter.img.bin"
BIN SPR_MizaRing_data "../res/sprite/mizaring.img.bin"
BIN SPR_MizaShock_data "../res/sprite/mizashock.img.bin"
BIN SPR_MizaFish_data "../res/sprite/mizafish.img.bin"
BIN SPR_MizaPellet_data "../res/sprite/mizapellet.img.bin"
BIN SPR_MizaRock_data "../res/sprite/mizarock.img.bin"
BIN SPR_BubbleIntro_data "../res/sprite/bubbleintro.img.bin"
BIN SPR_ItemImage_data "../res/sprite/itemimage.img.bin"
BIN SPR_ItemImageG_data "../res/sprite/itemimage_g.img.bin"
BIN SPR_ItemWin_data "../res/itemwindow.img.bin"
BIN SPR_Prompt_data "../res/prompt.img.bin"
BIN SPR_J_Prompt_data "../res/prompt_ja.img.bin"
BIN SPR_XXIsland_data "../res/xxIsland.img.bin"


BIN BG_Blue		"../res/back/bkMimi.img.bin" 
BIN BG_Eggs		"../res/back/bkEggs.img.bin" 
BIN BG_Gard		"../res/back/bkGard.img.bin" 
BIN BG_Gray		"../res/back/bkGrayRegu.img.bin" 
BIN BG_Cent		"../res/back/bkCent.img.bin" 
BIN BG_Maze		"../res/back/bkMaze.img.bin" 
BIN BG_Maze2	"../res/back/bkMaze.img.bin" 
BIN BG_Red		"../res/back/bkRed.img.bin" 
BIN BG_Water	"../res/back/bkWater.img.bin"
BIN BG_LabB		"../res/back/bkMimiRegu.img.bin" 
BIN BG_Stream	"../res/back/bkStream.img.bin" 
BIN BG_Sand		"../res/back/bkSand.img.bin" 
BIN BG_Hell		"../res/back/bkHell.img.bin" 
BIN BG_Fall		"../res/back/bkFall.img.bin" 




# Alternate for waterway (Green background)

BIN PAL_RiverAlt "../res/Stage/PrtRiver_alt_vert.pal.bin"

# Tile Attributes
BIN PXA_Almond	"../res/Stage/Almond.pxa"
BIN PXA_Barr	"../res/Stage/Barr.pxa"
BIN PXA_Cave	"../res/Stage/Cave.pxa"
BIN PXA_Cent	"../res/Stage/Cent.pxa"
BIN PXA_EggIn	"../res/Stage/EggIn.pxa"
BIN PXA_EggX	"../res/Stage/EggX.pxa"
BIN PXA_Fall	"../res/Stage/Fall.pxa"
BIN PXA_Gard	"../res/Stage/Gard.pxa"
BIN PXA_Jail	"../res/Stage/Jail.pxa"
BIN PXA_Labo	"../res/Stage/Labo.pxa"
BIN PXA_Oside	"../res/Stage/Oside.pxa"
BIN PXA_Pens	"../res/Stage/Pens.pxa"
BIN PXA_River	"../res/Stage/River.pxa"
BIN PXA_Sand	"../res/Stage/Sand.pxa"
BIN PXA_Store	"../res/Stage/Store.pxa"
BIN PXA_Weed	"../res/Stage/Weed.pxa"

# Optimized Tileset Stages
BIN PXA_Mimi		"../res/Stage/Mimi/Mimi.pxa"
BIN PXM_Barr		"../res/Stage/Mimi/Barr.cpxm"
BIN PXM_Cemet		"../res/Stage/Mimi/Cemet.cpxm"
BIN PXM_e_Ceme		"../res/Stage/Mimi/e_Ceme.cpxm"
BIN PXM_MiBox		"../res/Stage/Mimi/MiBox.cpxm"
BIN PXM_Mimi		"../res/Stage/Mimi/Mimi.cpxm"
BIN PXM_Plant		"../res/Stage/Mimi/Plant.cpxm"
BIN PXM_Pool		"../res/Stage/Mimi/Pool.cpxm"

BIN PXA_Eggs		"../res/Stage/Eggs/Eggs.pxa"
BIN PXM_Eggs		"../res/Stage/Eggs/Eggs.cpxm"

BIN PXA_Eggs2		"../res/Stage/Eggs/Eggs2.pxa"
BIN PXM_Eggs2		"../res/Stage/Eggs/Eggs2.cpxm"

BIN PXA_Maze		"../res/Stage/Maze/Maze.pxa"
BIN PXM_e_Maze		"../res/Stage/Maze/e_Maze.cpxm"
BIN PXM_MazeB		"../res/Stage/Maze/MazeB.cpxm"
BIN PXM_MazeD		"../res/Stage/Maze/MazeD.cpxm"
BIN PXM_MazeH		"../res/Stage/Maze/MazeH.cpxm"
BIN PXM_MazeI		"../res/Stage/Maze/MazeI.cpxm"
BIN PXM_MazeO		"../res/Stage/Maze/MazeO.cpxm"
BIN PXM_MazeS		"../res/Stage/Maze/MazeS.cpxm"
BIN PXM_MazeW		"../res/Stage/Maze/MazeW.cpxm"

BIN PXA_MazeM		"../res/Stage/Maze/MazeM.pxa"
BIN PXM_MazeM		"../res/Stage/Maze/MazeM.cpxm"

BIN PXA_Blcny		"../res/Stage/White/Blcny.pxa"
BIN PXM_e_Blcn		"../res/Stage/White/e_Blcn.cpxm"
BIN PXM_Blcny1		"../res/Stage/White/Blcny1.cpxm"
BIN PXM_Blcny2		"../res/Stage/White/Blcny2.cpxm"

BIN PXA_Kings		"../res/Stage/White/Kings.pxa"
BIN PXM_Kings		"../res/Stage/White/Kings.cpxm"
BIN PXM_Ring1		"../res/Stage/White/Ring1.cpxm"
BIN PXM_Ostep		"../res/Stage/White/Ostep.cpxm"

BIN PXA_Ring2		"../res/Stage/White/Ring2.pxa"
BIN PXM_Ring2		"../res/Stage/White/Ring2.cpxm"

BIN PXA_Ring3		"../res/Stage/White/Ring3.pxa"
BIN PXM_Ring3		"../res/Stage/White/Ring3.cpxm"

BIN PXA_Hell		"../res/Stage/Hell/Hell.pxa"
BIN PXM_Hell1		"../res/Stage/Hell/Hell1.cpxm"
BIN PXM_Hell2		"../res/Stage/Hell/Hell2.cpxm"
BIN PXM_Hell3		"../res/Stage/Hell/Hell3.cpxm"
BIN PXM_Hell4		"../res/Stage/Hell/Hell4.cpxm"
BIN PXM_Hell42		"../res/Stage/Hell/Hell42.cpxm"
BIN PXM_Ballo1		"../res/Stage/Hell/Ballo1.cpxm"
BIN PXM_Ballo2		"../res/Stage/Hell/Ballo2.cpxm"

BIN PXA_Statue		"../res/Stage/Hell/Statue.pxa"
BIN PXM_Statue		"../res/Stage/Hell/Statue.cpxm"

# Stages (PXM)
BIN PXM_Almond	"../res/Stage/Almond.cpxm"
BIN PXM_Cave	"../res/Stage/Cave.cpxm"
BIN PXM_Cent	"../res/Stage/Cent.cpxm"
BIN PXM_CentW	"../res/Stage/CentW.cpxm"
BIN PXM_Chako	"../res/Stage/Chako.cpxm"
BIN PXM_Clock	"../res/Stage/Clock.cpxm"
BIN PXM_Comu	"../res/Stage/Comu.cpxm"
BIN PXM_Cthu	"../res/Stage/Cthu.cpxm"
BIN PXM_Cthu2	"../res/Stage/Cthu2.cpxm"
BIN PXM_Curly	"../res/Stage/Curly.cpxm"
BIN PXM_CurlyS	"../res/Stage/CurlyS.cpxm"
BIN PXM_Dark	"../res/Stage/Dark.cpxm"
BIN PXM_Drain	"../res/Stage/Drain.cpxm"
BIN PXM_e_Jenk	"../res/Stage/e_Jenk.cpxm"
BIN PXM_e_Labo	"../res/Stage/e_Labo.cpxm"
BIN PXM_e_Malc	"../res/Stage/e_Malc.cpxm"
BIN PXM_e_Sky	"../res/Stage/e_Sky.cpxm"
BIN PXM_EgEnd1	"../res/Stage/EgEnd1.cpxm"
BIN PXM_EgEnd2	"../res/Stage/EgEnd2.cpxm"
BIN PXM_Egg1	"../res/Stage/Egg1.cpxm"
BIN PXM_Egg6	"../res/Stage/Egg6.cpxm"
BIN PXM_EggR	"../res/Stage/EggR.cpxm"
BIN PXM_EggR2	"../res/Stage/EggR2.cpxm"
BIN PXM_EggX	"../res/Stage/EggX.cpxm"
BIN PXM_EggX2	"../res/Stage/EggX2.cpxm"
BIN PXM_Fall	"../res/Stage/Fall.cpxm"
BIN PXM_Frog	"../res/Stage/Frog.cpxm"
BIN PXM_Gard	"../res/Stage/Gard.cpxm"
BIN PXM_Itoh	"../res/Stage/Itoh.cpxm"
BIN PXM_Island	"../res/Stage/Island.cpxm"
BIN PXM_Jail1	"../res/Stage/Jail1.cpxm"
BIN PXM_Jail2	"../res/Stage/Jail2.cpxm"
BIN PXM_Jenka1	"../res/Stage/Jenka1.cpxm"
BIN PXM_Jenka2	"../res/Stage/Jenka2.cpxm"
BIN PXM_Little	"../res/Stage/Little.cpxm"
BIN PXM_Lounge	"../res/Stage/Lounge.cpxm"
BIN PXM_Malco	"../res/Stage/Malco.cpxm"
BIN PXM_Mapi	"../res/Stage/Mapi.cpxm"
BIN PXM_MazeA	"../res/Stage/MazeA.cpxm"
BIN PXM_Momo	"../res/Stage/Momo.cpxm"
BIN PXM_Oside	"../res/Stage/Oside.cpxm"
BIN PXM_Pens1	"../res/Stage/Pens1.cpxm"
BIN PXM_Pens2	"../res/Stage/Pens2.cpxm"
BIN PXM_Pixel	"../res/Stage/Pixel.cpxm"
BIN PXM_Pole	"../res/Stage/Pole.cpxm"
BIN PXM_Prefa1	"../res/Stage/Prefa1.cpxm"
BIN PXM_Prefa2	"../res/Stage/Prefa2.cpxm"
BIN PXM_Priso1	"../res/Stage/Priso1.cpxm"
BIN PXM_Priso2	"../res/Stage/Priso2.cpxm"
BIN PXM_River	"../res/Stage/River.cpxm"
BIN PXM_Sand	"../res/Stage/Sand.cpxm"
BIN PXM_SandE	"../res/Stage/SandE.cpxm"
BIN PXM_Santa	"../res/Stage/Santa.cpxm"
BIN PXM_Shelt	"../res/Stage/Shelt.cpxm"
BIN PXM_Start	"../res/Stage/Start.cpxm"
BIN PXM_Stream	"../res/Stage/Stream.cpxm"
BIN PXM_Weed	"../res/Stage/Weed.cpxm"
BIN PXM_WeedB	"../res/Stage/WeedB.cpxm"
BIN PXM_WeedD	"../res/Stage/WeedD.cpxm"
BIN PXM_WeedS	"../res/Stage/WeedS.cpxm"

# Entities (PXE)
BIN PXE_0       "../res/Stage/0.pxe"
BIN PXE_Almond	"../res/Stage/Almond.pxe"
BIN PXE_Ballo1	"../res/Stage/Ballo1.pxe"
BIN PXE_Ballo2	"../res/Stage/Ballo2.pxe"
BIN PXE_Barr	"../res/Stage/Barr.pxe"
BIN PXE_Blcny1	"../res/Stage/Blcny1.pxe"
BIN PXE_Blcny2	"../res/Stage/Blcny2.pxe"
BIN PXE_Cave	"../res/Stage/Cave.pxe"
BIN PXE_Cemet	"../res/Stage/Cemet.pxe"
BIN PXE_Cent	"../res/Stage/Cent.pxe"
BIN PXE_CentW	"../res/Stage/CentW.pxe"
BIN PXE_Chako	"../res/Stage/Chako.pxe"
BIN PXE_Clock	"../res/Stage/Clock.pxe"
BIN PXE_Comu	"../res/Stage/Comu.pxe"
BIN PXE_Cthu	"../res/Stage/Cthu.pxe"
BIN PXE_Cthu2	"../res/Stage/Cthu2.pxe"
BIN PXE_Curly	"../res/Stage/Curly.pxe"
BIN PXE_CurlyS	"../res/Stage/CurlyS.pxe"
BIN PXE_Dark	"../res/Stage/Dark.pxe"
BIN PXE_Drain	"../res/Stage/Drain.pxe"
BIN PXE_e_Blcn	"../res/Stage/e_Blcn.pxe"
BIN PXE_e_Ceme	"../res/Stage/e_Ceme.pxe"
BIN PXE_e_Jenk	"../res/Stage/e_Jenk.pxe"
BIN PXE_e_Labo	"../res/Stage/e_Labo.pxe"
BIN PXE_e_Malc	"../res/Stage/e_Malc.pxe"
BIN PXE_e_Maze	"../res/Stage/e_Maze.pxe"
BIN PXE_e_Sky	"../res/Stage/e_Sky.pxe"
BIN PXE_EgEnd1	"../res/Stage/EgEnd1.pxe"
BIN PXE_EgEnd2	"../res/Stage/EgEnd2.pxe"
BIN PXE_Egg1	"../res/Stage/Egg1.pxe"
BIN PXE_Egg6	"../res/Stage/Egg6.pxe"
BIN PXE_EggR	"../res/Stage/EggR.pxe"
BIN PXE_EggR2	"../res/Stage/EggR2.pxe"
BIN PXE_Eggs	"../res/Stage/Eggs.pxe"
BIN PXE_Eggs2	"../res/Stage/Eggs2.pxe"
BIN PXE_EggX	"../res/Stage/EggX.pxe"
BIN PXE_EggX2	"../res/Stage/EggX2.pxe"
BIN PXE_Fall	"../res/Stage/Fall.pxe"
BIN PXE_Frog	"../res/Stage/Frog.pxe"
BIN PXE_Gard	"../res/Stage/Gard.pxe"
BIN PXE_Hell1	"../res/Stage/Hell1.pxe"
BIN PXE_Hell2	"../res/Stage/Hell2.pxe"
BIN PXE_Hell3	"../res/Stage/Hell3.pxe"
BIN PXE_Hell4	"../res/Stage/Hell4.pxe"
BIN PXE_Hell42	"../res/Stage/Hell42.pxe"
BIN PXE_Island	"../res/Stage/Island.pxe"
BIN PXE_Itoh	"../res/Stage/Itoh.pxe"
BIN PXE_Jail1	"../res/Stage/Jail1.pxe"
BIN PXE_Jail2	"../res/Stage/Jail2.pxe"
BIN PXE_Jenka1	"../res/Stage/Jenka1.pxe"
BIN PXE_Jenka2	"../res/Stage/Jenka2.pxe"
BIN PXE_Kings	"../res/Stage/Kings.pxe"
BIN PXE_Little	"../res/Stage/Little.pxe"
BIN PXE_Lounge	"../res/Stage/Lounge.pxe"
BIN PXE_Malco	"../res/Stage/Malco.pxe"
BIN PXE_Mapi	"../res/Stage/Mapi.pxe"
BIN PXE_MazeA	"../res/Stage/MazeA.pxe"
BIN PXE_MazeB	"../res/Stage/MazeB.pxe"
BIN PXE_MazeD	"../res/Stage/MazeD.pxe"
BIN PXE_MazeH	"../res/Stage/MazeH.pxe"
BIN PXE_MazeI	"../res/Stage/MazeI.pxe"
BIN PXE_MazeM	"../res/Stage/MazeM.pxe"
BIN PXE_MazeO	"../res/Stage/MazeO.pxe"
BIN PXE_MazeS	"../res/Stage/MazeS.pxe"
BIN PXE_MazeW	"../res/Stage/MazeW.pxe"
BIN PXE_MiBox	"../res/Stage/MiBox.pxe"
BIN PXE_Mimi	"../res/Stage/Mimi.pxe"
BIN PXE_Momo	"../res/Stage/Momo.pxe"
BIN PXE_Oside	"../res/Stage/Oside.pxe"
BIN PXE_Ostep	"../res/Stage/Ostep.pxe"
BIN PXE_Pens1	"../res/Stage/Pens1.pxe"
BIN PXE_Pens2	"../res/Stage/Pens2.pxe"
BIN PXE_Pixel	"../res/Stage/Pixel.pxe"
BIN PXE_Plant	"../res/Stage/Plant.pxe"
BIN PXE_Pole	"../res/Stage/Pole.pxe"
BIN PXE_Pool	"../res/Stage/Pool.pxe"
BIN PXE_Prefa1	"../res/Stage/Prefa1.pxe"
BIN PXE_Prefa2	"../res/Stage/Prefa2.pxe"
BIN PXE_Priso1	"../res/Stage/Priso1.pxe"
BIN PXE_Priso2	"../res/Stage/Priso2.pxe"
BIN PXE_Ring1	"../res/Stage/Ring1.pxe"
BIN PXE_Ring2	"../res/Stage/Ring2.pxe"
BIN PXE_Ring3	"../res/Stage/Ring3.pxe"
BIN PXE_River	"../res/Stage/River.pxe"
BIN PXE_Sand	"../res/Stage/Sand.pxe"
BIN PXE_SandE	"../res/Stage/SandE.pxe"
BIN PXE_Santa	"../res/Stage/Santa.pxe"
BIN PXE_Shelt	"../res/Stage/Shelt.pxe"
BIN PXE_Start	"../res/Stage/Start.pxe"
BIN PXE_Statue	"../res/Stage/Statue.pxe"
BIN PXE_Stream	"../res/Stage/Stream.pxe"
BIN PXE_Weed	"../res/Stage/Weed.pxe"
BIN PXE_WeedB	"../res/Stage/WeedB.pxe"
BIN PXE_WeedD	"../res/Stage/WeedD.pxe"
BIN PXE_WeedS	"../res/Stage/WeedS.pxe"

# NPC Table
BIN     NPC_TABLE		"../res/npc.tbl"

/* Background tilemaps */
# Moon
BIN		PAT_MoonTop		"../res/back/bkMoonTop.pat"
BIN		MAP_MoonTop		"../res/back/bkMoonTop.map"
BIN		PAT_MoonBtm		"../res/back/bkMoonBottom.pat"
BIN		MAP_MoonBtm		"../res/back/bkMoonBottom.map"
# Fog
BIN		PAT_FogTop		"../res/back/bkFogTop.pat"
BIN		MAP_FogTop		"../res/back/bkFogTop.map"
BIN		PAT_FogBtm		"../res/back/bkFogBottom.pat"
BIN		MAP_FogBtm		"../res/back/bkFogBottom.map"
# Sound Test
BIN		PAT_SndTest	    "../res/back/soundtest.pat"
BIN		MAP_SndTest	    "../res/back/soundtest.map"


.section .rodata
/* 0x200000 */

    .globl smp_null
    .globl TILE_BLANK
    .globl BLANK_DATA
smp_null:
TILE_BLANK:
BLANK_DATA:
.rept 16
    .dc.l    0, 0, 0, 0
.endr

    .ascii  "SEGA MEGA DRIVE "
    .ascii  "                "
    .ascii  "https://www.youtube.com/watch?v=y-kIvItmQMc     "
    .ascii  "https://www.youtube.com/watch?v=HnyGSl3K-IE     "
    .ascii  "GM 00001009-00"
    .dc.w    0



/* 0x380000 */
#    .align 0x80000

# Japanese Font - 1bpp bitmap data
#BIN     BMP_Ascii		"../res/ja_ascii.dat"
#BIN     BMP_Kanji		"../res/ja_kanji.dat"

# Japanese stage names and credits text
#BIN     JStageName		"../res/ja_stagename.dat"
#BIN     JCreditStr		"../res/ja_credits.dat"
#BIN     JConfigText		"../res/ja_config.dat"

/* Credits Illustrations */
BIN		PAT_Ill01	"../res/credits/ill01.pat"
BIN		MAP_Ill01	"../res/credits/ill01.map"
BIN		PAT_Ill02	"../res/credits/ill02.pat"
BIN		MAP_Ill02	"../res/credits/ill02.map"
BIN		PAT_Ill03	"../res/credits/ill03.pat"
BIN		MAP_Ill03	"../res/credits/ill03.map"
BIN		PAT_Ill04	"../res/credits/ill04.pat"
BIN		MAP_Ill04	"../res/credits/ill04.map"
BIN		PAT_Ill05	"../res/credits/ill05.pat"
BIN		MAP_Ill05	"../res/credits/ill05.map"
BIN		PAT_Ill06	"../res/credits/ill06.pat"
BIN		MAP_Ill06	"../res/credits/ill06.map"
BIN		PAT_Ill07	"../res/credits/ill07.pat"
BIN		MAP_Ill07	"../res/credits/ill07.map"
BIN		PAT_Ill08	"../res/credits/ill08.pat"
BIN		MAP_Ill08	"../res/credits/ill08.map"
BIN		PAT_Ill09	"../res/credits/ill09.pat"
BIN		MAP_Ill09	"../res/credits/ill09.map"
BIN		PAT_Ill10	"../res/credits/ill10.pat"
BIN		MAP_Ill10	"../res/credits/ill10.map"
BIN		PAT_Ill11	"../res/credits/ill11.pat"
BIN		MAP_Ill11	"../res/credits/ill11.map"
BIN		PAT_Ill12	"../res/credits/ill12.pat"
BIN		MAP_Ill12	"../res/credits/ill12.map"

BIN		PAT_Ill14	"../res/credits/ill14.pat"
BIN		MAP_Ill14	"../res/credits/ill14.map"
BIN		PAT_Ill15	"../res/credits/ill15.pat"
BIN		MAP_Ill15	"../res/credits/ill15.map"
BIN		PAT_Ill16	"../res/credits/ill16.pat"
BIN		MAP_Ill16	"../res/credits/ill16.map"
BIN		PAT_Ill17	"../res/credits/ill17.pat"
BIN		MAP_Ill17	"../res/credits/ill17.map"
BIN		PAT_Ill18	"../res/credits/ill18.pat"
BIN		MAP_Ill18	"../res/credits/ill18.map"

/* Level Select data */
BIN LS_00		"../res/save/00_firstcave.sram-trim"
BIN LS_01		"../res/save/01_mimigavillage.sram-trim"
BIN LS_02		"../res/save/02_eggcorridor.sram-trim"
BIN LS_03		"../res/save/03_grasstown.sram-trim"
BIN LS_04		"../res/save/04_malco.sram-trim"
BIN LS_05		"../res/save/05_balfrog.sram-trim"
BIN LS_06		"../res/save/06_sandzone.sram-trim"
BIN LS_07		"../res/save/07_omega.sram-trim"
BIN LS_08		"../res/save/08_storehouse.sram-trim"
BIN LS_09		"../res/save/09_labyrinth.sram-trim"
BIN LS_10		"../res/save/10_monsterx.sram-trim"
BIN LS_11		"../res/save/11_labyrinthm.sram-trim"
BIN LS_12		"../res/save/12_core.sram-trim"
BIN LS_13		"../res/save/13_waterway.sram-trim"
BIN LS_14		"../res/save/14_eggcorridor2.sram-trim"
BIN LS_15		"../res/save/15_outerwall.sram-trim"
BIN LS_16		"../res/save/16_plantation.sram-trim"
BIN LS_17		"../res/save/17_lastcave.sram-trim"
BIN LS_18		"../res/save/18_lastcave2.sram-trim"
BIN LS_19		"../res/save/19_balcony.sram-trim"
BIN LS_20		"../res/save/20_sacredground.sram"
BIN LS_21		"../res/save/21_sealchamber.sram"


/* Start of localizable data */
.ascii "LANGDAT\0"

    .globl LANGUAGE
LANGUAGE:
.ascii "EN\0\0"

    .globl BMP_ASCII
BMP_ASCII:
.dc.l 0
    .globl BMP_KANJI
BMP_KANJI:
.dc.l 0

    .globl STAGE_NAMES
STAGE_NAMES:
.dc.l 0
    .globl CREDITS_STR
CREDITS_STR:
.dc.l 0
    .globl CONFIG_STR
CONFIG_STR:
.dc.l 0

/* Pointer Tables */
    .globl TSC_GLOB
TSC_GLOB:
.dc.l TSC_ArmsItem
.dc.l TSC_Head
.dc.l TSC_StageSelect
.dc.l TSC_Credits
    .globl TSC_STAGE
TSC_STAGE:
.dc.l 0
.dc.l TSC_Pens1
.dc.l TSC_Eggs
.dc.l TSC_EggX
.dc.l TSC_Egg6
.dc.l TSC_EggR
.dc.l TSC_Weed
.dc.l TSC_Santa
.dc.l TSC_Chako
.dc.l TSC_MazeI
.dc.l TSC_Sand
.dc.l TSC_Mimi
.dc.l TSC_Cave
.dc.l TSC_Start
.dc.l TSC_Barr
.dc.l TSC_Pool
.dc.l TSC_Cemet
.dc.l TSC_Plant
.dc.l TSC_Shelt
.dc.l TSC_Comu
.dc.l TSC_MiBox
.dc.l TSC_EgEnd1
.dc.l TSC_Cthu
.dc.l TSC_Egg1
.dc.l TSC_Pens2
.dc.l TSC_Malco
.dc.l TSC_WeedS
.dc.l TSC_WeedD
.dc.l TSC_Frog
.dc.l TSC_Curly
.dc.l TSC_WeedB
.dc.l TSC_Stream
.dc.l TSC_CurlyS
.dc.l TSC_Jenka1
.dc.l TSC_Dark
.dc.l TSC_Gard
.dc.l TSC_Jenka2
.dc.l TSC_SandE
.dc.l TSC_MazeH
.dc.l TSC_MazeW
.dc.l TSC_MazeO
.dc.l TSC_MazeD
.dc.l TSC_MazeA
.dc.l TSC_MazeB
.dc.l TSC_MazeS
.dc.l TSC_MazeM
.dc.l TSC_Drain
.dc.l TSC_Almond
.dc.l TSC_River
.dc.l TSC_Eggs2
.dc.l TSC_Cthu2
.dc.l TSC_EggR2
.dc.l TSC_EggX2
.dc.l TSC_Oside
.dc.l TSC_EgEnd2
.dc.l TSC_Itoh
.dc.l TSC_Cent
.dc.l TSC_Jail1
.dc.l TSC_Momo
.dc.l TSC_Lounge
.dc.l TSC_CentW
.dc.l TSC_Jail2
.dc.l TSC_Blcny1
.dc.l TSC_Priso1
.dc.l TSC_Ring1
.dc.l TSC_Ring2
.dc.l TSC_Prefa1
.dc.l TSC_Priso2
.dc.l TSC_Ring3
.dc.l TSC_Little
.dc.l TSC_Blcny2
.dc.l TSC_Fall
.dc.l TSC_Kings
.dc.l TSC_Pixel
.dc.l TSC_e_Maze
.dc.l TSC_e_Jenk
.dc.l TSC_e_Malc
.dc.l TSC_e_Ceme
.dc.l TSC_e_Sky
.dc.l TSC_Prefa2
.dc.l TSC_Hell1
.dc.l TSC_Hell2
.dc.l TSC_Hell3
.dc.l TSC_Mapi
.dc.l TSC_Hell4
.dc.l TSC_Hell42
.dc.l TSC_Statue
.dc.l TSC_Ballo1
.dc.l TSC_Ostep
.dc.l TSC_e_Labo
.dc.l TSC_Pole
.dc.l TSC_Island
.dc.l TSC_Ballo2
.dc.l TSC_e_Blcn
.dc.l TSC_Clock

# Scripts (TSC) - English
# Global
BIN TSC_ArmsItem	"../res/tsc/en/ArmsItem.tsb"
BIN TSC_Head		"../res/tsc/en/Head.tsb"
BIN TSC_StageSelect	"../res/tsc/en/StageSelect.tsb"
BIN TSC_Credits		"../res/tsc/en/Stage/0.tsb"
# Stage Specific
BIN TSC_Almond	"../res/tsc/en/Stage/Almond.tsb"
BIN TSC_Ballo1	"../res/tsc/en/Stage/Ballo1.tsb"
BIN TSC_Ballo2	"../res/tsc/en/Stage/Ballo2.tsb"
BIN TSC_Barr	"../res/tsc/en/Stage/Barr.tsb"
BIN TSC_Blcny1	"../res/tsc/en/Stage/Blcny1.tsb"
BIN TSC_Blcny2	"../res/tsc/en/Stage/Blcny2.tsb"
BIN TSC_Cave	"../res/tsc/en/Stage/Cave.tsb"
BIN TSC_Cemet	"../res/tsc/en/Stage/Cemet.tsb"
BIN TSC_Cent	"../res/tsc/en/Stage/Cent.tsb"
BIN TSC_CentW	"../res/tsc/en/Stage/CentW.tsb"
BIN TSC_Chako	"../res/tsc/en/Stage/Chako.tsb"
BIN TSC_Clock	"../res/tsc/en/Stage/Clock.tsb"
BIN TSC_Comu	"../res/tsc/en/Stage/Comu.tsb"
BIN TSC_Cthu	"../res/tsc/en/Stage/Cthu.tsb"
BIN TSC_Cthu2	"../res/tsc/en/Stage/Cthu2.tsb"
BIN TSC_Curly	"../res/tsc/en/Stage/Curly.tsb"
BIN TSC_CurlyS	"../res/tsc/en/Stage/CurlyS.tsb"
BIN TSC_Dark	"../res/tsc/en/Stage/Dark.tsb"
BIN TSC_Drain	"../res/tsc/en/Stage/Drain.tsb"
BIN TSC_e_Blcn	"../res/tsc/en/Stage/e_Blcn.tsb"
BIN TSC_e_Ceme	"../res/tsc/en/Stage/e_Ceme.tsb"
BIN TSC_e_Jenk	"../res/tsc/en/Stage/e_Jenk.tsb"
BIN TSC_e_Labo	"../res/tsc/en/Stage/e_Labo.tsb"
BIN TSC_e_Malc	"../res/tsc/en/Stage/e_Malc.tsb"
BIN TSC_e_Maze	"../res/tsc/en/Stage/e_Maze.tsb"
BIN TSC_e_Sky	"../res/tsc/en/Stage/e_Sky.tsb"
BIN TSC_EgEnd1	"../res/tsc/en/Stage/EgEnd1.tsb"
BIN TSC_EgEnd2	"../res/tsc/en/Stage/EgEnd2.tsb"
BIN TSC_Egg1	"../res/tsc/en/Stage/Egg1.tsb"
BIN TSC_Egg6	"../res/tsc/en/Stage/Egg6.tsb"
BIN TSC_EggR	"../res/tsc/en/Stage/EggR.tsb"
BIN TSC_EggR2	"../res/tsc/en/Stage/EggR2.tsb"
BIN TSC_Eggs	"../res/tsc/en/Stage/Eggs.tsb"
BIN TSC_Eggs2	"../res/tsc/en/Stage/Eggs2.tsb"
BIN TSC_EggX	"../res/tsc/en/Stage/EggX.tsb"
BIN TSC_EggX2	"../res/tsc/en/Stage/EggX2.tsb"
BIN TSC_Fall	"../res/tsc/en/Stage/Fall.tsb"
BIN TSC_Frog	"../res/tsc/en/Stage/Frog.tsb"
BIN TSC_Gard	"../res/tsc/en/Stage/Gard.tsb"
BIN TSC_Hell1	"../res/tsc/en/Stage/Hell1.tsb"
BIN TSC_Hell2	"../res/tsc/en/Stage/Hell2.tsb"
BIN TSC_Hell3	"../res/tsc/en/Stage/Hell3.tsb"
BIN TSC_Hell4	"../res/tsc/en/Stage/Hell4.tsb"
BIN TSC_Hell42	"../res/tsc/en/Stage/Hell42.tsb"
BIN TSC_Island	"../res/tsc/en/Stage/Island.tsb"
BIN TSC_Itoh	"../res/tsc/en/Stage/Itoh.tsb"
BIN TSC_Jail1	"../res/tsc/en/Stage/Jail1.tsb"
BIN TSC_Jail2	"../res/tsc/en/Stage/Jail2.tsb"
BIN TSC_Jenka1	"../res/tsc/en/Stage/Jenka1.tsb"
BIN TSC_Jenka2	"../res/tsc/en/Stage/Jenka2.tsb"
BIN TSC_Kings	"../res/tsc/en/Stage/Kings.tsb"
BIN TSC_Little	"../res/tsc/en/Stage/Little.tsb"
BIN TSC_Lounge	"../res/tsc/en/Stage/Lounge.tsb"
BIN TSC_Malco	"../res/tsc/en/Stage/Malco.tsb"
BIN TSC_Mapi	"../res/tsc/en/Stage/Mapi.tsb"
BIN TSC_MazeA	"../res/tsc/en/Stage/MazeA.tsb"
BIN TSC_MazeB	"../res/tsc/en/Stage/MazeB.tsb"
BIN TSC_MazeD	"../res/tsc/en/Stage/MazeD.tsb"
BIN TSC_MazeH	"../res/tsc/en/Stage/MazeH.tsb"
BIN TSC_MazeI	"../res/tsc/en/Stage/MazeI.tsb"
BIN TSC_MazeM	"../res/tsc/en/Stage/MazeM.tsb"
BIN TSC_MazeO	"../res/tsc/en/Stage/MazeO.tsb"
BIN TSC_MazeS	"../res/tsc/en/Stage/MazeS.tsb"
BIN TSC_MazeW	"../res/tsc/en/Stage/MazeW.tsb"
BIN TSC_MiBox	"../res/tsc/en/Stage/MiBox.tsb"
BIN TSC_Mimi	"../res/tsc/en/Stage/Mimi.tsb"
BIN TSC_Momo	"../res/tsc/en/Stage/Momo.tsb"
BIN TSC_Oside	"../res/tsc/en/Stage/Oside.tsb"
BIN TSC_Ostep	"../res/tsc/en/Stage/Ostep.tsb"
BIN TSC_Pens1	"../res/tsc/en/Stage/Pens1.tsb"
BIN TSC_Pens2	"../res/tsc/en/Stage/Pens2.tsb"
BIN TSC_Pixel	"../res/tsc/en/Stage/Pixel.tsb"
BIN TSC_Plant	"../res/tsc/en/Stage/Plant.tsb"
BIN TSC_Pole	"../res/tsc/en/Stage/Pole.tsb"
BIN TSC_Pool	"../res/tsc/en/Stage/Pool.tsb"
BIN TSC_Prefa1	"../res/tsc/en/Stage/Prefa1.tsb"
BIN TSC_Prefa2	"../res/tsc/en/Stage/Prefa2.tsb"
BIN TSC_Priso1	"../res/tsc/en/Stage/Priso1.tsb"
BIN TSC_Priso2	"../res/tsc/en/Stage/Priso2.tsb"
BIN TSC_Ring1	"../res/tsc/en/Stage/Ring1.tsb"
BIN TSC_Ring2	"../res/tsc/en/Stage/Ring2.tsb"
BIN TSC_Ring3	"../res/tsc/en/Stage/Ring3.tsb"
BIN TSC_River	"../res/tsc/en/Stage/River.tsb"
BIN TSC_Sand	"../res/tsc/en/Stage/Sand.tsb"
BIN TSC_SandE	"../res/tsc/en/Stage/SandE.tsb"
BIN TSC_Santa	"../res/tsc/en/Stage/Santa.tsb"
BIN TSC_Shelt	"../res/tsc/en/Stage/Shelt.tsb"
BIN TSC_Start	"../res/tsc/en/Stage/Start.tsb"
BIN TSC_Statue	"../res/tsc/en/Stage/Statue.tsb"
BIN TSC_Stream	"../res/tsc/en/Stage/Stream.tsb"
BIN TSC_Weed	"../res/tsc/en/Stage/Weed.tsb"
BIN TSC_WeedB	"../res/tsc/en/Stage/WeedB.tsb"
BIN TSC_WeedD	"../res/tsc/en/Stage/WeedD.tsb"
BIN TSC_WeedS	"../res/tsc/en/Stage/WeedS.tsb"
