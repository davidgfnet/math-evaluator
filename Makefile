CC = gcc
FLAGS = -Os #-ggdb

all:
	$(CC) -c expression.c $(FLAGS)
	$(CC) -c main.c $(FLAGS)
	$(CC) -o test main.o expression.o -lm $(FLAGS)
	strip -s test

clean:
	rm -f *.o test
