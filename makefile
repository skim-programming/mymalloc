CC = gcc
CFLAGS = -Wall -Wextra -g

mymalloc: main.o my_malloc.o debug_malloc.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f mymalloc *.o
