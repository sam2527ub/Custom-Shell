#include "rem_fun.h"

void remove_non_alphabet_prefix(char *str)
{
    int first_alpha_pos = -1;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (isalpha((unsigned char)str[i]))
        {
            first_alpha_pos = i;
            break;
        }
    }

    if (first_alpha_pos != -1)
    {
        memmove(str, str + first_alpha_pos, strlen(str + first_alpha_pos) + 1);
    }
    else
    {
        str[0] = '\0'; 
    }
}

void removeFirstCharacter(char *str)
{
    if (str == NULL || *str == '\0')
    {
        return;
    }
    
    int len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        str[i] = str[i + 1];
    }
}

void removeSubstring(char *str, const char *sub)
{
    char *match;
    int len = strlen(sub);

    while ((match = strstr(str, sub)) != NULL)
    {
        memmove(match, match + len, strlen(match + len) + 1);
    }
}
void subtractFromRight(char *str, char character)
{
    char *lastOccurrence = strrchr(str, character);
    if (lastOccurrence != NULL)
    {
        *lastOccurrence = '\0'; 
    }
}

void reduceContinuousSpaces(char *input)
{
    char prevChar = '\0';
    int inputIndex = 0;
    int outputIndex = 0;

    while (input[inputIndex] != '\0')
    {
        if (!(prevChar == ' ' && input[inputIndex] == ' '))
        {
            input[outputIndex] = input[inputIndex];
            prevChar = input[inputIndex];
            outputIndex++;
        }
        inputIndex++;
    }
    input[outputIndex] = '\0';
}

int isDirectory(const char *path)
{
    struct stat statbuf;

    if (stat(path, &statbuf) != 0)
    {
        return 0;
    }

    return S_ISDIR(statbuf.st_mode);
}

int compare_strings(const void *a, const void *b)
{
    char *str_a = *(char **)a;
    char *str_b = *(char **)b;

    char copy_1[200];
    char copy_2[200];

    strcpy(copy_1, str_a);
    strcpy(copy_2, str_b);

    int hidden_a = (strcmp(".", str_a) == 0 || strcmp("..", str_a) == 0);
    int hidden_b = (strcmp(".", str_b) == 0 || strcmp("..", str_b) == 0);

    if (hidden_a && !hidden_b)
    {
        return -1; 
    }
    else if (!hidden_a && hidden_b)
    {
        return 1; 
    }

    else if (hidden_a && hidden_b)
    {
        if (strcmp(".", str_a) == 0 && strcmp("..", str_b) == 0)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }

    else
    {
        remove_non_alphabet_prefix(copy_1);
        remove_non_alphabet_prefix(copy_2);

        char lower_a[MAX_PATH_LENGTH];
        char lower_b[MAX_PATH_LENGTH];
        for (int i = 0; copy_1[i]; i++)
        {
            lower_a[i] = tolower(copy_1[i]);
        }
        lower_a[strlen(copy_1)] = '\0';

        for (int i = 0; copy_2[i]; i++)
        {
            lower_b[i] = tolower(copy_2[i]);
        }
        lower_b[strlen(copy_2)] = '\0';

        return strcmp(lower_a, lower_b);
    }
}

void print_colored(const char *str, const char *color_code)
{
    printf("%s%s%s\n", color_code, str, "\033[0m");
}


void executeSystemCommand(const char *command) {
    char command_copy[MAX_PATH_LENGTH];
    strncpy(command_copy, command, sizeof(command_copy));
    command_copy[sizeof(command_copy) - 1] = '\0';  

    char *command_args[256];
    char yes[MAX_PATH_LENGTH];
    int count=0;

    for(int i=0;i<256;i++)
    {
        command_args[i]=(char*)malloc(128*sizeof(char));
    }

    char* token=strtok(command_copy," ");

    while(token!=NULL)
    {
        strcpy(command_args[count],token);

        if(count==0)
        {
            strcpy(yes,token);
        }

        token=strtok(NULL, " ");
        count++;
    }
    command_args[count]=NULL;

    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        return;
    }

    if (child_pid == 0) {
        execvp(yes, command_args);

        if (execvp(command, command_args) == -1) 
        {
              printf("\033[31m");
            printf("ERROR: '%s' is not a valid command\n", command_copy);
            printf("\033[0m");
            exit(1);
        }

    } else {
        int status;
        waitpid(child_pid, &status, 0);

        if (!WIFEXITED(status)) {
            printf("Child process did not exit normally.\n");
        }
    }
}

