


#---------------------------------------------------
#---------------CSMD makefile section---------------
#---------------------------------------------------

MARSDEV ?= ${HOME}/mars
MARSBIN  = $(MARSDEV)/m68k-elf/bin
TOOLSBIN = $(MARSDEV)/bin

TARGET = doukutsu

# Z80 Assembler to build XGM driver
ASMZ80   = $(TOOLSBIN)/sjasm
# SGDK Tools
BINTOS   = ../bin/bintos
RESCOMP  = ../bin/rescomp
WAVTORAW = ../bin/wavtoraw
XGMTOOL  = bin/xgmtool
# Sik's Tools
MDTILER  = $(TOOLSBIN)/mdtiler
SLZ      = $(TOOLSBIN)/slz
UFTC     = $(TOOLSBIN)/uftc
# Cave Story Tools
TSCOMP   = ../bin/tscomp
PATCHROM = bin/patchrom

# Some files needed are in a versioned directory
GCC_VER := $(shell $(CC) -dumpversion)
PLUGIN   = $(MARSDEV)/m68k-elf/libexec/gcc/m68k-elf/$(GCC_VER)
LTO_SO   = liblto_plugin.so
ifeq ($(OS),Windows_NT)
    LTO_SO = liblto_plugin-0.dll
endif

INCS     = -Isrc -Ires -Iinc
LIBS     = -L$(MARSDEV)/m68k-elf/lib/gcc/m68k-elf/$(GCC_VER)
CCFLAGS  = -m68000 -Wall -Wextra -std=c99 -ffreestanding -fcommon -mshort
OPTIONS  =
ASFLAGS  = -m68000 --register-prefix-optional
LDFLAGS  = -T mdssf.ld -nostdlib
Z80FLAGS = -isrc/xgm

# Stage layout files to compress
PXMS  = $(wildcard ../res/Stage/*.pxm)
PXMS += $(wildcard ../res/Stage/*/*.pxm)
CPXMS = $(PXMS:.pxm=.cpxm)

# Tilesets to compress
TSETS  = $(wildcard ../res/Stage/*_vert.png)
TSETS += $(wildcard ../res/Stage/**/*_vert.png)
PTSETS = $(TSETS:.png=.pat)
CTSETS = $(TSETS:.png=.uftc)

# TSC to convert to TSB
TSCS  = $(wildcard ../res/tsc/en/*.txt)
TSCS += $(wildcard ../res/tsc/en/Stage/*.txt)
TSBS  = $(TSCS:.txt=.tsb)

# TSBs for translations
TL_TSCS  = $(wildcard ../res/tsc/*/*.txt)
TL_TSCS += $(wildcard ../res/tsc/*/Stage/*.txt)
TL_TSBS  = $(TL_TSCS:.txt=.tsb)

# mdtiler scripts to generate tile patterns & mappings
MDTS  = $(wildcard ../res/*.mdt)
MDTS += $(wildcard ../res/*/*.mdt)
PATS  = $(MDTS:.mdt=.pat)
MAPS  = $(MDTS:.mdt=.map)

