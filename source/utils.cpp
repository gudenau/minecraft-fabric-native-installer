#include "utils.h"

int fileExists(const char* path){
    // Simply a wrapepr
    return !access(path, F_OK);
}

int appendPath(char* buffer, const char* path){
    // Create a new buffer
    char temp[MAX_PATH + 1];
    memset(temp, 0, MAX_PATH + 1);

    // Print the existing and new path into the buffer
    auto result = snprintf(temp, MAX_PATH, "%s/%s", buffer, path);
    if(result >= MAX_PATH){
        // Uh-oh, too big
        return false;
    }

    // Copy the buffer over the original. snprintf doesn't like overlapping buffers
    memcpy(buffer, temp, result);

    return true;
}
