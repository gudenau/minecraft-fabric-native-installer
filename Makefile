# Determines the names of a bunch of files
TARGET		:=	NativeFabricLauncher
# Version number. Major, minor, patch
# Bump patch every release
VERSION		:=	0.0.0
# Fabric installer version
FVERSION	:=	0.7.3

DIR_SRC		:=	source
DIR_INC		:=	include
DIR_BLD		:=	build
DIR_OUT		:=	out
DIR_COSMO	:=	cosmopolitan
DIR_RELEASE	:=	release

# Make should have this as a built-in
rwildcard	=	$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

FILES_C		:=	$(call rwildcard,$(DIR_SRC),*.c)
FILES_CXX	:=	$(call rwildcard,$(DIR_SRC),*.cpp)
FILES_O		:=	$(patsubst $(DIR_SRC)/%.c,$(DIR_BLD)/%.c.o,$(FILES_C)) \
				$(patsubst $(DIR_SRC)/%.cpp,$(DIR_BLD)/%.cpp.o,$(FILES_CXX))
# The jar file to bundle with the executable
FILE_JAR	:=	fabric-installer-$(FVERSION).jar

CC			:=	gcc
CXX			:=	g++
LD			:=	gcc
OBJCP		:=	objcopy
MKDIR		:=	mkdir
ZIP			:=	zip
7ZIP		:=	7z

# Enable all warnings, extra warnings, warnings as errors, optimize for space, debug info, static executable, no standard libs
# Disable red zone (funny stack stuff), not sure, not sure, and compile without linking
FLAGS_CCOM	:=	-Wall -Wextra -Werror -Os -g -static -nostdlib -nostdinc \
				-mno-red-zone -fno-omit-frame-pointer -pg -c \
				-include $(DIR_COSMO)/cosmopolitan.h \
				$(addprefix -I,$(DIR_INC))
FLAGS_CC	:=	$(FLAGS_CCOM)
# Disable funny C++ stuff that breaks the C ABI compat
FLAGS_CXX	:=	$(FLAGS_CCOM) \
				-fno-exceptions -fno-rtti
# Make the executable non-relocatable and some linker stuff
# Use bdf as a linker backend and don't use standard libs
FLAGS_LD	:=	$(DIR_COSMO)/crt.o $(DIR_COSMO)/ape.o $(DIR_COSMO)/cosmopolitan.a -fno-pie -no-pie -Wl,-T,$(DIR_COSMO)/ape.lds \
				-fuse-ld=bfd -nostdlib
FLAGS_OBJCP	:=	-S -O binary
FLAGS_MKDIR	:=	-p
FLAGS_ZIP	:=	-9
FLAGS_7ZIP	:=	a -mx9 -mmt4

# Makes other things shorter and easier to modify
# Release archives will be NativeFabricLauncher-version-fabric installer version.extension
RELEASE_BASE:=	$(TARGET)-$(VERSION)-$(FVERSION)

# Just make the executable and everything that needs
all: $(DIR_OUT)/$(TARGET).com

# Create all of the release archives
release: $(DIR_RELEASE)/$(RELEASE_BASE).zip $(DIR_RELEASE)/$(RELEASE_BASE).7z

# Create a zip release
$(DIR_RELEASE)/$(RELEASE_BASE).zip: $(DIR_OUT)/$(TARGET).com
	@$(MKDIR) $(FLAGS_MKDIR) $(@D)
	$(ZIP) $(FLAGS_ZIP) $@ $^

# Create a 7Z release
$(DIR_RELEASE)/$(RELEASE_BASE).7z: $(DIR_OUT)/$(TARGET).com
	@$(MKDIR) $(FLAGS_MKDIR) $(@D)
	$(7ZIP) $(FLAGS_7ZIP) $@ $^

# Deletes all the build artifacts
clean:
	$(RM) $(FLAGS_RM) $(FILES_O) $(DIR_OUT)/$(TARGET).com $(DIR_BLD)/$(TARGET).com.dbg (DIR_BLD)/$(TARGET).com

# FIXME, this probably doesn't work on non-unix machiens
# Generates the final executable with the jar appended to it
$(DIR_OUT)/$(TARGET).com: $(DIR_BLD)/$(TARGET).com $(FILE_JAR)
	$(shell cat $(DIR_BLD)/$(TARGET).com $(FILE_JAR) > $@)
	$(shell chmod +x $@)

# Creates the base executable without the jar
$(DIR_BLD)/$(TARGET).com: $(DIR_BLD)/$(TARGET).com.dbg
	@$(MKDIR) $(FLAGS_MKDIR) $(@D)
	$(OBJCP) $(FLAGS_OBJCP) $^ $@

# Unsure why this is needed, the cosmopolitan example has it though
$(DIR_BLD)/$(TARGET).com.dbg: $(FILES_O)
	@$(MKDIR) $(FLAGS_MKDIR) $(@D)
	$(LD) -o $@ $^ $(FLAGS_LD)

# Compile c files into o.c files in the build dir
$(DIR_BLD)/%.c.o: $(DIR_SRC)/%.c
	@$(MKDIR) $(FLAGS_MKDIR) $(@D)
	$(CC) $(FLAGS_CC) -o $@ $^

# Compile c++ files into o.cpp files in the build dir
$(DIR_BLD)/%.cpp.o: $(DIR_SRC)/%.cpp
	@$(MKDIR) $(FLAGS_MKDIR) $(@D)
	$(CXX) $(FLAGS_CXX) -o $@ $^
