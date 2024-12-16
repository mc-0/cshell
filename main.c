#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "consts.h"


#define CS_RL_BUFFER_SIZE 1024;
#define CS_TOKEN_BUFFER_SIZE 64
#define CS_TOKEN_DELIM " \t\r\n\a"

void process_loop();

char *cs_readline();

char **cs_split_lines(char *line);

void check_buffer(const char *buffer);

int cs_launch(char **args);
int cs_execute(char **args);

int cs_cd(char **args);
int cs_help(char **args);
int cs_exit(char **args);

char *builtin_str[] = {"cd", "help", "exit"};
int (*builtin_func[]) (char **) = {
        &cs_cd,
        &cs_help,
        &cs_exit
};

int cs_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}


int main(int argc, char **argv) {
    // Load config files, if any.

    // Run command loop.
    process_loop();

    // Perform any shutdown/cleanup.

    return EXIT_SUCCESS;
}

void process_loop() {
    char *line;
    char **args;
    int status;

    do {
        printf("₵ > ");
        line = cs_readline();
        args = cs_split_lines(line);
        status = cs_execute(args);

        free(line);
        free(args);
    } while (status);


}

/*
 * the `readline()` function provided by stdio.h would make the below function much simpler.
 * an implementation using it can be seen at the end of this file
 */
char *cs_readline() {
    int buffsize = CS_RL_BUFFER_SIZE; // initial buffer size
    int position = 0;
    char *buffer = malloc(sizeof(char) * buffsize); // current string
    int c; // parsing ascii characters requires an int to detect EOF!

    check_buffer(buffer);

    while (1) {
        c = getchar(); // read character from std in

        // if the next character is EOF or a newline terminate string & return it
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        }
        else {
            buffer[position] = c;
        }
        position++;

        // If buffer size is exceeded -> reallocate more memory
        if (position >= buffsize) {
            buffsize += CS_RL_BUFFER_SIZE;
            buffer = realloc(buffer, buffsize);
            check_buffer(buffer);
            printf("!C$ ALERT! ::: !MEMORY BUFFER EXCEEDED! :::\n");
        }
    }
}

void check_buffer (const char *buffer) {
    if (!buffer) {
        fprintf(stderr,"C$: !Memory Allocation Error!");
        exit(EXIT_FAILURE);
    }
}

char **cs_split_lines(char *line) {
    int buffsize = CS_TOKEN_BUFFER_SIZE;
    int position = 0;
    char **tokens = malloc(buffsize * sizeof(char *));
    char *token;

    check_buffer((const char *) tokens);

    token = strtok(line, CS_TOKEN_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position > buffsize) {
            buffsize += CS_TOKEN_BUFFER_SIZE;
            tokens = realloc(tokens, buffsize * sizeof(char *));
            check_buffer(*tokens);
        }

        token = strtok(NULL, CS_TOKEN_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int cs_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("cs");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("cs");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int cs_execute(char **args) {
    int i;

    if (args[0] == NULL) {
        return 1; // Empty command entered
    }

    for (i = 0; i < cs_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return cs_launch(args);
}

int cs_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "cs: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("cs");
        }
    }
    return 1;
}

int cs_help(char **args)
{
    int i;
    printf("%s\n", msg);
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < cs_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int cs_exit(char **args)
{
    return 0;
}

/* Straight forward approach using stdin.h provided `readline()`
char *cs_readline() {

}*/
