#include <stdio.h>
#include <stdlib.h>

void lineout(int leaves, int cols){
        for(int j=0; j<cols;j++){   //foreach character in a row
            if(abs(j-(cols-1)/2)<=leaves)   //calc if in range
                printf("*");                
            else
                printf(" ");
        }
        printf("\n");           //newline
}

void main(){
    int layers, cols;
    layers=0; cols=21;          //Mathmatical Defs
    printf("Number of leave lines: ");
    scanf("%d", &layers);       //collects #of lines
    if(layers>10) layers = 10;  //truncates to 10
    for(int i=0; i<layers;i++)  //prints leaves
        lineout(i,cols);
    for(int i=0; i<3;i++)       //prints stump
        lineout(0, cols);
}
