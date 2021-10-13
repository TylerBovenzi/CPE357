#include <stdlib.h>
#include <stdio.h>

#define PAGESIZE 1024

unsigned char myheap[1048576];

struct chunkhead{
    unsigned int size;
    unsigned int info;
    struct chunkhead *next,*prev;
}chunkhead;
struct chunkhead *head;

void init(){
    head = (struct chunkhead*)&myheap[0];
    head->size=1048576;     //sizeof(chunkhead);
    head->info=0;           //empty head;
    head->next=0;head->prev=0;
}

char* mymalloc(int size){
    if(size<=0) return 0;   //handle bad inputs
    struct chunkhead *addr = head;
    size+=24; //add 24 to size to handle chunk
    int pages = 1+(int)(( (float)size / (float) PAGESIZE));
    if(size % PAGESIZE) size = PAGESIZE*pages;      //only modify if not clean
    while(addr){                                    //iterate through all chunks
        if(addr->size >= size && !addr->info){      //check for suitable chunk
            //suitable chunk is found
            int sizeDif = addr->size - size;        
            addr->size=size;                        //set new size of addr  
            addr->info=1;                           //set chunk as occupied 
            if(sizeDif){                            //handle splitting
                int ima = ((char*)addr)+size-((char*)head);
                //struct chunkhead *newNext = (struct chunkhead*)&myheap[ima];
                struct chunkhead *newNext = (char*)addr+size;
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
        } else {                                    //no suitable chunk
            addr = addr->next;                      //check next
        }
    }
    return 0;                                       //no available chunks
}

void myfree(char *address){
    struct chunkhead *addr = (struct chunkhead*)address;
    addr-=1;
    addr->info = 0;
    if(addr->prev && !addr->prev->info){  //if there is an empty previous
        addr->prev->size = addr->size+addr->prev->size;
        addr->prev->next = addr->next;
        if(addr->next) addr->next->prev = addr->prev;
        addr = addr->prev;
    }
    if(addr->next && !addr->next->info){  //if there is an empty next
        addr->size = addr->size+addr->next->size;
        addr->next = addr->next->next;
        if(addr->next) addr->next->prev = addr;
    }
}

void analyse(){
    struct chunkhead *addr = head;
    int i=1;
    while(addr){
        printf("Chunk #%d:\n", i);
        printf("\tSize: %d\n", addr->size);
        if(addr->info) printf("\t\033[0;31mOccupied\033[0m\n");
        else printf ("\t\033[0;32mFree\033[0m\n");
        printf("\tPrev: %p\n", addr->prev);
        printf("\tNext: %p\n", addr->next);
        i++;
        addr=addr->next;
    }
}

void main(){
    init();
    char *a = mymalloc(5000);
    char *b = mymalloc(5000);
    char *c = mymalloc(5000);
    
    myfree(b);

    b = mymalloc(1000);

    analyse();


}