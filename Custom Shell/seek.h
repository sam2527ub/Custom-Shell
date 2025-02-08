#ifndef HEADERS_H6_
#define HEADERS_H6_

#include "headers.h"
#include "rem_fun.h"

void search_directory_tree(char *dir_path, char *target, char *current_dir, char *home_dir, int dir_count, int file_count, int e_count, int *arr, char *rel_path, char **store);
void handle_seek(char *command, char *current_dir, char *home_dir, char *prev_dir);

#endif