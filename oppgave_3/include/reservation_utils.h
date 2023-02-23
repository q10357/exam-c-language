#ifndef RESERVATION_UTILS_H
#define RESERVATION_UTILS_H

#define OK 0
#define ERROR 1
#define ERROR_ALLOCATING 2
#define ERROR_LIST_EMPTY 3
#define NOT_FOUND -1

#define MAX_SIZE_NAME 100
#define MAX_SIZE_ROOM 4

typedef struct _RESERVATION_INFO {
    unsigned int iStartDate;
    int iNightsStaying;
    float fPricePerNight;
    char szName[MAX_SIZE_NAME];
    char szRoom[MAX_SIZE_ROOM];
} RES_INFO;

typedef struct _RESERVATION {
    struct _RESERVATION_INFO* psResInfo;
    struct _RESERVATION* pNext;
    struct _RESERVATION* pPrev;
} RESERVATION;


typedef struct _RESERVATION_LIST {
    RESERVATION* psReservationHead;
    RESERVATION *psReservationTail;
} RESERVATION_LIST;

void printReservation(RESERVATION* psListItem);
void formatDate(int date, char* str);
int isReservationActiveOnDate(RESERVATION* psListItem, int date);
int addToList(RESERVATION_LIST *pList, RESERVATION* psListItem);
int removeNode(RESERVATION_LIST* psList, RESERVATION* psListItemToDelete);
int getTotalByDate(RESERVATION_LIST* psList, int date);
int deleteEndedStays(RESERVATION_LIST* pList, int date);
int isEmpty(RESERVATION_LIST* psList);
int compareDates(int iDateNow, int iDateCompare);

RESERVATION* searchForReservationByName(RESERVATION_LIST* pList, char* szName);
RESERVATION* createNode(RES_INFO* psData);

#endif