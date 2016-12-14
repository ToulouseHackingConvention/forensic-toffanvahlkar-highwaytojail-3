#ifndef H_CRYPTO
#define H_CRYPTO

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#define KEY_SIZE 32
#define IV_SIZE 16

void initialise_openssl();
void close_openssl();
void handleErrors();
int get_random_bytes(unsigned char *buffer, unsigned int num);
int encrypt(unsigned char *key, unsigned char *iv, int fsrc, int fdst);
int decrypt(unsigned char *key, int fsrc, int fdst);

#endif
