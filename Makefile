
# vim: ft=make noexpandtab

ALLEGO_C_FILES := src/main.c src/grid.c

MD := mkdir -p

CFLAGS_GCC :=

SANITIZE := -Werror -Wall -g -fsanitize=address -fsanitize=leak -fsanitize=undefined

aleggo: CC_GCC := gcc
aleggo: CFLAGS_GCC := $(shell pkg-config allegro --cflags) $(SANITIZE)
aleggo: LDFLAGS_GCC := $(shell pkg-config allegro --libs) $(SANITIZE)

aleggd.exe: CC_GCC := i586-pc-msdosdjgpp-gcc
aleggd.exe: CFLAGS_GCC += -DDOS -fgnu89-inline -I$(ALLEGRO_DJGPP_ROOT)/include
aleggd.exe: LDFLAGS_GCC += -L$(ALLEGRO_DJGPP_ROOT)/lib -lalleg

.PHONY: clean

all:

aleggo: $(addprefix obj/$(shell uname -s)/,$(subst .c,.o,$(ALLEGO_C_FILES)))
	$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC)

aleggd.exe: $(addprefix obj/dos/,$(subst .c,.o,$(ALLEGO_C_FILES)))
	#$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC)
	wcl386 -l=dos32a -fe=$@ -s -3s -k128k dos/clib3s.lib alleg.lib $^

obj/$(shell uname -s)/%.o: %.c
	$(MD) $(dir $@)
	$(CC_GCC) -c -o $@ $< $(CFLAGS_GCC)

obj/dos/%.o: %.c
	$(MD) $(dir $@)
	#$(CC_GCC) $(CFLAGS_GCC) -c -o $@ $<
	wcc386 -DDOS -bt=dos32a -s -3s -fo=$@ $(<:%.c=%)

clean:
	rm -rf obj aleggo aleggw32.exe *.err aleggd.exe

