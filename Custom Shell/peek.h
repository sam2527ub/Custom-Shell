#ifndef HEADERS_H2_
#define HEADERS_H2_

#include "headers.h"
#include "rem_fun.h"

void peek_command(const char *path, int show_all, int show_details);
void handle_peek_command(char *command, char *current_dir, char *home_dir, char *prev_dir);

#endif