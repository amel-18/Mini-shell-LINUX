#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(){
    char command[100];
    
    while(1){
        printf("$ ");
        fflush(stdout); // vider le tampon et afficher $
        
        if(fgets(command,100,stdin)==NULL){
            //lire la commande entrée
            // en cas d'erreur fin du fichier : quitter shell
            break;
        }
        command[strcspn(command,"\n")]='\0'; //supprimer le saut de ligne
        if (strcmp(command,"exit")==0){
            // si la commande entrée est exit on quit le shell
            break;
        }
        // création d'un nouveau processus
        pid_t pid =fork();
        
        // Afficher un erreur si la creation du processus échoue
        if(pid<0){
            perror("erreur du fork");
            exit(1);
        }else if( pid == 0){
            //processus fils
            if(execlp(command,command,NULL)<0){
                perror("erreur d'execution");
                exit(1);
            }
            else{
                // processus pere
                wait(NULL); //attendre la fin du processus fils
            }
        }
    }
    return 0;
}
    
