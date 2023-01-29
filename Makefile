
# vim: ft=make noexpandtab

C_FILES := src/main.c src/grid.c

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: aleggo.sdl aleggo.ale aleggod.exe aleggow.exe aleggo.html

$(eval $(call DIRTOXPMS,blocks,src))

# Unix Allegro

$(eval $(call TGTUNIXALE,aleggo,src/blocks_xpm.h))

# Unix SDL

$(eval $(call TGTUNIXSDL,aleggo,src/blocks_xpm.h))

# WASM

$(eval $(call TGTWASMSDL,aleggo,src/blocks_xpm.h))

# DOS

$(eval $(call TGTDOSALE,aleggo,src/blocks_xpm.h))

# WinNT

$(eval $(call TGTWINNT,aleggo,src/blocks_xpm.h))

# Win386

$(eval $(call TGTWIN386,aleggo,src/blocks_xpm.h))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

