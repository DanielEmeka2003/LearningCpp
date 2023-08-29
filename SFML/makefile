# .PHONY used to indicate a certain target is not a file
# various marcos or variables
INCULDES = -I
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
CXX = g++
CXXFLAGS = -g -ggdb -std=c++23 -Wall -Wextra -Wsign-conversion -Werror -Wshadow -Wenum-compare
OBJ = clip.o LoadingText.o
UHPP = functions.hpp includes.hpp constants.hpp 
# you can use the del or erase keyword
RM = -del

all: clip.exe

clip.exe: $(OBJ)	
	$(CXX) $(CXXFLAGS) -o clip $(OBJ) $(LIBS)

clip.o: clip.cpp LoadingText.hpp
	$(CXX) $(CXXFLAGS) -c clip.cpp 

LoadingText.o: clip.cpp LoadingText.hpp 
	$(CXX) $(CXXFLAGS) -c LoadingText.cpp 

main.exe: main.o learning.o	
	$(CXX) $(CXXFLAGS) -o main main.o learning.o $(LIBS)

main.o: main.cpp 
	$(CXX) $(CXXFLAGS) -c main.cpp

learning.o: learning.cpp
	$(CXX) $(CXXFLAGS) -c learning.cpp

.PHONY : clean

clean:
	$(RM) *.o main.exe
