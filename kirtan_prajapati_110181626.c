#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

// SECTION STARTS: "CONSTANTS AND DEFINITIONS"
#define MAX_INPUT_SIZE 1024 // Maximum size of input line
#define MAX_ARGS 5          // Maximum arguments per command (including command name)
#define MAX_COMMANDS 6      // Maximum commands in a pipeline (5 pipes + 1)
#define MAX_SEQ_COMMANDS 4  // Maximum commands in sequential execution
// SECTION ENDS: "CONSTANTS AND DEFINITIONS"

// SECTION STARTS: "GLOBAL VARIABLES"
// Global variables to track all shell processes for killallterms command
pid_t current_pid;
// SECTION ENDS: "GLOBAL VARIABLES"

// SECTION STARTS: "FUNCTION PROTOTYPES"
// Function prototypes
void display_prompt();
int read_input(char *input, size_t size);
int parse_input(char *input, char ***commands, int *command_count, char *special_char);
void execute_command(char **args);
void execute_piped_commands(char ***commands, int command_count);
void execute_reverse_piped_commands(char ***commands, int command_count);
void execute_sequential_commands(char ***commands, int command_count);
void execute_conditional_commands(char ***commands, int command_count, char *operators);
void append_files(char *file1, char *file2);
void count_words(char *filename);
void concatenate_files(char **filenames, int count);
void cleanup_commands(char ***commands, int command_count);
int validate_args_count(char **args);
void handle_redirection(char **args, int *in_fd, int *out_fd);
void killterm_command();
void killallterms_command();
// SECTION ENDS: "FUNCTION PROTOTYPES"

// SECTION STARTS: "MAIN SHELL LOOP"
/**
 * Main function - Entry point of the shell program
 */
int main()
{
    char input[MAX_INPUT_SIZE];  // Buffer to store user input
    char ***commands = NULL;     // Array to store parsed commands
    int command_count = 0;       // Number of commands in input
    char special_char[10] = {0}; // Special character in the command

    // Store the current process ID
    current_pid = getpid();

    // Main shell loop
    while (1)
    {
        // Display shell prompt
        display_prompt();

        // Read user input
        if (read_input(input, MAX_INPUT_SIZE) == 0)
        {
            continue; // Empty input, show prompt again
        }

        // Allocate memory for commands array
        commands = (char ***)malloc(MAX_COMMANDS * sizeof(char **));
        if (!commands)
        {
            perror("Memory allocation failed");
            continue;
        }

        // Parse the user input
        if (parse_input(input, commands, &command_count, special_char) != 0)
        {
            // Parsing error occurred
            cleanup_commands(commands, command_count);
            free(commands);
            continue;
        }

        // Handle built-in commands
        if (command_count == 1 && commands[0][0] != NULL)
        {
            if (strcmp(commands[0][0], "killterm") == 0)
            {
                killterm_command();
                cleanup_commands(commands, command_count);
                free(commands);
                continue;
            }
            else if (strcmp(commands[0][0], "killallterms") == 0)
            {
                killallterms_command();
                cleanup_commands(commands, command_count);
                free(commands);
                continue;
            }
        }

        // Execute commands based on special character
        if (special_char[0] == '|')
        {
            // Forward piping
            execute_piped_commands(commands, command_count);
        }
        else if (special_char[0] == '=')
        {
            // Reverse piping
            execute_reverse_piped_commands(commands, command_count);
        }
        else if (special_char[0] == '~' && command_count == 2)
        {
            // Append files
            append_files(commands[0][0], commands[1][0]);
        }
        else if (special_char[0] == '#' && command_count == 1)
        {
            // Count words
            count_words(commands[0][0]);
        }
        else if (special_char[0] == '+')
        {
            // Concatenate files
            // Extract file names from commands for concatenation
            char *files[MAX_COMMANDS];
            for (int i = 0; i < command_count; i++)
            {
                files[i] = commands[i][0];
            }
            concatenate_files(files, command_count);
        }
        else if (special_char[0] == ';')
        {
            // Sequential execution
            execute_sequential_commands(commands, command_count);
        }
        else if (special_char[0] == '&' || special_char[0] == '|')
        {
            // Conditional execution with && and ||
            execute_conditional_commands(commands, command_count, special_char);
        }
        else if (command_count == 1)
        {
            // Single command execution
            execute_command(commands[0]);
        }
        else
        {
            printf("w25shell: Unsupported command or operation\n");
        }

        // Clean up allocated memory
        cleanup_commands(commands, command_count);
        free(commands);
    }

    return 0;
}
// SECTION ENDS: "MAIN SHELL LOOP"

