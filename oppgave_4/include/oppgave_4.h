#ifndef __HTTP_UTILS_H__
#define __HTTP_UTILS_H__

#define MAX_LENGTH_SERVER 16
#define MAX_LENGTH_CONTENT_TYPE 20

typedef struct _MYHTTP {
    float fHttpVersion;
    int iHttpCode;
    int iContentLength;
    bool bIsSuccess;
    char szServer[MAX_LENGTH_SERVER];
    char szContentType[MAX_LENGTH_CONTENT_TYPE];
}MYHTTP, PMYHTTP; 
//MISTAKe #1 *PMYHTTP

void printHttpHeaders(MYHTTP* psHttp);
MYHTTP* processHttpHeader(char *pszHttp);

#endif