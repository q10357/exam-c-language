#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "include/reservation_utils.h"

#define TRUE 1
#define FALSE 0

/*
TODO: FIX DELETEOLDRESERVATIONS() => ITS NOT QUITE WORKING!!
BE SURE NOT TO BE ABLE TO SET UNREALISTIC DATE
*/

void printMenu();
void printMessage();
void makeReservation(RESERVATION_LIST* pList);
void deleteLastMadeReservation(RESERVATION_LIST* pList);
void deleteOldReservations(RESERVATION_LIST* pList);
int getDateAndLookupTotal(RESERVATION_LIST* pList);
int getActiveReservations(RESERVATION_LIST* pList);
int getCurrentDateAsInteger();
RESERVATION* searchByName(RESERVATION_LIST* psList);

int main(void){
    RESERVATION_LIST pList= {NULL, NULL};
    int iChoice = 0;

    while(iChoice != 7){
        printMenu();
        scanf("%d", &iChoice);
        getchar();
        switch(iChoice) 
        {
            case 1:
                //Create new node
                makeReservation(&pList);
                break;
            case 2:
            if(isEmpty(&pList)){
                char* buff = "No reservations found in system registry";
                printMessage(buff);
            }else{
                deleteLastMadeReservation(&pList);
            }
                break;
            case 3:
                deleteOldReservations(&pList);
                break;
            case 4: 
                if(isEmpty(&pList)){
                    char* buff = "No reservations found, search aborted";
                    printMessage(buff);
                }else{
                    RESERVATION* searchResult = searchByName(&pList);
                    if(searchResult){
                        printReservation(searchResult);
                    }
                   
                }
                break;
            case 5: 
                getDateAndLookupTotal(&pList);
                break;
            case 6:
                if(getActiveReservations(&pList) == FALSE){
                    char* buff = "There are no active reservations";
                    printMessage(buff);
                }
                break;
            case 7:
            //Exit 
            //Clean up all reservations
            while(pList.psReservationTail != NULL){
                removeNode(&pList, pList.psReservationTail);
            }
            break;
        }
    }
    return 0;
}

void printMenu(){
    puts("");
    puts("\t****************************************************");
    puts("\t******                                        ******");
    puts("\t******   CSTARS TONIGHT RESERVATION SYSTEM    ******");
    puts("\t******                                        ******");
    puts("\t****************************************************");
    printf("\t1. New Reservation\n");
    printf("\t2. Delete last reservation\n");
    printf("\t3. Delete old reservations\n");
    printf("\t4. Search for reservation by name\n");
    printf("\t5. Get date total\n");
    printf("\t6. Print today's reservations\n");
    printf("\t7. Exit\n");
    printf("\t");
}

void printMessage(char* txt){
    puts("\n\t####################################################");
    puts("\t###                                              ###");
    printf("\t###   %40s   ###\n", txt);
    puts("\t###                                              ###");
    puts("\t####################################################\n");
    puts("\tPress any key to return to menu");
    getchar();
}

RESERVATION* searchByName(RESERVATION_LIST* pList){
    char name[MAX_SIZE_NAME];
    printf("\t%s\n", "What name is the reservation registered on?");
    fgets(name, MAX_SIZE_NAME, stdin);
    name[strcspn(name, "\r\n")] = 0;
    RESERVATION* searchResult = searchForReservationByName(pList, name);

    if(!searchResult){
        printf("No reservation found for %s\n", name);
        return NULL;
    }
    printf("\t%s\n", "Reservation found: ");
    
    return searchResult;
}

