
CXXFLAGS=-std=c++11 -Ofast -pg
# MAC
LDFLAGS=-lSDL -lSDLmain -framework Cocoa
CXX=g++-4.9
#CXX=clang++
# Linux
# LDFLAGS=-lSDL
# CXX=g++

life: quickcg.o main.o Game.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o$@ $^

#life: quickcg.o main.o Game.o

clean:
	rm -rf *.o life
