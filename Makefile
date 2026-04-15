CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -Wno-unused-variable -Wuninitialized -Werror=vla
LDFLAGS = -lSDL2 -lSDL2_ttf -lSDL2_mixer -lm

SRC = src/main.c src/tetris.c src/ui.c src/utils.c src/score.c
OBJ = $(SRC:.c=.o)

all: main

main: $(OBJ)
	$(CC) $(OBJ) -o main $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

clean:
	rm -f $(OBJ) main

