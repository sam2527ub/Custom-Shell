#include "seek.h"

void search_directory_tree(char *dir_path, char *target, char *current_dir, char *home_dir, int dir_count, int file_count, int e_count, int *arr, char *rel_path, char **store)
{
    DIR *dir = opendir(dir_path);
    if (dir == NULL)
    {
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char full_path[1024]; 
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        char copy[1024];
        strcpy(copy, entry->d_name);
        char *token = strtok(copy, ".");
        struct stat path_stat;

        if (strcmp(token, target) == 0)
        {
            char copy_mid[MAX_PATH_LENGTH];
            strcpy(copy_mid, full_path);

            if (lstat(copy_mid, &path_stat) == 0)
            {
                if (dir_count && S_ISDIR(path_stat.st_mode))
                {
                    if (strlen(copy_mid) >= strlen(rel_path))
                    {
                        removeSubstring(copy_mid, rel_path);
                        char new_string[MAX_PATH_LENGTH] = ".";
                        strcat(new_string, copy_mid);
                        strcpy(store[arr[0]], new_string);
                    }
                    else
                    {
                        strcpy(store[arr[0]], copy_mid);
                    }
                    arr[0]++;
                }

                else if (file_count && !(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                {
                    if (strlen(copy_mid) > strlen(rel_path))
                    {
                        removeSubstring(copy_mid, rel_path);
                        char new_string[MAX_PATH_LENGTH] = ".";
                        strcat(new_string, copy_mid);
                        strcpy(store[arr[0]], new_string);
                    }
                    else
                    {
                        strcpy(store[arr[0]], copy_mid);
                    }
                    arr[0]++;
                }
            }
        }

        if (entry->d_type == DT_DIR)
        {
            search_directory_tree(full_path, target, current_dir, home_dir, dir_count, file_count, e_count, arr, rel_path, store); // Recursive call for subdirectory
        }
    }

    closedir(dir);
}

void handle_seek(char *command, char *current_dir, char *home_dir, char *prev_dir)
{
    char *token = strtok(command, " ");
    int count = 0;

    char *arguments[5];

    for (int i = 0; i < 5; i++)
    {
        arguments[i] = (char *)malloc(1024 * sizeof(char));
    }

    char *store[MAX_PATH_LENGTH];

    for (int i = 0; i < MAX_PATH_LENGTH; i++)
    {
        arguments[i] = (char *)malloc(1024 * sizeof(char));
    }

    int arr[1];
    arr[0] = 0;
    int dir_count = 0;
    int file_count = 0;
    int e_count = 0;

    while (token != NULL)
    {
        strcpy(arguments[count], token);
        count++;

        if (strcmp(token, "-d") == 0)
        {
            dir_count = 1;
        }

        if (strcmp(token, "-f") == 0)
        {
            file_count = 1;
        }

        if (strcmp(token, "-e") == 0)
        {
            e_count = 1;
        }

        token = strtok(NULL, " ");
    }

    if (dir_count == file_count && dir_count == 1)
    {
        printf("\033[31m");
        printf("Invalid flags!\n");
        printf("\033[0m");
        return;
    }

    if (dir_count == 0 && file_count == 0)
    {
        dir_count = 1;
        file_count = 1;
    }

    if (count == 2)
    {
        search_directory_tree(current_dir, arguments[1], current_dir, home_dir, dir_count, file_count, e_count, arr, current_dir, store);

        if (arr[0] == 0)
        {
            printf("\033[31m");
            printf("No match found!\n");
            printf("\033[0m");
        }
        else if (arr[0] == 1)
        {
            if (e_count == 1)
            {
                struct stat path_stat;
                char exec[1024];
                strcpy(exec, current_dir);
                char ok[1024];
                strcpy(ok, store[0]);
                removeFirstCharacter(ok);
                strcat(exec, ok);
                if (access(exec, R_OK))
                {
                    printf("\033[31m");
                    printf("Missing permission for task\n");
                    printf("\033[0m");
                }

                if (lstat(exec, &path_stat) == 0)
                {
                    if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                    {
                        char command[MAX_PATH_LENGTH];
                        snprintf(command, sizeof(command), "cat %s", exec);
                        system(command);
                    }
                    else if ((S_ISDIR(path_stat.st_mode)))
                    {
                        strcpy(prev_dir, current_dir);
                        strcat(store[0], "/");
                        printf("%s\n", store[0]);
                        strcpy(current_dir, exec);
                    }
                }
            }
            else
            {
                struct stat path_stat;
                char exec[1024];
                strcpy(exec, current_dir);
                char ok[1024];
                strcpy(ok, store[0]);
                removeFirstCharacter(ok);
                strcat(exec, ok);

                if (lstat(exec, &path_stat) == 0)
                {
                    if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                    {
                        const char *greenCode = "\x1B[32m";
                        const char *resetCode = "\x1B[0m";
                        printf("%s%s%s\n", greenCode, store[0], resetCode);
                    }
                    else if ((S_ISDIR(path_stat.st_mode)))
                    {
                        const char *blueCode = "\x1B[34m";
                        const char *resetCode = "\x1B[0m";

                        printf("%s%s%s\n", blueCode, store[0], resetCode);
                    }
                }
            }
        }

        else if (arr[0] > 1)
        {
            for (int i = 0; i < arr[0]; i++)
            {
                struct stat path_stat;
                char exec[1024];
                strcpy(exec, current_dir);
                char ok[1024];
                strcpy(ok, store[i]);
                removeFirstCharacter(ok);
                strcat(exec, ok);

                if (lstat(exec, &path_stat) == 0)
                {
                    if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                    {
                        const char *greenCode = "\x1B[32m";
                        const char *resetCode = "\x1B[0m";
                        printf("%s%s%s\n", greenCode, store[i], resetCode);
                    }
                    else if ((S_ISDIR(path_stat.st_mode)))
                    {
                        const char *blueCode = "\x1B[34m";
                        const char *resetCode = "\x1B[0m";

                        printf("%s%s%s\n", blueCode, store[i], resetCode);
                    }
                }
            }
        }
    }
    else if (count == 3)
    {
        if (strcmp(arguments[1], "-f") != 0 && strcmp(arguments[1], "-d") != 0 && strcmp(arguments[1], "-e") != 0)
        {
            char path[MAX_PATH_LENGTH];

            if (arguments[2][0] == '.' && arguments[2][1] != '.')
            {
                removeSubstring(arguments[2], ".");
                strcpy(path, current_dir);
                strcat(path, arguments[2]);
            }

            else if (arguments[2][0] == '~')
            {
                removeSubstring(arguments[2], "~");
                strcpy(path, home_dir);
                strcat(path, arguments[2]);
            }
            else if (arguments[2][0] == '/')
            {
                strcpy(path, arguments[2]);
            }
            else if (arguments[2][0] == '.' && arguments[2][1] == '.')
            {
                snprintf(path, sizeof(path), "%s", current_dir);
                char *last_slash = strrchr(path, '/');
                if (last_slash != NULL)
                {
                    *last_slash = '\0';
                }

                removeSubstring(arguments[2], "..");

                strcat(path, arguments[2]);
            }
            else if (arguments[2][0] == '-')
            {
                strcpy(path, prev_dir);
            }

            search_directory_tree(path, arguments[count - 2], current_dir, home_dir, dir_count, file_count, e_count, arr, path, store);

            if (arr[0] == 0)
            {
                printf("\033[31m");
                printf("No match found!\n");
                printf("\033[0m");
            }
            else if (arr[0] == 1)
            {
                if (e_count == 1)
                {
                    struct stat path_stat;
                    char exec[1024];
                    strcpy(exec, path);
                    char ok[1024];
                    strcpy(ok, store[0]);
                    removeFirstCharacter(ok);
                    strcat(exec, ok);
                    if (access(exec, R_OK))
                    {
                        printf("\033[31m");
                        printf("Missing permission for task\n");
                        printf("\033[0m");
                    }

                    if (lstat(exec, &path_stat) == 0)
                    {
                        if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                        {
                            char command[MAX_PATH_LENGTH];
                            snprintf(command, sizeof(command), "cat %s", exec);
                            system(command);
                        }
                        else if ((S_ISDIR(path_stat.st_mode)))
                        {
                            strcpy(prev_dir, current_dir);
                            strcat(store[0], "/");
                            printf("%s\n", store[0]);
                            strcpy(current_dir, exec);
                        }
                    }
                }

                else
                {
                    struct stat path_stat;
                    char exec[1024];
                    strcpy(exec, path);
                    char ok[1024];
                    strcpy(ok, store[0]);
                    removeFirstCharacter(ok);
                    strcat(exec, ok);

                    if (lstat(exec, &path_stat) == 0)
                    {
                        if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                        {
                            const char *greenCode = "\x1B[32m";
                            const char *resetCode = "\x1B[0m";
                            printf("%s%s%s\n", greenCode, store[0], resetCode);
                        }
                        else if ((S_ISDIR(path_stat.st_mode)))
                        {
                            const char *blueCode = "\x1B[34m";
                            const char *resetCode = "\x1B[0m";

                            printf("%s%s%s\n", blueCode, store[0], resetCode);
                        }
                    }
                }
            }

            else if (arr[0] > 1)
            {
                for (int i = 0; i < arr[0]; i++)
                {
                    struct stat path_stat;
                    char exec[1024];
                    strcpy(exec, path);
                    char ok[1024];
                    strcpy(ok, store[i]);
                    removeFirstCharacter(ok);
                    strcat(exec, ok);

                    if (lstat(exec, &path_stat) == 0)
                    {
                        if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                        {
                            const char *greenCode = "\x1B[32m";
                            const char *resetCode = "\x1B[0m";
                            printf("%s%s%s\n", greenCode, store[i], resetCode);
                        }
                        else if ((S_ISDIR(path_stat.st_mode)))
                        {
                            const char *blueCode = "\x1B[34m";
                            const char *resetCode = "\x1B[0m";

                            printf("%s%s%s\n", blueCode, store[i], resetCode);
                        }
                    }
                }
            }
        }
        else
        {
            search_directory_tree(current_dir, arguments[count - 1], current_dir, home_dir, dir_count, file_count, e_count, arr, current_dir, store);

            if (arr[0] == 0)
            {
                printf("\033[31m");
                printf("No match found!\n");
                printf("\033[0m");
            }
            else if (arr[0] == 1)
            {
                if (e_count == 1)
                {
                    struct stat path_stat;
                    char exec[1024];
                    strcpy(exec, current_dir);
                    char ok[1024];
                    strcpy(ok, store[0]);
                    removeFirstCharacter(ok);
                    strcat(exec, ok);
                    if (access(exec, R_OK))
                    {
                        printf("\033[31m");
                        printf("Missing permission for task\n");
                        printf("\033[0m");
                    }

                    if (lstat(exec, &path_stat) == 0)
                    {
                        if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                        {
                            char command[MAX_PATH_LENGTH];
                            snprintf(command, sizeof(command), "cat %s", exec);
                            system(command);
                        }
                        else if ((S_ISDIR(path_stat.st_mode)))
                        {
                            strcpy(prev_dir, current_dir);
                            strcat(store[0], "/");
                            printf("%s\n", store[0]);
                            strcpy(current_dir, exec);
                        }
                    }
                }
                else
                {
                    struct stat path_stat;
                    char exec[1024];
                    strcpy(exec, current_dir);
                    char ok[1024];
                    strcpy(ok, store[0]);
                    removeFirstCharacter(ok);
                    strcat(exec, ok);

                    if (lstat(exec, &path_stat) == 0)
                    {
                        if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                        {
                            const char *greenCode = "\x1B[32m";
                            const char *resetCode = "\x1B[0m";
                            printf("%s%s%s\n", greenCode, store[0], resetCode);
                        }
                        else if ((S_ISDIR(path_stat.st_mode)))
                        {
                            const char *blueCode = "\x1B[34m";
                            const char *resetCode = "\x1B[0m";

                            printf("%s%s%s\n", blueCode, store[0], resetCode);
                        }
                    }
                }
            }

            else if (arr[0] > 1)
            {
                for (int i = 0; i < arr[0]; i++)
                {
                    struct stat path_stat;
                    char exec[1024];
                    strcpy(exec, current_dir);
                    char ok[1024];
                    strcpy(ok, store[i]);
                    removeFirstCharacter(ok);
                    strcat(exec, ok);

                    if (lstat(exec, &path_stat) == 0)
                    {
                        if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                        {
                            const char *greenCode = "\x1B[32m";
                            const char *resetCode = "\x1B[0m";
                            printf("%s%s%s\n", greenCode, store[i], resetCode);
                        }
                        else if ((S_ISDIR(path_stat.st_mode)))
                        {
                            const char *blueCode = "\x1B[34m";
                            const char *resetCode = "\x1B[0m";

                            printf("%s%s%s\n", blueCode, store[i], resetCode);
                        }
                    }
                }
            }
        }
    }
    else if (count == 4)
    {
        if (strcmp(arguments[2], "-f") != 0 && strcmp(arguments[2], "-d") != 0 && strcmp(arguments[2], "-e") != 0)
        {
            char path[MAX_PATH_LENGTH];
            if (arguments[3][0] == '.' && arguments[3][1] != '.')
            {
                removeSubstring(arguments[3], ".");
                strcpy(path, current_dir);
                strcat(path, arguments[3]);
            }

            else if (arguments[3][0] == '~')
            {
                removeSubstring(arguments[3], "~");
                strcpy(path, home_dir);
                strcat(path, arguments[3]);
            }

            else if (arguments[3][0] == '/')
            {
                strcpy(path, arguments[3]);
            }

            else if (arguments[3][0] == '.' && arguments[3][1] == '.')
            {
                snprintf(path, sizeof(path), "%s", current_dir);
                char *last_slash = strrchr(path, '/');
                if (last_slash != NULL)
                {
                    *last_slash = '\0';
                }

                removeSubstring(arguments[3], "..");

                strcat(path, arguments[3]);
            }
            else if (arguments[2][0] == '-')
            {
                strcpy(path, prev_dir);
            }

            search_directory_tree(path, arguments[count - 2], current_dir, home_dir, dir_count, file_count, e_count, arr, path, store);

            if (arr[0] == 0)
            {
                printf("\033[31m");
                printf("No match found!\n");
                printf("\033[0m");
            }
            else if (arr[0] == 1)
            {
                if (e_count == 1)
                {
                    struct stat path_stat;
                    char exec[1024];
                    strcpy(exec, path);
                    char ok[1024];
                    strcpy(ok, store[0]);
                    removeFirstCharacter(ok);
                    strcat(exec, ok);

                    if (access(exec, R_OK))
                    {
                        printf("\033[31m");
                        printf("Missing permission for task\n");
                        printf("\033[0m");
                    }

                    if (lstat(exec, &path_stat) == 0)
                    {
                        if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                        {
                            char command[MAX_PATH_LENGTH];
                            snprintf(command, sizeof(command), "cat %s", exec);
                            system(command);
                        }
                        else if ((S_ISDIR(path_stat.st_mode)))
                        {
                            strcpy(prev_dir, current_dir);
                            strcat(store[0], "/");
                            printf("%s\n", store[0]);
                            strcpy(current_dir, exec);
                        }
                    }
                }
                else
                {
                    struct stat path_stat;
                    char exec[1024];
                    strcpy(exec, path);
                    char ok[1024];
                    strcpy(ok, store[0]);
                    removeFirstCharacter(ok);
                    strcat(exec, ok);

                    if (lstat(exec, &path_stat) == 0)
                    {
                        if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                        {
                            const char *greenCode = "\x1B[32m";
                            const char *resetCode = "\x1B[0m";
                            printf("%s%s%s\n", greenCode, store[0], resetCode);
                        }
                        else if ((S_ISDIR(path_stat.st_mode)))
                        {
                            const char *blueCode = "\x1B[34m";
                            const char *resetCode = "\x1B[0m";

                            printf("%s%s%s\n", blueCode, store[0], resetCode);
                        }
                    }
                }
            }

            else if (arr[0] > 1)
            {
                for (int i = 0; i < arr[0]; i++)
                {
                    struct stat path_stat;
                    char exec[1024];
                    strcpy(exec, path);
                    char ok[1024];
                    strcpy(ok, store[i]);
                    removeFirstCharacter(ok);
                    strcat(exec, ok);

                    if (lstat(exec, &path_stat) == 0)
                    {
                        if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                        {
                            const char *greenCode = "\x1B[32m";
                            const char *resetCode = "\x1B[0m";
                            printf("%s%s%s\n", greenCode, store[i], resetCode);
                        }
                        else if ((S_ISDIR(path_stat.st_mode)))
                        {
                            const char *blueCode = "\x1B[34m";
                            const char *resetCode = "\x1B[0m";

                            printf("%s%s%s\n", blueCode, store[i], resetCode);
                        }
                    }
                }
            }
        }
        else
        {
            search_directory_tree(current_dir, arguments[count - 1], current_dir, home_dir, dir_count, file_count, e_count, arr, current_dir, store);

            if (arr[0] == 0)
            {
                printf("\033[31m");
                printf("No match found!\n");
                printf("\033[0m");
            }
            else if (arr[0] == 1)
            {
                if (e_count == 1)
                {
                    struct stat path_stat;
                    char exec[1024];
                    strcpy(exec, current_dir);
                    char ok[1024];
                    strcpy(ok, store[0]);
                    removeFirstCharacter(ok);
                    strcat(exec, ok);
                    if (access(exec, R_OK))
                    {
                        printf("\033[31m");
                        printf("Missing permission for task\n");
                        printf("\033[0m");
                    }

                    if (lstat(exec, &path_stat) == 0)
                    {
                        if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                        {
                            char command[MAX_PATH_LENGTH];
                            snprintf(command, sizeof(command), "cat %s", exec);
                            system(command);
                        }
                        else if ((S_ISDIR(path_stat.st_mode)))
                        {
                            strcpy(prev_dir, current_dir);
                            strcat(store[0], "/");
                            printf("%s\n", store[0]);
                            strcpy(current_dir, exec);
                        }
                    }
                }
                else
                {
                    struct stat path_stat;
                    char exec[1024];
                    strcpy(exec, current_dir);
                    char ok[1024];
                    strcpy(ok, store[0]);
                    removeFirstCharacter(ok);
                    strcat(exec, ok);

                    if (lstat(exec, &path_stat) == 0)
                    {
                        if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                        {
                            const char *greenCode = "\x1B[32m";
                            const char *resetCode = "\x1B[0m";
                            printf("%s%s%s\n", greenCode, store[0], resetCode);
                        }
                        else if ((S_ISDIR(path_stat.st_mode)))
                        {
                            const char *blueCode = "\x1B[34m";
                            const char *resetCode = "\x1B[0m";

                            printf("%s%s%s\n", blueCode, store[0], resetCode);
                        }
                    }
                }
            }

            else if (arr[0] > 1)
            {
                for (int i = 0; i < arr[0]; i++)
                {
                    struct stat path_stat;
                    char exec[1024];
                    strcpy(exec, current_dir);
                    char ok[1024];
                    strcpy(ok, store[i]);
                    removeFirstCharacter(ok);
                    strcat(exec, ok);

                    if (lstat(exec, &path_stat) == 0)
                    {
                        if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                        {
                            const char *greenCode = "\x1B[32m";
                            const char *resetCode = "\x1B[0m";
                            printf("%s%s%s\n", greenCode, store[i], resetCode);
                        }
                        else if ((S_ISDIR(path_stat.st_mode)))
                        {
                            const char *blueCode = "\x1B[34m";
                            const char *resetCode = "\x1B[0m";

                            printf("%s%s%s\n", blueCode, store[i], resetCode);
                        }
                    }
                }
            }
        }
    }

    else if (count == 5)
    {
        char path[MAX_PATH_LENGTH];
        if (arguments[4][0] == '.' && arguments[4][1] != '.')
        {
            removeSubstring(arguments[4], ".");
            strcpy(path, current_dir);
            strcat(path, arguments[4]);
        }

        else if (arguments[4][0] == '~')
        {
            removeSubstring(arguments[4], "~");
            strcpy(path, home_dir);
            strcat(path, arguments[4]);
        }

        else if (arguments[4][0] == '/')
        {
            strcpy(path, arguments[4]);
        }
        else if (arguments[4][0] == '.' && arguments[4][1] == '.')
        {
            snprintf(path, sizeof(path), "%s", current_dir);
            char *last_slash = strrchr(path, '/');
            if (last_slash != NULL)
            {
                *last_slash = '\0';
            }

            removeSubstring(arguments[4], "..");

            strcat(path, arguments[4]);
        }
        else if (arguments[2][0] == '-')
        {
            strcpy(path, prev_dir);
        }

        search_directory_tree(path, arguments[count - 2], current_dir, home_dir, dir_count, file_count, e_count, arr, path, store);

        if (arr[0] == 0)
        {
            printf("\033[31m");
            printf("No match found!\n");
            printf("\033[0m");
        }
        else if (arr[0] == 1)
        {
            if (e_count == 1)
            {
                struct stat path_stat;
                char exec[1024];
                strcpy(exec, path);
                char ok[1024];
                strcpy(ok, store[0]);
                removeFirstCharacter(ok);
                strcat(exec, ok);
                if (access(exec, R_OK))
                {
                    printf("\033[31m");
                    printf("Missing permission for task\n");
                    printf("\033[0m");
                }

                if (lstat(exec, &path_stat) == 0)
                {
                    if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                    {
                        char command[MAX_PATH_LENGTH];
                        snprintf(command, sizeof(command), "cat %s", exec);
                        system(command);
                    }
                    else if ((S_ISDIR(path_stat.st_mode)))
                    {
                        strcpy(prev_dir, current_dir);
                        strcat(store[0], "/");
                        printf("%s\n", store[0]);
                        strcpy(current_dir, exec);
                    }
                }
            }
            else
            {
                struct stat path_stat;
                char exec[1024];
                strcpy(exec, path);
                char ok[1024];
                strcpy(ok, store[0]);
                removeFirstCharacter(ok);
                strcat(exec, ok);

                if (lstat(exec, &path_stat) == 0)
                {
                    if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                    {
                        const char *greenCode = "\x1B[32m";
                        const char *resetCode = "\x1B[0m";
                        printf("%s%s%s\n", greenCode, store[0], resetCode);
                    }
                    else if ((S_ISDIR(path_stat.st_mode)))
                    {
                        const char *blueCode = "\x1B[34m";
                        const char *resetCode = "\x1B[0m";

                        printf("%s%s%s\n", blueCode, store[0], resetCode);
                    }
                }
            }
        }

        else if (arr[0] > 1)
        {
            for (int i = 0; i < arr[0]; i++)
            {
                struct stat path_stat;
                char exec[1024];
                strcpy(exec, path);
                char ok[1024];
                strcpy(ok, store[i]);
                removeFirstCharacter(ok);
                strcat(exec, ok);

                if (lstat(exec, &path_stat) == 0)
                {
                    if (!(path_stat.st_mode & S_IXUSR) && !(S_ISDIR(path_stat.st_mode)))
                    {
                        const char *greenCode = "\x1B[32m";
                        const char *resetCode = "\x1B[0m";
                        printf("%s%s%s\n", greenCode, store[i], resetCode);
                    }
                    else if ((S_ISDIR(path_stat.st_mode)))
                    {
                        const char *blueCode = "\x1B[34m";
                        const char *resetCode = "\x1B[0m";

                        printf("%s%s%s\n", blueCode, store[i], resetCode);
                    }
                }
            }
        }
    }
}
