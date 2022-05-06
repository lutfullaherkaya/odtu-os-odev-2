OBJS	= main.o Er.o hw2_output.o Mintika.o HataAyiklama.o Emirler.o
SOURCE	= main.cpp Er.cpp hw2_output.c Mintika.cpp HataAyiklama.cpp Emirler.cpp
HEADER	= main.h Er.h hw2_output.h Mintika.h HataAyiklama.h Emirler.h
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

HataAyiklama.o: HataAyiklama.cpp
	$(CC) $(FLAGS) HataAyiklama.cpp

Emirler.o: Emirler.cpp
	$(CC) $(FLAGS) Emirler.cpp


clean:
	rm -f $(OBJS) $(OUT)
