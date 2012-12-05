

OBJECTS := main.o commands.o builtin.o ps.o kill.o


all: shell

clean:
	rm $(OBJECTS)
	rm shell

shell: $(OBJECTS)
	gcc -lc -o $@ $(OBJECTS)

.c.o:
	gcc --std=c99 -c -g -o $@ $<
