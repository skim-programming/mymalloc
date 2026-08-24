#include "my_malloc.h"
#include <unistd.h>


block_t* head = NULL;

void* my_malloc(size_t size){
	block_t* block = head;
	block_t* previous;
	while(block != NULL){
		if(block->free && block->size >= size){
			if(block->size >= size + sizeof(block_t)) {
				block->size = size;
				block_t* split_block = (block_t*)((char*)(block+1) + size);
				split_block->size = block->size-size-sizeof(block_t);
				split_block->next = NULL;
				split_block->free = 1;
				block->next = split_block;
			}
			block->free = 0;
			return block + 1;
		}
		previous = block;
		block = block->next;
	}
	block_t* new_block;
	new_block = sbrk(sizeof(block_t) + size);
	new_block->size = size;
	new_block->free = 0;
	new_block->next = NULL;
	if(head == NULL){
		head = new_block;
	}
	else{
		previous->next = new_block;
	}
	return new_block+1;
}

void my_free(void* ptr){
	if(ptr == NULL){
		return;
	}
	block_t* block = (block_t*)ptr - 1;
	block->free = 1;
}
