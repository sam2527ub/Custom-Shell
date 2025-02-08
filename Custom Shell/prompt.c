#include "prompt.h"

void print_prompt(const char *username, const char *systemname, const char *current_dir, char *home_dir)
{
    char prompt[MAX_PATH_LENGTH + sizeof(PROMPT_FORMAT)];

    if (strcmp(current_dir, home_dir) == 0)
    {
        snprintf(prompt, sizeof(prompt), PROMPT_FORMAT, username, systemname, "~");
    }
    else if (strlen(current_dir) < strlen(home_dir))
    {
        snprintf(prompt, sizeof(prompt), PROMPT_FORMAT, username, systemname, current_dir);
    }
    else
    {
        char new_dir[MAX_PATH_LENGTH];
        strcpy(new_dir, current_dir);

        removeSubstring(new_dir, home_dir);

        char yo[MAX_PATH_LENGTH + 1] = "~";

        if (strlen(new_dir) > 0)
        {
            strcat(yo, new_dir);
        }

        snprintf(prompt, sizeof(prompt), PROMPT_FORMAT, username, systemname, yo);
    }
    printf("\033[31m");
    printf("%s", prompt);
    printf("\033[0m");
    fflush(stdout);
}