void executeSystemCommand1(const char *command) {
    char command_copy[MAX_PATH_LENGTH];
    strncpy(command_copy, command, sizeof(command_copy));
    command_copy[sizeof(command_copy) - 1] = '\0';  

    char *command_args[256];
    char yes[MAX_PATH_LENGTH];
    int count=0;

    for(int i=0;i<256;i++)
    {
        command_args[i]=(char*)malloc(128*sizeof(char));
    }

    char* token=strtok(command_copy," ");

    while(token!=NULL)
    {
        strcpy(command_args[count],token);

        if(count==0)
        {
            char finale[MAX_PATH_LENGTH];
            strcpy(finale,"/bin/");
            strcat(finale,token);
            strcpy(yes,finale);
        }

        token=strtok(NULL, " ");
        count++;
    }
    command_args[count]=NULL;

    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        return;
    }

    if (child_pid == 0) {
        execv(yes, command_args);

        if (execv(command, command_args) == -1) 
        {
              printf("\033[31m");
            printf("ERROR: '%s' is not a valid command\n", command_copy);
            printf("\033[0m");
            exit(1);
        }

    } else {
        int status;
        waitpid(child_pid, &status, 0);

        if (!WIFEXITED(status)) {
            printf("Child process did not exit normally.\n");
        }
    }
}


void childHandler(int signal)
{
    int status;
    int pid;
    chdir(home_dir);
    FILE *output_file = fopen("output.txt", "a");
    FILE *file = fopen("output1.txt", "r");
    FILE *file1 = fopen("output2.txt", "w");
    fseek(file, 0, SEEK_SET);
    fseek(file1, 0, SEEK_SET);

    char buffer[1024];
    char copy[1024];
    char temp[1024];
    char del[] = " ";
    char *token;
    int count = 0;
    int flag = 0;

    if (output_file == NULL)
    {
        perror("Failed to open output file");
        return;
    }
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (WIFEXITED(status))
        {
            while (fgets(buffer, sizeof(buffer), file) != NULL)
            {
                flag = 0;
                count = 0;
                strcpy(copy, buffer);
                char *token = strtok(buffer, del);
                while (token != NULL)
                {
                    if (count == 0)
                    {
                        if (atoi(token) == pid)
                        {
                            flag = 1;
                        }
                    }
                    if (count == 1)
                    {
                        if (flag == 1)
                        {
                            strcpy(temp, token);
                        }
                        break;
                    }
                    token = strtok(NULL, del);
                    count++;
                }

                if (flag == 0)
                {
                    fprintf(file1, "%s\n", copy);
                }
            }
            fclose(file1);
            fclose(file);
            file = fopen("output1.txt", "w");
            file1 = fopen("output2.txt", "r");
            fseek(file, 0, SEEK_SET);
            fseek(file1, 0, SEEK_SET);

            while (fgets(buffer, sizeof(buffer), file1) != NULL)
            {
                fprintf(file, "%s\n", buffer);
            }

            fclose(file);
            fclose(file1);

            file1 = fopen("output2.txt", "w");

            fclose(file1);

            fprintf(output_file, "%s with pid %d exited normally\n", temp, pid);
        }
        else
        {
            fprintf(output_file, "Background process with PID %d did not exit normally.\n", pid);
        }
    }

    fclose(output_file);
}

void exitHandler()
{
    chdir(home_dir);
    FILE *f = fopen("mid.txt", "w");
    int count = 0;
    int val = -1;

    for (int i = 0; i < past_count; i++)
    {
        if (strcmp(arr[i],"\n")!=0 && strcmp(arr[i],"\0")!=0)
        {
            fprintf(f, "%s\n", arr[i]);
            free(arr[i]);
            count++;
            val = i;
        }
    }
    fprintf(f, "%s\n", "exit");
    past_count++;
    FILE *file = fopen("bas.txt", "w");
    fprintf(file, "%d\n", past_count);
    fclose(file);
    fclose(f);
    exit(0);
}

void sigintHandler(int sig)
{
    exitHandler();
}
void removeLastNewline(char *str)
{
    int length = strlen(str);

    if (length > 0 && str[length - 1] == '\n')
    {
        str[length - 1] = '\0'; 
    }
}

bool containsCharacter(const char *str, char target)
{
    while (*str != '\0')
    {
        if (*str == target)
        {
            return true;
        }
        str++; 
    }
    
    return false;
}
