#include "warp.h"

void warp_command(char *path, char *current_dir, char *home_dir, char *prev_dir)
{
    char new_path[MAX_PATH_LENGTH];

    if (strcmp(path, "~") == 0)
    {
        snprintf(new_path, sizeof(new_path), "%s", home_dir);
    }

    else if (strcmp(path, "-") == 0)
    {
        snprintf(new_path, sizeof(new_path), "%s", prev_dir);
    }

    else if (strcmp(path, ".") == 0)
    {
        snprintf(new_path, sizeof(new_path), "%s", current_dir);
    }

    else if (strcmp(path, "..") == 0)
    {
        snprintf(new_path, sizeof(new_path), "%s", current_dir);
        char *last_slash = strrchr(new_path, '/');
        if (last_slash != NULL)
        {
            *last_slash = '\0';
        }
    }

    else if (path[0] == '~')
    {
        char trest[MAX_PATH_LENGTH];
        char trest1[MAX_PATH_LENGTH];

        strcpy(trest, path);
        strcpy(trest1, home_dir);

        removeSubstring(trest, "~");

        strcat(trest1, trest);

        snprintf(new_path, sizeof(new_path), "%s", trest1);
    }

    else if (path[0] == '/')
    {
        snprintf(new_path, sizeof(new_path), "%s", path);
    }

    else
    {
        char check_path[MAX_PATH_LENGTH];
        char yo[] = "/";

        strcpy(check_path, current_dir);
        strcat(yo, path);

        strcat(check_path, yo);
        if (isDirectory(check_path))
        {
            snprintf(new_path, sizeof(new_path), "%s/%s", current_dir, path);
        }
        else
        {
            strcpy(new_path, current_dir);
            printf("\033[31mNo Such Directory\n");

            printf("\033[0m");
        }
    }

    strcpy(prev_dir, current_dir);
    printf("%s\n", new_path);

    strcpy(current_dir, new_path);
}

void handle_warp_command(char *command, char *current_dir, char *home_dir, char *prev_dir)
{
    if (strcmp(command, "warp") == 0)
    {
        strcpy(current_dir, home_dir);
    }
    else
    {
        char warp_commands[256];
        strncpy(warp_commands, command + 5, sizeof(warp_commands));
        char *warp_token;
        char *warp_saveptr = warp_commands;

        while ((warp_token = strtok_r(warp_saveptr, " ", &warp_saveptr)))
        {
            warp_command(warp_token, current_dir, home_dir, prev_dir);
        }
    }
}

