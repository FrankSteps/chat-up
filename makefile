CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SRC = backend/main.cpp backend/server.cpp app/window.cpp app/user.cpp
BUILD = builds
TARGET = $(BUILD)/chatup

all: $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(TARGET): $(SRC) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run:
	sudo fuser -k 5000/tcp || true
	./$(TARGET)

clean:
	rm -rf $(BUILD)