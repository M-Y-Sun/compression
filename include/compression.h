#pragma once

#ifndef COMPRESSION_H
#define COMPRESSION_H

#define __CMP_BUFSIZ 0x1000 // 4096

typedef unsigned char __cmp_huffman_symbol;

#define __HUFFMAN_SYMBOL_MAX (1 << (8 * sizeof (__cmp_huffman_symbol)))

#include <stdbool.h>
#include <sys/types.h>

typedef struct __cmp_huffman_node_struct_t {
    bool is_leaf;
    int freq;
    __cmp_huffman_symbol symb;

    struct __cmp_huffman_node_struct_t *lc;
    struct __cmp_huffman_node_struct_t *rc;
    struct __cmp_huffman_node_struct_t *pa;
} __cmp_huffman_node_t;

typedef struct __cmp_huffman_struct {
    int fd;
    __cmp_huffman_node_t *root;
} cmp_huffman_t[1];

typedef struct __cmp_huffman_struct *restrict cmp_huffman_restrict_ptr_t;

extern void cmp_huffman_init (cmp_huffman_t this, int fd);

extern void cmp_huffman_deinit (cmp_huffman_t this);

extern void cmp_huffman_reinit (cmp_huffman_t this, int fd);

extern int __cmp_huffman_node_ptr_compar (const void *p_lhs,
                                          const void *p_rhs);

extern ssize_t cmp_huffman_encode_pq (cmp_huffman_t this);

extern ssize_t cmp_huffman_encode (cmp_huffman_t this);

#endif
