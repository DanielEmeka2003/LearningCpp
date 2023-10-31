
# .PHONY used to indicate a certain target is not a file

includes = -Iheader
libs = 
cxx = g++.exe
cxxflags = -std=c++23 -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror -Wshadow -Wenum-compare
configurationflags = debug
preprocessorflags = -E
compile = -c
target = main
obj_files = main.o myfunctions1.o
rm = -del
subsystem = sfml
program_name = ncc


ifeq ($(configurationflags), debug)
configurationflags = -g -ggdb
else
configurationflags =  -O2 -DNDEBUG
endif

ifeq ($(program_name), blackjack)
obj_files += card.o player.o dealer.o blackjack.o
endif
ifeq ($(program_name), hi_lo)
obj_files += hi_lo.o
endif
ifeq ($(program_name), fiftheenpuzzle)
obj_files += point2d.o board.o fiftheenpuzzle.o
endif
ifeq ($(program_name), filediscoveryinput)
obj_files += filediscoveryinput.o
endif
ifeq ($(program_name), pl0c)
obj_files += readfile.o pl0_lexer.o pl0_parser.o
endif
ifeq ($(program_name), ncc)
obj_files += readfile.o drawboxaroundtext.o ncLexer.o ncParser.o ncCodeGen.o
endif

all: intro $(target).exe

intro:
	echo [make.exe running]; Program Expected: $(program_name)

#Base Targets────────────────────────────────────────────────────────────────────────
$(target).exe: $(obj_files)
	$(cxx) $(includes) $(configurationflags) $(cxxflags) -o $(target) $(obj_files)

$(target).o: $(target).cpp header/myfunctions1.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) $(target).cpp 

myfunctions1.o: myfunctions1.cpp header/myfunctions1.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) myfunctions1.cpp
#Base Targets────────────────────────────────────────────────────────────────────────


timer.o: timer.cpp header/timer.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) timer.cpp

point2d.o: point2d.cpp header/point2d.h 
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) point2d.cpp

readfile.o: readfile.cpp header/readfile.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) readfile.cpp

drawboxaroundtext.o: drawboxaroundtext.cpp header/drawboxaroundtext.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) drawboxaroundtext.cpp



# Hi_lo object files────────────────────────────────────────────────────────────────────────
hi_lo.o: hi_lo.cpp header/random.h header/myfunctions1.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) hi_lo.cpp
# Hi_lo object files────────────────────────────────────────────────────────────────────────


# BlackJack object files────────────────────────────────────────────────────────────────────────
card.o: card.cpp header/card.h header/random.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) card.cpp

player.o: player.cpp header/player.h header/card.h header/myfunctions1.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) player.cpp

dealer.o: dealer.cpp header/dealer.h header/player.h header/card.h header/myfunctions1.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) dealer.cpp

blackjack.o: blackjack.cpp header/blackjack.h header/dealer.h header/player.h header/card.h header/myfunctions1.h 
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) blackjack.cpp
# BlackJack object files────────────────────────────────────────────────────────────────────────


#FiftheenPuzzle object files────────────────────────────────────────────────────────────────────────
board.o: board.cpp header/board.h header/point2d.h header/random.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) board.cpp

fiftheenpuzzle.o: fiftheenpuzzle.cpp header/myfunctions1.h header/fiftheenpuzzle.h header/board.h header/point2d.h
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
ncLexer.o: ncLexer.cpp header/ncLexer.h header/drawboxaroundtext.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) ncLexer.cpp

ncParser.o: ncParser.cpp header/ncParser.h header/ncLexer.h header/drawboxaroundtext.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) ncParser.cpp

ncCodeGen.o: ncCodeGen.cpp header/ncCodeGen.h
	$(cxx) $(includes) $(configurationflags) $(cxxflags) $(compile) ncCodeGen.cpp
#ncc────────────────────────────────────────────────────────────────────────


subsystem:
	cd $(subsystemDIR) && $(MAKE)

.PHONY : clean

clean:
	$(rm) *.o $(target).exe


# Unimportant targets
checkVersion: 
	$(cxx) --version

.cpp.o: 
	$(cxx) $(cxxflags) $(compile) *.cpp

.cpp: .cpp.o
	$(cxx) $(cxxflags) $(compile) $(target) *.o