void makeReservation(RESERVATION_LIST* pList){
    int iRc = ERROR;
    RES_INFO* psResInfo = malloc(sizeof(RES_INFO));
    printf("\t%s", "Name of guest:");
    printf("%10s", "");
    fgets(psResInfo->szName, MAX_SIZE_NAME, stdin);
    psResInfo->szName[strcspn(psResInfo->szName, "\r\n")] = 0;
    printf("\t%s", "Room to reserve:");
    printf("%8s", "");
    fgets(psResInfo->szRoom, MAX_SIZE_ROOM, stdin);
    psResInfo->szRoom[strcspn(psResInfo->szRoom, "\r\n")] = 0;
    getchar();
    char dateString[11];
    printf("\tArrival\n\t(format: ddmmyyyy):");
    printf("%5s", "");
    fgets(dateString, 11, stdin);
    int iDate= atoi(dateString);
    int iCurrentDate = getCurrentDateAsInteger();
    if(!iDate){
        printf("\tError parsing date, returning to main menu\n");
        free(psResInfo);
        return;
    }
    printf("\tNumber of days:");
    int lengthOfStay;
    printf("%9s", "");
    scanf("%d", &lengthOfStay);
    float fPricePerNight;
    printf("\tPrice per night:");
    printf("%2s", "");
    scanf("%f", &fPricePerNight);
    getchar();
    
    psResInfo->iStartDate = iDate;
    psResInfo->iNightsStaying = lengthOfStay;
    psResInfo->fPricePerNight = fPricePerNight;

    RESERVATION* psCurrentRes = createNode(psResInfo);
    if(addToList(pList, psCurrentRes) == OK){
        printf("\n\tNew reservation registered! \n");
        printf("\t*****************************\n\n");
        printReservation(psCurrentRes);
        iRc = OK;
    }else{
        free(psResInfo);
        psResInfo = NULL;
    }
}

void deleteOldReservations(RESERVATION_LIST* pList){
    int date = getCurrentDateAsInteger();


    int iDeletedStays = deleteEndedStays(pList, date); 
    if(iDeletedStays >= 0){
        printf("\t%d reservations deleted.\n", iDeletedStays);
    }else{
        printf("\tError deleting reservations.\n");
    }
}

int getActiveReservations(RESERVATION_LIST *psList){
    int date = getCurrentDateAsInteger();

    RESERVATION* pThis = psList->psReservationHead;
    int isFound = FALSE;
    
    while(pThis != NULL){
        if(isReservationActiveOnDate(pThis, date)){
            printReservation(pThis);
            isFound = TRUE;
        }
        pThis = pThis->pNext;
    }

    return isFound;
}

void deleteLastMadeReservation(RESERVATION_LIST* psList){
    int iRc = OK;
    char cAnswer;
    RESERVATION* psLastReservationMade = psList->psReservationTail;
    printf("\tLast registered reservation: \n");
    printReservation(psLastReservationMade);
    printf("\tDelete this reservation? (y/n)\n\t");
    int esc = FALSE;

    while(esc != TRUE){
        scanf(" %c", &cAnswer);
        if(cAnswer == 'y' || (cAnswer + 0x20) == 'y'){
            iRc = removeNode(psList, psLastReservationMade);
            esc = TRUE;
            if(iRc == OK){
                printf("\tReservation deleted.\n");
            }else{
                printf("\tError deleting reservation.\n");
            }
        }else if(cAnswer == 'n' || (cAnswer +=0x20) == 'n'){
            printf("\tReturning to main menu.\n");
            esc = TRUE;
        }else{
            printf("You put in %c ( valid: y/n )", cAnswer);
        }
    }
}

int getDateAndLookupTotal(RESERVATION_LIST* psList){
    printf("\n\t-------------------------------------\n");
    printf("\tInput date to check total\n");
    printf("\tformat ddmmyyyy\n");
    printf("\t-------------------------------------\n\t");
    char buff[20];
    fgets(buff, 20, stdin);
    buff[strcspn(buff, "\r\n")] = 0;
    int date = atoi(buff);
    if(!date){
        printf("\tError parsing date, maybe check your input?\n");
        return ERROR;
    }
    int total = getTotalByDate(psList, date);
    printf("\tTotal %s: %d\n", buff, total);
}

int getCurrentDateAsInteger(){
    time_t t = time(NULL);
    char buff[20];
    struct tm tm = *localtime(&t);
    sprintf(buff, "%d%d%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    int date = atoi(buff);

    return date;
}

int getLowerCase(char c){
    if(c >= 'A' && c <= 'Z'){
        c += 0x20;;
    }
    
    return c;
}