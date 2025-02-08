#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/time.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_PATH_LENGTH PATH_MAX
#define PROMPT_FORMAT "<%s@%s:%s> "
#define MAX_ARGS 64
#define TABLE_SIZE 100

extern int past_count;
extern char *arr[15];
extern char home_dir[MAX_PATH_LENGTH];

#endif