// SECTION STARTS: "SHELL INTERFACE"
/**
 * Function to display the shell prompt
 */
void display_prompt()
{
    printf("w25shell$ ");
    fflush(stdout);
}

/**
 * Function to read user input
 *
 * @param input Buffer to store the input
 * @param size Size of the input buffer
 * @return Length of input read, or 0 for empty input
 */
int read_input(char *input, size_t size)
{
    if (fgets(input, size, stdin) == NULL)
    {
        // Handle EOF (Ctrl+D)
        printf("\n");
        exit(0);
    }

    // Remove newline character
    size_t length = strlen(input);
    if (length > 0 && input[length - 1] == '\n')
    {
        input[length - 1] = '\0';
        length--;
    }

    return length;
}
// SECTION ENDS: "SHELL INTERFACE"

// SECTION STARTS: "COMMAND PARSING"
/**
 * Function to parse the input and identify special characters
 *
 * @param input The user input string
 * @param commands Array to store parsed commands
 * @param command_count Pointer to store number of commands
 * @param special_char Pointer to store identified special character
 * @return 0 on success, non-zero on error
 */
int parse_input(char *input, char ***commands, int *command_count, char *special_char)
{
    char *token;
    char *saveptr1, *saveptr2;
    char *input_copy = strdup(input);

    if (!input_copy)
    {
        perror("Memory allocation failed");
        return 1;
    }

    // Check for special characters
    if (strstr(input, " | "))
    {
        strcpy(special_char, "|");
        token = strtok_r(input_copy, "|", &saveptr1);
    }
    else if (strstr(input, " = "))
    {
        strcpy(special_char, "=");
        token = strtok_r(input_copy, "=", &saveptr1);
    }
    else if (strstr(input, " ~ "))
    {
        strcpy(special_char, "~");
        token = strtok_r(input_copy, "~", &saveptr1);
    }
    else if (strstr(input, " # "))
    {
        strcpy(special_char, "#");
        token = strtok_r(input_copy, "#", &saveptr1);
    }
    else if (strstr(input, " + "))
    {
        strcpy(special_char, "+");
        token = strtok_r(input_copy, "+", &saveptr1);
    }
    else if (strstr(input, " ; "))
    {
        strcpy(special_char, ";");
        token = strtok_r(input_copy, ";", &saveptr1);
    }
    else if (strstr(input, " && "))
    {
        strcpy(special_char, "&&");
        token = strtok_r(input_copy, "&&", &saveptr1);
    }
    else if (strstr(input, " || "))
    {
        strcpy(special_char, "||");
        token = strtok_r(input_copy, "||", &saveptr1);
    }
    else
    {
        // No special character
        special_char[0] = '\0';
        token = input_copy;
    }

    *command_count = 0;

    // Parse the first command
    while (token != NULL && *command_count < MAX_COMMANDS)
    {
        // Remove leading and trailing whitespace
        while (*token == ' ')
            token++;

        char *cmd_copy = strdup(token);
        if (!cmd_copy)
        {
            perror("Memory allocation failed");
            free(input_copy);
            return 1;
        }

        // Allocate memory for command arguments
        commands[*command_count] = (char **)malloc(MAX_ARGS * sizeof(char *));
        if (!commands[*command_count])
        {
            perror("Memory allocation failed");
            free(cmd_copy);
            free(input_copy);
            return 1;
        }

        // Initialize all argument pointers to NULL
        for (int i = 0; i < MAX_ARGS; i++)
        {
            commands[*command_count][i] = NULL;
        }

        // Parse command arguments
        int arg_count = 0;
        char *arg = strtok_r(cmd_copy, " ", &saveptr2);

        while (arg != NULL && arg_count < MAX_ARGS)
        {
            commands[*command_count][arg_count] = strdup(arg);
            arg_count++;
            arg = strtok_r(NULL, " ", &saveptr2);
        }

        // Validate argument count (must be between 1 and 5)
        if (arg_count < 1 || arg_count > MAX_ARGS)
        {
            printf("w25shell: Invalid number of arguments for command\n");
            free(cmd_copy);
            free(input_copy);
            return 1;
        }

        // If more arguments exist but we've reached MAX_ARGS, report error
        if (arg != NULL)
        {
            printf("w25shell: Too many arguments (max %d allowed)\n", MAX_ARGS);
            free(cmd_copy);
            free(input_copy);
            return 1;
        }

        free(cmd_copy);
        (*command_count)++;

        // Get next command if special character exists
        if (special_char[0] != '\0')
        {
            token = strtok_r(NULL, special_char, &saveptr1);
            // Remove leading whitespace from token
            if (token != NULL)
            {
                while (*token == ' ')
                    token++;
            }
        }
        else
        {
            token = NULL;
        }
    }

    free(input_copy);
    return 0;
}
// SECTION ENDS: "COMMAND PARSING"

