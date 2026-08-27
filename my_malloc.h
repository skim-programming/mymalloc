#ifndef MY_MALLOC_H
#define MY_MALLOC_H
#include <stdio.h>

typedef struct block {
	size_t size;
	int free;
	struct block *next;
	struct block *prev;
} block_t;

size_t align_size(size_t size);

void* my_malloc(size_t size);

void my_free(void* ptr);

#endif
