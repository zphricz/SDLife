CXXFLAGS = -std=c++11 -Ofast
LDFLAGS = -lSDL2
OS:= $(shell uname -s)

ifeq ($(OS), Darwin)
	LDFLAGS += -framework Cocoa
	CXX = g++-4.9
endif
ifeq ($(OS), Linux)
	CXX = clang++
	CXX = g++
endif

life: main.o Screen.o Game.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o$@ $^

clean:
	rm -rf *.o life
