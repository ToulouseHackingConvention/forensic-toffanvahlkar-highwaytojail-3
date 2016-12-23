#ifndef H_EXPLORER
#define H_EXPLORER

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include "crypto.h"

enum action {ENCRYPT, DECRYPT};

int handle_folder(unsigned char *key, enum action action, int dirfd);
int handle_file(unsigned char *key, enum action action, int ftarget);

#endif