# VGM files to convert to XGC
VGMS  = $(wildcard res/bgm/*.vgm)
XGCS  = $(VGMS:.vgm=.xgc)

# WAV files to convert to raw PCM
WAVS  = $(wildcard ../res/sfx/*.wav)
PCMS  = $(WAVS:.wav=.pcm)

RESS  = res/resources.res

#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/gba_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary data
# GRAPHICS is a list of directories containing files to be processed by grit
#
# All directories are specified relative to the project directory where
# the makefile is found
#
#---------------------------------------------------------------------------------
TARGET		:= $(notdir $(CURDIR))
BUILD		:= build
SOURCES		:= src
INCLUDES	:= inc res
RESOURCES   := res
DATA		:=
MUSIC		:= maxmod_data

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-mthumb -mthumb-interwork

CFLAGS	:=	-g -Wall -O3\
		-mcpu=arm7tdmi -mtune=arm7tdmi\
		$(ARCH)

CFLAGS	+=	$(INCLUDE)

CXXFLAGS	:=	$(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-g $(ARCH) -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= -lmm -lgba


#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=	$(LIBGBA)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------


ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)/ai) \
			$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)/db) \
			$(foreach dir,$(RESOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
			$(foreach dir,$(GRAPHICS),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CFILES	+=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/ai/*.c)))
CFILES	+=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/db/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s))) 
SFILES		+=	$(foreach dir,$(RESOURCES),$(notdir $(wildcard $(dir)/*.s))) 
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

ifneq ($(strip $(MUSIC)),)
	export AUDIOFILES	:=	$(foreach dir,$(notdir $(wildcard $(MUSIC)/*.*)),$(CURDIR)/$(MUSIC)/$(dir))
	BINFILES += soundbank.bin
endif

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN := $(addsuffix .o,$(BINFILES)) 

export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
 
export OFILES := $(OFILES_BIN) $(OFILES_SOURCES) 

export HFILES := $(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-iquote $(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	rm -fr $(BUILD) $(TARGET).elf $(TARGET).gba
	rm -f $(CPXMS) $(XGCS) $(PCMS) $(PATS) $(MAPS) $(PTSETS) $(CTSETS) $(ZOBJ) $(OBJS)
	rm -f $(TSBS) $(TL_TSBS)
	rm -f res/patches/*.patch
	rm -f src/xgm/z80_xgm.s src/xgm/z80_xgm.o80 src/xgm/z80_xgm.h out.lst
	rm -f res/resources.h res/resources.s inc/ai_gen.h


#---------------------------------------------------------------------------------
else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------



$(OUTPUT).gba	:	$(OUTPUT).elf

$(OUTPUT).elf	:	prereq $(OFILES)

$(OFILES_SOURCES) : $(HFILES)

prereq: $(RESCOMP) resources.s head-gen grit-gen $(BINTOS) $(TSCOMP) $(WAVTORAW) 
prereq: $(CPXMS) $(XGCS) $(PCMS) $(CTSETS) $(ZOBJ) $(TSBS) $(PATS)

head-gen:
	rm -f inc/ai_gen.h
	python ../aigen.py

grit-gen:
	python3 ../gritgen.py


$(RESCOMP):
	cc ../tools/rescomp/src/*.c -Itools/rescomp/inc -o $@

resources.s: 
	$(RESCOMP) ../res/resources.res 

%.o: %.s 
	@echo "AS $<"
	@$(AS) $(ASFLAGS) $< -o $@

%.s: %.res
	$(RESCOMP) $< $@


$(BINTOS): 
	cc ../tools/bintos/src/*.c -o $@
	


$(XGMTOOL): bin
	cc ../tools/xgmtool/src/*.c -Itools/xgmtool/inc -o $@ -lm

$(WAVTORAW): 
	cc ../tools/wavtoraw/src/*.c -o $@ -lm

# Cave Story tools
$(TSCOMP): 
	cc ../tools/tscomp/tscomp.c -o $@

$(PATCHROM): bin
	cc tools/patchrom/patchrom.c -o $@


# Compression of stage layouts
%.cpxm: %.pxm
	$(SLZ) -c "$<" "$@"

#%.pat: %.mdt
#	$(MDTILER) -b "$(CURDIR)/$<"

# Compression of tilesets
%.uftc: %.pat
	$(UFTC) -c "$<" "$@"

#%.pat: %.png
#	$(MDTILER) -b "$(CURDIR)/$<"

# Convert VGM
%.xgc: %.vgm
	$(XGMTOOL) "$<" "$@" -s

# Convert WAV
%.pcm: %.wav
	$(WAVTORAW) "$<" "$@" 14000

# Convert TSC
../res/tsc/en/%.tsb: ../res/tsc/en/%.txt
	$(TSCOMP) -l=en "$<"
../res/tsc/ja/%.tsb: ../res/tsc/ja/%.txt
	$(TSCOMP) -l=ja "$<"
../res/tsc/es/%.tsb: ../res/tsc/es/%.txt
	$(TSCOMP) -l=es "$<"
../res/tsc/pt/%.tsb: ../res/tsc/pt/%.txt
	$(TSCOMP) -l=pt "$<"
../res/tsc/fr/%.tsb: ../res/tsc/fr/%.txt
	$(TSCOMP) -l=fr "$<"
../res/tsc/it/%.tsb: ../res/tsc/it/%.txt
	$(TSCOMP) -l=it "$<"
../res/tsc/de/%.tsb: ../res/tsc/de/%.txt
	$(TSCOMP) -l=de "$<"
../res/tsc/br/%.tsb: ../res/tsc/br/%.txt
	$(TSCOMP) -l=br "$<"
../res/tsc/zh/%.tsb: ../res/tsc/zh/%.txt
	$(TSCOMP) -l=zh "$<"
../res/tsc/ko/%.tsb: ../res/tsc/ko/%.txt
	$(TSCOMP) -l=ko "$<"

# Generate patches
res/patches/$(TARGET)-%.patch: res/patches/$(TARGET)-%.s
	$(AS) $(ASFLAGS) "$<" -o "temp.o"
	$(LD) $(LDFLAGS) "temp.o" -o "temp.elf"
	$(OBJC) -O binary "temp.elf" "$@"


%.s: %.res
	$(RESCOMP) $< $@

#---------------------------------------------------------------------------------
# The bin2o rule should be copied and modified
# for each extension used in the data directories
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
# rule to build soundbank from music files
#---------------------------------------------------------------------------------
soundbank.bin soundbank.h : $(AUDIOFILES)
#---------------------------------------------------------------------------------
	@mmutil $^ -osoundbank.bin -hsoundbank.h

#---------------------------------------------------------------------------------
# This rule links in binary data with the .bin extension
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)


-include $(DEPSDIR)/*.d
#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
