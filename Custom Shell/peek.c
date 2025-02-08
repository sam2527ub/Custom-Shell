#include "peek.h"

void peek_command(const char *path, int show_all, int show_details)
{
    DIR *dir = opendir(path);

    if (dir == NULL)
    {
        printf("\033[31m");

        perror("opendir");

        printf("\033[0m");
        return;
    }

    struct dirent *entry;
    char **names = NULL;
    int num_entries = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        if (!show_all && entry->d_name[0] == '.')
        {
            continue; 
        }

        char full_path[MAX_PATH_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat entry_stat;
        if (lstat(full_path, &entry_stat) == -1)
        {
            printf("\033[31m");

            perror("lstat");
            
            printf("\033[0m");
            closedir(dir);
            return;
        }

        names = (char **)realloc(names, (num_entries + 1) * sizeof(char *));
        names[num_entries] = strdup(entry->d_name);
        num_entries++;
    }

    closedir(dir);

    qsort(names, num_entries, sizeof(char *), compare_strings);

    for (int i = 0; i < num_entries; i++)
    {
        char full_path[MAX_PATH_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, names[i]);

        struct stat entry_stat;
        if (lstat(full_path, &entry_stat) == -1)
        {
            printf("\033[31m");
            
            perror("lstat");

            printf("\033[0m");
            continue;
        }

        if (show_details)
        {
            print_permissions(entry_stat.st_mode);

            struct passwd *user_info = getpwuid(entry_stat.st_uid);
            struct group *group_info = getgrgid(entry_stat.st_gid);
            printf(" %2ld %s %s %6ld ", entry_stat.st_nlink, user_info->pw_name, group_info->gr_name, entry_stat.st_size);

            char time_buffer[80];
            struct tm *timeinfo = localtime(&entry_stat.st_mtime);
            strftime(time_buffer, sizeof(time_buffer), "%b %d %H:%M", timeinfo);
            printf("%s ", time_buffer);
            if (S_ISDIR(entry_stat.st_mode))
            {
                print_colored(names[i], "\033[34m"); 
            }
            else if (entry_stat.st_mode & S_IXUSR)
            {
                print_colored(names[i], "\033[32m"); 
            }
            else
            {
                print_colored(names[i], "\033[0m"); 
            }
        }
        else
        {
            if (S_ISDIR(entry_stat.st_mode))
            {
                print_colored(names[i], "\033[34m"); 
            }
            else if (entry_stat.st_mode & S_IXUSR)
            {
                print_colored(names[i], "\033[32m"); 
            }
            else
            {
                print_colored(names[i], "\033[0m"); 
            }
        }

        free(names[i]);
    }

    free(names);
}

