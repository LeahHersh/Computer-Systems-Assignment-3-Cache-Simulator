CC = g++
CONSERVATIVE_FLAGS = -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)
LIBS += -lm


csim: main.o
	$(CC) $(CFLAGS) -o csim $^ $(LIBS)

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

clean:
	rm -f *.o *~