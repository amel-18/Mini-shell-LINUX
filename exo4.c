#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

//constante pour definir le nbr max d'args
#define MAXARG 16

int parse_line(char *s, char ***argv){
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
    while(element != NULL && argc < MAXARG){
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

int main(){
    char input[100];
    char **argv = NULL;
    int argc;
    int output;
    
    while(1){
        printf("Entrez une commande : ");
        fgets(input,sizeof(input),stdin);
        //supprimer le caractere de nouvelle ligne
        input[strcspn(input,"\n")]='\0';
        
        // si exit quitter la boucle
        if(strcmp(input,"exit")==0){
            break;
        }
        
        //appel de la fonction
        argc= parse_line(input,&argv);

        if (argc >= 3 && strcmp(argv[argc - 2], ">")==0)
        {
            output = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output == -1)
            {
                perror("impossible d'ouvrir le fichier");
                exit(1);
            }

            if(dup2(output, STDOUT_FILENO) == -1)
            {
                perror("redirection ineffectuer");
                exit(1);
            }

            free(argv[argc -2]);
            free(argv[argc - 1]);
            argv[argc - 2] = NULL;
            argv[argc - 1] = NULL;
            argc -=2;
            printf("Redirection effectuée avec succès. \n");
        }
        else
        {
            printf("redirection échouée. \n");
        }
        
        //executer la commandeavec les arguments
        printf("execution de la commande : %s\n" , argv[0]);
        for(int i =1;i<argc;i++){
            printf("argument %d : %s\n",i,argv[i]);
        }
        //liberer la memoire
        if(argv != NULL)
        {
            for(int i =0;i<argc;i++)
            {
            free(argv[i]);
            }
            //liberer la memoire
            free(argv);
        }
        close(output);
    }
    return 0;
}