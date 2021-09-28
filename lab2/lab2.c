#include <stdio.h>
#include <stdlib.h>


struct listelement *head = NULL;
struct listelement *tail = NULL;
int length = 0;
struct listelement
{
    struct listelement *next,*prev;
    char text[1000];
}listelement;

void appendTo(){
    char flush;
    struct listelement* new = (struct listelement*)malloc(sizeof(listelement));
    new->next = NULL;
    new->prev = NULL;
    printf("\nInput String: ");
    scanf("%c",&flush);
    scanf("%[^\n]",new->text);
    if(!tail){
        head = new;
        tail = new;
    } else {
        tail->next = new;
        struct listelement *temp = tail;
        tail = new;
        tail->prev = temp;
    }
    length++;
}

void printList(){
    printf("\nList Contents:");
    struct listelement *addr = head;
    while(addr){  
        printf("\n%s", addr->text);
        addr = addr->next;
    }
    printf("\n");
}

void deleteItem(){
    int index =0;
    struct listelement *addr = head;
    if(length == 0){
        printf("\nList is already empty\n");
        return;
    }
    if(length == 1){
        printf("\nDeleted Only Element\n");
        free(head);
        tail = NULL;
        head = NULL;
        length = 0;
        return;
    }
    while(index<1 || index>length){
        printf("Please enter an integer between 1 and %d: ",length);
        while(scanf("%d",&index) != 1){
            printf("Please enter an integer:");
            while(getchar() != '\n');
        }
    }
    for(int i=1;i<index;i++){
        if(addr->next) addr = addr->next;
        else return;
    }
    if(addr->prev){ //if not the first element
        if(addr->next){ //if middle
            addr->prev->next=addr->next;
            addr->next->prev=addr->prev;
        } else {  //if end
            addr->prev->next=NULL;
            tail=addr->prev;
        }
    } else { //if fist elelement (must have next)
        head = addr->next;
        addr->next->prev = NULL;
    }
    printf("\nDeleted Element %d\n",index);
    free(addr);
    length--;
}

void main(){
    int selection;
    selection=0;
    while (selection != 4){
        printf("\nSelect:\n1 push string\n2 print list\n3 delete item\n4 end program\nSelection:");
            while(scanf("%d",&selection) != 1){
                printf("Please enter an integer:");
                while(getchar() != '\n');
            }
        if( selection<1 || selection>4) printf("\nInvalid Selection\n\n");
        else if(selection == 1) appendTo();
        else if(selection == 2) printList();
        else if(selection == 3) deleteItem();
    }
    while(head){            //Free remaining memory
        free(head);         
        head=head->next;
    }
    printf("\nExiting...\n");
}
