OBJS	= main.o tokenizer.o utils.o internal_command.o
SOURCE	= main.c tokenizer.c utils.c internal_command.c
HEADER	= tokenizer.h utils.h internal_command.h
OUT	= mysh
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c 

tokenizer.o: tokenizer.c
	$(CC) $(FLAGS) tokenizer.c 

utils.o: utils.c
	$(CC) $(FLAGS) utils.c 

internal_command.o: internal_command.c
	$(CC) $(FLAGS) internal_command.c

clean:
	rm -f $(OBJS) $(OUT)