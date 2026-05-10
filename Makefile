#---------------------------------------------------
#--------------- GBA Makefile section --------------
#---------------------------------------------------

TARGET = doukutsu

# Local Tools (These are built automatically via the Makefile)
MMUTIL   = ../bin/mmutil
BINTOS   = ../bin/bintos
RESCOMP  = ../bin/rescomp
WAVTORAW = ../bin/wavtoraw
XGMTOOL  = bin/xgmtool
TSCOMP   = ../bin/tscomp
PATCHROM = bin/patchrom

# Sik's Tools Source and Binaries
TOOLSMD  = ../toolsmd
SLZ      = ../bin/slz

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
#---------------------------------------------------------------------------------
TARGET		:= $(notdir $(CURDIR))
BUILD		:= build
SOURCES		:= src
INCLUDES	:= inc res src
RESOURCES   := res
DATA		:=
MUSIC		:= maxmod_data

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-mthumb -mthumb-interwork

LTO ?= 0

CFLAGS	:=	-g -Wall -Wno-builtin-declaration-mismatch -Os \
		-mcpu=arm7tdmi -mtune=arm7tdmi\
		$(ARCH)

CFLAGS	+=	$(INCLUDE)

CXXFLAGS	:=	$(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS	:=	-g $(ARCH)

LDFLAGS = -g $(ARCH) -Wl,-Map,$(notdir $*.map)

ifeq ($(LTO),1)
	CFLAGS  += -flto
	LDFLAGS += -flto=auto
endif

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
BINFILES := soundbank.bin
ifneq ($(strip $(MUSIC)),)
    export AUDIOFILES := $(foreach dir,$(notdir $(wildcard $(MUSIC)/*.*)),$(CURDIR)/$(MUSIC)/$(dir))
	SFILES += resources.s
endif

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
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
	rm -f res/resources.h res/resources.s src/ai_gen.h
	rm -f bin/rescomp bin/mmutil

#---------------------------------------------------------------------------------
else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

$(OUTPUT).gba	:	$(OUTPUT).elf

$(OUTPUT).elf : prereq $(OFILES)

$(OFILES_SOURCES) : $(HFILES)

$(OFILES_SOURCES) : resources.h ../src/ai_gen.h soundbank.h soundbank_bin.h

soundbank_bin.h: soundbank.bin

main.o: soundbank.h

prereq: $(RESCOMP) resources.s resources.h ../src/ai_gen.h grit-gen.stamp $(BINTOS) $(TSCOMP) $(WAVTORAW) soundbank.h
prereq: $(SLZ)
prereq: $(CPXMS) $(XGCS) $(PCMS) $(CTSETS) $(ZOBJ) $(TSBS)

# Sik's Tools compilation rules
$(SLZ):
	@mkdir -p $(dir $@)
	cc $(TOOLSMD)/slz/tool/*.c -o $@

../src/ai_gen.h: ../aigen.py
	python ../aigen.py
		
grit-gen.stamp: ../gritgen.py
	python ../gritgen.py
	touch $@

$(RESCOMP):
	cc -g ../tools/rescomp/src/*.c -Itools/rescomp/inc -o $@

resources.stamp: ../res/resources.res $(RESCOMP)
	$(RESCOMP) $< resources.s
	@touch $@ resources.s resources.h

resources.s resources.h: resources.stamp
	@:

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

# Local mmutil build
$(MMUTIL):
	@mkdir -p $(dir $@)
	cc -O3 ../mmutil/source/*.c -o $@ -lm -Wno-multichar -Wno-unused-result -DPACKAGE_VERSION=\"6.7\"

# 1. Compress stage layouts (PXM -> CPXM)
%.cpxm: %.pxm | $(SLZ)
	$(SLZ) -c "$<" "$@"

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
# rule to build soundbank from music files
#---------------------------------------------------------------------------------
soundbank.stamp: $(AUDIOFILES) $(MMUTIL)
	@echo "Generating Soundbank"
	@$(MMUTIL) $(AUDIOFILES) -osoundbank.bin -hsoundbank.h
	@touch $@

soundbank.bin soundbank.h: soundbank.stamp
	@:

soundbank.bin.o: soundbank.bin

#---------------------------------------------------------------------------------
# This rule links in binary data with the .bin extension
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
	@echo $(notdir $<)
	@$(bin2o)


-include $(DEPSDIR)/*.d
#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------