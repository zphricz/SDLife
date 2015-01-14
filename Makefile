CXXFLAGS = -std=c++11 -Ofast -Wall -Werror
LDFLAGS = -lSDL2
OS:= $(shell uname -s)

ifeq ($(OS), Darwin)
	CXX = g++-4.9
endif
ifeq ($(OS), Linux)
	CXX = g++
endif

life: main.o Screen.o Game.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o$@ $^

clean:
	rm -rf *.o life
