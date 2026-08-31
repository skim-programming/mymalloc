#include "my_malloc.h"
#include <unistd.h>


block_t* head = NULL;

size_t align_size(size_t size){
	return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

size_t block_size(void* ptr){
	return ((block_t*)ptr-1)->size;
}

void* my_malloc(size_t size){
	block_t* block = head;
	block_t* previous;
	size = align_size(size);
	while(block != NULL){
		if(block->free && block->size >= size){
			if(block->size > size + sizeof(block_t)) {
				block_t* split_block = (block_t*)((char*)(block+1) + size);
				split_block->size = block->size-size-sizeof(block_t);
				split_block->next = block->next;
				split_block->prev = block;
				if(block->next != NULL) block->next->prev = split_block;
				split_block->free = 1;
				block->size = size;
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
	if(new_block == (void*)-1) return NULL;
	new_block->size = size;
	new_block->free = 0;
	new_block->next = NULL;
	if(head == NULL){
		head = new_block;
		new_block->prev = NULL;
	}
	else{
		previous->next = new_block;
		new_block->prev = previous;
	}
	return new_block+1;
}

void* my_calloc(size_t num, size_t size){
	size_t alloc_size;
	if(num * size > SIZE_MAX) return NULL;
	else alloc_size = num * size;

	char* m_block = my_malloc(alloc_size);
	for(size_t i = 0; i < alloc_size; i++){
		m_block[i] = 0;
	}
	return m_block;
}

void my_free(void* ptr){
	if(ptr == NULL){
		return;
	}
	block_t* block = (block_t*)ptr - 1;
	block->free = 1;
	
	// Coalesce blocks
	if(block->next != NULL && (block_t*)((char*)(block+1)+block->size) == block->next && block->next->free){
		block->size = block->size + block->next->size + sizeof(block_t);
		block->next = block->next->next;
		if(block->next != NULL) block->next->prev = block;
	}
	if(block->prev != NULL && (block_t*)((char*)(block-1)-block->prev->size) == block->prev && block->prev->free){
		block->prev->size = block->prev->size + block->size + sizeof(block_t);
		block->prev->next = block->next;
		if(block->next != NULL) block->next->prev = block->prev;
		block = block->prev;
	}

	// Decrement program break if possible
	if(block->next == NULL && (char*)(block+1)+block->size == sbrk(0)){
		block_t* prev = block->prev;
		
		if(sbrk(-(block->size + sizeof(block_t))) == (void*)-1){
			return;
		}
		
		if(prev == NULL){
			head = NULL;
		}
		else{
			prev->next = NULL;
		}
	}
}
