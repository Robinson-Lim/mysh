OBJS	= main.o tokenizer.o utils.o
SOURCE	= main.c tokenizer.c utils.c
HEADER	= tokenizer.h utils.h
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

clean:
	rm -f $(OBJS) $(OUT)