#include "numtheory.h"
#include "randstate.h"
#include "ss.h"

#include <gmp.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int opt;
    mpz_t n;
    mpz_init(n);
    FILE *input = stdin;

    FILE *output = stdout;

    FILE *pbfile = fopen("ss.pub", "r");
    int verbose = 0;
    while ((opt = getopt(argc, argv, "hvi:o:n:")) != -1) {
        switch (opt) {
        // set input file to optarg or user input
        case 'i': input = fopen(optarg, "r"); break;
        // set output file to optarg or user input
        case 'o': output = fopen(optarg, "w"); break;
        // set pb file to optarg or user input
        case 'n': pbfile = fopen(optarg, "r"); break;
        case 'v': verbose = 1; break;
        case 'h': // help message
            printf("SYNOPSIS\n"
                   "   Encrypts data using SS encryption.\n"
                   "   Encrypted data is decrypted by the decrypt program.\n\n"
                   "USAGE\n"
                   "   ./encrypt [OPTIONS]\n\n"
                   "OPTIONS\n"
                   "   -h             Display program help and usage.\n"
                   "   -v             Display verbose program output.\n"
                   "   -i infile      Input file of data to encrypt (default: stdin).\n"
                   "   -o outfile     Output file for encrypted data (default: stdout).\n"
                   "   -n pbfile      Public key file (default: ss.pub).\n");
            return 0;

        default:
            printf("SYNOPSIS\n"
                   "   Encrypts data using SS encryption.\n"
                   "   Encrypted data is decrypted by the decrypt program.\n\n"
                   "USAGE\n"
                   "   ./encrypt [OPTIONS]\n\n"
                   "OPTIONS\n"
                   "   -h             Display program help and usage.\n"
                   "   -v             Display verbose program output.\n"
                   "   -i infile      Input file of data to encrypt (default: stdin).\n"
                   "   -o outfile     Output file for encrypted data (default: stdout).\n"
                   "   -n pbfile      Public key file (default: ss.pub).\n");
            return 1;
        }
    }

    if (!pbfile || !output || !input) { // checks for error when opening file
        fprintf(stderr, "Error with file.\n"); // error message
        fclose(pbfile);
        fclose(input);
        fclose(output);
        return -1;
    }

    char *username = getenv("USER"); // gets username

    ss_read_pub(n, username, pbfile); // read public file w/ ss_make_pub()

    if (verbose == 1) { // verbose enabled
        fprintf(stderr, "user = %s \n", username);
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
    }

    ss_encrypt_file(input, output, n); // encrypt file
    // free memory
    mpz_clear(n);

    fclose(pbfile);
    fclose(input);
    fclose(output);
    return 0;
}
