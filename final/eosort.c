#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <time.h>


void mySync2(int id,int par_count, long *ready, int stage){
    while(*ready<(id+((stage-1)*par_count))){}
    (*ready)++;
    while(*ready < (par_count * stage)){}
}

void printArr(int *data, int nums){
    printf("(");
    for(int i =0; i<nums; i++){
        printf(" %d\t", data[i]);
    }
    printf(")\n");
}

void sortStage(int id, int procs, int nums, int odd,int *data){
    
    int start = ((id*nums)/procs)+odd;
    int stop = ((((id+1)*nums)/procs)-2)+odd;

    for(int i = start; (i<=stop) && (i<nums-1); i+=2){
        if(data[i]> data[i+1]){     //If out of order
            int temp = data[i+1];   //Swap
            data[i+1] = data[i];    //..
            data[i] = temp;         //..
        }
    }
}

int readFile(char *filename, int *data, int size){
    FILE *file = fopen(filename, "r");          //Open File
    if(!file) return -1;                         //If file cant be opened return -1
    int ans = 0;
    int *temp = malloc(sizeof(int));
    while(ans<size && (fscanf(file, "%d[^\n]", temp) != -1)){
        data[ans] = *temp;
        ans++;
    }
    fclose(file);
    free(temp);
    return ans;

}

void main(int argc, char *argv[]){
    
    if(argc!=3){
        printf("Parameters Error\n");
        return;
    }
    int max = 1024; //maximum number of integers 
    //can be increased to accomodate larger text files
    
    long *logi = mmap(0,sizeof(double) , 0x1 | 0x2, 0x20|0x01, -1, 0);
    int *data = mmap(0,sizeof(int) * max , 0x1 | 0x2, 0x20|0x01, -1, 0);
    int nums = readFile(argv[1], data, max);
    if(nums<0){
        printf("Invalid File Name\n");
        return;
    }
    int procs = atoi(argv[2]);
    if(procs > nums/2){
        printf("Number of processes not exceed half the number of integers\n");
        return;
    }
    if(procs < 1){
        procs = 1;
        printf("Warning: Number of processes must be positive. Assuming 1\n");
    }
    int id = 0;
    clock_t start;
    logi[0] = 0;
    for(int i = 1; i<procs; i++){
        if(!fork()){
            id = i;
            break;
        }
    }

    mySync2(id, procs, logi, 1);
    start = clock();
    for(int stage = 0; stage < nums; stage+=2){
        sortStage(id, procs, nums, 0, data);    //Even
        mySync2(id, procs, logi, stage+2);      //Sync
        sortStage(id, procs, nums, 1, data);    //Odd
        mySync2(id, procs, logi, stage+3);      //Sync
    }
    if(id) return; //Terminate all children
    printArr(data, nums);
    long clocks = clock() - start;
    printf("Time: %ldus\n", (long)(1000000*clocks/((double)CLOCKS_PER_SEC)));
    munmap(logi, sizeof(double));
    munmap(data, sizeof(int)*max);
}
