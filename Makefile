ELFNAME = life
SUBMODULES = Threadpool Screen
OBJDIR = objs
CXXFLAGS = -std=c++14 -Ofast -Wall -Werror
CXXFLAGS = -std=c++14 -g -Wall -Werror -fsanitize=address
LDFLAGS = -lSDL2
OS = $(shell uname -s)
SRC = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.cpp, $(OBJDIR)/%.o, $(SRC))
DEPS = $(patsubst %.cpp, $(OBJDIR)/%.d, $(SRC))
CXXFLAGS += $(foreach SUBMOD, $(SUBMODULES), -I $(SUBMOD))

ifeq ($(OS), Darwin)
	CXX = clang++
endif
ifeq ($(OS), Linux)
	CXX = g++
	LDFLAGS += -Wl,--no-as-needed -lpthread
endif

all: $(ELFNAME)

$(ELFNAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o$@ $^ $(LDFLAGS) 

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -MMD -MP $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

-include $(DEPS)

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(OBJDIR)/*.d
	rm -f $(ELFNAME)
