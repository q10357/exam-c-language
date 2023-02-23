#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define OK 0
#define NOT_GOOD 1
#define EVEN_WORSE 2

int writeToBufferUntilDelimiter(char* source, char* dest, char delimiter);
int getNumberOfOccurances(char* pszStr, char* pszSubString, int* iNum);
int readFile(const char *pszFileName, char **ppcBuf, long *plSize);
//TODO: TAKE FILE NAME AS ARGUMENT
int main(int iArgC, char* pszArgV[]){
     if(iArgC < 2){
        printf("Usage: %s <name of requested file>\n", pszArgV[0]);
        return 0;
    }

    char* filename = pszArgV[1];
    FILE* pFileIn = NULL;
    FILE* pFileOut = NULL;

    char* pszFileContent = NULL;
    char pszSubString[] = "for";
    char* out = NULL;
    unsigned long ulSze = 0;
    int iRc = OK;

    /*
    WE READ THE FILE INTO A BUFFER
    */
    iRc = readFile(filename, &pszFileContent, &ulSze);
    if(iRc != OK){
        perror("Error opening file");
        return 1;
    }
    printf("%s\n", pszFileContent);
    

    int iNumberOfForLoops = 0;
    getNumberOfOccurances(pszFileContent, pszSubString, &iNumberOfForLoops);
    printf("Number of for loops: %d\n", iNumberOfForLoops);
    out = (char*)malloc(sizeof(char) * (ulSze * 2));
    memcpy(out, pszFileContent, ulSze);

    if(iNumberOfForLoops > 0){
        char* pForLoop = strstr(out, pszSubString); 
        while(pForLoop){
            int index = pForLoop - out;
            /*
            Program need to know the size of the string to be replaced
            Storing it in variable iSzeOldString
            */
            int iSzeOldString = 0;
            /*
            If spaces ex: for(   int a), we move forward, and also increment iSzeOldString
            */
            pForLoop += strlen(pszSubString); while(pForLoop[0] == ' ') { pForLoop++; iSzeOldString++;}

            char initialization[100];
            char evaluation[100];
            char update[100];
            int x = 0;
            if(pForLoop[0] == 0x28){
                pForLoop++;
                x = writeToBufferUntilDelimiter(pForLoop, initialization, ';');
                //initialization[x] = 0;
                char str[] = "for(";
                
                while(pForLoop[iSzeOldString] != '{'){iSzeOldString++;}
                iSzeOldString+= strlen(str);
                iSzeOldString++;

                int n = 0;
                while(!isalpha(pForLoop[n])){ iSzeOldString++; n++;}
                
                while(pForLoop[0] != ';'){pForLoop++;}
                pForLoop++;
                
                x = writeToBufferUntilDelimiter(pForLoop, evaluation, ';');
                //evaluation[x] = 0;

                while(pForLoop[0] != ';'){pForLoop++;}
                pForLoop++;
                
                x = writeToBufferUntilDelimiter(pForLoop, update, ')');
                //update[x] = 0;
                
                char new [100];
                if(sprintf(new, "%s;\n\twhile(%s){\n\t\t%s;\n", initialization, evaluation, update)){
                    memmove(out + index + strlen(new), out + index + iSzeOldString, strlen(out+index));
                    memcpy(out + index, new, strlen(new));
                }else{
                    puts("Error when beautifying for loop");
                }
            }
            pForLoop = strstr(pForLoop + 1, pszSubString);
        }
    }

    /*Replacing all tabs with three spaces*/
    char* pPointerTabs = strchr(out, '\t');
    int iTabOccurances = 0;
    //First getting number of tab occurances
    while(pPointerTabs){
        iTabOccurances++;
        pPointerTabs = strchr(pPointerTabs + 1, '\t');
    }  
    //We need three spaces for every tab
    int iNeededMemorySpace = 3 * iTabOccurances;
    
    int index = 0;
    pPointerTabs = strchr(out, '\t');
    while(pPointerTabs){
        index = pPointerTabs - out;
        memmove(out + index + 3, out + index + 1, strlen(out+index));
        memcpy(out + index, "   ", 3);
        pPointerTabs = strchr(pPointerTabs + 1, '\t');
    }  

    pFileOut = fopen("test_beautified.c", "w");
    if(pFileOut != NULL){
        int bytes_wrote = fprintf(pFileOut, "%s", out);
        if(bytes_wrote == 0){
            printf("Error writing to file, 0 elements");
            return 2;
        }
    fclose(pFileOut);
    }else{
        perror("Failed to open output file");
    }

    free(pszFileContent);
    free(out);
    return 0;
}