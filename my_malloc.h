#ifndef MY_MALLOC_H
#define MY_MALLOC_H
#include <stdio.h>

typedef struct block {
	size_t size;
	int free;
	struct block *next;
} block_t;

void* my_malloc(size_t size);

void my_free(void* ptr);

#endif
