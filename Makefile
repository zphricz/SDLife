
CXXFLAGS=-std=c++11 -Ofast -pg
# MAC
LDFLAGS=-lSDL -lSDLmain -framework Cocoa
CXX=g++-4.9
# Linux
# LDFLAGS=-lSDL
# CXX=g++

life: quickcg.o main.o
	$(CXX) quickcg.o main.o $(CXXFLAGS) $(LDFLAGS) -olife

clean:
	rm -rf *.o life
