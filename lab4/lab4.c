#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <time.h>


typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef unsigned char BYTE;

#define help "\n\033[0;32m---------------------------------------\n\tImage Brightener Usage:\n---------------------------------------\033[0m\n\nPlease specify the following parameters:\n\t1. Input File 1 Name\n\t2. Brightness Amount (0.0-1.0)\n\t3. Parallel 1=y, 0=n\n\t4. Output File Name\n\n\033[1;31mParameters must be supplied in this order\033[0m\n\n"


typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;  //specifies the file type
    DWORD bfSize;  //specifies the size in bytes of the bitmap file
    WORD bfReserved1;  //reserved; must be 0
    WORD bfReserved2;  //reserved; must be 0
    DWORD bfOffBits;  //specifies the offset in bytes from the bitmapfileheader to the bitmap bits
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;  //specifies the number of bytes required by the struct
    LONG biWidth;  //specifies width in pixels
    LONG biHeight;  //specifies height in pixels
    WORD biPlanes;  //specifies the number of color planes, must be 1
    WORD biBitCount;  //specifies the number of bits per pixel
    DWORD biCompression;  //specifies the type of compression
    DWORD biSizeImage;  //size of image in bytes
    LONG biXPelsPerMeter;  //number of pixels per meter in x axis
    LONG biYPelsPerMeter;  //number of pixels per meter in y axis
    DWORD biClrUsed;  //number of colors used by the bitmap
    DWORD biClrImportant;  //number of colors that are important
}BITMAPINFOHEADER;

typedef struct tagBMP
{
    BITMAPFILEHEADER FileHead;
    BITMAPINFOHEADER InfoHead;
    BYTE* data;
    int WIDTH;
    int HEIGHT;
    int pixelWidth;
}BMP;

BYTE checkFile(char* filename){
    FILE *temp;
    if(temp = fopen(filename, "rb")){
        fclose(temp);
        return 1;
    }
    return 0;
}

unsigned char getBlue(int x, int y, BMP bmp){
    if(x>bmp.WIDTH) x = bmp.WIDTH;
    if(x<0) x = 0;
    if(y>bmp.HEIGHT) y = bmp.HEIGHT;
    if(y<0) y = 0;
    return bmp.data[x*3+y*bmp.pixelWidth+0];
}

unsigned char getGreen(int x, int y, BMP bmp){
    if(x>bmp.WIDTH) x = bmp.WIDTH;
    if(x<0) x = 0;
    if(y>bmp.HEIGHT) y = bmp.HEIGHT;
    if(y<0) y = 0;
    return bmp.data[x*3+y*bmp.pixelWidth+1];
}

unsigned char getRed(int x, int y, BMP bmp){
    if(x>bmp.WIDTH) x = bmp.WIDTH;
    if(x<0) x = 0;
    if(y>bmp.HEIGHT) y = bmp.HEIGHT;
    if(y<0) y = 0;
    return bmp.data[x*3+y*bmp.pixelWidth+2];
}

void setBlue(int x, int y, BMP bmp, unsigned char val){
    bmp.data[x*3+y*bmp.pixelWidth+0]=val;return;
}

void setGreen(int x, int y, BMP bmp, unsigned char val){
    bmp.data[x*3+y*bmp.pixelWidth+1]=val;return;
}

void setRed(int x, int y, BMP bmp, unsigned char val){
    bmp.data[x*3+y*bmp.pixelWidth+2]=val;return;
}

BMP readBMP(const char* name){
    BMP bmp;
    FILE *fp = fopen(name, "rb");
    fread(&bmp.FileHead.bfType, 2, 1, fp);
    fread(&bmp.FileHead.bfSize, 4, 1, fp);
    fread(&bmp.FileHead.bfReserved1, 2, 1, fp);
    fread(&bmp.FileHead.bfReserved2, 2, 1, fp);
    fread(&bmp.FileHead.bfOffBits, 4, 1, fp);
    fread(&bmp.InfoHead, sizeof(bmp.InfoHead), 1, fp);
    //bmp.data = (BYTE*) malloc(bmp.InfoHead.biSizeImage);
    bmp.data = mmap(0,bmp.InfoHead.biSizeImage, 0x1 | 0x2, 0x20|0x01, -1, 0);
    fread(bmp.data, bmp.InfoHead.biSizeImage,1,fp);
    fclose(fp);   
    bmp.WIDTH=bmp.InfoHead.biWidth;
    bmp.HEIGHT=bmp.InfoHead.biHeight;
    bmp.pixelWidth = 3*bmp.WIDTH;
    if(4-bmp.pixelWidth%4 != 4) bmp.pixelWidth = bmp.pixelWidth+ 4-bmp.pixelWidth%4; 
    return bmp;
}

