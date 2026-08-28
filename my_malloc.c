#include "my_malloc.h"
#include <unistd.h>


block_t* head = NULL;

size_t align_size(size_t size){
	return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

size_t get_list_size(block_t* head){
	block_t* block = head;
	size_t count = 0;
	while(block != NULL){
		block=block->next;
	}
	return count;
}

size_t whats_broken(block_error_t* list_container){
	block_t* block = head;
	size_t broken = 0;
	size_t i = 0;
	while(block != NULL){
		list_container[i] = (block_error_t){.block = block, .flags = 0};
		if(block -> next != NULL && block->next->prev != block){ // check if next->prev = block
			list_container[i].flags |= FLAG_NEXT_LINK;
		}
		if(block->prev != NULL && block->prev->next != block){ // check if prev->next = block
			list_container[i].flags |= FLAG_PREV_LINK;
		}
		if(block->next != NULL && (block_t*)((char*)(block+1)+block->size) != block->next){ // check if block at next is adjacent
			list_container[i].flags |= FLAG_NEXT_ADJ;
		}
		if(block->prev != NULL && (block_t*)((char*)(block-1)-block->prev->size) != block->prev){ // check if block prev is adjacent
			list_container[i].flags |= FLAG_PREV_ADJ;
		}
		if(block->next != NULL && block->next->free && block->free){ // check if next is 
			list_container[i].flags |= FLAG_NEXT_FREE;
		}
		if(block->prev != NULL && block->prev->free && block->free){
			list_container[i].flags |= FLAG_PREV_FREE;
		}
		if(block->size != align_size(block->size)){
			list_container[i].flags |= FLAG_ALIGNMENT;
		}
		if(list_container[i].flags != 0){
			broken++;
		}
		block = block->next;
		i++;
	}
	return broken;
}

size_t check_heap(){
	size_t error_count = 0;
	block_t* block = head;
	while(block != NULL){
		if(block -> next != NULL && block->next->prev != block){
			error_count++;
		}
		if(block->prev != NULL && block->prev->next != block){
			error_count++;
		}
		if(block->next != NULL && (block_t*)((char*)(block+1)+block->size) != block->next){
			error_count++;
		}
		if(block->prev != NULL && (block_t*)((char*)(block-1)-block->prev->size) != block->prev){
			error_count++;
		}
		if(block->next != NULL && block->next->free && block->free){
			error_count++;
		}
		if(block->prev != NULL && block->prev->free && block->free){
			error_count++;
		}
		if(block->size != align_size(block->size)){
			error_count++;
		}
		block = block->next;
	}
	return error_count;
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

void my_free(void* ptr){
	if(ptr == NULL){
		return;
	}
	block_t* block = (block_t*)ptr - 1;
	block->free = 1;
	if(block->next != NULL && (block_t*)((char*)(block+1)+block->size) == block->next && block->next->free){
		block->size = block->size + block->next->size + sizeof(block_t);
		block->next = block->next->next;
		if(block->next != NULL) block->next->prev = block;
	}
	if(block->prev != NULL && (block_t*)((char*)(block-1)-block->prev->size) == block->prev && block->prev->free){
		block->prev->size = block->prev->size + block->size + sizeof(block_t);
		block->prev->next = block->next;
		if(block->next != NULL) block->next->prev = block->prev;
	}
}
