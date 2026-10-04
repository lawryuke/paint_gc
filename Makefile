CXX = g++
CXXFLAGS = -Wall -std=c++14 -Iinclude
LDFLAGS = -lGL -lGLU -lglut

TARGET = build/paint
SRCS = src/main.cpp src/App.cpp src/Shape.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

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
