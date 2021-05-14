#include "jvm.h"
#include "utils.h"

int main(int argc, char** argv){
    (void)argc;

    // Yeah this should never happen, but you never know.
    if(argc == 0){
        printf("Somehow this program was launched without an executable path defined.\n");
        return 1;
    }

    // MC doesn't even support other platforms as far as I know.
    if(!(IsWindows() || IsLinux() || IsXnu())){
        fprintf(stderr, "Your platform is not supported, please execute \"java -jar %s\"\n", argv[0]);
        return 1;
    }

    // FIXME, OSX support.
    if(IsXnu()){
        fprintf(stderr, "OSX is not support yet, for now please execute \"java -jar %s\"\n", argv[0]);
        return 1;
    }

    printf("Locating JVM...\n");

    JavaVersion jvmVersion;
    char* jvm = findJvm(&jvmVersion);
    if(!jvm){
        printf("Failed to locate JVM.\n");
        return 1;
    }

    printf("Found at %s\n", jvm);
    printf("Version %d.%d.%d\n", jvmVersion.feature, jvmVersion.interim, jvmVersion.update);
    
    // Should not happen, but you never know.
    if(!appendPath(jvm, IsWindows() ? "bin/javaw.exe" : "bin/java")){
        printf("Path too large");
        return 1;
    }

    printf("Launching fabric installer...\n");
    // Create a new argument list that include everything that was passed to us, to be nice.
    char** arguments = malloc(sizeof(char*) * (argc + 3));
    arguments[0] = jvm;
    arguments[1] = "-jar";
    memcpy(arguments + 2, argv, sizeof(char*) * argc);
    arguments[argc + 2] = NULL;

    // And execute!
    execv(jvm, arguments);
    free(arguments);
    free(jvm);

    return 0;
}
