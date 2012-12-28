#include "file.h"

#include <stdio.h>

unsigned int File_Load(const char* fileName, char* buffer, size_t* size) {
    FILE* file;
    size_t result;
    
    if (!fileName)
        return 0;
    
    file = fopen(fileName, "rb");
    if (!file) {
        printf("File_Load: Could not open file %s (fopen)", fileName);
        return 0;
    }
    
    /* Get file size */
    fseek(file, 0, SEEK_END);
    (*size) = (size_t)ftell(file);
    rewind(file);
    
    if (!(*size))
    {
        printf("File_Load: Could not open file %s (size is 0)", fileName);
        fclose(file);
        return 0;
    }
    
    buffer = (char*)malloc(sizeof(char) * (*size));
    if (!buffer) {
        printf("File_Load: Failed to allocate buffer for file %s (size %u)", fileName, (*size));
        fclose(file);
        return 0;
    }
    
    result = fread(buffer, sizeof(char), (*size), file);
    if (result != (*size)) {
        printf("File_Load: Could not read the same number of bytes (size != result) for file %s (size %u, result %u)", fileName, (*size), result);
        free(buffer);
        buffer = NULL;
        fclose(file);
        return 0;
    }
    
    fclose(file);
    return 1;
    
}

unsigned int File_Save(const char* fileName, const char* buffer, size_t size) {
    FILE* file;
    size_t result;
    
    if (!fileName || !buffer || !size)
        return 0;
    
    file = fopen(fileName, "wb");
    
    if (!file) {
        printf("File_Save: Could not open file %s (fopen)", fileName);
        return 0;
    }
    
    result = fwrite(buffer, sizeof(char), size, file);
    if (result != size) {
        fclose(file);
        printf("File_Save: Could not write the same number of bytes (size != result) for file %s (size %u, result %u)", fileName, size, result);
        return 0;
    }
    
    fclose(file);
    return 1;
}

unsigned int File_Remove(const char* fileName) {
    return !remove(fileName);
}