
# .PHONY used to indicate a certain target is not a file
system = linux
includes = -Iheader -I"C:/Program Files/Boost"
libs = 
cxx = g++
#cxxflags = -std=c++23 -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror -Wshadow -Wenum-compare
cxxflags = -std=c++23 -pedantic-errors -Wall -Werror -Wenum-compare
configurationflags = debug
preprocessorflags = -E
compile = -c
target = main
obj_files = main.o misc_utility.o
target_dependacies = header/misc_utility.h
rm = -del
subsystem = sfml

# defines the name of the program you wish to compile
program_name = ncc

ifeq ($(system), linux)
includes = -Iheader -I/usr/local/boost_1_85_0
rm = -rm
endif

ifeq ($(configurationflags), debug)
configurationflags = -g -ggdb
else
configurationflags =  -O2 -DNDEBUG
endif


ifeq ($(program_name), fiftheenpuzzle)
obj_files += point2d.o board.o fiftheenpuzzle.o u8char.o
target_dependacies += header/board.h header/point2d.h header/random.h header/fiftheenpuzzle.h header/u8char.h
endif

ifeq ($(program_name), filediscoveryinput) #deprecated
obj_files += filediscoveryinput.o
#target_dependacies
endif

ifeq ($(program_name), pl0c)
obj_files += readfile.o pl0_lexer.o pl0_parser.o
#target_dependacies
endif

ifeq ($(program_name), ncc)
obj_files += ncLexer.o ncParser.o
target_dependacies += header/ncLexer.h header/ncLog.h header/ncParser.h header/misc_utility.h header/u8char.h
endif

#taget definitions start here:

all: intro $(target)

nccTest: #target is no longer used
	main main.nc && gcc -m32 assembly.s -o asm && asm

intro:
	@echo "\033[33;1;52mmake running\nCompiling package: { $(program_name) }\n───────────────────────────────────\033[0m"

#Base Targets───────────────────────────────────────────────────────────────────────
$(target): $(obj_files)
	$(cxx) $(includes) $(configurationflags) $(cxxflags) -o $(target) $(obj_files)

$(target).o: $(target).cpp $(target_dependacies)
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) $(target).cpp 

misc_utility.o: misc_utility.cpp header/misc_utility.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) misc_utility.cpp
#Base Targets────────────────────────────────────────────────────────────────────────


point2d.o: point2d.cpp header/point2d.h 
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) point2d.cpp


#FiftheenPuzzle object files────────────────────────────────────────────────────────────────────────
board.o: board.cpp header/board.h header/point2d.h header/random.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) board.cpp

fiftheenpuzzle.o: fiftheenpuzzle.cpp header/misc_utility.h header/fiftheenpuzzle.h header/board.h header/point2d.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) fiftheenpuzzle.cpp
#FiftheenPuzzle object files────────────────────────────────────────────────────────────────────────


#FileDiscovery object files────────────────────────────────────────────────────────────────────────
filediscoveryinput.o: filediscoveryinput.cpp header/filediscoveryinput.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) filediscoveryinput.cpp
#FileDiscovery object files────────────────────────────────────────────────────────────────────────


#Pl0c────────────────────────────────────────────────────────────────────────
pl0_lexer.o: pl0_lexer.cpp header/pl0_lexer.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) pl0_lexer.cpp

pl0_parser.o: pl0_parser.cpp header/pl0_parser.h header/pl0_lexer.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) pl0_parser.cpp
#Pl0c────────────────────────────────────────────────────────────────────────


#ncc────────────────────────────────────────────────────────────────────────
ncLexer.o: ncLexer.cpp header/ncLexer.h header/ncLog.h header/misc_utility.h header/u8char.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) ncLexer.cpp

ncParser.o: ncParser.cpp header/ncParser.h header/ncLexer.h  header/ncLog.h header/misc_utility.h header/u8char.h #header/ncAst.h header/ncNonTerminal.h header/ncTerminal.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) ncParser.cpp

ncCodeGen.o: ncCodeGen.cpp header/ncCodeGen.h header/ncAst.h header/ncNonTerminal.h header/ncTerminal.h header/u8char.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) ncCodeGen.cpp

ncAst.o: ncAst.cpp header/ncAst.h header/ncNonTerminal.h header/ncTerminal.h header/u8char.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) ncAst.cpp
#ncc────────────────────────────────────────────────────────────────────────


subsystem:
	cd $(subsystemDIR) && $(MAKE)

.PHONY : clean

clean:
	$(rm) *.o $(target)


# Unimportant targets
checkVersion: 
	$(cxx) --version

.cpp.o: 
	$(cxx) $(cxxflags) $(compile) *.cpp

.cpp: .cpp.o
	$(cxx) $(cxxflags) $(compile) $(target) *.o
