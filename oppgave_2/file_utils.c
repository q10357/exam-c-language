#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./include/file_utils.h"

#define OK 0
#define ERROR_READING 1
#define ERROR_ALLOCATING 2
#define NOT_FOUND -1

typedef unsigned char BYTE;

int readFileContentToCharArray(const char* filename, char** ppcBuff){
    /*---------READ --------*/
    int iRc = ERROR_READING;
    long lElementsRead;
    long lSize = 0;

    //Open file
    FILE* file = fopen(filename, "r");
    if(file != NULL){
        //If not null, find length
        if(fseek(file, 0, SEEK_END) == 0){
            lSize = ftell(file);
            rewind(file);
            //Allocate memory, plus one for zero terminator
            *ppcBuff = malloc(lSize + 1);
            //Saving characters to buffer array 
            if(*ppcBuff){
                lElementsRead = fread(*ppcBuff, sizeof(BYTE), lSize, file);
                if(lElementsRead == lSize){
                    //Add zero to end 
                    *(ppcBuff + (lSize + 1)) = 0;
                    iRc = OK;
                }else{
                    puts("Error reading to array");
                    return ERROR_READING;        
                }
            }else{
                puts("Error allocating");
                return ERROR_ALLOCATING;
            }
        }else{
            puts("Error reading file");
            return ERROR_READING;
        }
        fclose(file);
    }else{
        puts("Error opening file");
        return ERROR_READING;
    }

    return iRc;
}


int writeHexRepresentationToTextFile(const char* filename, char** ppcText){
    /*--------WRITE----------*/
    int iRc = ERROR_READING;
    char* str = *ppcText;
    
    FILE* file = fopen(filename, "w");
    if(file != NULL){
        int n = 0;
        unsigned char c = str[n];
        while(c != 0){
            fprintf(file, "%02x", c);
            n++;
            c = str[n];
        }
        iRc = OK;
    }else{
        puts("Error reading file");
        iRc = ERROR_READING;
    }

    fclose(file);
    return iRc;
}   