GCC = gcc
CONSERVATIVE_FLAGS = -std=c99 -lm -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)


csim: main.o
	$(GCC) -o csim $^

main.o: main.cpp
	$(GCC) -c main.cpp

clean:
	rm -f *.o *~