// SECTION STARTS: "BASIC COMMAND EXECUTION"
/**
 * Function to execute a single command
 *
 * @param args Command and its arguments
 */
void execute_command(char **args)
{
    // Validate argument count
    if (!validate_args_count(args))
    {
        return;
    }

    int in_fd = STDIN_FILENO;
    int out_fd = STDOUT_FILENO;

    // Check for redirection in the command
    handle_redirection(args, &in_fd, &out_fd);

    // Create a child process
    pid_t pid = fork();

    if (pid < 0)
    {
        // Fork failed
        perror("fork failed");
        return;
    }
    else if (pid == 0)
    {
        // Child process

        // Handle input/output redirection
        if (in_fd != STDIN_FILENO)
        {
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }

        if (out_fd != STDOUT_FILENO)
        {
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }

        // Execute the command
        execvp(args[0], args);

        // If execvp returns, it means an error occurred
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process

        // Close any open file descriptors
        if (in_fd != STDIN_FILENO)
            close(in_fd);
        if (out_fd != STDOUT_FILENO)
            close(out_fd);

        // Wait for the child process to complete
        waitpid(pid, NULL, 0);
    }
}
// SECTION ENDS: "BASIC COMMAND EXECUTION"

// SECTION STARTS: "FORWARD PIPING"
/**
 * Function to execute piped commands
 *
 * @param commands Array of commands and their arguments
 * @param command_count Number of commands
 */
