
# vim: ft=make noexpandtab

include maug/Makexpm.inc

ALEGGO_C_FILES := src/main.c src/grid.c

CC_GCC := gcc
CC_WATCOM := wcc386
LD_WATCOM := wcl386
MD := mkdir -p

GLOBAL_DEFINES :=

CFLAGS_GCC := $(GLOBAL_DEFINES) -Imaug/src
CFLAGS_WATCOM := $(GLOBAL_DEFINES) -imaug/src
LDFLAGS_GCC := -lm
LDFLAGS_WATCOM :=

# Optional builds.
ifneq ("$(RELEASE)","RELEASE")
	CFLAGS_WATCOM += -we -d3
	CFLAGS_GCC += -DDEBUG -Wall -g -fsanitize=address -fsanitize=leak -fsanitize=undefined -DDEBUG -DDEBUG_LOG -DDEBUG_THRESHOLD=1
	LDFLAGS_GCC += -g -fsanitize=address -fsanitize=leak -fsanitize=undefined
endif

ifeq ("$(SDL_VER)","1")
	CFLAGS_GCC_SDL := -DRETROFLAT_API_SDL1 $(shell pkg-config sdl --cflags)
	LDFLAGS_GCC_SDL := $(shell pkg-config sdl --libs) -lSDL_ttf
else
	CFLAGS_GCC_SDL := -DRETROFLAT_API_SDL2 $(shell pkg-config sdl2 --cflags)
	LDFLAGS_GCC_SDL := $(shell pkg-config sdl2 --libs) -lSDL_ttf
endif

# Target-specific options.
.PHONY: clean

all: aleggo.sdl aleggo.ale aleggd.exe aleggw.exe aleggo.html

$(eval $(call DIRTOXPMS,blocks,src))

# Unix Allegro

aleggo.ale: $(addprefix obj/$(shell uname -s)-allegro/,$(subst .c,.o,$(ALEGGO_C_FILES)))
	$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC) $(shell pkg-config allegro --libs)

obj/$(shell uname -s)-allegro/%.o: %.c | src/blocks_xpm.h
	$(MD) $(dir $@)
	$(CC_GCC) -c -o $@ $< -DRETROFLAT_OS_UNIX $(CFLAGS_GCC) -DBLOCKS_XPM \
		-DRETROFLAT_API_ALLEGRO $(shell pkg-config allegro --cflags)

# Unix SDL

aleggo.sdl: $(addprefix obj/$(shell uname -s)-sdl/,$(subst .c,.o,$(ALEGGO_C_FILES)))
	$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC) $(LDFLAGS_GCC_SDL)

obj/$(shell uname -s)-sdl/%.o: %.c | src/blocks_xpm.h
	$(MD) $(dir $@)
	$(CC_GCC) -c -o $@ $< -DRETROFLAT_OS_UNIX $(CFLAGS_GCC) $(CFLAGS_GCC_SDL) \
		-DBLOCKS_XPM

# WASM

aleggo.html: $(addprefix obj/wasm/,$(subst .c,.o,$(ALEGGO_C_FILES)))
	emcc -o $@ $^ -s USE_SDL=2 -s USE_SDL_TTF=2

obj/wasm/%.o: %.c | src/blocks_xpm.h
	$(MD) $(dir $@)
	emcc -c -o $@ $< -DRETROFLAT_OS_WASM -DRETROFLAT_API_SDL2 -s USE_SDL=2 -Imaug/src -s USE_SDL_TTF=2 -DBLOCKS_XPM

# DOS

aleggd.exe: $(addprefix obj/dos/,$(subst .c,.o,$(ALEGGO_C_FILES)))
	#i586-pc-msdosdjgpp-gcc -o $@ $^ -L$(ALLEGRO_DJGPP_ROOT)/lib -lalleg
	wcl386 -l=dos32a -s -3s -k128k dos/clib3s.lib alleg.lib $(LDFLAGS_WATCOM) -fe=$@ $^

obj/dos/%.o: %.c | src/blocks_xpm.h
	$(MD) $(dir $@)
	#i586-pc-msdosdjgpp-gcc -fgnu89-inline -I$(ALLEGRO_DJGPP_ROOT)/include -DRETROFLAT_OS_DOS -DRETROFLAT_API_ALLEGRO -c -o $@ $<
	wcc386 -bt=dos32a -s -3s -DRETROFLAT_OS_DOS -DRETROFLAT_API_ALLEGRO $(CFLAGS_WATCOM) -fo=$@ $(<:%.c=%) -DBLOCKS_XPM

# WinNT

aleggnt.exe: $(addprefix obj/nt/,$(subst .c,.o,$(ALEGGO_C_FILES)))
	wlink name $@ system nt_win libr wing32 fil {$^}

obj/nt/%.o: %.c | src/blocks_xpm.h
	$(MD) $(dir $@)
	wcc386 -bt=nt -i$(WATCOM)/h/nt -DRETROFLAT_API_WIN32 -DRETROFLAT_OS_WIN $(CFLAGS_WATCOM) -fo=$@ $(<:%.c=%) -DBLOCKS_XPM

# Win386

aleggw.rex: $(addprefix obj/win16/,$(subst .c,.o,$(ALEGGO_C_FILES)))
	wcl386 -l=win386 $(LDFLAGS_WATCOM) -fe=$@ $^

aleggw.exe: aleggw.rex
	wbind $< -s $(WATCOM)/binw/win386.ext -R $@

obj/win16/%.o: %.c | src/blocks_xpm.h
	$(MD) $(dir $@)
	wcc386 -bt=windows -i$(WATCOM)/h/win -DRETROFLAT_API_WIN16 -DRETROFLAT_OS_WIN $(CFLAGS_WATCOM) -fo=$@ $(<:%.c=%) -DBLOCKS_XPM

# Clean

clean:
	rm -rf obj aleggo.sdl aleggo.ale aleggw32.exe *.err aleggd.exe aleggw.exe *.rex aleggnt.exe src/blocks_xpm.h aleggo.html aleggo.wasm aleggo.js

