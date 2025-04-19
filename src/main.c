#include "compression.h"
#include "heap.h"

int
main (void)
{
    cdsa_heap_t heap;
    cdsa_heap_init (heap, 0);
    cdsa_heap_deinit (heap);

    return 0;
}
