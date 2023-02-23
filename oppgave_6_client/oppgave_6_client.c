#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include "./include/httpfunc.h"
#include "./include/file_utils.h"

#define TRUE 1
#define FALSE 0

#define BUFFER_SIZE_READ 512

int sendRequest(int socket, char *pszFileName);
int recieveFromServer(int socket, int timeout, char* filename);

//Code inspired from lecture 12, lecturers browser.c program
int main(int iArgC, char *pszArgV[]){
    if(iArgC < 2){
        printf("Usage: %s <name of requested file>\n", pszArgV[0]);
        return 0;
    }/*else if(strlen(*(pszArgV[1]) >= 256)){
        printf("Filename too large, max size: %d\n", 256);
        return 0;
    }*/

    char* filename = pszArgV[1];
    int sockFd = -1;
    struct sockaddr_in saAddr;
    int iPort = 0;
    char* pszHttpReply = NULL;
    
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockFd < 0) {
        printf("socket failed failed width: %s\n", strerror(errno));
    }else{
        iPort = atoi("30000");
        saAddr.sin_family = AF_INET;
        saAddr.sin_addr.s_addr = htonl(0x7f000001);
        saAddr.sin_port = htons(iPort);
        
        if(connect(sockFd, (const struct sockaddr*) &saAddr, sizeof(saAddr)) < 0){
            //Error connecting socket
            printf("Connect failed %s\n", strerror(errno));
        }else{
            char szBuffer [BUFFER_SIZE_READ] = "Hello Server\r\n";
            printf("%s", szBuffer);
            int n = send(sockFd, szBuffer, strlen(szBuffer), 0);
            if(n < 0){
                printf("send failed with %i\n", errno);
            }else{
                memset(szBuffer, 0, BUFFER_SIZE_READ);
                strcpy(szBuffer, "Hello Again\r\n");
                printf("%s", szBuffer);
                int n = send(sockFd, szBuffer, strlen(szBuffer), 0);
                if(n > 0){
                    //We have completed handshake!
                    memset(szBuffer, 0, BUFFER_SIZE_READ);
                    sprintf(szBuffer, "GET %s HTTP/1.1\r\n", filename);
                    int n = send(sockFd, szBuffer, strlen(szBuffer),0);
                    if(n > 0){
                        //Request sent, wait for response
                        if(recieveFromServer(sockFd, 4,filename) == 0){
                            //Success
                            puts("File downloaded");
                        }else{
                            puts("Error downloading file");
                        }
                    }
                }
            }
        }
       
    }
    return 0;
}

int sendRequest(int socket, char *pszFileName){
    int res = send(socket, pszFileName, strlen(pszFileName), 0);
    if(res == -1){
        printf("%s: %s\n", "Error sending data", strerror(errno));
    }
    return res;
}

int recieveFromServer(int socket, int timeout, char* filename){
    char buffer[BUFFER_SIZE_READ];
    double timeDiff;
    int iRc = 1;

    while(1) {
        memset(buffer, 0, BUFFER_SIZE_READ);
        int n = recv(socket, buffer, BUFFER_SIZE_READ, 0);
        if(n < 0){
            //Error getting data
            break;
        }else if (n == 0){
            break;
        }else{
            printf("\n\nResult HTTP response: \n%s\n", buffer);
            char* pszStartHttpHeaders = strstr(buffer, "HTTP/");
            if(pszStartHttpHeaders){
                //RECIEVED OUR SERVER RESPONSE
                printf("%s\n", pszStartHttpHeaders);
                MYHTTP* psHttpResponse = processHttpHeader(pszStartHttpHeaders);
                printHttpHeaders(psHttpResponse);
                if(psHttpResponse->bIsSuccess == true){
                    /*
                    200 OK!
                    When http returns, its protocol to seperate 
                    headers and content with an empty line, we can
                    search for \r\n\r\n, and point to content
                    */
                    char* content = strstr(buffer, "\r\n\r\n");
                    content += strlen("\r\n\r\n");
                    char pszFileDest[512];
                    if(sprintf(pszFileDest, "output.txt", filename)){
                        FILE* f = fopen(pszFileDest, "w");
                        if(f){
                            iRc = 0;
                            fprintf(f, "%s", content);
                            fclose(f);
                        }else{
                            printf("%s\n","Error opening file %s\n", pszFileDest);
                        }
                    }
                }
                break;
            }
        }
    }
    return iRc;
}

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
        n = 0;
        while(pszPtr[n] != '\r') {
            n++;
        }

        if(n >= MAX_LENGTH_SERVER){
            n = MAX_LENGTH_SERVER - 1;
        }

        strncpy(pHttp->szServer, pszPtr, n);
    }

    pszPtr = strstr(pszHttp, "Content-Type");
    if(pszPtr){
        pszPtr += 12; while(!isalpha(pszPtr[0])) pszPtr++;
        n = 0;
        while(pszPtr[n] != ';' && pszPtr[n] != '\r') {
            n++;
        }
        
        if(n >= MAX_LENGTH_CONTENT_TYPE){
            n = MAX_LENGTH_CONTENT_TYPE - 1;
        }

        strncpy(pHttp->szContentType, pszPtr, n);
        pszPtr[strlen(pHttp->szContentType)] = '\n';
    }

    pszPtr = strstr(pszHttp, "Content-Length");
    if(pszPtr) {
        pszPtr += 14; while(!isdigit(pszPtr[0])) pszPtr++;
        pHttp->iContentLength = atoi(pszPtr);
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
