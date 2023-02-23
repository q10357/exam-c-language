#include <errno.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "./include/oppgave_5.h"

#define THREAD_NUM 2
#define POSSIBLE_BYTES 256

#define TRUE 1
#define FALSE 0

int main(void){
    FILE* pFile;
    unsigned long lSize = 0;

    pFile = fopen("PG3401-Hjemmeeksamen-14dager-H22.pdf", "rb");
    if(pFile == NULL){
        puts("Error opening file");
    }

    BUFFER *pBuffer = malloc(sizeof(BUFFER));
    pBuffer->pFile = (void*)pFile;
    pthread_t th[THREAD_NUM];
    
    pthread_mutex_init(&pBuffer->mutex, NULL);
    sem_init(&pBuffer->semEmpty, 0, BUFFER_SIZE);
    sem_init(&pBuffer->semFull, 0, 0);

    int i;
    for(i = 0; i < THREAD_NUM; i++){
        if(i > 0){
            if(pthread_create(th + i, NULL, &readFile, (void*)pBuffer) != 0){
                perror("Failed to create thread: ");
                exit(1);
            }
        } else {
            if(pthread_create(th + i, NULL, &writeByteOccurances, (void*)pBuffer) != 0){
                perror("Failed to create thread: ");
                exit(1);
            }
        }
    }

    for(i = 0; i < THREAD_NUM; i++){
        if(pthread_join(th[i], NULL) != 0){
            perror("Failed to join thread: ");
        }
    }

    free(pBuffer);
    pthread_mutex_destroy(&pBuffer->mutex);
    sem_destroy(&pBuffer->semEmpty);
    sem_destroy(&pBuffer->semFull);
    
    return 0;
}

void* readFile(void* args){
    BUFFER* pBuf = (BUFFER*)args;
    unsigned long ulSize = 0;
    int iElementsReadFromFile = 0;

    FILE* pFile = (FILE*)pBuf->pFile;
    if(fseek((FILE*)pBuf->pFile, 0, SEEK_END) == 0){
        pBuf->iFileSize = ftell(pFile);
        rewind(pFile);
    }

    do{
        sem_wait(&pBuf->semEmpty);
        pthread_mutex_lock(&pBuf->mutex);
        pBuf->szBuffer[pBuf->count] = fgetc(pFile);
        pBuf->count++;
        pthread_mutex_unlock(&pBuf->mutex);
        sem_post(&pBuf->semFull);
        iElementsReadFromFile++;
    }while(iElementsReadFromFile<pBuf->iFileSize);

    fclose((FILE*)pFile);
}

void* writeByteOccurances(void* args){
    int iBytesOccuranes[POSSIBLE_BYTES];
    
    unsigned char y;
    int iElementsReadFromBuffer = 0;
    BUFFER* pBuf = (BUFFER*)args;
    do{
        sem_wait(&pBuf->semFull);
        pthread_mutex_lock(&pBuf->mutex);
        y = pBuf->szBuffer[pBuf->count -1];
        pBuf->count--;
        pthread_mutex_unlock(&pBuf->mutex);
        sem_post(&pBuf->semEmpty);
        iElementsReadFromBuffer++;
        iBytesOccuranes[y]++;
    }while(iElementsReadFromBuffer < pBuf->iFileSize);

    int sum = 0;
    puts("Printing occurances of each byte in file...");
    for(int i = 0; i < POSSIBLE_BYTES; i++){
        printf("Byte %02x: occured %d times", i, iBytesOccuranes[i]);
        if((POSSIBLE_BYTES - i + 1) % 3 == 0){
            printf("\n");
        }else{
            printf("\t");
        }
        sum += iBytesOccuranes[i];

    }

    printf("\nSUM: %d\n", sum);
    if(sum != pBuf->iFileSize){
        printf("Error!\nNumber of bytes in file != bytes read\n");
    }
}