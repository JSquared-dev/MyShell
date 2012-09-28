

OBJECTS := main.o command.o builtins.o


all: shell

clean:
	rm $(OBJECTS)
	rm shell

shell: $(OBJECTS)
	gcc -lc -o $@ $(OBJECTS)

.c.o:
	gcc --c99 -c -g -o $@ $<