void execute_piped_commands(char ***commands, int command_count)
{
    int i;
    int pipefd[2 * (command_count - 1)];
    pid_t pid;

    // Create all required pipes
    for (i = 0; i < command_count - 1; i++)
    {
        if (pipe(pipefd + 2 * i) < 0)
        {
            perror("pipe failed");
            return;
        }
    }

    // Execute each command in the pipeline
    for (i = 0; i < command_count; i++)
    {
        // Validate argument count for current command
        if (!validate_args_count(commands[i]))
        {
            // Clean up pipes
            for (int j = 0; j < 2 * (command_count - 1); j++)
            {
                close(pipefd[j]);
            }
            return;
        }

        pid = fork();

        if (pid < 0)
        {
            perror("fork failed");
            return;
        }
        else if (pid == 0)
        {
            // Child process

            // Set up input (read from previous pipe)
            if (i > 0)
            {
                dup2(pipefd[(i - 1) * 2], STDIN_FILENO);
            }

            // Set up output (write to next pipe)
            if (i < command_count - 1)
            {
                dup2(pipefd[i * 2 + 1], STDOUT_FILENO);
            }

            // Close all pipe file descriptors
            for (int j = 0; j < 2 * (command_count - 1); j++)
            {
                close(pipefd[j]);
            }

            // Execute command
            execvp(commands[i][0], commands[i]);

            // If execvp returns, it means an error occurred
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    }

    // Parent process closes all pipe file descriptors
    for (i = 0; i < 2 * (command_count - 1); i++)
    {
        close(pipefd[i]);
    }

    // Wait for all children to complete
    for (i = 0; i < command_count; i++)
    {
        wait(NULL);
    }
}
// SECTION ENDS: "FORWARD PIPING"

// SECTION STARTS: "REVERSE PIPING"
/**
 * Function to execute reverse piped commands
 *
 * @param commands Array of commands and their arguments
 * @param command_count Number of commands
 */
void execute_reverse_piped_commands(char ***commands, int command_count)
{
    int i;
    int pipefd[2 * (command_count - 1)];
    pid_t pid;

    // Create all required pipes
    for (i = 0; i < command_count - 1; i++)
    {
        if (pipe(pipefd + 2 * i) < 0)
        {
            perror("pipe failed");
            return;
        }
    }

    // Execute commands in reverse order
    for (i = command_count - 1; i >= 0; i--)
    {
        // Validate argument count for current command
        if (!validate_args_count(commands[i]))
        {
            // Clean up pipes
            for (int j = 0; j < 2 * (command_count - 1); j++)
            {
                close(pipefd[j]);
            }
            return;
        }

        pid = fork();

        if (pid < 0)
        {
            perror("fork failed");
            return;
        }
        else if (pid == 0)
        {
            // Child process

            // Set up input (read from previous pipe)
            if (i < command_count - 1)
            {
                dup2(pipefd[i * 2], STDIN_FILENO);
            }

            // Set up output (write to next pipe)
            if (i > 0)
            {
                dup2(pipefd[(i - 1) * 2 + 1], STDOUT_FILENO);
            }

            // Close all pipe file descriptors
            for (int j = 0; j < 2 * (command_count - 1); j++)
            {
                close(pipefd[j]);
            }

            // Execute command
            execvp(commands[i][0], commands[i]);

            // If execvp returns, it means an error occurred
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    }

    // Parent process closes all pipe file descriptors
    for (i = 0; i < 2 * (command_count - 1); i++)
    {
        close(pipefd[i]);
    }

    // Wait for all children to complete
    for (i = 0; i < command_count; i++)
    {
        wait(NULL);
    }
}
// SECTION ENDS: "REVERSE PIPING"

// SECTION STARTS: "SEQUENTIAL EXECUTION"
/**
 * Function to execute sequential commands (;)
 *
 * @param commands Array of commands and their arguments
 * @param command_count Number of commands
 */
void execute_sequential_commands(char ***commands, int command_count)
{
    // Execute each command sequentially
    for (int i = 0; i < command_count; i++)
    {
        execute_command(commands[i]);
    }
}
// SECTION ENDS: "SEQUENTIAL EXECUTION"

// SECTION STARTS: "CONDITIONAL EXECUTION"
/**
 * Function to execute conditional commands (&& and ||)
 *
 * @param commands Array of commands and their arguments
 * @param command_count Number of commands
 * @param operators String containing operator types (&&, ||)
 */
void execute_conditional_commands(char ***commands, int command_count, char *operators)
{
    // Execute first command
    int status = 0;
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return;
    }
    else if (pid == 0)
    {
        // Child process
        execvp(commands[0][0], commands[0]);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        waitpid(pid, &status, 0);
    }

    // Execute remaining commands based on previous results
    for (int i = 1; i < command_count; i++)
    {
        int execute = 0;

        // Determine whether to execute current command based on operator
        if (operators[i - 1] == '&' && WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            // Execute if previous command succeeded (&&)
            execute = 1;
        }
        else if (operators[i - 1] == '|' && (!WIFEXITED(status) || WEXITSTATUS(status) != 0))
        {
            // Execute if previous command failed (||)
            execute = 1;
        }

        if (execute)
        {
            pid = fork();

            if (pid < 0)
            {
                perror("fork failed");
                return;
            }
            else if (pid == 0)
            {
                // Child process
                execvp(commands[i][0], commands[i]);
                perror("execvp failed");
                exit(EXIT_FAILURE);
            }
            else
            {
                // Parent process
                waitpid(pid, &status, 0);
            }
        }
    }
}
// SECTION ENDS: "CONDITIONAL EXECUTION"

// SECTION STARTS: "FILE OPERATIONS - APPEND"
/**
 * Function to append contents between two text files
 *
 * @param file1 First file
 * @param file2 Second file
 */
void append_files(char *file1, char *file2)
{
    FILE *f1, *f2;
    char buffer[4096];
    size_t bytes_read;

    // Read contents of file2
    f2 = fopen(file2, "r");
    if (!f2)
    {
        perror("Failed to open second file");
        return;
    }

    // Append contents of file2 to file1
    f1 = fopen(file1, "a");
    if (!f1)
    {
        perror("Failed to open first file");
        fclose(f2);
        return;
    }

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f2)) > 0)
    {
        fwrite(buffer, 1, bytes_read, f1);
    }

    fclose(f1);
    fclose(f2);

    // Read contents of file1
    f1 = fopen(file1, "r");
    if (!f1)
    {
        perror("Failed to open first file");
        return;
    }

    // Append contents of file1 to file2
    f2 = fopen(file2, "a");
    if (!f2)
    {
        perror("Failed to open second file");
        fclose(f1);
        return;
    }

    rewind(f1); // Go back to beginning of file1

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f1)) > 0)
    {
        fwrite(buffer, 1, bytes_read, f2);
    }

    fclose(f1);
    fclose(f2);

    printf("Files appended successfully\n");
}
// SECTION ENDS: "FILE OPERATIONS - APPEND"

