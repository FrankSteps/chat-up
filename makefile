CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SRC = backend/main.cpp backend/server.cpp backend/window.cpp 
BUILD = builds
TARGET = $(BUILD)/chatup

all: $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(TARGET): $(SRC) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run:
	./$(TARGET)

clean:
	rm -rf $(BUILD)