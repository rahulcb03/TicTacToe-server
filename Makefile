CC = gcc
CFLAGS = -std=c99 -Wall -g -fsanitize=address,undefined -pthread

OBJ_TTTS = ttts.o
OBJ_TTT = ttt.o

all: ttts ttt

ttts: $(OBJ_TTTS)
	$(CC) $(CFLAGS) -o $@ $^

ttt: $(OBJ_TTT)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o ttts ttt
