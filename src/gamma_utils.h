/**
 * @file
 * Helper functions for parsing
 */

#ifndef GAMMA_GAMMA_UTILS_H
#define GAMMA_GAMMA_UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * Prints message about successful operation
 * @param line - line where operation was defined
 */
inline void opCompleted(int line) {
    printf("OK %d\n", line);
};

/**
 * Prints message about unsuccessful operation
 * @param line - line where operation was defined
 */
inline void error(int line) {
    fprintf(stderr, "ERROR %d\n", line);
};

/**
 * checks if string has only white characters
 * @param string - string to check
 * @return if contains only white characters
 */
bool checkIfStringHasOnlyWhiteChars(const char* string);

/**
 * Parse number from string
 * @param[in] string - input string
 * @param[out] stringLeftToParse - part of string that wasn't parsed
 * @param[out] errorFlag - if error happened while parsing
 * @return parsed number
 */
uint32_t parse32bitInt(char* string, char** stringLeftToParse, bool* errorFlag);

#endif //GAMMA_GAMMA_UTILS_H
