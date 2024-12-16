#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define CS_RL_BUFFER_SIZE 1024;
#define CS_TOKEN_BUFFER_SIZE 64
#define CS_TOKEN_DELIM " \t\r\n\a"

void process_loop();

char *cs_readline();

char **cs_split_lines(char *line);

void check_buffer (const char *buffer);

        int main(int argc, char **argv)
{
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
        printf("C$ > ");
        line = cs_readline();
        args = cs_split_lines(line);
//        status = cs_execute(args)

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
    char **tokens = malloc(buffsize * sizeof(char*));
    char *token;

    check_buffer(*tokens);

    token = strtok(line,CS_TOKEN_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position > buffsize) {
            buffsize += CS_TOKEN_BUFFER_SIZE;
            tokens = realloc(tokens, buffsize * sizeof(char*));
            check_buffer(*tokens);
        }

        token = strtok(NULL, CS_TOKEN_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}


/* Straight forward approach using stdin.h provided `readline()`
char *cs_readline() {

}*/
