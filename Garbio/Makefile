exec = garbio.out
sources = $(wildcard src/*.c)
objects = $(sources:.c=.o)
flags = -g


$(exec): $(objects)
	gcc $(objects) $(flags) -o $(exec)

%.o: %.c include/%.h
	gcc -c $(flags) $< -o $@

install:
	make
	cp ./garbio.out /usr/local/bin/garbio

clean:
	-rm *.out
	-rm *.o
	-rm src/*.o