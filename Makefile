
# vim: ft=make noexpandtab

#WING=1
VDP=1
C_FILES := src/main.c src/grid.c
RETROFLAT_DOS_MEM_LARGE=1
GLOBAL_DEFINES += -DRETROCON_TRACE_LVL=1

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: aleggo.$(shell uname -m).sdl aleggo.$(shell uname -m).ale aleggod.exe aleggow.exe aleggo.html aleggow3.exe

#$(eval $(call DIRTOXPMS,blocks,src))

# Unix Allegro

$(eval $(call TGTUNIXALE,aleggo))

# Unix SDL

$(eval $(call TGTSDLICO,aleggo))

$(eval $(call TGTUNIXSDL,aleggo))

# WASM

$(eval $(call TGTWASMSDL,aleggo))

# DOS

$(eval $(call TGTDOSALE,aleggo))

$(eval $(call TGTDOSBIOS,aleggo))

# Win386

$(eval $(call TGTWINICO,aleggo))

$(eval $(call TGTWIN386,aleggo))

$(eval $(call TGTWIN16,aleggo))

# WinNT

$(eval $(call TGTWINNT,aleggo))

$(eval $(call TGTWINSDL,aleggo))

# OS/2

$(eval $(call TGTOS2SDL,aleggo))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

