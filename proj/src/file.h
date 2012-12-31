#ifndef FILE_H_
#define FILE_H_

#include <stdio.h>

int File_Load(const char* fileName, char** buffer, size_t* size);
int File_Save(const char* fileName, const char* buffer, size_t size);
int File_Remove(const char* fileName);

#endif
