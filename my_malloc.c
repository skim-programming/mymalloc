#include "my_malloc.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>


block_t* head = NULL;

size_t align_size(size_t size){
	return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

size_t block_size(void* ptr){
	return ((block_t*)ptr-1)->size;
}

block_t* get_block(void* ptr){
	return (block_t*)ptr-1;
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

void* my_realloc(void* ptr, size_t new_size){
	if(ptr == NULL) return my_malloc(new_size);
	if(new_size == 0) {my_free(ptr); return NULL;}
	block_t* block = get_block(ptr);
	new_size = align_size(new_size);
	
	size_t block_space = block->size;
	block_t* temp_block = block->next;
	while(temp_block != NULL && temp_block->free){
		block_space += temp_block->size + sizeof(block_t);

		if(temp_block->next != NULL && (char*)temp_block+temp_block->size+sizeof(block_t) == (char*)temp_block->next)
			temp_block = temp_block->next;
		else
			temp_block = NULL;
	}

	if(block->next == NULL && (char*)ptr + block->size == sbrk(0)){
		if(sbrk((intptr_t)new_size - (intptr_t)block->size) == (void*)-1){
			return NULL;
		}
		block->size = new_size;
		return ptr;
	}
	else if(block->next != NULL && block->next->free && new_size > block->size && block_space >= new_size){
		if(block_space > new_size + sizeof(block_t)){
			block_t* new_block = (block_t*)((char*)(block + 1) + new_size);
			new_block->free = 1;
			new_block->prev = block;
			new_block->next = temp_block;
			new_block->size = block_space - (new_size + sizeof(block_t));
			if(temp_block != NULL) temp_block->prev = new_block;
			block->next = new_block;
			block->size = new_size;
		}
		else{
			block->next = temp_block;
			if(temp_block != NULL) temp_block -> prev = block;

			block->size = block_space;
		}
		return block+1;
	}
	else{
		void* new_block =  my_malloc(new_size);
		if(new_block == NULL){
			return NULL;
		}
		memcpy(new_block, ptr, (new_size > block->size) ? block->size : new_size);
		my_free(ptr);
		return new_block;
	}
}

void my_free(void* ptr){
	if(ptr == NULL){
		return;
	}
	block_t* block = get_block(ptr);
	block->free = 1;
	
	// Coalesce blocks
	if(block->next != NULL && (char*)(block+1)+block->size == (char*)block->next && block->next->free){
		block->size = block->size + block->next->size + sizeof(block_t);
		block->next = block->next->next;
		if(block->next != NULL) block->next->prev = block;
	}
	if(block->prev != NULL && (char*)block == (char*)(block->prev + 1) + block->prev->size && block->prev->free){
		block->prev->size = block->prev->size + block->size + sizeof(block_t);
		block->prev->next = block->next;
		if(block->next != NULL) block->next->prev = block->prev;
		block = block->prev;
	}

	// Decrement program break if possible
	if(block->next == NULL && (char*)(block+1)+block->size == sbrk(0)){
		block_t* prev = block->prev;
		
		if(sbrk(-(intptr_t)(block->size + sizeof(block_t))) == (void*)-1){
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
