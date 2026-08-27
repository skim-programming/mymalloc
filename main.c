#include "my_malloc.h"
#include <stdio.h>

void print_address(char ptrname, void* ptr){
	printf("%c address: %p\n", ptrname, ptr);
}

int main(){
	void* a = my_malloc(100);
	void* b = my_malloc(100);
	void* c = my_malloc(100);

	print_address('A', a);
	print_address('B', b);
	print_address('C', c);

	my_free(a);
	my_free(c);
	my_free(b);
	
	void* d = my_malloc(300);
	print_address('D', d);
	return 0;
}
