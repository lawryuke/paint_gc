CXX = g++
CXXFLAGS = -Wall -std=c++14 -Iinclude
LDFLAGS = -lGL -lGLU -lglut

TARGET = build/paint
SRCS = src/main.cpp src/App.cpp src/Shape.cpp src/Transform2D.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

.PHONY: all clean test
test:
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o build/scanline_test tests/scanline_test.cpp src/Shape.cpp src/Transform2D.cpp $(LDFLAGS)
	./build/scanline_test

$(TARGET): $(OBJS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)
	@echo "Limpiando archivos objeto..."
	rm -f $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf build
