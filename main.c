#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#define BUFFER_SIZE 1024
/**
 * 
*/
void execute_command(char *args[]);

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while (1) {
        printf("($) ");
        fflush(stdout);

        // Read input line
        read = getline(&line, &len, stdin);
        if (read == -1) {
            break;
        }

        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';

        // Tokenize input
        char *args[BUFFER_SIZE];
        int arg_count = 0;

        char *token = strtok(line, " ");
        while (token != NULL) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        // Execute command
        execute_command(args);
    }

    // Free allocated memory
    free(line);

    return 0;
}


void execute_command(char *args[]) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execve(args[0], args, NULL) == -1) {
            perror("Execution error");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        perror("Fork error");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}