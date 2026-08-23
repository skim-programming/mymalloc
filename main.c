#include "my_malloc.h"
#include <stdio.h>

int main(){
	void* a = my_malloc(100);
	void* b = my_malloc(200);
	(void)b;
	printf("a address: %p\n", a);
	my_free(a);
	void* c = my_malloc(100);
	printf("c address: %p\n", c);
	return 0;
}
