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
    mpz_t pq;
    mpz_t d;
    mpz_init(pq);
    mpz_init(d);
    FILE *input = stdin;

    FILE *output = stdout;

    FILE *pvfile = fopen("ss.priv", "r");

    int verbose = 0;
    while ((opt = getopt(argc, argv, "hvi:o:n:")) != -1) {
        switch (opt) {
        // set input file to optarg or user input
        case 'i': input = fopen(optarg, "r"); break;
        // set output file to optarg or user input
        case 'o': output = fopen(optarg, "w"); break;
        // set pv file to optarg or user input
        case 'n': pvfile = fopen(optarg, "r"); break;
        case 'v': verbose = 1; break;
        case 'h': // help message
            printf("SYNOPSIS\n"
                   "   Decrypts data using SS decryption.\n"
                   "   Encrypted data is encrypted by the encrypt program.\n\n"
                   "USAGE\n"
                   "   ./decrypt [OPTIONS]\n\n"
                   "OPTIONS\n"
                   "   -h             Display program help and usage.\n"
                   "   -v             Display verbose program output.\n"
                   "   -i infile      Input file of data to decrypt (default: stdin).\n"
                   "   -o outfile     Output file for decrypted data (default: stdout).\n"
                   "   -n pvfile      Private key file (default: ss.priv).\n");
            return 0;

        default:
            printf("SYNOPSIS\n"
                   "   Decrypts data using SS decryption.\n"
                   "   Encrypted data is encrypted by the encrypt program.\n\n"
                   "USAGE\n"
                   "   ./decrypt [OPTIONS]\n\n"
                   "OPTIONS\n"
                   "   -h             Display program help and usage.\n"
                   "   -v             Display verbose program output.\n"
                   "   -i infile      Input file of data to decrypt (default: stdin).\n"
                   "   -o outfile     Output file for decrypted data (default: stdout).\n"
                   "   -n pvfile      Private key file (default: ss.priv).\n");
            return 1;
        }
    }

    if (!input || !output || !pvfile) { // checks for failure of file
        fprintf(stderr, "Error with file.\n"); // error message
        fclose(pvfile);
        fclose(input);
        fclose(output);
        return -1;
    }

    ss_read_priv(pq, d, pvfile); // open private key file

    if (verbose == 1) { // verbose enabled
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    ss_decrypt_file(input, output, d, pq); // decrypt file with ss\_decrypt\_file()
    // free memory
    mpz_clears(pq, d, NULL);

    fclose(pvfile);
    fclose(input);
    fclose(output);
    return 0;
}
