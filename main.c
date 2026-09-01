#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_malloc.h"
#include "debug_malloc.h"

void verify_heap(const char *where)
{
    uint8_t flags = check_heap();

    if (flags > 0) {
        printf("HEAP CORRUPTION after %s\n", where);
        print_flags(flags);
        exit(1);
    }
}

int main(void)
{
    #define BLOCKS 1000

    void *blocks[BLOCKS];

    /* Initialize */
    for (int i = 0; i < BLOCKS; i++)
        blocks[i] = NULL;

    /* Basic allocations */
    for (int i = 0; i < BLOCKS; i++) {
    blocks[i] = my_malloc((i % 200) + 1);

    if (blocks[i] == NULL) {
        printf("malloc failed at %d\n", i);
        return 1;
    }

    memset(blocks[i], i & 0xFF, (i % 200) + 1);
	}

	verify_heap("initial allocations");

    /* Free every other block */
    for (int i = 0; i < BLOCKS; i += 2) {
        my_free(blocks[i]);
        blocks[i] = NULL;

        verify_heap("freeing every other block");
    }

    /* Reuse the holes */
    for (int i = 0; i < BLOCKS; i += 2) {
        size_t size = (i % 150) + 20;

        blocks[i] = my_malloc(size);

        if (blocks[i] == NULL) {
            printf("malloc failed while reusing holes at %d\n", i);
            return 1;
        }

        verify_heap("reusing freed blocks");
    }

    /* Realloc growing and shrinking */
    for (int i = 0; i < BLOCKS; i++) {
        size_t new_size = (i % 2 == 0) ? 500 : 20;

        blocks[i] = my_realloc(blocks[i], new_size);

        if (blocks[i] == NULL) {
            printf("realloc failed at %d\n", i);
            return 1;
        }

        verify_heap("realloc grow/shrink");
    }

    /* Create lots of free blocks */
    for (int i = 0; i < BLOCKS; i += 3) {
        if (blocks[i] != NULL) {
            my_free(blocks[i]);
            blocks[i] = NULL;

            verify_heap("creating free blocks");
        }
    }

    /* Realloc blocks next to free blocks */
    for (int i = 0; i < BLOCKS; i++) {
        if (blocks[i] != NULL) {
            size_t new_size = 1000 + (i % 500);

            void *new_ptr = my_realloc(blocks[i], new_size);

            if (new_ptr == NULL) {
                printf("realloc failed during coalescing test at %d\n", i);
                return 1;
            }

            blocks[i] = new_ptr;
            verify_heap("realloc coalescing");
        }
    }

    /* Realloc down repeatedly */
    for (int i = 0; i < BLOCKS; i++) {
        if (blocks[i] != NULL) {
            for (int j = 0; j < 10; j++) {
                size_t new_size = 50 + j * 8;

                blocks[i] = my_realloc(blocks[i], new_size);

                if (blocks[i] == NULL) {
                    printf("realloc shrink failed at %d\n", i);
                    return 1;
                }

                verify_heap("repeated realloc shrinking");
            }
        }
    }

    /* Realloc up repeatedly */
    for (int i = 0; i < BLOCKS; i++) {
        if (blocks[i] != NULL) {
            for (int j = 0; j < 10; j++) {
                size_t new_size = 500 + j * 100;

                blocks[i] = my_realloc(blocks[i], new_size);

                if (blocks[i] == NULL) {
                    printf("realloc growth failed at %d\n", i);
                    return 1;
                }

                verify_heap("repeated realloc growing");
            }
        }
    }

    /* Free everything */
    for (int i = 0; i < BLOCKS; i++) {
        if (blocks[i] != NULL) {
            my_free(blocks[i]);
            blocks[i] = NULL;

            verify_heap("final cleanup");
        }
    }

    printf("ALL TESTS PASSED\n");
    return 0;
}