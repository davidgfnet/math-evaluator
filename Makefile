
FLAGS = -Os #-ggdb

all:
	gcc -c expression.c $(FLAGS)
	gcc -c main.c $(FLAGS)
	gcc -o test main.o expression.o -lm $(FLAGS)
	strip -s test

clean:
	rm -f *.o test