// SECTION STARTS: "FILE OPERATIONS - COUNT WORDS"
/**
 * Function to count words in a text file
 *
 * @param filename Name of the file
 */
void count_words(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file");
        return;
    }

    int word_count = 0;
    int in_word = 0;
    int c;

    // Count words
    while ((c = fgetc(file)) != EOF)
    {
        if (c == ' ' || c == '\n' || c == '\t')
        {
            in_word = 0;
        }
        else if (in_word == 0)
        {
            in_word = 1;
            word_count++;
        }
    }

    fclose(file);
    printf("Number of words in %s: %d\n", filename, word_count);
}
// SECTION ENDS: "FILE OPERATIONS - COUNT WORDS"

// SECTION STARTS: "FILE OPERATIONS - CONCATENATE"
/**
 * Function to concatenate multiple text files
 *
 * @param filenames Array of filenames
 * @param count Number of files
 */
void concatenate_files(char **filenames, int count)
{
    FILE *file;
    char buffer[4096];
    size_t bytes_read;

    // Process each file
    for (int i = 0; i < count; i++)
    {
        file = fopen(filenames[i], "r");
        if (!file)
        {
            fprintf(stderr, "Failed to open file %s: %s\n", filenames[i], strerror(errno));
            continue;
        }

        // Read and output file contents
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
        {
            fwrite(buffer, 1, bytes_read, stdout);
        }

        fclose(file);
    }
}
// SECTION ENDS: "FILE OPERATIONS - CONCATENATE"

// SECTION STARTS: "I/O REDIRECTION"
/**
 * Function to handle input/output redirection
 *
 * @param args Command and arguments array
 * @param in_fd Pointer to input file descriptor
 * @param out_fd Pointer to output file descriptor
 */
