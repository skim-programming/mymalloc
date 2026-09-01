#include "debug_malloc.h"
#include <stdio.h>

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
	
		if(block -> next != NULL && block->next->prev != block) // check if next->prev = block
			list_container[i].flags |= FLAG_NEXT_LINK;

		if(block->prev != NULL && block->prev->next != block) // check if prev->next = block
			list_container[i].flags |= FLAG_PREV_LINK;
		
		if(block->next != NULL && (block_t*)((char*)(block+1)+block->size) != block->next) // check if block at next is adjacent
			list_container[i].flags |= FLAG_NEXT_ADJ;

		if(block->prev != NULL && (block_t*)((char*)(block-1)-block->prev->size) != block->prev) // check if block prev is adjacent
			list_container[i].flags |= FLAG_PREV_ADJ;
		
		if(block->next != NULL && block->next->free && block->free) // check if next is 
			list_container[i].flags |= FLAG_NEXT_FREE;
		
		if(block->prev != NULL && block->prev->free && block->free)
			list_container[i].flags |= FLAG_PREV_FREE;
		
		if(block->size != align_size(block->size))
			list_container[i].flags |= FLAG_ALIGNMENT;
		
		if(list_container[i].flags != 0)
			broken++;
		
		block = block->next;
		i++;
	}
	return broken;
}

size_t check_heap(){
	uint8_t flags = 0;
	block_t* block = head;
	while(block != NULL){
	
		if(block -> next != NULL && block->next->prev != block) // check if next->prev = block
			flags |= FLAG_NEXT_LINK;

		if(block->prev != NULL && block->prev->next != block) // check if prev->next = block
			flags |= FLAG_PREV_LINK;
		
		if(block->next != NULL && (block_t*)((char*)(block+1)+block->size) != block->next) // check if block at next is adjacent
			flags |= FLAG_NEXT_ADJ;

		if(block->prev != NULL && (block_t*)((char*)(block-1)-block->prev->size) != block->prev) // check if block prev is adjacent
			flags |= FLAG_PREV_ADJ;
		
		if(block->next != NULL && block->next->free && block->free) // check if next is 
			flags |= FLAG_NEXT_FREE;
		
		if(block->prev != NULL && block->prev->free && block->free)
			flags |= FLAG_PREV_FREE;
		
		if(block->size != align_size(block->size))
			flags |= FLAG_ALIGNMENT;
		
		if(flags != 0)
			break;
		block = block->next;
	}
	return flags;
}

void print_flags(uint8_t flags) {
    if (flags & FLAG_NEXT_LINK)
        printf("FLAG_NEXT_LINK\n");

    if (flags & FLAG_PREV_LINK)
        printf("FLAG_PREV_LINK\n");

    if (flags & FLAG_NEXT_ADJ)
        printf("FLAG_NEXT_ADJ\n");

    if (flags & FLAG_PREV_ADJ)
        printf("FLAG_PREV_ADJ\n");

    if (flags & FLAG_NEXT_FREE)
        printf("FLAG_NEXT_FREE\n");

    if (flags & FLAG_PREV_FREE)
        printf("FLAG_PREV_FREE\n");

    if (flags & FLAG_ALIGNMENT)
        printf("FLAG_ALIGNMENT\n");
}