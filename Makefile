
# vim: ft=make noexpandtab

ALLEGO_C_FILES := src/main.c src/grid.c

CC_GCC := gcc
CC_WATCOM := wcc386
LD_WATCOM := wcl386
MD := mkdir -p

CFLAGS_GCC := -Imaug/src
CFLAGS_WATCOM := -imaug/src
LDFLAGS_GCC :=
LDFLAGS_WATCOM :=

# Optional builds.
ifneq ("$(RELEASE)","RELEASE")
	CFLAGS_WATCOM += 
	CFLAGS_GCC += -DDEBUG -Wall -g -fsanitize=address -fsanitize=leak -fsanitize=undefined
	LDFLAGS_GCC += -g -fsanitize=address -fsanitize=leak -fsanitize=undefined
endif

ifeq ("$(API)","SDL")
	CFLAGS_GCC += -DRETROFLAT_API_SDL $(shell pkg-config sdl2 --cflags)
	LDFLAGS_GCC += $(shell pkg-config sdl2 --libs)
else
	CFLAGS_GCC += -DRETROFLAT_API_ALLEGRO $(shell pkg-config allegro --cflags)
	LDFLAGS_GCC += $(shell pkg-config allegro --libs)
endif

# Target-specific options.
aleggo: CFLAGS_GCC += -DRETROFLAT_OS_UNIX -DRETROFLAT_MOUSE

aleggd.exe: CC_GCC := i586-pc-msdosdjgpp-gcc
aleggd.exe: DEFINES := -DRETROFLAT_OS_DOS -DRETROFLAT_API_ALLEGRO 
aleggd.exe: CFLAGS_GCC += $(DEFINES) -fgnu89-inline -I$(ALLEGRO_DJGPP_ROOT)/include
aleggd.exe: CFLAGS_WATCOM += $(DEFINES) -bt=dos32a -s -3s
aleggd.exe: LDFLAGS_GCC += -L$(ALLEGRO_DJGPP_ROOT)/lib -lalleg
aleggd.exe: LDFLAGS_WATCOM += -l=dos32a -s -3s -k128k dos/clib3s.lib alleg.lib

aleggw.exe: DEFINES := -DRETROFLAT_OS_WIN -DRETROFLAT_API_WIN16 -DRETROFLAT_MOUSE
aleggw.exe: CFLAGS_WATCOM += $(DEFINES) -bt=windows -i$(WATCOM)/h/win
aleggw.exe: LDFLAGS_WATCOM += -l=win386

.PHONY: clean

all:

aleggo: $(addprefix obj/$(shell uname -s)/,$(subst .c,.o,$(ALLEGO_C_FILES)))
	$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC)

aleggd.exe: $(addprefix obj/dos/,$(subst .c,.o,$(ALLEGO_C_FILES)))
	#$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC)
	wcl386 $(LDFLAGS_WATCOM) -fe=$@ $^

aleggw.rex: $(addprefix obj/win16/,$(subst .c,.o,$(ALLEGO_C_FILES)))
	wcl386 $(LDFLAGS_WATCOM) -fe=$@ $^

aleggw.exe: aleggw.rex
	wbind $< -s $(WATCOM)/binw/win386.ext -R $@

obj/$(shell uname -s)/%.o: %.c
	$(MD) $(dir $@)
	$(CC_GCC) -c -o $@ $< $(CFLAGS_GCC)

obj/dos/%.o: %.c
	$(MD) $(dir $@)
	#$(CC_GCC) $(CFLAGS_GCC) -c -o $@ $<
	wcc386 $(CFLAGS_WATCOM) -fo=$@ $(<:%.c=%)

obj/win16/%.o: %.c
	$(MD) $(dir $@)
	wcc386 $(CFLAGS_WATCOM) -fo=$@ $(<:%.c=%)

clean:
	rm -rf obj aleggo aleggw32.exe *.err aleggd.exe aleggw.exe *.rex

