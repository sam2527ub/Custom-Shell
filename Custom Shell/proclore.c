#include "proclore.h"

void print_permissions(mode_t mode)
{
    putchar((S_ISDIR(mode)) ? 'd' : '-');
    putchar((mode & S_IRUSR) ? 'r' : '-');
    putchar((mode & S_IWUSR) ? 'w' : '-');
    putchar((mode & S_IXUSR) ? 'x' : '-');
    putchar((mode & S_IRGRP) ? 'r' : '-');
    putchar((mode & S_IWGRP) ? 'w' : '-');
    putchar((mode & S_IXGRP) ? 'x' : '-');
    putchar((mode & S_IROTH) ? 'r' : '-');
    putchar((mode & S_IWOTH) ? 'w' : '-');
    putchar((mode & S_IXOTH) ? 'x' : '-');
}

void print_process_info(pid_t pid, char *home_dir)
{
    char status;
    char process_status[10];
    pid_t process_group;
    unsigned int virtual_memory;
    char executable_path[1024];

    FILE *status_file = fopen("/proc/self/status", "r");
    if (status_file == NULL)
    {
        printf("\033[31m");

        perror("Error opening status file");

        printf("\033[0m");
        return;
    }
    fscanf(status_file, "State: %c", &status);
    char line1[MAX_PATH_LENGTH];
    char final_line[MAX_PATH_LENGTH];

    while (fgets(line1, sizeof(line1), status_file))
    {
        if (strncmp(line1, "State:", 6) == 0)
        {
            strcpy(final_line, line1 + 7);
        }
    }
    char *token = strtok(final_line, " ");

    fclose(status_file);

    FILE *stat_file = fopen("/proc/self/stat", "r");
    if (stat_file == NULL)
    {
        printf("\033[31m");

        perror("Error opening status file");
        
        printf("\033[0m");
        return;
    }
    fscanf(stat_file, "%*d %*s %*c %d", &process_group);
    fclose(stat_file);

    char status_path[64];
    snprintf(status_path, sizeof(status_path), "/proc/%d/status", pid);
    FILE *vm_file = fopen(status_path, "r");
    if (vm_file == NULL)
    {
        printf("\033[31m");
        
        perror("Error opening status file");

        printf("\033[0m");
        return;
    }
    while (fgets(executable_path, sizeof(executable_path), vm_file))
    {
        if (sscanf(executable_path, "VmSize: %u", &virtual_memory) == 1)
        {
            break;
        }
    }
    fclose(vm_file);

    char exe_path[1024];
    snprintf(exe_path, sizeof(exe_path), "/proc/%d/exe", pid);
    ssize_t path_size = readlink(exe_path, executable_path, sizeof(executable_path) - 1);
    if (path_size != -1)
    {
        executable_path[path_size] = '\0';
    }
    else
    {
        strcpy(executable_path, "Unknown");
    }
    char line[MAX_PATH_LENGTH];

    if (strlen(executable_path) < strlen(home_dir))
    {
        snprintf(line, sizeof(line), "%s", executable_path);
    }
    else
    {
        char new_dir[MAX_PATH_LENGTH];
        strcpy(new_dir, executable_path);

        removeSubstring(new_dir, home_dir);

        char yo[MAX_PATH_LENGTH + 1] = "~";

        if (strlen(new_dir) > 0)
        {
            strcat(yo, new_dir);
        }

        strcpy(line, yo);
    }

    int plus_check=tcgetpgrp(STDIN_FILENO);

    printf("pid : %d\n", pid);
    if(plus_check)
    {
        strcat(token,"+");
        printf("process status : %s\n", token);
    }
    else
    {
        printf("process status : %s\n", token);
    }
    printf("Process Group : %d\n", process_group);
    printf("Virtual memory : %u\n", virtual_memory);
    printf("executable path : %s\n", line);
}