void handle_redirection(char **args, int *in_fd, int *out_fd)
{
    int i;

    // Find redirection symbols
    for (i = 0; args[i] != NULL; i++)
    {
        if (strcmp(args[i], "<") == 0)
        {
            // Input redirection
            if (args[i + 1] == NULL)
            {
                fprintf(stderr, "w25shell: No input file specified\n");
                return;
            }

            *in_fd = open(args[i + 1], O_RDONLY);
            if (*in_fd < 0)
            {
                perror("Failed to open input file");
                return;
            }

            // Remove redirection symbols from arguments
            args[i] = NULL;
        }
        else if (strcmp(args[i], ">") == 0)
        {
            // Output redirection
            if (args[i + 1] == NULL)
            {
                fprintf(stderr, "w25shell: No output file specified\n");
                return;
            }

            *out_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (*out_fd < 0)
            {
                perror("Failed to open output file");
                return;
            }

            // Remove redirection symbols from arguments
            args[i] = NULL;
        }
        else if (strcmp(args[i], ">>") == 0)
        {
            // Output redirection with append
            if (args[i + 1] == NULL)
            {
                fprintf(stderr, "w25shell: No output file specified\n");
                return;
            }

            *out_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (*out_fd < 0)
            {
                perror("Failed to open output file");
                return;
            }

            // Remove redirection symbols from arguments
            args[i] = NULL;
        }
    }
}
// SECTION ENDS: "I/O REDIRECTION"

// SECTION STARTS: "MEMORY MANAGEMENT"
/**
 * Function to clean up allocated memory for commands
 *
 * @param commands Array of commands and their arguments
 * @param command_count Number of commands
 */
void cleanup_commands(char ***commands, int command_count)
{
    for (int i = 0; i < command_count; i++)
    {
        if (commands[i])
        {
            for (int j = 0; j < MAX_ARGS; j++)
            {
                if (commands[i][j])
                {
                    free(commands[i][j]);
                }
            }
            free(commands[i]);
        }
    }
}

/**
 * Function to validate argument count for a command
 *
 * @param args Command and its arguments
 * @return 1 if valid, 0 if invalid
 */
int validate_args_count(char **args)
{
    int count = 0;

    while (args[count] != NULL)
    {
        count++;
        if (count > MAX_ARGS)
        {
            fprintf(stderr, "w25shell: Too many arguments (max %d allowed)\n", MAX_ARGS);
            return 0;
        }
    }

    if (count < 1)
    {
        fprintf(stderr, "w25shell: Command cannot be empty\n");
        return 0;
    }

    return 1;
}
// SECTION ENDS: "MEMORY MANAGEMENT"

// SECTION STARTS: "BUILT-IN COMMANDS"
/**
 * Function to handle killterm command
 * Terminates the current shell process
 */
void killterm_command()
{
    printf("Killing current terminal...\n");
    exit(0);
}
/**
 * Function to handle killallterms command
 * Kills all w25shell processes by sending SIGTERM to all processes with
 * the name "w25shell"
 */
void killallterms_command()
{
    printf("Killing all w25shell terminals...\n");

    // Use pgrep to find all w25shell processes
    FILE *fp;
    char command[100];
    char pid_str[16];

    // Create command to get PIDs of all w25shell processes
    sprintf(command, "pgrep w25shell");

    fp = popen(command, "r");
    if (fp == NULL)
    {
        perror("Failed to run pgrep");
        return;
    }

    // Read each PID and send SIGTERM
    while (fgets(pid_str, sizeof(pid_str), fp) != NULL)
    {
        pid_t pid = atoi(pid_str);

        // Skip current process (we'll exit ourselves at the end)
        if (pid != current_pid)
        {
            kill(pid, SIGTERM);
        }
    }

    pclose(fp);

    // Exit current shell
    exit(0);
}