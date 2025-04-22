#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#include <c_dsa/heap/heap.h>
#include <c_dsa/sll/sll.h>
#include <c_dsa/sll/structs.h>

#include "compression.h"

__huffman_node_t *
__huffman_build (cdsa_sll_restrict_ptr_t q1, cdsa_sll_restrict_ptr_t q2)
{
    while (q1->size + q2->size > 1) {
        __huffman_node_t *n1, *n2;

        n1 = q2->size == 0
                     || __huffman_node_ptr_compar (&q1->front, &q2->front) < 0
                 ? (__huffman_node_t *)cdsa_sll_popf (q1)
                 : (__huffman_node_t *)cdsa_sll_popf (q2);

        n2 = q2->size == 0
                     || __huffman_node_ptr_compar (&q1->front, &q2->front) < 0
                 ? (__huffman_node_t *)cdsa_sll_popf (q1)
                 : (__huffman_node_t *)cdsa_sll_popf (q2);

        __huffman_node_t *n_new = malloc (sizeof (__huffman_node_t));

        n_new->lc = n1;
        n_new->rc = n2;
        n_new->lc->pa = n_new->rc->pa = n_new;
        n_new->freq = n1->freq + n2->freq;
        n_new->is_leaf = false;

        cdsa_sll_pushb (q2, (__sll_data_t)n_new);
    }

    return (__huffman_node_t *)cdsa_sll_front (q1->size == 0 ? q2 : q1);
}

void
__huffman_initq_freq (cdsa_sll_t q, int *freqs)
{
    for (__huffman_symbol s = 0; s < __HUFFMAN_SYMBOL_MAX - 1; ++s) {
        if (freqs[s] == 0)
            continue;

        __huffman_node_t *new = malloc (sizeof (__huffman_node_t));
        new->is_leaf = true;
        new->symb = s;
        new->freq = freqs[s];
        cdsa_sll_pushb (q, (__sll_data_t) new);
    }

    cdsa_sll_sort (q, __huffman_node_ptr_compar);
}

static uint32_t *
__gen_dict (__huffman_node_t *root)
{
}

ssize_t
__huffman_encode (cmp_huffman_t huffman)
{
}
