
# vim: ft=make noexpandtab

ALEGGO_C_FILES := src/main.c src/grid.c

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
	CFLAGS_WATCOM += -d3
	CFLAGS_GCC += -DDEBUG -Wall -g -fsanitize=address -fsanitize=leak -fsanitize=undefined
	LDFLAGS_GCC += -g -fsanitize=address -fsanitize=leak -fsanitize=undefined
endif

ifeq ("$(API)","SDL")
	CFLAGS_GCC += -DRETROFLAT_API_SDL $(shell pkg-config sdl2 --cflags)
	LDFLAGS_GCC += $(shell pkg-config sdl2 --libs)
else
	CFLAGS_GCC += -DRETROFLAT_API_ALLEGRO $(shell pkg-config allegro --cflags)
	LDFLAGS_GCC += $(shell pkg-config allegro --libs)
	API := ALLEGRO
endif

# Target-specific options.
.PHONY: clean

all: aleggo aleggd.exe aleggw.exe

# Unix

aleggo: $(addprefix obj/$(shell uname -s)/,$(subst .c,.o,$(ALEGGO_C_FILES)))
	$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC)

obj/$(shell uname -s)/%.o: %.c
	$(MD) $(dir $@)
	$(CC_GCC) -c -o $@ $< -DRETROFLAT_OS_UNIX $(CFLAGS_GCC)

# DOS

aleggd.exe: $(addprefix obj/dos/,$(subst .c,.o,$(ALEGGO_C_FILES)))
	#i586-pc-msdosdjgpp-gcc -o $@ $^ -L$(ALLEGRO_DJGPP_ROOT)/lib -lalleg
	wcl386 -l=dos32a -s -3s -k128k dos/clib3s.lib alleg.lib $(LDFLAGS_WATCOM) -fe=$@ $^

obj/dos/%.o: %.c
	$(MD) $(dir $@)
	#i586-pc-msdosdjgpp-gcc -fgnu89-inline -I$(ALLEGRO_DJGPP_ROOT)/include -DRETROFLAT_OS_DOS -DRETROFLAT_API_$(API) -c -o $@ $<
	wcc386 -bt=dos32a -s -3s -DRETROFLAT_OS_DOS -DRETROFLAT_API_$(API) $(CFLAGS_WATCOM) -fo=$@ $(<:%.c=%)

# WinNT

aleggnt.exe: $(addprefix obj/nt/,$(subst .c,.o,$(ALEGGO_C_FILES)))
	wcl386 -l=nt -bw $(LDFLAGS_WATCOM) -fe=$@ $^

obj/nt/%.o: %.c
	$(MD) $(dir $@)
	wcc386 -bt=nt -i$(WATCOM)/h/nt -DRETROFLAT_API_WIN32 -DRETROFLAT_OS_WIN $(CFLAGS_WATCOM) -fo=$@ $(<:%.c=%)

# Win386

aleggw.rex: $(addprefix obj/win16/,$(subst .c,.o,$(ALEGGO_C_FILES)))
	wcl386 -l=win386 $(LDFLAGS_WATCOM) -fe=$@ $^

aleggw.exe: aleggw.rex
	wbind $< -s $(WATCOM)/binw/win386.ext -R $@

obj/win16/%.o: %.c
	$(MD) $(dir $@)
	wcc386 -bt=windows -i$(WATCOM)/h/win -DRETROFLAT_API_WIN16 -DRETROFLAT_OS_WIN $(CFLAGS_WATCOM) -fo=$@ $(<:%.c=%)

# Clean

clean:
	rm -rf obj aleggo aleggw32.exe *.err aleggd.exe aleggw.exe *.rex aleggnt.exe

