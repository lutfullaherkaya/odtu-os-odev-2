OBJS	= main.o Er.o hw2_output.o Mintika.o
SOURCE	= main.cpp Er.cpp hw2_output.c Mintika.cpp
HEADER	= main.h Er.h hw2_output.h Mintika.h
OUT	= hw2
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = -lpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp

Er.o: Er.cpp
	$(CC) $(FLAGS) Er.cpp

hw2_output.o: hw2_output.c
	$(CC) $(FLAGS) hw2_output.c

Mintika.o: Mintika.cpp
	$(CC) $(FLAGS) Mintika.cpp


clean:
	rm -f $(OBJS) $(OUT)
