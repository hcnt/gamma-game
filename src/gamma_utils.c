#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include "gamma_utils.h"

uint32_t parse32bitInt(char* string, char** stringLeftToParse, bool* errorFlag) {
    errno = 0;
    unsigned long n = strtoul(string, stringLeftToParse, 10);
    if (!isspace((*stringLeftToParse)[0])) {
        *errorFlag = true;
        return 0;
    }
    if (errno == EINVAL || errno == ERANGE || string == *stringLeftToParse) {
        *errorFlag = true;
        return 0;
    }
    if (n > UINT32_MAX) {
        *errorFlag = true;
        return 0;
    }
    return n;
}

bool checkIfStringHasOnlyWhiteChars(const char* string) {
    int i = 0;
    while (string[i] != '\0') {
        if (!isspace(string[i])) {
            return false;
        }
        i++;
    }
    return true;
}
