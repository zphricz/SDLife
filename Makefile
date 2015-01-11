CXXFLAGS = -std=c++11 -Ofast -pg
LDFLAGS = -lSDL2
OS:= $(shell uname -s)

ifeq ($(OS), Darwin)
	LDFLAGS += -lSDL2main -framework Cocoa
	CXX = g++-4.9
endif
ifeq ($(OS), Linux)
	CXX = clang++
endif

life: main.o Game.o Screen.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o$@ $^

clean:
	rm -rf *.o life
