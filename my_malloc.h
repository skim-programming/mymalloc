#ifndef MY_MALLOC_H
#define MY_MALLOC_H
#include <stdint.h>
#include <stddef.h>

typedef struct block {
	size_t size;
	int free;
	struct block *next;
	struct block *prev;
} block_t;

#define FLAG_NEXT_LINK 1;
#define FLAG_PREV_LINK 2;
#define FLAG_NEXT_ADJ 4;
#define FLAG_PREV_ADJ 8;
#define FLAG_NEXT_FREE 16;
#define FLAG_PREV_FREE 32;
#define FLAG_ALIGNMENT 64;

typedef struct block_error {
	block_t* block;
	uint8_t flags;
} block_error_t;

extern block_t* head;

#define ALIGNMENT 8
size_t align_size(size_t size);

size_t block_size(void* ptr);

void* my_malloc(size_t size);

void* my_calloc(size_t num, size_t size);

void my_free(void* ptr);

#endif
