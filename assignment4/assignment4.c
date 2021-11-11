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

struct childinfo{
    char *target, type, active;
    int id;
} childinfo;

char checkfile(char *filename, char *text){
    FILE *file = fopen(filename, "r");          //Open File
    if(!file) return 0;                         //If file cant be opened return 0
    fseek(file, 0L, SEEK_END);                  //Check size of file
    long bytes = ftell(file);                   //write to variable
    fseek(file, 0L, SEEK_SET);                  //reset position
    char *contents = (char *)malloc(bytes+1);   //allocate memory
    if(!contents) return 0;                     //if empty or mem error return 0
    contents[bytes] = 0;                        //add null char
    fread(contents, sizeof(char), bytes, file); //read file into memory
    fclose(file);                               //close file
    return strstr(contents, text) ? 1:0;        //return 1 if file containts target, else 0
    free(contents);                             //free memory
}

char *get_ext(char *filename){
    char *place = strrchr(filename, '.');
    if(!place|| place == filename) return "";
    return place+1;
}

/*
Search:
    Inputs:
        char type:      0-3 defining what kind of search 
        string target:  filename or search text depending on type of search
        string ext:     file extension (only used for text search [type 2 or 3])
    Outputs:
        long:           number of results found
*/
long search(char type, char *target, char *ext){
    long ans = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(dir->d_type != DT_DIR){
                if(type > 1){   //if searching for text
                    if(!strcmp(ext, get_ext(dir->d_name))){
                        if(checkfile(dir->d_name, target)){
                            char cwd[PATH_MAX];
                            getcwd(cwd, sizeof(cwd));
                            printf("%s contains \"%s\"\n", strcat(strcat(cwd,"/"),dir->d_name), target);
                            ans++;
                        }
                    }
                } else {        //if searching for name
                    if(!strcmp(target, dir->d_name)){
                        char cwd[PATH_MAX];
                        getcwd(cwd, sizeof(cwd));
                        printf("%s found in %s\n", target, cwd);
                        ans++;
                    }
                }
            }
            if(dir->d_type == DT_DIR && strcmp(dir->d_name, "..") && strcmp(dir->d_name,".")){
                if(type%2==1){
                    char temp[PATH_MAX] = "./";
                    strcat(temp, dir->d_name);
                    chdir(temp);
                    ans+=search(type, target, ext); //recursively search child directory
                    chdir("..");
                }
            }
            
        }
    }
    closedir(d);
    return ans;
}


//type=0    file non recursive
//type=1    file recursive
//type=2    text non recursive
//type=3    text recursive
void main(){
    struct childinfo *processes = mmap(0,120, 0x1 | 0x2, 0x20|0x01, -1, 0);
    for(int i =0; i<10; i++) processes[i].active =0;
    char numProcesses =0;
    char flush;
    char text[1000];
    char text2[1000];
    while(1==1){
        printf("\033[0;34mfindstuff\033[0m$");
        text2[0] = 0;
        text[0] = 0;
        scanf("%[^\n]",text);
        scanf("%c",&flush);
        strcpy(text2, text);
        if(!strcmp(text, "q")) break;
        else if(strlen(text2)>0 && strlen(strtok(text2, " ")) == 4){
            char temp[5] ="";
            strncpy(temp, text2, 4);
            temp[4] =0;
            if(!strcmp(temp, "kill")){
                if(strlen(text) == 4 || strlen(text) == 5){
                    printf("Please specify id\n");
                }else {
                    int addr = atoi(text+5);
                    char found =0;
                    for(int i =0; i <10; i++){
                        if(processes[i].active && processes[i].id == addr){
                            found = 1;
                            kill(addr, SIGKILL);
                            free(processes[i].target);
                            processes[i].active=0;
                            numProcesses--;
                            printf("Killed Process %d\n", addr);
                            break;
                        }
                    }
                    if(!found) printf("Process Not Found\n");
                }
            }
            else if(!strcmp(temp, "list")){
                for(int i =0; i<10; i++){
                    if(processes[i].active){
                        printf("Process %d: Searching for %s:%s\n", processes[i].id, processes[i].type > 1 ? "text":"file", processes[i].target);
                    }
                }
            }
            else if(!strcmp(temp, "find")){
                if(strlen(text) == 4 || strlen(text) == 5){
                    printf("Please specify target\n");
                } else {
                    char type = 0;
                    strtok(text, " ");
                    char *arg[3];
                    char *ext = "";
                    arg[0] = strtok(NULL, " ");
                    arg[1] = strtok(NULL, " ");
                    if(arg[1]) arg[2] = strtok(NULL, " ");
                    if(arg[0] && strlen(arg[0])>1 && arg[0][0]=='\"'){
                        type+=2;
                        arg[0]++;//remove first quote
                        arg[0][strlen(arg[0])-1] = 0;//remove second quote
                    }
                    if(arg[1]){
                        if(strstr(arg[1], "-s")){ type+=1;}
                        else if(arg[2]){
                            if(strstr(arg[2], "-s")) type +=1;
                        }
                    }

                    
                    if(arg[1]){
                        if(strlen(arg[1]) > 3 && !strncmp("-f:", arg[1], 3)){
                            ext = arg[1]+3;
                        }else if(arg[2]){
                            if(strlen(arg[2]) > 3 && !strncmp("-f:", arg[2], 3)){
                                ext = arg[2]+3;
                            }
                        }
                    }
                    if(!arg[0]){
                        printf("Please specify target\n");
                    } else {
                        if(numProcesses<10){
                            int id = fork();
                            if(!id){
                                sleep(5);
                                search(type, arg[0], ext);
                                printf("1:%d\n", getpid());
                                return;
                            }
                            printf("2:%d\n", id);
                            struct childinfo tempstruct;
                            for(int i =0; i<10; i++){
                                if(processes[i].active == 0){
                                    processes[i].id = id;
                                    processes[i].active = 1;
                                    processes[i].target = malloc(strlen(arg[0])+1);
                                    strcpy(processes[i].target, arg[0]);
                                    processes[i].type = type;
                                    numProcesses++;
                                    break;

                                }
                            }

                        } else {
                            printf("Maximum number of processess reached\n");
                        }
                        printf("FIND\n"); 
                            }

                }
                
            }
            else if(!strcmp(temp, "quit")){
                break;
            } else printf("Invalid Command\n");
        } else printf("Invalid Command\n");
    }

}
