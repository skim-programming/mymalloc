#include "my_malloc.h"
#include <stdio.h>

void print_size(size_t size_i){
	size_t tsize = align_size(size_i);
	printf("%zu size: %zu\n", size_i, tsize);
}

int main(){
	print_size(1);
	print_size(2);
	print_size(7);
	print_size(8);
	print_size(9);
	print_size(15);
	print_size(16);
	print_size(17);
	return 0;
}
