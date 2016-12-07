#include <stdio.h>
#include "crypto.h"

int lock_file(unsigned char *key, const char *path_src, const char *path_dst);
int unlock_file(unsigned char *key, const char *path_src, const char *path_dst);

int main(int argc, char *argv[])
{
    /* A 256 bit key. */
    unsigned char *key = (unsigned char *) malloc(KEY_SIZE*sizeof(unsigned char));
    /* Generate a random 256 bit key. */
    printf("Random key:\n");
    if (!get_random_bytes(key, KEY_SIZE)) handleErrors();
    BIO_dump_fp (stdout, key, KEY_SIZE);

    lock_file(key, "./flag.plain", "./flag.cipher");
    unlock_file(key, "./flag.cipher", "./flag.dec");

    return 0;
}

int lock_file(unsigned char *key, const char *path_src, const char *path_dst)
{
    /* File descriptors to be used. */
    int fplain;
    int fcipher;

    /* A 128 bit IV - Different for each file. */
    unsigned char *iv = (unsigned char *) malloc(IV_SIZE*sizeof(unsigned char));

    /* Generate IV and key. */
    printf("Getting random IV...\n");
    if (!get_random_bytes(iv, IV_SIZE)) handleErrors();
    BIO_dump_fp (stdout, iv, IV_SIZE);

    /* Encryption from fplain to fcipher. */
    printf("Encryption...\n");
    fplain = open(path_src, O_RDONLY);
    if (fplain > -1)
    {
        fcipher = creat(path_dst, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fcipher > -1)
        {
            /* Encrypt the plaintext. */
            encrypt (key, iv, fplain, fcipher);
            close(fcipher);
        }
        else
        {
            /* Handle errors. */
            printf("Error opening destination file!\n");
        }
        close(fplain);
    }
    else
    {
        /* Handle errors. */
        printf("Error opening source file!\n");
    }

    return 0;
}

int unlock_file(unsigned char *key, const char *path_src, const char *path_dst)
{
    /* File descriptors to be used. */
    int fcipher;
    int fdec;

    /* Decryption of fcipher to fdec. */
    printf("Decryption...\n");
    fcipher = open(path_src, O_RDONLY);
    if (fcipher > -1)
    {
        fdec = creat(path_dst, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fdec > -1)
        {
            /* Decrypt the ciphertext. */
            decrypt(key, fcipher, fdec);
            close(fdec);
        }
        else
        {
            /* Handle errors. */
            printf("Error opening destination file!\n");
        }
        close(fcipher);
    }
    else
    {
        /* Handle errors. */
        printf("Error opening source file!\n");
    }

    return 0;
}
