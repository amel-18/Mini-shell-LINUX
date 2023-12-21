#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    while(element != NULL){
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
    char **argv;
    int argc;
    
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
        
        //executer la commandeavec les arguments
        printf("execution de la commande : %s\n" , argv[0]);
        for(int i =1;i<argc;i++){
            printf("argument %d : %s\n",i,argv[i]);
        }
        //liberer la memoire
        for(int i =0;i<argc;i++){
            free(argv[i]);
        }
        //liberer la memoire
        free(argv);
    }
    return 0;
}
