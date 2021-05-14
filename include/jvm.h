#pragma once

#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
    /**
     * The feature version of a JVM.
     * 
     * I.E. 9 for Java 9, 10 for Java 10.
     */
    int feature;
    /**
     * The interim version of a JVM.
     */
    int interim;
    /**
     * The update version of a JVM.
     * 
     * Incremented for every update of a feature version.
     */
    int update;
} JavaVersion;

/**
 * Attempts to find the latest JVM that is in a handful of platform-specific directories.
 * 
 * @param version The version of the latest found JVM
 * @return The path to the latest found JVM, or null if none where found
 */
char* findJvm(JavaVersion* version);

/**
 * Runs a possible JVM and extracts it's version info.
 * 
 * @param path The path of the JVM to test
 * @param version The version of the tested JVM
 * @return non-0 if it's a valid JVM install, 0 if it is not
 */
int getJavaVersion(const char* path, JavaVersion* version);
/**
 * A basic version check, compares versionA and versionB.
 * 
 * @param versionA One version to check
 * @param versionB The other version to check
 * @return non-0 if versionB is newer than versionA, 0 if not
 */
int isNewer(JavaVersion* versionA, JavaVersion* versionB);

#ifdef __cplusplus
}
#endif
