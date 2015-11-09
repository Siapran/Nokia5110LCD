CC=gcc
CFLAGS=-W -Wall -lSDL2 -I../src/
LDFLAGS=-lSDL2 -I../src/
EXEC=ball
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)


all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)


.PHONY: clean rmproper

clean:
	@rm -rf $(OBJ)

rmproper: clean
	@rm -rf $(EXEC)


