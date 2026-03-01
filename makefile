CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGET = sim
SRCS = main.cpp LoadBalancer.cpp WebServer.cpp Switch.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) log.txt

run: $(TARGET)
	./$(TARGET)