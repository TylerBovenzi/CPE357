#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/mman.h>
void onkill(){
    //printf("\nPlease Don't Kill Me\n");
    return;
}

void doChildThing(){

    printf("Files in current directory: \n");
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
        if(dir->d_name[0] != '.') printf("[%s]\n", dir->d_name);
        }
        closedir(d);
    }
    printf("\n");
}


void main(){

    int forkOut =0;
    int parent = getpid();
    int a = 3;
    clock_t* last = (clock_t *)mmap(0,sizeof(clock_t), 0x1 | 0x2, 0x20|0x01, -1, 0);
    last[0] = clock()-(10*CLOCKS_PER_SEC);
    while(1==1){
        int i =0;
        forkOut = fork();
        signal(SIGINT, onkill);
        signal(SIGTERM, onkill);
        signal(SIGQUIT,onkill);
        signal(SIGHUP,onkill);
        if(!forkOut){
            while(1==1){
                
                if(getpgid(parent)<0) return;
                if(clock()>last[0]+(10*CLOCKS_PER_SEC)){
                    time_t T= time(NULL);
                    struct tm *now = localtime(&T);
                    printf("Time: %02d:%02d:%02d\n", now->tm_hour, now->tm_min, now->tm_sec);
                    last[0] = clock();
                    doChildThing();
                }
            }
        
        } else {
            printf("Child: %d\n", forkOut);
            printf("Parent: %ld\n\n", (long)getpid());
            wait(0);
            a--;
        }

    }
}