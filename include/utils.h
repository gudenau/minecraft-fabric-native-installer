#pragma once

#ifdef __cplusplus
extern "C"{
#endif

/**
 * The maximum path size that this program supports.
 * 
 * A (hopefully) large enough size that most things won't have issues.
 */
#define MAX_PATH 4096

/**
 * Checks if a file exists.
 * 
 * @param path The path of the file to check
 * @return non-0 if the file exists, 0 if it does not
 */
int fileExists(const char* path);
/**
 * Appends the supplied path onto the end of path in buffer.
 * 
 * @param buffer The first segment of the path as well as the destination
 * @param path The path to append
 * @return non-zero if the path was appened, 0 if it was not
 */
int appendPath(char* buffer, const char* path);

#ifdef __cplusplus
}
#endif
