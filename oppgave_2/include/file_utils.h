#ifndef FILE_UTILS_H
#define FILE_UTILS_H

int readFileContentToCharArray(const char* filename, char** pcBuff);
int writeHexRepresentationToTextFile(const char* filename, char** ppcText);

#endif