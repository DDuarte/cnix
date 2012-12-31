#ifndef COMPILE_H_
#define COMPILE_H_

/** @defgroup compile compile
 * @{
 * Functions for compiling code files created with the editor
 */

/**
 * @brief Executes gcc on a file and returns the output of the compilation
 *
 * @param file_name File name to compile (full path)
 * @return String with the output of the compilation
 */
char* compile(const char* file_name);

/**@}*/

#endif
