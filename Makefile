
# vim: ft=make noexpandtab

ALLEGO_C_FILES := src/main.c src/grid.c

MD := mkdir -p

.PHONY: clean

all: allego

allego: $(addprefix obj/$(shell uname -s)/,$(subst .c,.o,$(ALLEGO_C_FILES)))
	gcc -o $@ $^ $(shell pkg-config allegro --libs)

allegd.exe: $(addprefix obj/dos/,$(subst .c,.o,$(ALLEGO_C_FILES)))
	wcl386 -l=dos4g -fe=$@ -s -3s -k128k dos/clib3s.lib alleg.lib $^

allegw32.exe: $(addprefix obj/nt/,$(subst .c,.o,$(ALLEGO_C_FILES)))
	wcl386 -l=alleg -fe=$@ -bcl=nt_win $^

obj/$(shell uname -s)/%.o: %.c
	$(MD) $(dir $@)
	gcc -c -o $@ $< $(shell pkg-config allegro --cflags)

obj/dos/%.o: %.c
	$(MD) $(dir $@)
	wcc386 -DDOS -bt=dos4g -s -3s -fo=$@ $(<:%.c=%)

obj/nt/%.o: %.c
	$(MD) $(dir $@)
	wcc386 -bt=nt -fo=$@ $(<:%.c=%)

clean:
	rm -rf obj allego allegw32.exe *.err allegd.exe

