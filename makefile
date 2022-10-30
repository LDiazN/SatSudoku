CC := g++
CFLAGS := -Wall -std=c++17 -pthread
TARGET := SatSudoku
TARGET_DEBUG := SatSudokuDebug

# $(wildcard *.cpp /xxx/xxx/*.cpp): get all .cpp files from the current directory and dir "/xxx/xxx/"
SRCS := $(wildcard src/*.cpp)
# $(patsubst %.cpp,%.o,$(SRCS)): substitute all ".cpp" file name strings to ".o" file name strings
OBJS := $(patsubst src/%.cpp,bin/%.o,$(SRCS))
OBJS_DEBUG := $(patsubst src/%.cpp,debug/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

bin/%.o: src/%.cpp
	mkdir --parents bin
	$(CC) $(CFLAGS) -c $< 
	mv *.o bin/

clean:
	rm -rf $(TARGET) $(TARGET_DEBUG) bin debug
	
.PHONY: all clean

debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(OBJS_DEBUG)
	$(CC) -o $@ $^ $(CFLAGS) -g

debug/%.o: src/%.cpp
	mkdir --parents debug
	$(CC) $(CFLAGS) -c $< -g
	mv *.o debug/