void handle_peek_command(char *command, char *current_dir, char *home_dir, char *prev_dir)
{
    char delim[] = " ";
    char *mid[4];
    char *yes;
    int show_all = 0;
    int show_details = 0;
    int count1 = 0;

    yes = strtok(command, delim);

    while (yes != NULL)
    {
        mid[count1] = yes;
        count1++;
        yes = strtok(NULL, delim);
    }

    char peek_path[MAX_PATH_LENGTH];

    if (count1 == 1)
    {
        snprintf(peek_path, sizeof(peek_path), "%s", current_dir);
    }
    else if (count1 == 2)
    {
        snprintf(peek_path, sizeof(peek_path), "%s", current_dir);
        if (strcmp(mid[1], "-a") == 0)
        {
            show_all = 1;
        }
        if (strcmp(mid[1], "-l") == 0)
        {
            show_details = 1;
        }
        if (strcmp(mid[1], "-la") == 0)
        {
            show_all = 1;
            show_details = 1;
        }
        if (strcmp(mid[1], "-al") == 0)
        {
            show_all = 1;
            show_details = 1;
        }

        if (show_all == 0 && show_details == 0)
        {
            if (strcmp(mid[1], ".") == 0)
            {
                snprintf(peek_path, sizeof(peek_path), "%s", current_dir);
            }
            else if (strcmp(mid[1], "~") == 0)
            {
                snprintf(peek_path, sizeof(peek_path), "%s", home_dir);
            }
            else if (strcmp(mid[1], "-") == 0)
            {
                snprintf(peek_path, sizeof(peek_path), "%s", prev_dir);
            }
            else if (strcmp(mid[1], "..") == 0)
            {
                char test_path[MAX_PATH_LENGTH];
                strcpy(test_path, current_dir);
                subtractFromRight(test_path, '/');
                snprintf(peek_path, sizeof(peek_path), "%s", test_path);
            }
            else if (mid[1][0] == '/')
            {
                snprintf(peek_path, sizeof(peek_path), "%s", mid[1]);
            }
            else if (mid[1][0] == '~' && mid[1][1] == '/')
            {
                char test_path[MAX_PATH_LENGTH];
                strcpy(test_path, home_dir);
                removeSubstring(mid[1], "~");
                strcat(test_path, mid[1]);
                snprintf(peek_path, sizeof(peek_path), "%s", test_path);
            }
            else
            {
                char test_path[MAX_PATH_LENGTH];
                strcpy(test_path, current_dir);
                strcat(test_path, "/");
                strcat(test_path, mid[1]);

                snprintf(peek_path, sizeof(peek_path), "%s", test_path);
            }
        }
    }
    else if (count1 == 3)
    {
        if (strcmp(mid[1], "-a") == 0)
        {
            show_all = 1;
        }
        if (strcmp(mid[1], "-l") == 0)
        {
            show_details = 1;
        }
        if (strcmp(mid[1], "-la") == 0)
        {
            show_all = 1;
            show_details = 1;
        }
        if (strcmp(mid[1], "-al") == 0)
        {
            show_all = 1;
            show_details = 1;
        }

        if (strcmp(mid[2], "-a") == 0 || strcmp(mid[2], "-l") == 0)
        {
            show_all = 1;
            show_details = 1;
            snprintf(peek_path, sizeof(peek_path), "%s", current_dir);
        }
        else
        {
            if (strcmp(mid[2], ".") == 0)
            {
                snprintf(peek_path, sizeof(peek_path), "%s", current_dir);
            }
            else if (strcmp(mid[2], "~") == 0)
            {
                snprintf(peek_path, sizeof(peek_path), "%s", home_dir);
            }
            else if (strcmp(mid[2], "-") == 0)
            {
                snprintf(peek_path, sizeof(peek_path), "%s", prev_dir);
            }
            else if (strcmp(mid[2], "..") == 0)
            {
                char test_path[MAX_PATH_LENGTH];
                strcpy(test_path, current_dir);
                subtractFromRight(test_path, '/');
                snprintf(peek_path, sizeof(peek_path), "%s", test_path);
            }
            else if (mid[2][0] == '/')
            {
                snprintf(peek_path, sizeof(peek_path), "%s", mid[2]);
            }
            else if (mid[2][0] == '~' && mid[2][1] == '/')
            {
                char test_path[MAX_PATH_LENGTH];
                strcpy(test_path, home_dir);
                removeSubstring(mid[2], "~");
                strcat(test_path, mid[2]);
                snprintf(peek_path, sizeof(peek_path), "%s", test_path);
            }
            else
            {
                char test_path[MAX_PATH_LENGTH];
                strcpy(test_path, current_dir);
                strcat(test_path, "/");
                strcat(test_path, mid[2]);

                snprintf(peek_path, sizeof(peek_path), "%s", test_path);
            }
        }
    }

    else if (count1 == 4)
    {

        show_all = 1;
        show_details = 1;
        if (strcmp(mid[3], ".") == 0)
        {
            snprintf(peek_path, sizeof(peek_path), "%s", current_dir);
        }
        else if (strcmp(mid[3], "~") == 0)
        {
            snprintf(peek_path, sizeof(peek_path), "%s", home_dir);
        }
        else if (strcmp(mid[3], "-") == 0)
        {
            snprintf(peek_path, sizeof(peek_path), "%s", prev_dir);
        }
        else if (strcmp(mid[3], "..") == 0)
        {
            char test_path[MAX_PATH_LENGTH];
            strcpy(test_path, current_dir);
            subtractFromRight(test_path, '/');
            snprintf(peek_path, sizeof(peek_path), "%s", test_path);
        }
        else if (mid[3][0] == '/')
        {
            snprintf(peek_path, sizeof(peek_path), "%s", mid[3]);
        }
        else if (mid[3][0] == '~' && mid[3][1] == '/')
        {
            char test_path[MAX_PATH_LENGTH];
            strcpy(test_path, home_dir);
            removeSubstring(mid[3], "~");
            strcat(test_path, mid[3]);
            snprintf(peek_path, sizeof(peek_path), "%s", test_path);
        }
        else
        {
            char test_path[MAX_PATH_LENGTH];
            strcpy(test_path, current_dir);
            strcat(test_path, "/");
            strcat(test_path, mid[3]);

            snprintf(peek_path, sizeof(peek_path), "%s", test_path);
        }
    }
    peek_command(peek_path, show_all, show_details);
}

