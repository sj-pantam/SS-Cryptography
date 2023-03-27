#include <stdio.h>
#include <gmp.h>
#include <unistd.h>
#include "numtheory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include "randstate.h"
#include "ss.h"
#include <time.h>

void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
    mpz_t p1, q1, pmod, qmod;
    mpz_inits(p1, q1, pmod, qmod, NULL);
    uint64_t pbits
        = (nbits / 5)
          + random()
                % (((2 * nbits) / 5)
                    - (nbits
                        / 5)); // gets random number of bits from the range [nbits/5, (2*nbits)/5)
    make_prime(p, pbits, iters); // calls make prime w/ specified bits
    uint64_t qbits = nbits - (2 * pbits); // gets qbits
    make_prime(q, qbits, iters); // calls make prime w/ specified bits
        // set p-1 and q-1
    mpz_sub_ui(p1, p, 1);
    mpz_sub_ui(q1, q, 1);
    mpz_mod(pmod, p1, q);
    mpz_mod(qmod, q1, p);
    // p is not divisible by q-1 and q is not divisible by p-1
    while (!mpz_cmp(p, q) || !mpz_cmp_ui(pmod, 0) || !mpz_cmp_ui(qmod, 0)) {
        make_prime(q, qbits, iters); // new prime number
        mpz_sub_ui(q1, q, 1);
        mpz_mod(pmod, p1, q);
        mpz_mod(qmod, q1, p);
    }
    mpz_t pp;
    mpz_init(pp);
    // n = p*p * q
    mpz_mul(pp, p, p);
    mpz_mul(n, pp, q);
    mpz_clears(p1, q1, pmod, qmod, pp, NULL);
}

void ss_make_priv(mpz_t d, mpz_t pq, const mpz_t p, const mpz_t q) {
    mpz_t p1, q1, g, num, lcm, n;
    mpz_inits(p1, q1, g, num, lcm, n, NULL);
    // gets the lcm through gcd
    mpz_sub_ui(p1, p, 1);
    mpz_sub_ui(q1, q, 1);
    gcd(g, p1, q1); // gcd of p-1 and q-1
    mpz_mul(num, p1, q1); // p-1 * q-1
    mpz_div(lcm, num, g); // lcm = (p-1 * q-1) / gcd(p-1, q-1)

    mpz_mul(pq, p, q); // pq = p*q
    mpz_mul(n, p, p);
    mpz_mul(n, n, q); // n = p*p * q
    mod_inverse(d, n, lcm); //inverse of n mod lcm

    mpz_clears(p1, q1, g, num, lcm, n, NULL);
}

void ss_write_pub(const mpz_t n, const char username[],
    FILE *pbfile) { // writes the public key n and the username to a specified file
    gmp_fprintf(pbfile, "%Zx\n", n);
    fprintf(pbfile, "%s \n", username);
}

void ss_write_priv(const mpz_t pq, const mpz_t d,
    FILE *pvfile) { // writes pq and the private key d to a specified file
    gmp_fprintf(pvfile, "%Zx\n", pq);
    gmp_fprintf(pvfile, "%Zx\n", d);
}

void ss_read_pub(mpz_t n, char username[],
    FILE *pbfile) { // reads in the public key n and the username from the public file
    gmp_fscanf(pbfile, "%Zx \n", n);
    fscanf(pbfile, "%s \n", username);
}

void ss_read_priv(
    mpz_t pq, mpz_t d, FILE *pvfile) { // reads in pq and the private key d from the private file
    gmp_fscanf(pvfile, "%Zx \n", pq);
    gmp_fscanf(pvfile, "%Zx \n", d);
}

void ss_encrypt(mpz_t c, const mpz_t m, const mpz_t n) {
    pow_mod(c, m, n, n);
}

void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t n) {
    mpz_t m, c, n1;
    mpz_inits(m, c, n1, NULL);
    mpz_sqrt(n1, n);
    uint64_t k = (mpz_sizeinbase(n1, 2) - 1) / 8; // block size for data to be encrypted in blocks
    uint8_t *A = (uint8_t *) calloc(k, sizeof(uint8_t));
    A[0] = 0xFF; // set zeroth byte to 0xFF
    uint64_t j = 0;
    while (!feof(infile)) { //until there are still unprocessed bytes
        j = fread(A + 1, sizeof(uint8_t), k - 1, infile); // read k-1 bytes from infile
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, A); // converts read bytes
        ss_encrypt(c, m, n); // call ss_encrypt to encrypt message
        gmp_fprintf(outfile, "%Zx\n", c);
    }
    free(A);
    mpz_clears(m, c, n1, NULL);
}

void ss_decrypt(mpz_t m, const mpz_t c, const mpz_t d, const mpz_t pq) {
    pow_mod(m, c, d, pq);
}

void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t d, const mpz_t pq) {
    mpz_t m, c;
    mpz_init(m);
    mpz_init(c);
    uint64_t k = (mpz_sizeinbase(pq, 2) - 1) / 8; // block size for data to be decrypted in blocks
    uint8_t *A = (uint8_t *) calloc(k, sizeof(uint8_t));

    uint64_t j = 0;
    while (!feof(infile)) { // iterate until the end of encrypted file
        gmp_fscanf(infile, "%Zx \n", c); // scan in the encrypted data
        ss_decrypt(m, c, d, pq); // call ss_decrypt to decrypt
        mpz_export(A, &j, 1, sizeof(uint8_t), 1, 0, m);
        fwrite(A + 1, sizeof(uint8_t), j - 1, outfile); // write j-1 bytes each time to outfile
    }
    free(A);
    mpz_clears(m, c, NULL);
}
