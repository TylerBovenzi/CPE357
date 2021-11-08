#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

clock_t lastactive;
void list(char* location){
    DIR *d;
    struct dirent *dir;
    d = opendir(location);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
        if(dir->d_name[0] != '.') printf("%s%s\033[0m  ",dir->d_type != DT_DIR ? "\033[0m":"\033[1;34m" ,dir->d_name);
        }
        closedir(d);
    }
    printf("\n");
}

void onkill(){
    return;
}

void getInfo(char *text){
struct stat stats;
if(stat(text, &stats)){
    printf("Invalid File\n");
    return;
}
printf("Device ID:  %ld\n", stats.st_dev);
printf("INODE#:     %ld\n", stats.st_ino);
printf("Protection: %d\n", stats.st_mode);
printf("Links:      %ld\n", stats.st_nlink);
printf("User ID:    %d\n", stats.st_uid);
printf("Group ID:   %d\n", stats.st_gid);
printf("Device ID:  %ld\n", stats.st_rdev);
printf("Size:       %ld\n", stats.st_size);
printf("Block Size: %ld\n", stats.st_blksize);
printf("Blocks:     %ld\n", stats.st_blocks);
//printf("Accessed:   %s\n", stats.st_atim);
//printf("Modified:   %s\n", stats.st_mtim);
//printf("Status Mod: %s\n", stats.st_ctim);
}
void updateTime(){
    lastactive = clock();
    return;
}

void childProcess(){
    char flush;
    char text[1000];

    char *currentDir, *currentDirBuf;
    long size;
    while(1==1){
        size = pathconf(".", _PC_PATH_MAX);
        if((currentDirBuf = (char *)malloc((size_t)size)) != NULL) currentDir = getcwd(currentDirBuf, (size_t)size);
        else return;
        printf("\033[0;34mstat prog . %s\033[0m$", currentDir);
        scanf("%[^\n]",text);
        scanf("%c",&flush);
        kill(getppid(), SIGRTMIN);
        if(!strcmp(text, "list")) list(".");
        else if(!strcmp(text, "..")) chdir("..");
        else if(text[0] == '/'){
            char temp[1001] = ".";
            struct stat stats;
            strcat(temp, text);
            if(stat(temp, &stats) || S_ISREG(stats.st_mode)) printf("Invalid Directory\n");
            else chdir(temp);
        }
        else if(!strcmp(text, "q")) break;
        else getInfo(text);
    }
}


void resetTime(){
    lastactive = clock();
}

void main(){
    int cid;
    lastactive = clock();
    cid = fork();
    if(!cid){
        childProcess();
        return;
    }
    else{
        signal(SIGINT, onkill);
        signal(SIGTERM, onkill);
        signal(SIGQUIT,onkill);
        signal(SIGHUP,onkill);
        signal(SIGRTMIN, resetTime);
        printf("Child ID: %d   Parent ID: %d\n", cid, getpid());
        while(clock()<lastactive+(10*CLOCKS_PER_SEC))asm("NOP");
        printf("\nExiting Due To Inactivity\n");
        kill(cid,SIGKILL);
        wait(0);
    }
    
}
