#pragma once

#ifndef _ENCODE_H
#define _ENCODE_H

#include <sys/types.h>

#include <c_dsa/sll/sll.h>
#include <c_dsa/sll/structs.h>

#include "compression.h"

extern __cmp_huffman_node_t *__cmp_huffman_build (cdsa_sll_restrict_ptr_t q1,
                                          cdsa_sll_restrict_ptr_t q2);

extern void __cmp_huffman_initq_freq (cdsa_sll_t q, int *freqs);

extern ssize_t __cmp_huffman_encode (cmp_huffman_t huffman);

#endif
