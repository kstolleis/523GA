# Melanie Moses' Lab University of New Mexico, 
# Author: Matthew Fricke
# Date: Feb 19th 2013
# Description: Makefile for AntBots Evolver 

# Program version (increment this every time you do 'make distrib')
VERSION=release1.4

# Compile-time flags
DBUG_CFLAGS+=-g -Wall -pedantic -fopenmp -c -Werror
CFLAGS+=-Wall -pedantic -fopenmp -c

# Link-time flags
LFLAGS= -lgomp -pthread

# Command for compilation
COMPILER=g++

# Command for linking
LINKER=g++

# Final program name
APP=antbots

# Get the OS name
UNAME := $(shell uname)

# Set appropriate compile and linker flags for Linux
ifeq ($(UNAME), Linux)
CFLAGS+=-DLINUX
LFLAGS+= -lGL -lglut -lGLU
endif

# Set appropriate compile and linker flags for Darwin (OS X)
ifeq ($(UNAME), Darwin)
CFLAGS+=-DDARWIN
LFLAGS+=-framework OpenGL -framework GLUT
endif

# APP specific object files
OFILES=main.o GA.o Field.o Colony.o GUI.o

# Files For Which Changes should force everything to be remade:
ALLDEP=Makefile GA.h GUI.h Colony.h Ant.h Field.h Utils.h Location.h

# Link the main app and put in the directory above the source directory
$(APP): $(OFILES) $(ALLDEP) 
	@echo Linking for $(UNAME)
	$(LINKER) $(OFILES) $(LFLAGS) -o $(APP)

# Compiling the source files
%.o: %.cpp $(ALLDEP)
	$(COMPILER) $(CFLAGS) $< -o $@

# cleaning
clean:
	rm -f antbots *.o core *~

distclean:      clean
	rm -f $(APP)

# Automate the creation of a distrbution archive
distrib:        distclean
	tar cvzf ../$(APP)-$(VERSION).tgz *





