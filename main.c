#include "my_malloc.h"
#include <stdio.h>

int main(){
	int* calloc_test = my_calloc(10, sizeof(int));
	for(size_t i = 0; i < 10; i++)
		printf("%c\n", calloc_test[i]);
}
