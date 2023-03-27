#include "numtheory.h"
#include "randstate.h"
#include "ss.h"

#include <gmp.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define OPTIONS "hb:i:n:d:s:v"

int main(int argc, char **argv) {
    int opt = 0;

    mpz_t p;
    mpz_t q;
    mpz_t n;
    mpz_t d;
    mpz_t pq;

    mpz_init(p);
    mpz_init(q);
    mpz_init(n);
    mpz_init(d);
    mpz_init(pq);

    uint64_t bits = 256; // bits needed for public key
    uint64_t iters = 50; // miller rabin iterations for testing
    uint64_t seed = time(NULL);
    FILE *pbfile = fopen("ss.pub", "w");
    FILE *pvfile = fopen("ss.priv", "w");
    int verbose = 0;

    while ((opt = getopt(argc, argv, OPTIONS))
           != -1) { // Switch statement to check if label is matched
        switch (opt) {
        case 'b': bits = atoi(optarg); break;
        case 'i': iters = atoi(optarg); break;
        case 'n': pbfile = fopen(optarg, "w"); break;
        case 'd': pvfile = fopen(optarg, "w"); break;
        case 's': seed = atoi(optarg); break;
        // verbose enabled
        case 'v': verbose = 1; break;
        case 'h': // help message
            printf("SYNOPSIS\n"
                   "   Generates an SS public/private key pair.\n\n"
                   "USAGE\n"
                   "   ./keygen [OPTIONS]\n\n"
                   "OPTIONS\n"
                   "   -h             Display program help and usage.\n"
                   "   -v             Display verbose program output.\n"
                   "   -b bits        Minimum bits needed for public key n (default: 256).\n"
                   "   -i iterations  Miller-Rabin iterations for testing primes (default: 50).\n"
                   "   -n pbfile      Public key file (default: ss.pub).\n"
                   "   -d pvfile      Private key file (default: ss.priv).\n"
                   "   -s seed        Random seed for testing.\n");
            return 0;

        default: //default
            printf("SYNOPSIS\n"
                   "   Generates an SS public/private key pair.\n\n"
                   "USAGE\n"
                   "   ./keygen [OPTIONS]\n\n"
                   "OPTIONS\n"
                   "   -h             Display program help and usage.\n"
                   "   -v             Display verbose program output.\n"
                   "   -b bits        Minimum bits needed for public key n (default: 256).\n"
                   "   -i iterations  Miller-Rabin iterations for testing primes (default: 50).\n"
                   "   -n pbfile      Public key file (default: ss.pub).\n"
                   "   -d pvfile      Private key file (default: ss.priv).\n"
                   "   -s seed        Random seed for testing.\n");
            return 1;
        }
    }

    if (!pbfile || !pvfile) { // error opening file and writing
        fprintf(stderr, "Error with file.\n"); // error message
        fclose(pbfile);
        fclose(pvfile);
        return -1;
    }

    fchmod(fileno(pvfile), 0600); // private key file permissions

    randstate_init(seed); // initialize random state

    ss_make_pub(p, q, n, bits, iters); // make p and q prime numbers and n

    ss_make_priv(d, pq, p, q); // make private key d and pq

    char *username = getenv("USER"); // get username
    // writes information to pb and pv file
    ss_write_pub(n, username, pbfile);
    ss_write_priv(pq, d, pvfile);

    if (verbose == 1) { // verbose output
        fprintf(stderr, "user = %s \n", username);
        gmp_printf("p (%d bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%d bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("pq (%d bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
        gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }
    // free memory
    mpz_clears(p, q, n, d, pq, NULL);
    randstate_clear();
    fclose(pbfile);
    fclose(pvfile);
    return 0;
}
