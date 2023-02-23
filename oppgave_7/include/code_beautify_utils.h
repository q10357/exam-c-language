#ifndef CODE_BEAUTIFY_UTILS_H
#define CODE_BEAUTIFY_UTILS_H

int writeToBufferUntilDelimiter(char* source, char* dest, char delimiter);
int getNumberOfOccurances(char* pszStr, char* pszSubString, int* iNum);
int readFile(const char *pszFileName, char **ppcBuf, long *plSize);

#endif