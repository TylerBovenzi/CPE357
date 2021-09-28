#include <stdio.h>


struct listelement *head = NULL;
struct listelement *tail = NULL;

struct listelement
{
    struct listelement *next,*prev;
    char text[1000];
}listelement;

void appendTo(){
    struct listelement* new = (struct listelement*)malloc(sizeof(listelement));
    new->next = NULL;
    new->prev = NULL;
    printf("\nInput String: ");
    scanf("%s",new->text);
    if(!tail){
        head = new;
        tail = new;
    } else {
        tail->next = new;
        struct listelement *temp = tail;
        tail = new;
        tail->prev = temp;
    }
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
    if(tail){
        struct listelement *addr = tail;
        if(addr->prev){
            tail = addr->prev;
            tail->next = NULL;
        } else {
            tail = NULL;
            head = NULL;
        }
        free(addr);
    } else {
        printf("\nList is empty\n");
    }
}

void main(){
    int selection;
    selection=0;
    while (selection != 4){
        printf("\nSelect:\n1 push string\n2 print list\n3 delete item\n4 end program\nSelection:");
        scanf("%d",&selection);
        if( selection<1 || selection>4) printf("\nInvalid Selection\n\n");
        else if(selection == 1) appendTo();
        else if(selection == 2) printList();
        else if(selection == 3) deleteItem();
    }
    while(head) deleteItem();
    printf("\nExiting...\n");
}
