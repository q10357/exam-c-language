#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "./include/oppgave_4.h"

MYHTTP* processHttpHeader(char *pszHttp) {
    char* pszPtr;
    MYHTTP* pHttp = (MYHTTP*)malloc(sizeof(PMYHTTP));
    if(!pHttp) return NULL;
    memset(pHttp, 0, sizeof(MYHTTP));

    pHttp->fHttpVersion = atof(pszHttp += strlen("HTTP/"));
    pHttp->iHttpCode = atoi(pszHttp += strlen("x.x "));
    if(pHttp->iHttpCode == 200){
        pHttp->bIsSuccess = true;
    }

    int n;
    pszPtr = strstr(pszHttp, "Server");
    if(pszPtr){
        pszPtr += 6; while(!isalpha(pszPtr[0])) pszPtr++;
        //NO SETTING ZERO TERMINATORS
        //strchr(pszPtr, '\n')[0] = 0;
        n = 0;
        while(pszPtr[n] != '\r' && pszPtr[n] != '\n') {
            n++;
        }

        if(n >= MAX_LENGTH_SERVER){
            n = MAX_LENGTH_SERVER - 1;
        }
        
        strncpy(pHttp->szServer, pszPtr, n);
        //pszPtr[strlen(pHttp->szServer)] = 0;
        //printf("This is server: %s\n", pHttp->szServer);
    }

    pszPtr = strstr(pszHttp, "Content-Type");
    if(pszPtr){
        pszPtr += 12; while(!isalpha(pszPtr[0])) pszPtr++;
        /*
        Each row is seperated by linefeed, we use this linefeed
        as the zero terminator for the pointer, and set the value 
        in the struct
        */
        //strchr(pszPtr, '\n')[0] = 0;
        n = 0;
        while(pszPtr[n] != ';' && pszPtr[n] != '\r'  && pszPtr[n] != '\n') {
            n++;
        }
        
        if(n >= MAX_LENGTH_CONTENT_TYPE){
            n = MAX_LENGTH_CONTENT_TYPE - 1;
        }

        strncpy(pHttp->szContentType, pszPtr, n);
        pszPtr[strlen(pHttp->szContentType)] = '\n';
        //application/json is 16 long, we set new size in struct
    }

    pszPtr = strstr(pszHttp, "Content-Length");
    if(pszPtr) {
        pszPtr += 14; while(!isdigit(pszPtr[0])) pszPtr++;
        pHttp->iContentLength = atoi(pszPtr);
        /*
        Mistake #3 + '0' gives + ASCII 0, which is 48.
        Giving us content-length = content-length + 48
        */ 
    }
    return pHttp;
}

void printHttpHeaders(MYHTTP* psHttp){
    puts("\nPrinting Out Http Header");
    puts("------------------------------");
    printf("HttpVersion: %.01f\n", psHttp->fHttpVersion);
    printf("HttpCode: %i\n", psHttp->iHttpCode);
    printf("Server: %s\n", psHttp->szServer);
    printf("Content-Type: %s\n", psHttp->szContentType);
    printf("Content-Length: %d\n", psHttp->iContentLength);
    puts("");
}

int main(void){
    char reply[] = "HTTP/1.1 200 \nPragma: no-cache\nContent-Type: application/json; charset=utf8\r\nExpires: -1\r\nLocation: http://localhost:8081/api/contacts/6\r\nServer: Microsoft-IIS/8.0\r\nX-AspNet-Version: 4.0.30319\r\nContent-Length: 175\r\n";
    MYHTTP* http = NULL;
    http = processHttpHeader(reply);

    printHttpHeaders(http);
    free(http);
    http = NULL;
    return 0;
}