BMP cloneBMP(BMP bmp){
    BMP new;
    new.FileHead = bmp.FileHead;
    new.InfoHead = bmp.InfoHead;
    //new.data = (BYTE*) malloc(new.InfoHead.biSizeImage);
    new.data = mmap(0,bmp.InfoHead.biSizeImage, 0x1 | 0x2, 0x20|0x01, -1, 0);
    new.WIDTH = bmp.WIDTH;
    new.HEIGHT = bmp.HEIGHT;
    new.pixelWidth = bmp.pixelWidth;
    return new;
}

void writeBMP(const char* name, BMP bmp){
    FILE *fp = fopen(name,"wb");
    fwrite(&bmp.FileHead.bfType, 2, 1, fp);
    fwrite(&bmp.FileHead.bfSize, 4, 1, fp);
    fwrite(&bmp.FileHead.bfReserved1, 2, 1, fp);
    fwrite(&bmp.FileHead.bfReserved2, 2, 1, fp);
    fwrite(&bmp.FileHead.bfOffBits, 4, 1, fp);
    fwrite(&bmp.InfoHead, sizeof(bmp.InfoHead), 1, fp);
    fwrite(bmp.data, bmp.InfoHead.biSizeImage,1,fp);
    fclose(fp);
}

BYTE blur(BYTE c1, BYTE c2, float ratio){
    return((c1*ratio)+(c2*(1-ratio)));
}

BYTE min(int a, int b){
    if(a < b) return a;
    return b;
}

//if using photoshop -> export as 24bit RBG
void main(int args, char *arg[]){

    clock_t time = clock(); 

    if (args != 5){
        printf(help);
        return;
    }

    if (((float)atof(arg[2]) < 0) || ((float)atof(arg[2]) > 1)){
        printf("\033[1;31mError: Invalid Brightness\033[0m\nBrightness must be in range 0-1\n");
        return;
    }


    if (((float)atof(arg[3]) != 0) && ((float)atof(arg[3]) != 1)){
        printf("\033[1;31mError: Invalid Parallel Selection\033[0m\n0: No Parellel\n1: Parallel\n");
        return;
    }

    struct tagBMP bmp1,  bmpOut;
    if(!checkFile(arg[1])){
        printf("\033[1;31mError: Input file does not exist\033[0m\n");
        return;
    }

    bmp1 = readBMP(arg[1]);
    bmpOut = cloneBMP(bmp1);
    float ratio = (float)atof(arg[3]);

    if((float)atof(arg[3])){
        float half = (float)bmp1.HEIGHT / 2;
        int halfHeight = (int)half;
        if(0==fork()){
            for(int y=halfHeight; y<bmp1.HEIGHT; y++){
                for(int x =0; x<bmp1.WIDTH; x++){
                    setBlue(x, y, bmpOut, min(255, (((float)atof(arg[2])*255.0)+getBlue(x,y,bmp1))));
                    setRed(x, y, bmpOut, min(255, (((float)atof(arg[2])*255.0)+getRed(x,y,bmp1))));
                    setGreen(x, y, bmpOut, min(255, (((float)atof(arg[2])*255.0)+getGreen(x,y,bmp1))));
                }
            }
            return;
        } else {
            for(int y=0; y<halfHeight; y++){
                for(int x =0; x<bmp1.WIDTH; x++){
                    setBlue(x, y, bmpOut, min(255, (((float)atof(arg[2])*255.0)+getBlue(x,y,bmp1))));
                    setRed(x, y, bmpOut, min(255, (((float)atof(arg[2])*255.0)+getRed(x,y,bmp1))));
                    setGreen(x, y, bmpOut, min(255, (((float)atof(arg[2])*255.0)+getGreen(x,y,bmp1))));
                }
            }
            wait(0);
        }

    } else {
        for(int y=0; y<bmp1.HEIGHT; y++){
            for(int x =0; x<bmp1.WIDTH; x++){
                setBlue(x, y, bmpOut, min(255, (((float)atof(arg[2])*255.0)+getBlue(x,y,bmp1))));
                setRed(x, y, bmpOut, min(255, (((float)atof(arg[2])*255.0)+getRed(x,y,bmp1))));
                setGreen(x, y, bmpOut, min(255, (((float)atof(arg[2])*255.0)+getGreen(x,y,bmp1))));
            }
        }
    }

    msync(bmpOut.data, sizeof(bmpOut.InfoHead.biSizeImage), 4);
    writeBMP(arg[4],bmpOut);
    clock_t total = clock() - time;
    printf("\033[0;32mFile brightened in %ld us\n\033[0m", total);
    
    munmap(bmp1.data, sizeof(bmp1.data));
    munmap(bmpOut.data, sizeof(bmpOut.data));
}
