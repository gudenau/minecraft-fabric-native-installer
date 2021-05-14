#include "jvm.h"
#include "utils.h"

char* findJvm(JavaVersion* version){
    // If we are given a specific Java environment to use, use it
    char* env = getenv("JAVA_HOME");
    if(env && false){
        JavaVersion envVer;
        // If it's valid of course!
        if(getJavaVersion(env, &envVer)){
            memcpy(version, &envVer, sizeof(JavaVersion));
            char* path = (char*)malloc(MAX_PATH + 1);
            memset(path, 0, MAX_PATH + 1);
            strncpy(path, env, MAX_PATH + 1);
            return path;
        }
    }

    // Some work vairables to cache the latest version found
    JavaVersion newestVersion;
    memset(&newestVersion, 0, sizeof(JavaVersion));
    char* newestVersionPath = nullptr;

    // Needed for cross platform
    const char* roots[16];
    if(IsWindows()){
        int i = 0;
        roots[i++] = "C:\\Program Files\\AdoptOpenJDK";
        roots[i++] = "C:\\Program Files (x86)\\Minecraft Launcher\\runtime";
        roots[i++] = nullptr;
    }else{
        int i = 0;
        roots[i++] = "/lib/jvm";
        roots[i++] = "/usr/lib/jvm";
        roots[i++] = nullptr;
    }

    // Check each root...
    for(int rootIndex = 0; roots[rootIndex]; rootIndex++){
        auto root = roots[rootIndex];

        // Prepare for iterating the root
        auto directory = opendir(root);
        struct dirent* currentDir;
        // Returns null when it's done
        while((currentDir = readdir(directory))){
            // Append the dir name to the root
            char installDir[MAX_PATH + 1];
            memset(installDir, 0, MAX_PATH + 1);
            strcpy(installDir, root);
            if(!appendPath(installDir, currentDir->d_name)){
                // Path is too long, ignore
                continue;
            }

            // Get the Java version and make sure it's valid
            JavaVersion javaVersion;
            memset(&javaVersion, 0, sizeof(JavaVersion));
            if(getJavaVersion(installDir, &javaVersion)){
                // Check if it's newer
                if(isNewer(&newestVersion, &javaVersion)){
                    // It is, cache current version info
                    memcpy(&newestVersion, &javaVersion, sizeof(JavaVersion));
                    if(!newestVersionPath){
                        newestVersionPath = (char*)malloc(MAX_PATH + 1);
                    }
                    memset(newestVersionPath, 0, MAX_PATH + 1);
                    memcpy(newestVersionPath, installDir, MAX_PATH + 1);
                }
            }
        }
        // Close the directory since we are done with it
        closedir(directory);
    }

    // Is the latest we found 9 or newer? (9 changed versioning)
    if(newestVersion.feature > 1){
        memcpy(version, &newestVersion, sizeof(JavaVersion));
        return newestVersionPath;
    }else if(newestVersion.feature == 1){
        // We are before 9, check for 8
        if(newestVersion.interim > 8){
            // We have 8, we can work with that
            memcpy(version, &newestVersion, sizeof(JavaVersion));
            return newestVersionPath;
        }
    }

    // Too old, bail
    if(newestVersionPath){
        free(newestVersionPath);
    }
    memset(version, 0, sizeof(JavaVersion));

    return nullptr;
}

int getJavaVersion(const char* path, JavaVersion* version){
    // Make sure the path is not too long for our buffers
    if(strlen(path) >= MAX_PATH){
        return false;
    }

    // Check if the path exists
    if(!fileExists(path)){
        return false;
    }

    // Get the location of the java executable
    char newPath[MAX_PATH + 1];
    memset(newPath, 0, MAX_PATH + 1);
    memcpy(newPath, path, strlen(path));
    if(!appendPath(newPath, IsWindows() ? "bin/javaw.exe" : "bin/java")){
        // Path too large
        return false;
    }

    // Does it exist?
    if(!fileExists(newPath)){
        return false;
    }

    // Cool, now create a pipe for capturing output
    int link[2];
    if(pipe(link)){
        return false;
    }

    // Fork to execute the JVM
    auto pid = fork();
    if(pid == -1){
        fprintf(stderr, "Failed to fork!\n");
        abort();
    }
    if(pid == 0){
        // We are the child, redirect stderr and ask for the Java version
        dup2(link[1], STDERR_FILENO);
        close(link[0]);
        close(link[1]);
        execl(newPath, newPath, "-version", nullptr);
        abort();
    }

    // We are the parent, read the version info from the child
    close(link[1]);
    char pipeBuffer[4096 + 1];
    memset(pipeBuffer, 0, 4096 + 1);
    auto byteCount = read(link[0], pipeBuffer, 4096);

    // The version info is always in quotes, how nice
    auto firstQuote = strchr(pipeBuffer, '"');
    if(!firstQuote){
        return false;
    }
    auto secondQuote = strchr(firstQuote + 1, '"');
    if(!secondQuote){
        return false;
    }

    // Copy the version string to another buffer
    // TODO Get rid of this, was required for eariler testing but not anymore
    size_t versionStringSize = (size_t)secondQuote - (size_t)firstQuote - 1;    
    char versionString[256];
    memset(versionString, 0, 256);
    memcpy(versionString, firstQuote + 1, versionStringSize);

    // Extract version info
    auto token = strtok(versionString, ".");
    if(!token){
        // Huh, must not be Java after all
        return false;
    }else{
        // Get the feature version from the release info
        version->feature = atoi(token);
        token = strtok(nullptr, ".");
        if(token){
            // Get the interim version from the release info
            version->interim = atoi(token);
            token = strtok(nullptr, ".");
            // And finally the update version
            version->update = token ? atoi(token) : 0;
        }else{
            version->interim = 0;
            version->update = 0;
        }
    }

    return true;
}

int isNewer(JavaVersion* versionA, JavaVersion* versionB){
    // A stupid version check implementation
    if(versionA->feature > versionB->feature){
        return false;
    }else if(versionA->feature == versionB->feature){
        if(versionA->interim > versionB->interim){
            return false;
        }else if(versionA->interim == versionB->interim){
            if(versionA->update > versionB->update){
                return false;
            }
        }
    }

    return true;
}
