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

BIN SPR_Quote_data "../res/sprite/quote.img.bin"

BIN BG_Blue		"../res/back/bkBlue.img.bin" 
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
