CC=gcc
CFLAGS= -W -Wall -Os
LDFLAGS= -Os
EXEC= ball clock maze
SRC= $(wildcard *.c) $(wildcard **/*.c)
OBJ= $(SRC:.c=.o)
LCD_SRC= $(wildcard lcd/*.c)
LCD_OBJ= $(LCD_SRC:.c=.o)
EMULATED= false

.PHONY: all clean mrproper



all: emulated physical

emulated: LIBS = -lSDL2 -D LCD_EMULATED  

physical: LIBS = -lwiringPi
 
emulated physical: $(EXEC)

ball: ball.o $(LCD_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

clock: clock.o $(LCD_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

maze: maze.o $(LCD_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)



%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(LIBS)


clean:
	@rm -rf $(OBJ)

mrproper: clean
	@rm -rf $(EXEC)


lcd/font.o: lcd/font.h lcd/lcd.h
lcd/lcd.o: lcd/lcd.h
all: lcd/lcd.h lcd/font.h

