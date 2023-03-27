#include "numtheory.h"

#include "randstate.h"

#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void pow_mod(mpz_t o, const mpz_t a, const mpz_t d, const mpz_t n) {
    mpz_t temp_o; // declare and initialzie all mpz_t integers
    mpz_init(temp_o);
    mpz_set_ui(temp_o, 1);
    mpz_t temp_a;
    mpz_init(temp_a);
    mpz_set(temp_a, a);
    mpz_t temp_n;
    mpz_init(temp_n);
    mpz_set(temp_n, n);
    mpz_t temp_d;
    mpz_init(temp_d);
    mpz_set(temp_d, d);
    while (mpz_cmp_ui(temp_d, 0) > 0) { //check if d > 0 and if so keep running
        if (mpz_odd_p(temp_d)) { // checks for odd
            mpz_mul(temp_o, temp_o, temp_a);
            mpz_mod(temp_o, temp_o, temp_n);
        }
        mpz_mul(temp_a, temp_a, temp_a);
        mpz_mod(temp_a, temp_a, temp_n);
        mpz_fdiv_q_ui(temp_d, temp_d, 2); // floor divs for d
    }
    mpz_set(o, temp_o); //clear
    mpz_clear(temp_o);
    mpz_clear(temp_a);
    mpz_clear(temp_d);
    mpz_clear(temp_n);
}

/*
void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {
    mpz_t v, d1, p, n;
    mpz_inits(v, p, d1, NULL);
    mpz_set_ui(v, 1);
    mpz_set(p, a);
    mpz_set(d1, d);
    while (mpz_cmp_ui(d1, 0) == 1) {
        if (mpz_odd_p(d1)) {
            mpz_mul(v, v, p);
            mpz_mod(v, v, n);
        }
        mpz_mul(p, p, p);
        mpz_mod(p, p, n);
        mpz_fdiv_q_ui(d1, d1, 2);
    }
    mpz_set(o, v);
    mpz_clears(v, d1, p, NULL);
}
*/
bool is_prime(const mpz_t n, uint64_t iters) {
    mpz_t a, r, s, y, n1, j, t, n3;
    mpz_inits(a, r, s, y, n1, j, t, n3, NULL);
    mpz_set_ui(t, 2);
    mpz_sub_ui(n1, n, 1);
    mpz_set(r, n1);
    mpz_sub_ui(n3, n, 3);

    if (mpz_cmp_ui(n, 2) < 0) { // checks if n is less than two
        mpz_clears(a, r, s, y, n1, j, t, n3, NULL);
        return false;
    }
    if ((mpz_cmp_ui(n, 2)) == 0) { // checks if n is two
        mpz_clears(a, r, s, y, n1, j, t, n3, NULL);
        return true;
    }
    if ((mpz_cmp_ui(n, 3)) == 0) { //checks if n is three
        mpz_clears(a, r, s, y, n1, j, t, n3, NULL);
        return true;
    }
    if ((mpz_even_p(n)) != 0) { // checks if n is 4 or even numbers in general
        mpz_clears(a, r, s, y, n1, j, t, n3, NULL);
        return false;
    }

    while ((mpz_odd_p(r)) == 0) { // n-1 = 2^s * r
        mpz_fdiv_q_ui(r, r, 2); // divide r by two each iteration
        mpz_add_ui(s, s, 1);
    }

    mpz_sub_ui(s, s, 1);

    for (uint64_t i = 1; i <= iters; i++) {
        mpz_urandomm(a, state, n3); // pick a number between a to n-2 (base)
        mpz_add_ui(a, a, 2);
        pow_mod(y, a, r, n); // power mod
        if (!(mpz_cmp_ui(y, 1) == 0) && !(mpz_cmp(y, n1) == 0)) { // y not equal to 1 & n-1
            mpz_set_ui(j, 1);
            while (!(mpz_cmp(y, n1) == 0)
                   && (mpz_cmp(j, s)
                       <= 0)) { // while j is less than equal to s - 1 & y is not equal to n-1
                pow_mod(y, y, t, n);
                if ((mpz_cmp_ui(y, 1)) == 0) { // y equal to 1
                    mpz_clears(a, r, s, y, n1, j, t, n3, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            if (!(mpz_cmp(y, n1) == 0)) { // y is not equal to 1
                mpz_clears(a, r, s, y, n1, j, t, n3, NULL);
                return false;
            }
        }
    }
    mpz_clears(a, r, s, y, n1, j, t, n3, NULL);
    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) { // get large random prime number
    do {
        mpz_urandomb(p, state, bits); // random prime number w/ bits
    } while (is_prime(p, iters) == false); // checks for prime with miller rabin test
}

/*	
void gcd(mpz_t g, mpz_t a, mpz_t b) {
    mpz_t t;
    mpz_init(t);
    
    while (!(mpz_cmp_ui(b, 0) == 0)) {
        mpz_set(t, b);
        mpz_mod(b, a, b);
        mpz_set(a, t);
    }
    mpz_set(g, a);
    mpz_clear(t);
}
*/

void gcd(mpz_t g, const mpz_t a, const mpz_t b) {
    mpz_t a1; // declare and initialzie all mpz_t integers
    mpz_init(a1);
    mpz_set(a1, a);
    mpz_t b1;
    mpz_init(b1);
    mpz_set(b1, b);
    mpz_t t;
    mpz_init(t);
    while (mpz_cmp_ui(b1, 0) != 0) { // while b does not equal zero
        mpz_set(t, b1);
        mpz_mod(b1, a1, b1);
        mpz_set(a1, t);
    }
    mpz_set(g, a1); // set to output
    mpz_clear(t);
    mpz_clear(a1);
    mpz_clear(b1);
}

void mod_inverse(mpz_t o, const mpz_t a, const mpz_t n) {
    mpz_t r; // declare and initialzie all mpz_t integers
    mpz_init(r);
    mpz_set(r, n);

    mpz_t r_i;
    mpz_init(r_i);
    mpz_set(r_i, a);

    mpz_t t;
    mpz_init(t);

    mpz_t temp_r;
    mpz_init(temp_r);

    mpz_t temp_t; // declare and initialzie all mpz_t integers
    mpz_init(temp_t);

    mpz_t t_i;
    mpz_init(t_i);
    mpz_set_ui(t_i, 1);

    mpz_t q;
    mpz_init(q);

    while (mpz_cmp_ui(r_i, 0) != 0) {
        mpz_set(temp_r, r); // declare and initialzie all mpz_t integers
        mpz_fdiv_q(q, r, r_i);
        mpz_set(r, r_i);
        mpz_mul(r_i, q, r_i);
        mpz_sub(r_i, temp_r, r_i);

        mpz_set(temp_t, t); // declare and initialzie all mpz_t integers
        mpz_set(t, t_i);
        mpz_mul(t_i, q, t_i);
        mpz_sub(t_i, temp_t, t_i);
    }
    if (mpz_cmp_ui(r, 1) > 0) { // if f is dnot equal to zero
        mpz_set_ui(o, 0);
        mpz_clears(r, r_i, q, t, t_i, temp_t, temp_r, NULL);
        return;
    }
    if (mpz_cmp_ui(t, 0) < 0) { // return no inverse
        mpz_add(t, t, n);
    }
    mpz_set(o, t);
    mpz_clears(r, r_i, q, t, t_i, temp_t, temp_r, NULL);
}
