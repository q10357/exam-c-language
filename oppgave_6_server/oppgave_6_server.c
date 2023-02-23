#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "./include/httpfunc.h"
#include "./include/file_utils.h"

#define TRUE 1
#define FALSE 0

#define ERROR_CREATING_SOCKET -1

void bindToPort(int iSocket, int iPort);
void writeHardCodedHttpHeaders(MYHTTP* psHttp);
int readDataFromClient(int socket, char* pDataRecieved, int iLen);
int sendDataToClient(int socket, char* pData);
int getFileName(char* psRequest, char* psFilename);
int writeHttpResponseToBuffer(MYHTTP* psHttp, char* bufferTo);

int main(int iArgC, char* pszArgV[])
{
    int iPort = 30000;
    int sockFd = -1;

    //Creating socket
    sockFd = socket(PF_INET, SOCK_STREAM, 0);

    if(sockFd == -1){
        printf("Error creating socket, abort\n");
        exit(ERROR_CREATING_SOCKET);
    }
    
    bindToPort(sockFd, 30000);
    listen(sockFd, 10);
    puts("Server waiting for connection...");
    char buff[255];

    while(1){
        struct sockaddr_storage saAddrClient;
        unsigned int uiAddrClientSize = sizeof(saAddrClient);
        int iConnection = accept(sockFd, (struct sockaddr*)&saAddrClient, &uiAddrClientSize);
        int result = sendDataToClient(iConnection, "Hello from server\r\n");
        if(result != -1){
            readDataFromClient(iConnection, buff, sizeof(buff));
            if(strstr(buff, "Hello")){
                //waiting for second ACK
                readDataFromClient(iConnection, buff, sizeof(buff));
                if(strstr(buff, "Hello")){
                    //Handshake is done! now parsing to see what file to send to client
                    int n = readDataFromClient(iConnection, buff, sizeof(buff));
                    if(strstr(buff, "GET")){
                        char pszFilename[256];
                        if(getFileName(buff, pszFilename) == 0){
                            MYHTTP* pHttpResponse = (MYHTTP*)malloc(sizeof(MYHTTP));
                            if(pHttpResponse){
                                writeHardCodedHttpHeaders(pHttpResponse);
                                char* pszResponse = malloc(1000);
                                char* pszFileContent;
                                if(readFileContentToCharArray(pszFilename, &pszFileContent) == 0) {
                                    pHttpResponse->bIsSuccess = true;
                                    printf("%s\n", pszFileContent);
                                    pHttpResponse->iContentLength = strlen(pszFileContent);
                                    pHttpResponse->iHttpCode = 200;
                                    int n = writeHttpResponseToBuffer(pHttpResponse, pszResponse);
                                    if(n > 0){
                                        //Successfully wrote http response to array
                                        pszResponse[n + 1] = 0;
                                        strncat(pszResponse, pszFileContent, 400);
                                        printf("%s\n", pszResponse);
                                        sendDataToClient(iConnection, pszResponse);
                                    }else{
                                        //Error writing http headers return 500
                                    }
                                }else{
                                    //File not found (most likely), return 404
                                    pHttpResponse->bIsSuccess = false;
                                    pHttpResponse->iHttpCode = 404;
                                    pHttpResponse->iContentLength = 0;
                                    writeHttpResponseToBuffer(pHttpResponse, pszResponse);
                                    sendDataToClient(iConnection, pszResponse);
                                }
                                free(pszResponse);
                            }else{
                                //Error allocating httpresponse
                            }
                            free(pHttpResponse);
                        }else{
                            //Error parsing filename
                            
                        }
                    }else{
                        //Invalid request, GET not found
                    }
                }else{
                    sendDataToClient(iConnection, "You didnt say hello\r\n");
                }
            }
        }
    }
    return 0;
}

void writeHardCodedHttpHeaders(MYHTTP* psHttp){
    strcpy(psHttp->szServer, "MyCoolServer");
    psHttp->fHttpVersion = 1.1;
    strcpy(psHttp->szContentType, "text/html");
}

int writeHttpResponseToBuffer(MYHTTP* psHttp, char* bufferTo){
    char status[20];
    if(psHttp->bIsSuccess){
        sprintf(status, "%s", "OK");
    }else{
        sprintf(status, "%s", "NOT FOUND");
    }
    return sprintf(
        bufferTo,
        "HTTP/%.1f %d %s\r\nServer: %s\r\nContent-Type: %s;\r\nContent-Length: %d\r\n\r\n",
        psHttp->fHttpVersion, psHttp->iHttpCode, status, psHttp->szServer, psHttp->szContentType, psHttp->iContentLength
    );
}

int getFileName(char* psRequest, char* psFilename){
    printf("%s\n", psRequest);
    psRequest += strlen("GET ");
    int n = 0;

    //Out server cuts of at first space 
    //W do not suppport filenames with spaces, since we control server, that's okay for now
    while(psRequest[n] != 0x20){
        psFilename[n] = psRequest[n];
        n++;
    }

    if(psFilename != NULL){
        return 0;
    }
    return 1;
}

void bindToPort(int iSocket, int iPort){
    struct sockaddr_in saAddr;

    //Using IPv4
    saAddr.sin_family = PF_INET;

    //On port = iPort
    saAddr.sin_port = (in_port_t)htons(30000);

    //Accept connection from these addresses:
    saAddr.sin_addr.s_addr = htonl(0x7f000001);

    //Bind socket
    bind(iSocket, (struct sockaddr*)&saAddr, sizeof(saAddr));

    //We want to be able to connect from same client multiple times over 30 seconds, sett to reusable
    int isReusable = TRUE;
    if(setsockopt(iSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&isReusable, sizeof(int)) == -1){
        puts("Error setting socket option");
    }
}

int readDataFromClient(int socket, char* pDataRecieved, int iLen){
    char* tmp = pDataRecieved;
    int iTmp = iLen;
    int iRecieved = recv(socket, tmp, iTmp, 0);

    while((iRecieved > 0) && (tmp[iRecieved - 1] != '\n')){
        tmp += iRecieved;
        iTmp -= iRecieved;
        iRecieved = recv(socket, tmp, iTmp, 0);
    }
    if(iRecieved < 0){
        //Error, data sent is less than 0
        return iRecieved;
    }else if(iRecieved == 0){
        /*
        No data was sent
        There is no guarantee that data sent has zero terminator, 
        adding zero terminator, send back empty string
        */
        pDataRecieved[0] = 0;
    }else{
        tmp[iRecieved - 1] = 0;
        printf("Data: %s\n", tmp);
    }

    return iLen - iTmp;
}

int sendDataToClient(int socket, char* pData){
    int res = send(socket, pData, strlen(pData), 0);
    if(res == -1){
        printf("%s: %s\n", "Error when sending data", strerror(errno));
    }

    return res;
}
