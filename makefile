
# .PHONY used to indicate a certain target is not a file


# Marco for specifying inculde directories to the compiler
INCULDES = -Iheaders
# Marco for specifying library files to link against for the linker
LIBS = 
# Marco holding the cpp compiler
CXX = g++.exe
# Marco for specifying flags for the compiler
CXXFLAGS = -std=c++23 -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror -Wshadow -Wenum-compare
# Marco for specifying build configuration flags, empty for Release and not empty for debug
CONFIGURATIONFlAGS = DEBUG
# Marco for specifying flags for the preprocessor
CPPFLAGS = -O2 -DNDEBUG
# Marco for compiling .cpp files
COMPILE = -c
# Marco for linking object files
LINK = -o
# Marco for holding the target file name to compile, link and name the executable as
TARGET = main
# Marco for holding various object files to link against
OBJECT_FILES = main.o myfunctions1.o
# Marco for specifying command to remove files - shell specific 
RM = -del
# Marco for specifying the directory to change for make to run
SUBSYSTEMDIR = Game
PROGRAM_NAME = BLACKJACK


ifeq ($(CONFIGURATIONFlAGS), DEBUG)
CONFIGURATIONFlAGS = -g -ggdb
else
CONFIGURATIONFlAGS = $(CPPFLAGS)
endif

ifeq ($(PROGRAM_NAME), BLACKJACK)
OBJECT_FILES += card.o player.o dealer.o blackjack.o
endif
ifeq ($(PROGRAM_NAME), HI_LO)
OBJECT_FILES += hi_lo.o
endif

all: intro $(TARGET).exe

intro:
	echo [make.exe running]; Program Expected: $(PROGRAM_NAME).

$(TARGET).exe: $(TARGET).o $(OBJECT_FILES)
	$(CXX) $(CONFIGURATIONFlAGS) $(CXXFLAGS) $(LINK) $(TARGET) $(OBJECT_FILES)

$(TARGET).o: $(TARGET).cpp myfunctions1.h
	$(CXX) $(CONFIGURATIONFlAGS) $(CXXFLAGS) $(COMPILE) $(TARGET).cpp 

myfunctions1.o: myfunctions1.cpp myfunctions1.h
	$(CXX) $(CONFIGURATIONFlAGS) $(CXXFLAGS) $(COMPILE) myfunctions1.cpp

timer.o: timer.cpp timer.h
	$(CXX) $(CONFIGURATIONFlAGS) $(CXXFLAGS) $(COMPILE) timer.cpp


# Hi_lo object files
hi_lo.o: hi_lo.cpp random.h myfunctions1.h
	$(CXX) $(CONFIGURATIONFlAGS) $(CXXFLAGS) $(COMPILE) hi_lo.cpp
# Hi_lo object files

# BlackJack object files
card.o: card.cpp card.h random.h
	$(CXX) $(CONFIGURATIONFlAGS) $(CXXFLAGS) $(COMPILE) card.cpp

player.o: player.cpp player.h card.h myfunctions1.h
	$(CXX) $(CONFIGURATIONFlAGS) $(CXXFLAGS) $(COMPILE) player.cpp

dealer.o: dealer.cpp dealer.h player.h card.h myfunctions1.h
	$(CXX) $(CONFIGURATIONFlAGS) $(CXXFLAGS) $(COMPILE) dealer.cpp

blackjack.o: blackjack.cpp blackjack.h dealer.h player.h card.h myfunctions1.h 
	$(CXX) $(CONFIGURATIONFlAGS) $(CXXFLAGS) $(COMPILE) blackjack.cpp
# BlackJack object files


subsystem:
	cd $(SUBSYSTEMDIR) && $(MAKE)

.PHONY : clean

clean:
	$(RM) *.o $(TARGET).exe


# Unimportant targets
checkVersion: 
	$(CXX) --version

.cpp.o: 
	$(CXX) $(CXXFLAGS) $(COMPILE) *.cpp

.cpp: .cpp.o
	$(CXX) $(CXXFLAGS) $(LINK) $(TARGET) *.o

