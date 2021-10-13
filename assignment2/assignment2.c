#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define PAGESIZE 4096

//unsigned char myheap[1048576];
typedef unsigned char byte;
struct chunkhead{
    unsigned int size;
    unsigned int info;
    struct chunkhead *next,*prev;
}chunkhead;
struct chunkhead *head;

struct chunkhead* findBestFit(int size){
    struct chunkhead* addr = head;
    int min = -1;
    struct chunkhead* minAddr = 0;
    while(addr){
        if(!addr->info){
            if(addr->size == size) return addr;
            if(min == -1){
                min = addr->size;
                minAddr = addr;
            } else {
                if(addr->size < min){
                    min = addr->size;
                    minAddr = addr;
                }
            }
        }
        addr = addr->next;
    }
    if(min == -1){
        return 0;
    }
    return minAddr;
}

char* mymalloc(int size){
    if(size<=0) return 0;   //handle bad inputs
    struct chunkhead *addr = head;
    size+=24; //add 24 to size to handle chunk
    int pages = 1+(int)(( (float)size / (float) PAGESIZE));
    if(size % PAGESIZE) size = PAGESIZE*pages;      //only modify if not clean
    if(!head){
        head = sbrk(size);
        head->size=size;     //sizeof(chunkhead);
        head->info=1;           //empty head;
        head->next=0;head->prev=0;
        return (char *)(head +1);
    }

    struct chunkhead* best = findBestFit(size);
    if(best){
        addr = best;
        int sizeDif = addr->size - size;        
        addr->size=size;                        //set new size of addr  
        addr->info=1;                           //set chunk as occupied 
        if(sizeDif){                            //handle splitting
            int ima = ((char*)addr)+size-((char*)head);
            
            //struct chunkhead *newNext = (struct chunkhead*)&myheap[ima];
            struct chunkhead *newNext = (struct chunkhead*)((char*)(addr)+size);
            newNext->size = sizeDif;
            newNext->info = 0;
            newNext->prev = addr;
            if(addr->next){
                newNext->next = addr->next;
                newNext->next->prev = newNext;
            }
            addr->next = newNext;               //link to next
        }
        return (char*)(addr+1);
    }
<<<<<<< HEAD
    while(addr->next){
        addr = addr->next;
    }

    // while(1){                                    //iterate through all chunks
    //     if(addr->size >= size && !addr->info){      //check for suitable chunk
    //         //suitable chunk is found
    //         int sizeDif = addr->size - size;        
    //         addr->size=size;                        //set new size of addr  
    //         addr->info=1;                           //set chunk as occupied 
    //         if(sizeDif){                            //handle splitting
    //             int ima = ((char*)addr)+size-((char*)head);
                
    //             //struct chunkhead *newNext = (struct chunkhead*)&myheap[ima];
    //             struct chunkhead *newNext = (struct chunkhead*)((char*)(addr)+size);
    //             newNext->size = sizeDif;
    //             newNext->info = 0;
    //             newNext->prev = addr;
    //             if(addr->next){
    //                 newNext->next = addr->next;
    //                 newNext->next->prev = newNext;
    //             }
    //             addr->next = newNext;               //link to next
    //         }
    //         return (char*)(addr+1);
    //     } else {                                    //no suitable chunk
    //         if(!addr->next) break;
    //         addr = addr->next;                      //check next
    //     }
    // }



    struct chunkhead* new = sbrk(size);
    new->info = 1;
    new->size = size;
    new->next=0;
    new->prev=addr;
    addr->next=new;
    return (char *)(new+1);
=======
    struct chunkhead* new;
>>>>>>> 3e6b377d502fe6574252f12c605a0e59945a9d4e

    return 0;                                       //no available chunks
}

void myfree(char *address){
    struct chunkhead *addr = (struct chunkhead*)address;
    addr-=1;
    if(!address) return; 
    if(addr == head && !head->next){
        head = 0;
        brk(sbrk(0)-addr->size);
        return;
    }

    addr->info = 0;
    if(addr->prev && !addr->prev->info){  //if there is an empty previous
        addr->prev->size = addr->size+addr->prev->size;
        addr->prev->next = addr->next;
        if(addr->next) addr->next->prev = addr->prev;
        addr = addr->prev;
    }
    if(addr->next){  //if there is an empty next
        if(!addr->next->info){
            addr->size = addr->size+addr->next->size;
            addr->next = addr->next->next;
            if(addr->next) addr->next->prev = addr;
        }
    }
    if(!addr->next){
        if(addr == head){
            head = 0;
        } else {
            addr->prev->next = 0;
        }
        brk(sbrk(0)-addr->size);
        return;
    }
}

void analyze(){
    
    struct chunkhead *addr = head;
    int i=1;
    while(addr){
        printf("Chunk #%d:\n", i);
        printf("\tSize: %d\n", addr->size);
        if(addr->info) printf("\t\033[0;31mOccupied\033[0m\n");
        else printf ("\t\033[0;32mFree\033[0m\n");
        printf("\tPrev: %p\n", addr->prev);
        printf("\tAddr: %p\n", addr);
        printf("\tNext: %p\n", addr->next);
        i++;
        addr=addr->next;
    }
    //if (i==1) printf("No heap\n");
}

// void analyze(){
//     printf("\n--------------------------------------------------------------\n");
//     if(!head){
//         printf("no heap\n");
//         return;
//     }
//     struct chunkhead* ch = (struct chunkhead*)head;
//     for(int no=0; ch; ch = (struct chunkhead*)ch->next,no++){
//         printf("%d | current addr: %p |", no, ch);printf("size: %d | ", ch->size);printf("info: %d | ", ch->info);printf("next: %p | ", ch->next);printf("prev: %p", ch->prev);printf("      \n");
//     }
//     printf("program break on address: %p\n",sbrk(0));
// }

void test(){
    byte* a[100];
    //analyze();//50% points
    for(int i=0;i<100;i++)a[i]= mymalloc(1000);
    for(int i=0;i<90;i++)myfree(a[i]);
    //analyze(); //50% of points if this is correct
    myfree(a[95]);
    a[95] = mymalloc(1000);

    analyze();//25% points, this new chunk should fill the smaller free one 
    //(best fit)
    //return;
    for(int i=90;i<100;i++)myfree(a[i]);
    analyze();// 25% should be an empty heap now with the start address
    //from the program start
}



void main(){
    test();
    return;
   
    printf("Start: %p\n",sbrk(0));

    printf("Start: %p\n",sbrk(0));
    
    char *a = mymalloc(5000);
    char *b = mymalloc(12000);
    char *c = mymalloc(1000);
    char *d = mymalloc(8000);
    char *e = mymalloc(4000);
    myfree(b);
    myfree(d);

    //printf("out  : %p\n",findBestFit(4096));

    //char *f = mymalloc(4000);

    printf("End  : %p\n",sbrk(0));

    analyze();




}