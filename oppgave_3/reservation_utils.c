#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "./include/reservation_utils.h"

void printReservation(RESERVATION* psRes){
    RES_INFO* psResInfo = psRes->psResInfo;
    char pszStrDate[11];
    formatDate(psResInfo->iStartDate, pszStrDate);
    printf("\tReservation: \n");
    printf("\t------------------\n");
    printf("\tName: %s\n", psResInfo->szName);
    printf("\tRoom: %s\n", psResInfo->szRoom);
    printf("\tStart date of stay: %s\n", pszStrDate);
    printf("\tLength of stay: %d nights\n", psResInfo->iNightsStaying);
    printf("\tPrice per night: %.02f$\n", psResInfo->fPricePerNight);
}

RESERVATION* createNode(RES_INFO* psData){
    RESERVATION* pThis = (RESERVATION*)malloc(sizeof(RESERVATION));
    if(pThis != NULL){
        /*
        Successfully allocated
        Setting all addresses to 0's
        */
        pThis->psResInfo = psData;
    }
    return pThis;
}

int deleteEndedStays(RESERVATION_LIST *psList, int date){
    int iNumDeletedReservations = 0;
    RESERVATION* pThis = psList->psReservationHead;
    RESERVATION *pPrev = NULL;
    RESERVATION* pToDelete = NULL;
    
    while(pThis != NULL){
        unsigned int iEndOfStay = ((pThis->psResInfo->iStartDate) + (pThis->psResInfo->iNightsStaying));
        if(date > iEndOfStay){
            pToDelete = pThis;
            pThis = pThis->pNext;
            if(removeNode(psList, pToDelete) != OK){
                iNumDeletedReservations = -1;
                break;
            }
            iNumDeletedReservations++;
        }else{
            pThis = pThis->pNext;
        }
    }
    return iNumDeletedReservations;
}

int isReservationActiveOnDate(RESERVATION* psListItem, int date){
    int iEndOfStay = ((psListItem->psResInfo->iStartDate) + (psListItem->psResInfo->iNightsStaying));
    return date <= iEndOfStay && date >= psListItem->psResInfo->iStartDate;
}

int getTotalByDate(RESERVATION_LIST* psList, int date){
    RESERVATION* pThis = psList->psReservationHead;
    int total = 0;
     while(pThis != NULL){
        if(isReservationActiveOnDate(pThis, date)){
            total += pThis->psResInfo->fPricePerNight;
        }
        pThis = pThis->pNext;
    }
    return total;
}

int removeNode(RESERVATION_LIST* psList, RESERVATION* psListItemToDelete){
    int iRc = ERROR;
    RESERVATION* pThis = NULL;
    RESERVATION* pPrev = NULL;

    if(psList->psReservationHead != NULL){
        //List is not empty
        if(psList->psReservationHead == psListItemToDelete){
            /*deleting the first item in list*/
            if(psListItemToDelete->pPrev == NULL){
                /*Deleting one and only item in list*/ 
                psList->psReservationHead = NULL;
                psList->psReservationTail = NULL;
            }else{
                psList->psReservationHead = psListItemToDelete->pNext;
                psList->psReservationHead->pPrev = NULL;
            }
            iRc = OK;
        }
        else if(psList->psReservationTail == psListItemToDelete){
            /*Deleting the last item in list*/
            psList->psReservationTail = psListItemToDelete->pPrev;
            psList->psReservationTail->pNext = NULL;
            iRc = OK;
        }
        else {
            /*Iterate through list until list item found*/
            pThis = psList->psReservationHead;
            while(pThis != NULL){
                if(pThis == psListItemToDelete){
                    pPrev->pNext = pThis->pNext;
                    pThis->pNext->pPrev = pThis->pPrev;
                    iRc = OK;
                    break;
                }
                pPrev = pThis;
                pThis = pThis->pNext;
            }
        }
    }
    if(iRc == OK){
        free(psListItemToDelete->psResInfo);
        free(psListItemToDelete);
        psListItemToDelete= NULL;
    }

    return iRc;
}

RESERVATION* searchForReservationByName(RESERVATION_LIST* pList, char* pszName){
    RESERVATION* pSearchResult = NULL;
    RESERVATION* pThis = pList->psReservationHead;
    printf("%s\n", pszName);
    while(pThis != NULL){
        if(strcmp(pThis->psResInfo->szName, pszName) == 0){
            pSearchResult = pThis;
            break;
        }
        pThis = pThis->pNext;
    }
    return pSearchResult;
}

int isEmpty(RESERVATION_LIST* psList){
    return psList->psReservationHead == NULL;

}

int addToList(RESERVATION_LIST *psList, RESERVATION* psListItemToDelete){
    RESERVATION* pThis = psListItemToDelete;
    int iRc = ERROR;
    if(pThis != NULL) {
        if(psList->psReservationHead == NULL){
            //Empty list, setting both tail and head to current node
            psList->psReservationHead = pThis;
            psList->psReservationTail = pThis;
        }else{
            //Adding to end of list
            psList->psReservationTail->pNext = pThis;
            pThis->pPrev = psList->psReservationHead;
            psList->psReservationTail = pThis;
        }
        iRc = OK;
    }
    return iRc;
}


void formatDate(int date, char* str){
    char buff[9];
    sprintf(buff, "%d", date);
    int iDateFormat[2] = {2, 4};
    int n = 0;
    int i = 0;
    int j = 0;
    while(i < strlen(buff)){
        *(str + j) = buff[i];
        i++;
        j++;
        if(i == (iDateFormat[n])){
            *(str + j) = '-';
            j++;
            n++;
        }
    }

}