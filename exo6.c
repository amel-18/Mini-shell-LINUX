#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXARG 16

void ignore_sigint() {
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("Erreur lors de l'ignorance du signal SIGINT");
        exit(EXIT_FAILURE);
    }
}

int parse_line(char *s, char ***argv) {
    int argc = 0;
    char *token;

    *argv = malloc(sizeof(char*) * (MAXARG + 1));
    if (*argv == NULL) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    token = strtok(s, " ");
    while (token != NULL && argc < MAXARG) {
        (*argv)[argc] = strdup(token);
        if ((*argv)[argc] == NULL) {
            perror("Erreur d'allocation mémoire");
            exit(EXIT_FAILURE);
        }
        argc++;
        token = strtok(NULL, " ");
    }

    (*argv)[argc] = NULL;

    return argc;
}

int main() {
    char input[100];
    char **argv = NULL;
    int argc;
    int output_fd;
    
    ignore_sigint();

    while (1) {
        printf("Entrez une commande : ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            break;
        }

        argc = parse_line(input, &argv);

        if (argc >= 3 && strcmp(argv[argc - 2], ">") == 0) {
            output_fd = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd == -1) {
                perror("Erreur lors de l'ouverture du fichier");
                exit(EXIT_FAILURE);
            }

            if (dup2(output_fd, STDOUT_FILENO) == -1) {
                perror("Erreur lors de la redirection de la sortie standard");
                exit(EXIT_FAILURE);
            }

            free(argv[argc - 2]);
            free(argv[argc - 1]);
            argv[argc - 2] = NULL;
            argv[argc - 1] = NULL;
            argc -= 2;

            printf("Redirection effectuée.\n");
        }
        else {
            printf("Redirection échouée.\n");
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("Erreur lors de la création du processus fils");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Processus fils

            // Restaurer le gestionnaire de signal par défaut pour SIGINT dans le processus fils
            struct sigaction act;
            act.sa_handler = SIG_DFL;
            if (sigaction(SIGINT, &act, NULL) == -1) {
                perror("Erreur lors de la restauration du signal SIGINT dans le fils");
                exit(EXIT_FAILURE);
            }

            // Exécuter la commande
            execvp(argv[0], argv);
            perror("Erreur lors de l'exécution de la commande");
            exit(EXIT_FAILURE);
        } else {
            // Processus parent

            // Attendre le processus fils
            wait(NULL);

            close(output_fd);
        }

        // Libérer la mémoire allouée pour les arguments
        if (argv != NULL) {
            for (int i = 0; i < argc; i++) {
                free(argv[i]);
            }
            free(argv);
        }
    }

    return 0;
}
