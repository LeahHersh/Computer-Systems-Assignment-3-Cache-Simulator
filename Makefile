CXX = g++
CONSERVATIVE_FLAGS = -std=c99 -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)
LIBS += -lm


csim: main.o
	$(CXX) -o csim $^ $(LIBS)

main.o: main.cpp
	$(CXX) -c main.cpp

clean:
	rm -f *.o *~