CC=gcc
CFLAGS= -W -Wall -lSDL2 -Os
LDFLAGS= -lSDL2 -Os
EXEC= ball clock maze
SRC= $(wildcard *.c) $(wildcard **/*.c)
OBJ= $(SRC:.c=.o)
LCD_SRC= $(wildcard lcd/*.c)
LCD_OBJ= $(LCD_SRC:.c=.o)

.PHONY: all clean mrproper ball clock 

all: $(EXEC)

ball: ball.o $(LCD_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clock: clock.o $(LCD_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

maze: maze.o $(LCD_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)


clean:
	@rm -rf $(OBJ)

mrproper: clean
	@rm -rf $(EXEC)


lcd/font.o: lcd/font.h lcd/lcd.h
lcd/lcd.o: lcd/lcd.h
all: lcd/lcd.h lcd/font.h
