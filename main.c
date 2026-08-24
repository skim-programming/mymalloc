#include "my_malloc.h"
#include <stdio.h>

int main(){
	void* a = my_malloc(1000);
	printf("a address: %p\n", a);
	my_free(a);
	void* b = my_malloc(100);
	void* c = my_malloc(100);
	printf("b address: %p\nc address: %p\naddress difference: %td\n", b, c, c-b);
	return 0;
}
