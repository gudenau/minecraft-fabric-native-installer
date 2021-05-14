# NativeFabricInstaller

A wrapper intended for the Fabric installer. (Can be used for other things too!)

### What's it do?

This program will search for a JVM that is installed on the machine and execute a bundled jar file. If the JAVA_HOME environment variable is set and a valid JVM it will use that instead. 

Linux search paths:
- /lib/jvm
- /usr/lib/jvm

Windows search paths:
- C:\Program Files\AdoptOpenJDK
- C:\Program Files (x86)\Minecraft Launcher\runtime

OSX search paths:
- TODO

### What's so special about this wrapper?

It uses the same binary for all supported systems.

You read that right, the same file can be used on Linux, Windows and OSX. Just be warned that once you run it you can't run it on a different platform, it self-modifies to run a little faster the next time it's executed.

### Special thanks to “[jart](https://github.com/jart)”
Without [cosmopolitan](https://github.com/jart/cosmopolitan) this project would have taken a lot longer to do and the file sizes would also be much larger. Thanks a bundle!
