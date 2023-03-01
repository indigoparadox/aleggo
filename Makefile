
# vim: ft=make noexpandtab

C_FILES := src/main.c src/grid.c
WING=1

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: aleggo.sdl aleggo.ale aleggod.exe aleggow.exe aleggo.html aleggow3.exe

$(eval $(call DIRTOXPMS,blocks,src))

# Nintendo DS

$(eval $(call TGTNDSLIBN,aleggo,aleggo.bmp))

# Unix Allegro

$(eval $(call TGTUNIXALE,aleggo))

# Unix SDL

$(eval $(call TGTSDLICO,aleggo))

$(eval $(call TGTUNIXSDL,aleggo))

# WASM

$(eval $(call TGTWASMSDL,aleggo))

# DOS

$(eval $(call TGTDOSALE,aleggo))

# WinNT

$(eval $(call TGTWINICO,aleggo))

$(eval $(call TGTWINNT,aleggo))

# Win386

$(eval $(call TGTWIN16,aleggo))

$(eval $(call TGTWIN386,aleggo))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

