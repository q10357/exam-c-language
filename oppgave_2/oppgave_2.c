#include <stdio.h>
#include <stdlib.h>
#include "./include/file_utils.h"

int main(void) {
    int iRc;
    char* fileNameInput = "text.txt";
    char *pcBuff;

    iRc = readFileContentToCharArray(fileNameInput, &pcBuff);
    if(iRc != 0){
        //Error doing file operation
        return iRc;
    }

    char* fileNameOutput = "file_output.txt";
    iRc = writeHexRepresentationToTextFile(fileNameOutput, &pcBuff);
    if(iRc != 0){
        puts("Error writing to file");
        return iRc;
    } 
    free(pcBuff);
    //Set to null, free(str) if str == NULL -> wont crash program
    //Since this program is very short, it doesn't really matter, but good practice
    pcBuff = NULL;
    return 0;
}
