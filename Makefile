
# vim: ft=make noexpandtab

ALLEGO_C_FILES := src/main.c src/grid.c

MD := mkdir -p

CFLAGS_GCC :=

SANITIZE := -Werror -Wall -g -fsanitize=address -fsanitize=leak -fsanitize=undefined

allego: CC_GCC := gcc
allego: CFLAGS_GCC := $(shell pkg-config allegro --cflags) $(SANITIZE)
allego: LDFLAGS_GCC := $(shell pkg-config allegro --libs) $(SANITIZE)

allegd.exe: CC_GCC := i586-pc-msdosdjgpp-gcc
allegd.exe: CFLAGS_GCC += -DDOS -fgnu89-inline -I$(ALLEGRO_DJGPP_ROOT)/include
allegd.exe: LDFLAGS_GCC += -L$(ALLEGRO_DJGPP_ROOT)/lib -lalleg

.PHONY: clean

all:

allego: $(addprefix obj/$(shell uname -s)/,$(subst .c,.o,$(ALLEGO_C_FILES)))
	$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC)

allegd.exe: $(addprefix obj/dos/,$(subst .c,.o,$(ALLEGO_C_FILES)))
	$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC)

obj/$(shell uname -s)/%.o: %.c
	$(MD) $(dir $@)
	$(CC_GCC) -c -o $@ $< $(CFLAGS_GCC)

obj/dos/%.o: %.c
	$(MD) $(dir $@)
	$(CC_GCC) $(CFLAGS_GCC) -c -o $@ $<

clean:
	rm -rf obj allego allegw32.exe *.err allegd.exe

