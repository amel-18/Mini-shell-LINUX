#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

//constante pour definir le nbr max d'args
#define MAXARG 16

int parse_line(char *s, char ***argv, char **output_file){
    int argc = 0;
    char *element;
    
    //allouer de la memoire pour le tableau argv
    *argv = malloc(sizeof(char*)* (strlen(s)+1));
    if(*argv == NULL){
        perror("erreur malloc");
        exit(1);
    }
    //separer la chaine en mot par des espaces
    element = strtok(s, " ");
    while(element != NULL)
    {
        if(strcmp(element, ">") == 0)
        {
            element = strtok(NULL, " ");
            if (element != NULL)
            {
                *output_file = strdup(element);
            }
            else
            {
                perror("argument manquant pour la redirection");
                exit(1);
            }
            break;
        }
        // stocker chaque mot dans une case de argv
        (*argv)[argc] = strdup(element);
        if((*argv)[argc] == NULL){
            perror("erreur du malloc");
            exit(1);
        }
        argc++;
        element= strtok(NULL, " ");
    }
    //fin tableau
    (*argv)[argc] = NULL;
    return argc;
}

int main() {
    char input[100];
    char **argv;
    int argc;
    char *output_file = NULL;
    int pipefd[2];

    while (1) {
        printf("Entrez une commande : ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            break;
        }

        char *commands[2];
        commands[0] = strtok(input, "|");
        commands[1] = strtok(NULL, "|");

        if (commands[1] != NULL) {
            char *cmd1 = commands[0];
            char *cmd2 = commands[1];

            argc = parse_line(cmd1, &argv, &output_file);

            if (pipe(pipefd) == -1) {
                perror("Erreur lors de la création du tube");
                exit(EXIT_FAILURE);
            }

            pid_t pid = fork();
            if (pid == -1) {
                perror("Erreur lors de la création du processus enfant");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);

                execvp(argv[0], argv);
                perror("Erreur lors de l'exécution de la première commande");
                exit(EXIT_FAILURE);
            } else {
                close(pipefd[1]);
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);

                argc = parse_line(cmd2, &argv, &output_file);

                if (output_file != NULL) {
                    int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd == -1) {
                        perror("Erreur lors de l'ouverture du fichier de sortie");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }

                execvp(argv[0], argv);
                perror("Erreur lors de l'exécution de la deuxième commande");
                exit(EXIT_FAILURE);
            }
        } else {
            argc = parse_line(input, &argv, &output_file);

            if (output_file != NULL) {
                int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    perror("Erreur lors de l'ouverture du fichier de sortie");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            pid_t pid = fork();
            if (pid == -1) {
                perror("Erreur lors de la création du processus enfant");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                execvp(argv[0], argv);
                perror("Erreur lors de l'exécution de la commande");
                exit(EXIT_FAILURE);
            } else {
                wait(NULL);
            }
        }

        for (int i = 0; i < argc; i++) {
            free(argv[i]);
        }
        free(argv);

        if (output_file != NULL) {
            free(output_file);
            output_file = NULL;
        }
    }

    return 0;
}