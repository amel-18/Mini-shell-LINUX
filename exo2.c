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
    char s[] = "commande -v toto tata";
    char **argv;
    int argc = parse_line(s,&argv);
    
    //afficher les mots de argv
    for(int i=0;i<argc;i++){
        printf("%s\n",argv[i]);
    }
    
    //liberer la memoire
    for(int i = 0; i<argc ; i++){
        free(argv[i]);
    }
    
    //liberer la memoire alloué pour argv
    free(argv);
    return 0;
}

