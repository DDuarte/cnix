#ifndef FILE_H_
#define FILE_H_

#include <stdio.h>

/** @defgroup file file
 * @{
 * Common functions for manipulating files
 */

/**
 * @brief Loads a file
 *
 * @param fileName File name to load (full path)
 * @param buffer Buffer to write file's content to (out)
 * @param size Size of the buffer (out)
 * @return 0 if operation is successful, non-0 otherwise
 */
int File_Load(const char* fileName, char** buffer, size_t* size);

/**
 * @brief Saves a file
 *
 * @param fileName File name to load (full path)
 * @param buffer Buffer to save file's content to (in)
 * @param size Size of the buffer (in)
 * @return 0 if operation is successful, non-0 otherwise
 */
int File_Save(const char* fileName, const char* buffer, size_t size);

/**
 * @brief Deletes a file
 *
 * @param fileName File name to delete (full path)
 * @return 0 if operation is successful, non-0 otherwise
 */
int File_Remove(const char* fileName);

/**@}*/

#endif
