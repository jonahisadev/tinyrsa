#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <sys/random.h>
#include <gmp.h>

void totient(mpz_t p1, mpz_t q1, mpz_t* t)
{
  mpz_t p2, q2;
  mpz_inits(p2, q2, NULL);

  mpz_sub_ui(p2, p1, 1);
  mpz_sub_ui(q2, q1, 1);
  mpz_mul(*t, p2, q2);

  mpz_clears(p2, q2, NULL);
}

void random_int(mpz_t* ptr, uint32_t bits)
{
  gmp_randstate_t rand;
  gmp_randinit_mt(rand);
  uint32_t seed;
  getrandom(&seed, sizeof(seed), 0L);
  gmp_randseed_ui(rand, seed);
  mpz_urandomb(*ptr, rand, bits);
}

void encode_message(const char* buffer, mpz_t* encoded)
{
  size_t len = strlen(buffer);
  for (size_t i = 0; i < len; i++) {
    mpz_t c;
    mpz_init(c);
    mpz_set_ui(c, (int) buffer[i]);
    mpz_mul_2exp(c, c, i * 8);  // left shift
    mpz_xor(*encoded, *encoded, c);
    mpz_clear(c);
  }
}

void decode_message(mpz_t number, char* decoded) {
  size_t size = mpz_sizeinbase(number, 2);
  for (size_t i = 0; i < size; i += 8) {
    mpz_t temp, ff;
    mpz_inits(temp, ff, NULL);

    mpz_set_ui(ff, 0xFF);

    mpz_set(temp, number);
    mpz_fdiv_q_2exp(temp, temp, i); // right shift
    mpz_and(temp, temp, ff);
    short val = (short) mpz_get_si(temp);

    decoded[i / 8] = (char) val;

    mpz_clears(temp, ff, NULL);
  }

  decoded[size / 8] = '\0';
}

void generate()
{
  int bits;
  printf("Select bit size: ");
  scanf("%d", &bits);

  mpz_t p, q, n, t, e, d;
  mpz_inits(p, q, n, t, e, d, NULL);

  // P
  printf("Generating P...\n");
  uint32_t psize = bits / 2;
  random_int(&p, psize);
  mpz_setbit(p, psize - 1);
  mpz_setbit(p, psize - 2);
  mpz_setbit(p, 0);
  while (!mpz_probab_prime_p(p, 10)) {
    mpz_add_ui(p, p, 2L);
  }
  gmp_printf("Generated P: %Zd\n", p);

  // Q
  printf("Generating Q...\n");
  uint32_t qsize = bits / 2;
  random_int(&q, qsize);
  mpz_setbit(q, qsize - 1);
  mpz_setbit(q, qsize - 2);
  mpz_setbit(q, 0);
  while (!mpz_probab_prime_p(q, 10)) {
    mpz_add_ui(q, q, 2L);
  }
  gmp_printf("Generated Q: %Zd\n", q);

  // N
  mpz_mul(n, p, q);
  gmp_printf("Modulo: %Zd\n", n);

  // T
  totient(p, q, &t);

  // E
  mpz_set_ui(e, 65537);

  // D
  mpz_invert(d, e, t);

  // Public Key
  FILE* fp;
  fp = fopen("keys/public_key", "wb");
  mpz_out_raw(fp, n);
  mpz_out_raw(fp, e);
  fclose(fp);

  // Private Key
  fp = fopen("keys/private_key", "wb");
  mpz_out_raw(fp, n);
  mpz_out_raw(fp, d);
  fclose(fp);

  printf("Wrote keys\n");
  mpz_clears(p, q, n, t, e, d, NULL);
}

void encrypt() {
  char buffer[256];
  printf("Message: ");
  fgets(buffer, 256, stdin);

  mpz_t encoded;
  mpz_init(encoded);
  encode_message(buffer, &encoded);

  mpz_t n, e, c;
  mpz_inits(n, e, c, NULL);

  FILE* fp;
  fp = fopen("keys/public_key", "rb");
  mpz_inp_raw(n, fp);
  mpz_inp_raw(e, fp);
  fclose(fp);

  mpz_powm(c, encoded, e, n);

  gmp_printf("Encoded: %Zd\n", c);

  mpz_clears(n, e, c, NULL);
  mpz_clear(encoded);
}

void decrypt()
{
  char buffer[2048];
  printf("Encrypted message: ");

  mpz_t c, n, d, m;
  mpz_inits(c, n, d, m, NULL);
  mpz_inp_str(c, stdin, 10);

  FILE* fp;
  fp = fopen("keys/private_key", "rb");
  mpz_inp_raw(n, fp);
  mpz_inp_raw(d, fp);
  fclose(fp);

  mpz_powm(m, c, d, n);

  char message[512];
  decode_message(m, message);

  printf("Message: %s\n", message);

  mpz_clears(c, n, d, m, NULL);
}

int main(int argc, char** argv)
{
  if (argc < 2) {
    fprintf(stderr, "Too few arguments!\n");
    return 1;
  }

  printf("Jonah's TinyRSA\n");

  if (!strcmp(argv[1], "gen")) {
    generate();
  }

  else if (!strcmp(argv[1], "encrypt")) {
    encrypt();
  }

  else if (!strcmp(argv[1], "decrypt")) {
    decrypt();
  }

  return 0;
}
