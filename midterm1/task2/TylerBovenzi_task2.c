#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void main(){
    for(int i =0; i<100; i++){
        if(!fork()){
            if(!fork()){
                printf("Tyler ");
                return;
            }
            wait(0);
            printf("Mark ");

            return;
        }
        wait(0);
        printf("Bovenzi\n");
    }
}