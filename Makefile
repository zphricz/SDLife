
CXXFLAGS=-O3 -std=c++11
LDFLAGS=-lSDL
CXX=g++

life: quickcg.o main.o
	$(CXX) quickcg.o main.o $(CXXFLAGS) $(LDFLAGS) -olife

clean:
	rm -rf *.o life
