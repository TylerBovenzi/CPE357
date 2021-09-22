#include <stdio.h>
#include <stdlib.h>

void lineout(int leaves, int cols){
        for(int j=0; j<cols;j++){
            if(abs(j-(cols-1)/2)<=leaves)
                printf("*");
            else
                printf(" ");
        }
        printf("\n");
}

void main(){
    int layers, cols;
    layers=0; cols=21;
    printf("Number of leave lines: ");
    scanf("%d", &layers);
    if(layers>10) layers = 10;
    for(int i=0; i<layers;i++)
        lineout(i,cols);
    for(int i=0; i<3;i++)
        lineout(0, cols);
}
