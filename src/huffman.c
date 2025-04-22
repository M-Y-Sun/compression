#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>

#include <c_dsa/heap/heap.h>
#include <c_dsa/sll/sll.h>
#include <c_dsa/sll/structs.h>

#include "_encode.h"
#include "compression.h"

void
cmp_huffman_init (cmp_huffman_t this, int fd)
{
    this->fd = fd;
}

static void
__deinit (__huffman_node_t *root)
{
    if (root->is_leaf) {
        free (root);
        return;
    }

    __deinit (root->lc);
    __deinit (root->rc);
    free (root);
}

void
cmp_huffman_deinit (cmp_huffman_t this)
{
    __deinit (this->root);
}

void
cmp_huffman_reinit (cmp_huffman_t this, int fd)
{
    cmp_huffman_deinit (this);
    cmp_huffman_init (this, fd);
}

int
__huffman_node_ptr_compar (const void *p_lhs, const void *p_rhs)
{
    int f1 = (*(const __huffman_node_t *const *)p_lhs)->freq;
    int f2 = (*(const __huffman_node_t *const *)p_rhs)->freq;
    return (f1 > f2) - (f1 < f2);
}

ssize_t
cmp_huffman_encode_pq (cmp_huffman_t this)
{
    __huffman_symbol *buf = malloc (__CMP_BUFSIZ);

    if (buf == NULL)
        return -1;

    const ssize_t nbytes = read (this->fd, buf, __CMP_BUFSIZ);

    if (nbytes == -1)
        return -1;

    const size_t siz = nbytes / sizeof (__huffman_symbol);

    static int freqs[__HUFFMAN_SYMBOL_MAX];
    static __huffman_node_t *nodes[__HUFFMAN_SYMBOL_MAX];

    for (size_t i = 0; i < siz; ++i)
        ++freqs[buf[i]];

    size_t nsiz = 0;

    for (__huffman_symbol s = 0; s < __HUFFMAN_SYMBOL_MAX - 1; ++s) {
        if (freqs[s] == 0)
            continue;

        __huffman_node_t *new = malloc (sizeof (__huffman_node_t));
        new->is_leaf = true;
        new->freq = freqs[s];
        new->symb = s;

        nodes[nsiz++] = new;
    }

    cdsa_heap_t heap;
    cdsa_heap_init_arr_compar (heap, (__heap_val_t *)nodes, nsiz,
                               sizeof (nodes[0]), __huffman_node_ptr_compar);

    while (heap->size > 1) {
        __huffman_node_t *new = malloc (sizeof (__huffman_node_t));

        new->lc = (__huffman_node_t *)cdsa_heap_top (heap);
        cdsa_heap_delete (heap);
        new->rc = (__huffman_node_t *)cdsa_heap_top (heap);
        new->lc->pa = new->rc->pa = new;
        new->is_leaf = false;
        new->freq = new->lc->freq + new->rc->freq;

        cdsa_heap_replace_always (heap, (__heap_val_t) new);
    }

    this->root = (__huffman_node_t *)cdsa_heap_top (heap);

    cdsa_heap_deinit (heap);

    return __huffman_encode (this);
}

ssize_t
cmp_huffman_encode (cmp_huffman_t this)
{
    __huffman_symbol *buf = malloc (__CMP_BUFSIZ);

    if (buf == NULL)
        return -1;

    const ssize_t nbytes = read (this->fd, buf, __CMP_BUFSIZ);

    if (nbytes == -1)
        return -1;

    const size_t siz = nbytes / sizeof (__huffman_symbol);

    static int freqs[__HUFFMAN_SYMBOL_MAX];

    // TODO(M-Y-Sun): use preprocessor to determine if we need to use a hash
    // map for large _HUFFMAN SYMBOL_MAX

    for (size_t i = 0; i < siz; ++i)
        ++freqs[buf[i]];

    cdsa_sll_t q1, q2;
    cdsa_sll_init (q1);
    cdsa_sll_init (q2);

    __huffman_initq_freq (q1, freqs);
    this->root = __huffman_build (q1, q2);

    cdsa_sll_deinit (q1);
    cdsa_sll_deinit (q2);

    return __huffman_encode (this);
}
