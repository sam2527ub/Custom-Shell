#include "pastevents.c"
#include "peek.c"
#include "proclore.c"
#include "prompt.h"
#include "rem_fun.c"
#include "seek.c"
#include "warp.c"

int past_count=0;
char *arr[15];
char home_dir[MAX_PATH_LENGTH];

int main()
{
    signal(SIGINT, sigintHandler);
    char *username = getenv("USER");
    char systemname[256];

    for (int i = 0; i < 15; i++)
    {
        arr[i] = (char *)malloc(256);
    }

    FILE *file = fopen("mid.txt", "r");
    FILE *f = fopen("bas.txt", "r");
    char *buffer_mid = NULL;
    size_t len = 0;
    ssize_t read;

    int count_yes = 0;

    while ((read = getline(&buffer_mid, &len, f)) != -1)
    {
        removeLastNewline(buffer_mid);
        past_count = atoi(buffer_mid);
    }
    fclose(f);
    f = fopen("bas.txt", "w");
    fclose(f);

    if (past_count <= 15)
    {
        while ((read = getline(&buffer_mid, &len, file)) != -1)
        {
            removeLastNewline(buffer_mid);
            strcpy(arr[count_yes], buffer_mid);
            count_yes++;
        }
    }
    else if (past_count > 15)
    {
        while ((read = getline(&buffer_mid, &len, file)) != -1)
        {
            if (count_yes >= 1)
            {
                removeLastNewline(buffer_mid);
                strcpy(arr[count_yes], buffer_mid);
                past_count++;
            }
            count_yes++;
        }
        past_count = 15;
    }

    fclose(file);
    file = fopen("mid.txt", "w");
    fclose(file);

    gethostname(systemname, sizeof(systemname));

    char current_dir[MAX_PATH_LENGTH];
    char prev_dir[MAX_PATH_LENGTH];

    if (getcwd(home_dir, sizeof(home_dir)) == NULL)
    {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    strcpy(prev_dir, home_dir);
    strcpy(current_dir, home_dir);

    while (1)
    {

        print_prompt(username, systemname, current_dir, home_dir);

        char input[256];

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        reduceContinuousSpaces(input);

        char *delimiter1 = " ; ";
        char *saveptr1 = input;
        char pseudo[MAX_PATH_LENGTH];

        strcpy(pseudo, input);

        int flag = 0;
        char *token;

        int max_tokens = 100;
        char *tokens[max_tokens];
        int token_count = 0;

        if ((token = strstr(saveptr1, delimiter1)) != saveptr1)
        {
            while ((token = strstr(saveptr1, delimiter1)) != NULL && token_count < max_tokens)
            {
                *token = '\0';
                tokens[token_count] = saveptr1;
                if (strncmp(tokens[token_count], "pastevents", 10) == 0)
                {
                    flag = 1;
                }
                token_count++;
                saveptr1 = token + strlen(delimiter1);
            }
            tokens[token_count] = saveptr1;
            if (strncmp(tokens[token_count], "pastevents", 10) == 0)
            {
                flag = 1;
            }
            token_count++;
        }

        if (past_count == 15)
        {
            if (flag == 0 && strcmp(pseudo, arr[14]) != 0 && !containsCharacter(pseudo, '&'))
            {
                for (int i = 1; i < 15; i++)
                {
                    strcpy(arr[i - 1], arr[i]);
                }
                strcpy(arr[14], pseudo);
            }
        }
        else
        {
            if (past_count > 0)
            {
                if (strcmp(pseudo, arr[past_count - 1]) != 0 && flag == 0 && !containsCharacter(pseudo, '&'))
                {
                    strcpy(arr[past_count], pseudo);
                    past_count++;
                }
            }
            else
            {
                if (flag == 0 && !containsCharacter(pseudo, '&'))
                {
                    strcpy(arr[past_count], pseudo);
                    past_count++;
                }
            }
        }

        for (int i = 0; i < token_count; i++)
        {

            if (strchr(tokens[i], '&') != NULL)
            {
                struct sigaction sa;
                sa.sa_handler = childHandler;
                sigemptyset(&sa.sa_mask);
                sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
                sigaction(SIGCHLD, &sa, NULL);

                char *command_copy = strdup(tokens[i]);
                char *ampersand_pos = strchr(command_copy, '&');
                if (ampersand_pos != NULL)
                {
                    *ampersand_pos = '\0'; 
                }

                pid_t pid = fork(); 

                if (pid < 0)
                {
                    perror("Fork failed");
                    exit(EXIT_FAILURE);
                }
                else if (pid == 0)
                {
                    pid_t test = getpid();
                    FILE *file = fopen("output1.txt", "a");
                    fprintf(file, "%d %s\n", test, command_copy);
                    fclose(file);
                     chdir(current_dir);
                   executeSystemCommand(command_copy);
                   chdir(home_dir);
                   exit(EXIT_SUCCESS);
                }
                else
                {
                    double time_t;
                    struct timeval start, end;
                    printf("%d\n", pid);
                    char foreground_command[MAX_PATH_LENGTH];
                    snprintf(foreground_command, sizeof(foreground_command), "%s", ampersand_pos + 1);
                    removeFirstCharacter(foreground_command);

                    if (strncmp(foreground_command, "peek", 4) == 0)
                    {
                        gettimeofday(&start, NULL);
                        handle_peek_command(foreground_command, current_dir, home_dir, prev_dir);
                        gettimeofday(&end, NULL);
                        time_t = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
                    }
                    else if (strncmp(foreground_command, "warp", 4) == 0)
                    {
                        gettimeofday(&start, NULL);
                        handle_warp_command(foreground_command, current_dir, home_dir, prev_dir);
                        gettimeofday(&end, NULL);
                        time_t = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
                    }
                    else if (strncmp(foreground_command, "pastevents", 10) == 0)
                    {
                        gettimeofday(&start, NULL);
                        handle_past_events(foreground_command, current_dir, home_dir, prev_dir, arr);
                        gettimeofday(&end, NULL);
                        time_t = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
                    }
                    else if (strncmp(foreground_command, "proclore", 8) == 0)
                    {
                        gettimeofday(&start, NULL);
                        int check_count = 0;

                        char *token_space = strtok(foreground_command, " ");

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
                            printf("Invalid Command\n");
                        }

                        gettimeofday(&end, NULL);
                        time_t = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

                        free(new[0]);
                        free(new[1]);
                    }
                    else if (strncmp(foreground_command, "seek", 4) == 0)
                    {
                        gettimeofday(&start, NULL);
                        handle_seek(foreground_command, current_dir, home_dir, prev_dir);
                        gettimeofday(&end, NULL);
                        time_t = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
                    }
                    else
                    {
                        chdir(current_dir);
                        gettimeofday(&start, NULL);
                        executeSystemCommand1(foreground_command);
                        gettimeofday(&end, NULL);
                        time_t = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
                        chdir(home_dir);
                    }

                    if (time_t > 2)
                    {
                        char *check1 = strtok(foreground_command, " ");
                        printf("%s for %0.0f seconds\n", check1, time_t);
                    }
                }
            }
            else
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
                else if (strncmp(tokens[i], "pastevents", 10) == 0)
                {
                    gettimeofday(&start, NULL);
                    handle_past_events(tokens[i], current_dir, home_dir, prev_dir, arr);
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
                        printf("Invalid Command\n");
                    }

                    gettimeofday(&end, NULL);
                    time_t = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

                    free(new[0]);
                    free(new[1]);
                }
                else if (strncmp(tokens[i], "seek", 4) == 0)
                {
                    gettimeofday(&start, NULL);
                    handle_seek(tokens[i], current_dir, home_dir, prev_dir);
                    gettimeofday(&end, NULL);
                    time_t = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
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
        
        chdir(home_dir);

        FILE *file = fopen("output.txt", "r");
        if (file == NULL)
        {
            printf("\033[31m");
            perror("File opening failed");
            printf("\033[0m");
        }

        char buffer[1024];
        size_t bytes_read;

        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
        {
            fwrite(buffer, 1, bytes_read, stdout);
        }

        fclose(file);
        
        file = fopen("output.txt", "w");
        if (file == NULL)
        {
            printf("\033[31m");
            perror("File opening failed");
            printf("\033[0m");
        }
        fclose(file);
    }

    return 0;
}
