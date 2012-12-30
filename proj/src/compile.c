#include <stdio.h>
#include <stdlib.h>

int Load(FILE* file, char** buffer, int* size)
{
    size_t result;

    // Get file size
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);

    if (!*size)
        return 1;

    printf("size: %d", *size);
        
    *buffer = (char*)malloc(*size * sizeof(char));
    if (!*buffer)
        return 1;

    result = fread(*buffer, sizeof(char), *size, file);
    if (result != *size)
        return 1;

    return 0;
}

char* compile(const char* file_name)
{
	FILE *fp;
    int fp_size;
	int status, i;
	char *result;
    int res;
    char command[1000] = "gcc -o ";
    char line[80];
    char * file_base_name = (char*)strrchr(file_name, '/');
    char * file_dot = (char*)strrchr(file_name, '.');
    
    
    
    strncat(command, file_base_name+1, file_dot - file_base_name - 1); 
    *file_dot = '.';
    strcat(command, " "); 
    strcat(command, file_name);
    
    
    printf("%s\n", command);
    
	/* Open the command for reading. */
	fp = (FILE*)popen(command, "r");
	if (fp == NULL) {
		printf("Failed to run command\n");
		exit;
	}
    
    Load(fp, &result, &i);
    
	pclose(fp);

    printf("%d\n", i);
    
	return result;
}
