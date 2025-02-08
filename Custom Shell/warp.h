#ifndef HEADERS_H7_
#define HEADERS_H7_

#include "headers.h"
#include "rem_fun.h"

void warp_command(char *path, char *current_dir, char *home_dir, char *prev_dir);
void handle_warp_command(char *command, char *current_dir, char *home_dir, char *prev_dir);

#endif