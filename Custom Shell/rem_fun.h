#ifndef HEADERS_H5_
#define HEADERS_H5_

#include "headers.h"
#include "rem_fun.h"

void remove_non_alphabet_prefix(char *str);
void removeFirstCharacter(char *str);
void removeSubstring(char *str, const char *sub);
void subtractFromRight(char *str, char character);
void reduceContinuousSpaces(char *input);
int isDirectory(const char *path);
int compare_strings(const void *a, const void *b);
void print_colored(const char *str, const char *color_code);
void executeSystemCommand(const char *command);
void childHandler(int signal);
void exitHandler();
void sigintHandler(int sig);
void removeLastNewline(char *str);
bool containsCharacter(const char *str, char target);
void executeSystemCommand1(const char *command);

#endif
