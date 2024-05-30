#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

void create_pipes(int pipes[][2], int cmd_count) {
    int i = 0;
    while (i < cmd_count - 1) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
        i++;
    }
}

void close_pipes(int pipes[][2], int cmd_count) {
    int i = 0;
    while (i < cmd_count - 1) {
        close(pipes[i][0]);
        close(pipes[i][1]);
        i++;
    }
}

void execute_command(char *cmd, char **envp, int input_fd, int output_fd) {
    char *args[3];
    char *command = strtok(cmd, " ");
    char *arg = strtok(NULL, " ");
    args[0] = command;
    args[1] = arg;
    args[2] = NULL;

    if (input_fd != -1) {
        dup2(input_fd, STDIN_FILENO);
        close(input_fd);
    }

    if (output_fd != -1) {
        dup2(output_fd, STDOUT_FILENO);
        close(output_fd);
    }

    execve(args[0], args, envp);
    perror("execve failed");
    exit(EXIT_FAILURE);
}

void handle_child(int i, int cmd_count, int pipes[][2], char **argv, char **envp, char *inputfile, char *outputfile) {
    int input_fd = -1, output_fd = -1;
    if (i == 0) {
        input_fd = open(inputfile, O_RDONLY);
        if (input_fd == -1) {
            perror("open inputfile failed");
            exit(EXIT_FAILURE);
        }
        output_fd = pipes[i][1];
    } else if (i == cmd_count - 1) {
        input_fd = pipes[i - 1][0];
        output_fd = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd == -1) {
            perror("open outputfile failed");
            exit(EXIT_FAILURE);
        }
    } else {
        input_fd = pipes[i - 1][0];
        output_fd = pipes[i][1];
    }

    close_pipes(pipes, cmd_count);
    execute_command(argv[i + 3], envp, input_fd, output_fd);
}

int main(int argc, char **argv, char **envp) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s inputfile outputfile cmd1 cmd2 ... cmdN\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *inputfile = argv[1];
    char *outputfile = argv[2];
    int cmd_count = argc - 3;
    int pipes[cmd_count - 1][2];
    pid_t pids[cmd_count];
    int i = 0;

    create_pipes(pipes, cmd_count);

    while (i < cmd_count) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } else if (pids[i] == 0) {
            handle_child(i, cmd_count, pipes, argv, envp, inputfile, outputfile);
        }
        i++;
    }

    close_pipes(pipes, cmd_count);

    i = 0;
    while (i < cmd_count) {
        waitpid(pids[i], NULL, 0);
        i++;
    }

    return 0;
}
