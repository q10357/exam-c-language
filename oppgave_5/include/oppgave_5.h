#ifndef _THREADING_H__
#define _THREADING_H__

#define BUFFER_SIZE 4096

typedef struct _BUFFER {
    unsigned char szBuffer[BUFFER_SIZE];
    void* *pFile;
    int iFileSize;
    sem_t semEmpty;
    sem_t semFull;
    pthread_mutex_t mutex;
    int count;
} BUFFER;

void* readFile(void* args);
void* writeByteOccurances(void* args);

#endif // _THREADING_H__
