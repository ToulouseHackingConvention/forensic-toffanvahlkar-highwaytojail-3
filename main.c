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

#define SRC_PATH "./flag.plain"
#define DST_PATH "./flag.cipher"

void handleErrors();
int get_random_bytes(unsigned char *buffer, unsigned int num);
int encrypt(unsigned char *key, unsigned char *iv, int fsrc, int fdst);
int decrypt(unsigned char *key, int fsrc, int fdst);

int main (int argc, char *argv[])
{
    /* Set up the key and iv. Do I need to say to not hard code these in a real application? :-) */

    /* A 256 bit key. */
    unsigned char *key = (unsigned char *) malloc(32*sizeof(unsigned char));

    /* A 128 bit IV. */
    unsigned char *iv = (unsigned char *) malloc(16*sizeof(unsigned char));

    /* File descriptors to be used. */
    int fplain;
    int fcipher;
    int fdec;

    /* Initialise the library. */
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);

    /* Generate IV and key. */
    printf("Getting random iv...\n");
    if (!get_random_bytes(iv, 16)) handleErrors();
    BIO_dump_fp (stdout, iv, 16);
    printf("Getting random key...\n");
    if (!get_random_bytes(key, 32)) handleErrors();
    BIO_dump_fp (stdout, key, 32);

    /* Encryption from fplain to fcipher. */
    printf("Encryption...\n");
    fplain = open("./flag.plain", O_RDONLY);
    if (fplain > -1)
    {
        fcipher = creat("./flag.cipher", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
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

    /* Decryption of fcipher to fdec. */
    printf("Decryption...\n");
    fcipher = open("./flag.cipher", O_RDONLY);
    if (fcipher > -1)
    {
        fdec = creat("./flag.dec", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
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

    /* Clean up. */
    EVP_cleanup();
    ERR_free_strings();

    return 0;
}

int get_random_bytes(unsigned char *buffer, unsigned int num)
{
    static int initialised = 0;

    if (!initialised)
    {
        RAND_poll();
        initialised = 1;
    }

    int rc = RAND_bytes(buffer, num);
    unsigned long err = ERR_get_error();

    if(rc != 1) {
        /* RAND_bytes failed */
        /* `err` is valid    */
        printf("Error!\n");
        ERR_print_errors_fp(stderr);
    }

    return rc;
}

/*
 * Handle errors related to the OpenSSL library.
 * */
void handleErrors()
{
    ERR_print_errors_fp(stderr);
    abort();
}

/*
 * Encrypt a file fsrc to another fdst.
 * Key (256 bits) and IV (128 bits) should be randomised.
 * */
int encrypt(unsigned char *key, unsigned char *iv, int fsrc, int fdst)
{
    EVP_CIPHER_CTX *ctx;

    /* Message to be encrypted. */
    unsigned char plaintext[128];
    /* Length of the current plaintext buffer. */
    int plaintext_len = 0;

    /* Buffer for ciphertext.
     * Ensure the buffer is long enough for the ciphertext which may be longer than the plaintext, dependant on the algorithm and mode.  */
    unsigned char ciphertext[255];
    /* Total length of the resulting ciphertext. */
    int ciphertext_len = 0;

    /* Length variable used by cryptographic primitives. */
    int len = 0;

    /* Write the 128 bit IV at the beginning of the file. */
    len = write(fdst, iv, 16);
    if (len != 16)
    {
        printf("Error writing IV ; len: %d\n", len);
        exit(0);
    }
    ciphertext_len += len;

    /* Create and initialise the context. */
    if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    /* Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher.
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits. */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) handleErrors();

    /* Get the beginning of the plaintext. */
    plaintext_len = read(fsrc, plaintext, 128);
    /* Start encrypting the message. */
    while (plaintext_len > 0)
    {
        /* Provide the message to be encrypted, and obtain the encrypted output.
         * EVP_EncryptUpdate can be called multiple times if necessary. */
        if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) handleErrors();
        ciphertext_len += len;

        /* Write the resulting ciphertext. */
        write(fdst, ciphertext, len);

        /* Get the remaining of the plaintext. */
        plaintext_len = read(fsrc, plaintext, 128);
    }

    /* Finalise the encryption. Further ciphertext bytes may be written at this stage. */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext, &len)) handleErrors();
    ciphertext_len += len;

    /* Write the resulting ciphertext. */
    write(fdst, ciphertext, len);

    /* Clean up. */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

/*
 * Decrypt an encrypted file fsrc to a file fdst.
 * Key is 256 bits long.
 * The 128 bits IV should be at the beginning of the file to decrypt.
 * */
int decrypt(unsigned char *key, int fsrc, int fdst)
{
    EVP_CIPHER_CTX *ctx;

    /* The 128 bit IV. */
    unsigned char iv[16];

    /* Buffer for ciphertext. */
    unsigned char ciphertext[128];
    /* Length of the current ciphertext buffer. */
    int ciphertext_len = 0;

    /* Buffer for the decrypted text. */
    unsigned char plaintext[255];
    /* Total length of the resulting plaintext. */
    int plaintext_len = 0;

    /* Length variable used by cryptographic primitives. */
    int len = 0;

    /* Retrieve the 128 bit IV at the beginning of the file. */
    len = read(fsrc, iv, 16);
    if (len != 16)
    {
        printf("Error reading IV ; len: %d\n", len);
        exit(0);
    }

    /* Create and initialise the context. */
    if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    /* Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher.
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits. */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /* Get the beginning of the ciphertext. */
    ciphertext_len = read(fsrc, ciphertext, 128);
    while(ciphertext_len > 0)
    {
        /* Provide the message to be decrypted, and obtain the plaintext output.
         * EVP_DecryptUpdate can be called multiple times if necessary. */
        if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
            handleErrors();
        plaintext_len += len;

        /* Write the resulting ciphertext. */
        write(fdst, plaintext, len);

        /* Get the remaining of the plaintext. */
        ciphertext_len = read(fsrc, ciphertext, 128);
    }

    /* Finalise the decryption. Further plaintext bytes may be written at this stage. */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext, &len)) handleErrors();
    plaintext_len += len;

    /* Write the resulting plaintext. */
    write(fdst, plaintext, len);

    /* Clean up. */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}
