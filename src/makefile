OBJS	= main.o CPU.o Platform.o
SOURCE	= main.cpp CPU.cpp Platform.cpp
HEADER	= CPU.h Platform.h
OUT	= chip8.exe
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -IC:\SDL2-w64\include -LC:\SDL2-w64\lib -lSDL2main -lSDL2 -o $(OUT) $(LFLAGS)

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp 

CPU.o: CPU.cpp
	$(CC) $(FLAGS) CPU.cpp

Platform.o: Platform.cpp
	$(CC) $(FLAGS) Platform.cpp

clean:
	rm -f $(OBJS) $(OUT)