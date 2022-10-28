CC := g++
CFLAGS := -Wall -g
TARGET := SatSudoku

# $(wildcard *.cpp /xxx/xxx/*.cpp): get all .cpp files from the current directory and dir "/xxx/xxx/"
SRCS := $(wildcard src/*.cpp)
# $(patsubst %.cpp,%.o,$(SRCS)): substitute all ".cpp" file name strings to ".o" file name strings
OBJS := $(patsubst src/%.cpp,bin/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

bin/%.o: src/%.cpp
	mkdir --parents bin
	$(CC) $(CFLAGS) -c $< 
	mv *.o bin/

clean:
	rm -rf $(TARGET) bin/*.o
	
.PHONY: all clean