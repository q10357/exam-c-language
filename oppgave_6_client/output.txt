#ifndef __HTTP_UTILS_H__
#define __HTTP_UTILS_H__

typedef struct _MYHTTP {
    float fHttpVersion;
    int iHttpCode;
    int iContentLength;
    bool bIsSuccess;
    char szServer[16];
    char szContentType[16];
}MYHTTP, PMYHTTP; 
//MISTAKe #1 *MYHTTP

void makeHttpResponse(MYHTTP* psHttp);

#endif