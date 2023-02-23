#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define OK 0
#define NOT_GOOD 1
#define EVEN_WORSE 2

int writeToBufferUntilDelimiter(char* source, char* dest, char delimiter){
    int x = 0;
    while(source[0] != delimiter){
        if(source[0] != 0x20){
            dest[x] = source[0];
            x++;
        }
        *source++;
    }
    return x;
}

int getNumberOfOccurances(char* pszStr, char* pszSubString, int* iNum){
    char* pszForLoopOccurance = strstr(pszStr, pszSubString);
    while(pszForLoopOccurance){
        pszForLoopOccurance += strlen(pszSubString);
        pszForLoopOccurance = strstr(pszForLoopOccurance, pszSubString);
        *iNum += 1;
    }
}

int readFile(const char *pszFileName, char **ppcBuf, long *plSize){
    int iRc = NOT_GOOD;
    long lSize, lBytesRead;
    char string[100];
    FILE *f;
    
    f = fopen(pszFileName, "rb");
    if(f != NULL){
        if(fseek(f, 0L, SEEK_END) == 0) {
            //fseek is successfull
            lSize = ftell(f);
            rewind(f);
            //We have the size, allocate buffer
            *ppcBuf = (char *) malloc(lSize + 1);
            if(*ppcBuf){
                //Read file
                lBytesRead = fread(*ppcBuf, sizeof(char), lSize, f);
                if(lBytesRead == lSize) {
                    //We got it all!
                    *plSize = lSize;
                    (*ppcBuf)[lSize] = 0; // Add a terminating '\0'
                    iRc = OK;
                }
                else {
                    iRc = NOT_GOOD;
                    free(*ppcBuf);
                    *ppcBuf = NULL;
                }
            }
            else{
                //Not able to allocate memory
                iRc = EVEN_WORSE;
            }
        }
        fclose(f);
    }
    return iRc;
}