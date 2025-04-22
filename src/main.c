#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "compression.h"

int
main (int argc, char **argv)
{
#ifndef NDEBUG
    if (argc < 2) {
        fputs ("Expected file path\n", stderr);
        return EXIT_FAILURE;
    }
#else
    argv[1] = "io-test/in.bin";
#endif

    int fd = open (argv[1], O_RDONLY | O_EXCL, 0644);

    if (fd == -1) {
        perror (argv[0]);
        return EXIT_FAILURE;
    }

    cmp_huffman_t huffman;
    cmp_huffman_init (huffman, fd);
    cmp_huffman_encode (huffman);
    // cmp_huffman_encode_pq (huffman);
    cmp_huffman_deinit (huffman);

    close (fd);

    return 0;
}
