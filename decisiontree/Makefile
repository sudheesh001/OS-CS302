OBJS = main.o # Form this object from the main function.
CC = g++ # Use the g++ compiler from CC module and assign default compiling to g++
CFLAGS =-c -g -std=gnu++0x -Wall -O0 # No optimization O0 and using standard GNU+0x
LFLAGS =
LIBS =

dtl: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $@ 

main.o : main.cpp
	$(CC) $(CFLAGS) -o $@ main.cpp

.PHONY :  clean

clean:
	rm -f dtl $(OBJS)

run: dtl training.txt
	./dtl training.txt

test: dtl test.txt training.txt
	./dtl training.txt test.txt
