#include "pastevents.h"

void handle_past_events(char *command, char *current_dir, char *home_dir, char *prev_dir, char *arr[15])
{
    if (strcmp(command, "pastevents") == 0)
    {
        for (int i = 0; i < past_count; i++)
        {
            if(strcmp(arr[i],"/0")!=0)
            {
                printf("%s\n", arr[i]);
            }
        }
    }

    else if (strcmp(command, "pastevents purge") == 0)
    {
        for (int i = 0; i < past_count; i++)
        {
            free(arr[i]);
            arr[i] = NULL;
        }
        for (int i = 0; i < 15; i++)
        {
            arr[i] = (char *)malloc(256);
        }

        past_count = 0;
    }

    else
    {
        char limit[] = " ";

        int need = 0;
        char *yes;
        char *mid[3];

        yes = strtok(command, limit);

        while (yes != NULL)
        {
            mid[need] = yes;
            need++;
            yes = strtok(NULL, limit);
        }

        if (need == 3)
        {
            int x = atoi(mid[2]);

            char pleb[MAX_PATH_LENGTH];

            strcpy(pleb, arr[past_count - x]);

            if (past_count == 15)
            {
                if (strcmp(pleb, arr[14]) != 0)
                {
                    for (int i = 1; i < 15; i++)
                    {
                        strcpy(arr[i - 1], arr[i]);
                    }
                    strcpy(arr[14], pleb);
                }
            }
            else
            {
                if (past_count > 0)
                {
                    if (strcmp(pleb, arr[past_count - 1]) != 0)
                    {
                        strcpy(arr[past_count], pleb);
                        past_count++;
                    }
                }
                else
                {
                    strcpy(arr[past_count], pleb);
                    past_count++;
                }
            }

            char *delimiter1 = " ; ";
            char *ptr = pleb;

            char *token;

            int max_tokens = 100;
            char *tokens[max_tokens];
            int token_count = 0;

            while ((token = strstr(ptr, delimiter1)) != NULL && token_count < max_tokens)
            {
                *token = '\0';
                tokens[token_count] = ptr;
                token_count++;
                ptr = token + strlen(delimiter1);
            }
            tokens[token_count] = ptr;
            token_count++;

            for (int i = 0; i < token_count; i++)
            {
                double time_t;
                struct timeval start, end;
                if (strncmp(tokens[i], "peek", 4) == 0)
                {
                    gettimeofday(&start, NULL);
                    handle_peek_command(tokens[i], current_dir, home_dir, prev_dir);
                    gettimeofday(&end, NULL);
                    time_t = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
                }
                else if (strncmp(tokens[i], "warp", 4) == 0)
                {
                    gettimeofday(&start, NULL);
                    handle_warp_command(tokens[i], current_dir, home_dir, prev_dir);
                    gettimeofday(&end, NULL);
                    time_t = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
                }

                else if (strncmp(tokens[i], "proclore", 8) == 0)
                {
                    gettimeofday(&start, NULL);
                    int check_count = 0;

                    char *token_space = strtok(tokens[i], " ");

                    char *new[2];
                    new[0] = (char *)malloc(MAX_PATH_LENGTH);
                    new[1] = (char *)malloc(MAX_PATH_LENGTH);

                    while (token_space != NULL)
                    {
                        strcpy(new[check_count], token_space);
                        check_count++;
                        token_space = strtok(NULL, " ");
                    }

                    if (check_count == 1)
                    {
                        pid_t pid;
                        pid = getpid();
                        print_process_info(pid, home_dir);
                    }

                    else if (check_count == 2)
                    {
                        int b = atoi(new[1]);

                        print_process_info(b, home_dir);
                    }

                    else
                    {
                        printf("\033[31m");
                        printf("Invalid Command\n");
                        printf("\033[0m");
                    }

                    gettimeofday(&end, NULL);
                    time_t = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
                    free(new[0]);
                    free(new[1]);
                }
                else if (strncmp(tokens[i], "seek", 4) == 0)
                {
                    handle_seek(tokens[i], current_dir, home_dir, prev_dir);
                }

                else
                {
                    chdir(current_dir);
                    gettimeofday(&start, NULL);
                    executeSystemCommand1(tokens[i]);
                    gettimeofday(&end, NULL);
                    time_t = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
                    chdir(home_dir);
                }

                if (time_t > 2)
                {
                    char *check1 = strtok(tokens[i], " ");
                    printf("%s for %0.0f seconds\n", check1, time_t);
                }
            }
        }
    }
}
