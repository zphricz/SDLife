
CXXFLAGS=-O3
LDFLAGS=-lSDL
CXX=g++

life: quickcg.o main.o
	$(CXX) quickcg.o main.o $(CXXFLAGS) $(LDFLAGS) -olife

quickcg.o: quickcg.cpp
	$(CXX) quickcg.cpp -c $(CXXFLAGS) -oquickcg.o

main.o: main.cpp
	$(CXX) main.cpp -c $(CXXFLAGS) -omain.o

clean:
	rm -rf *.o life
