#include <stdlib.h>
#include "lcd.h"

void LCD_Wrap();
void LCD_PutChar(char c);

void LCD_Text(const char *string);
void LCD_TextN(const char *string, size_t n);
void LCD_Print(const char *string);
void LCD_PrintN(const char *string, size_t n);

void LCD_TextMode(LCD_COLOR mode);
void LCD_TextLocate(int x, int y);


#define LCD_CHAR_WIDTH 3
#define LCD_CHAR_HEIGHT 5

static int LCD_text_X = 0;
static int LCD_text_Y = 0;
static LCD_COLOR LCD_text_mode = OR;

// pico8 style font
static const unsigned char LCD_font[][3] = {
	{0x00, 0x00, 0x00}, // 20
	{0x00, 0x17, 0x00}, // 21 !
	{0x03, 0x00, 0x03}, // 22 "
	{0x1F, 0x0A, 0x1F}, // 23 #
	{0x16, 0x1F, 0x0D}, // 24 $
	{0x19, 0x04, 0x13}, // 25 %
	{0x0A, 0x15, 0x0A}, // 26 &
	{0x02, 0x01, 0x00}, // 27 '
	{0x0E, 0x11, 0x00}, // 28 (
	{0x00, 0x11, 0x0E}, // 29 )
	{0x15, 0x0E, 0x15}, // 2a *
	{0x04, 0x0E, 0x04}, // 2b +
	{0x10, 0x08, 0x00}, // 2c ,
	{0x04, 0x04, 0x04}, // 2d -
	{0x00, 0x10, 0x00}, // 2e .
	{0x10, 0x0E, 0x01}, // 2f /
	{0x1F, 0x11, 0x1F}, // 30 0
	{0x11, 0x1F, 0x10}, // 31 1
	{0x1D, 0x15, 0x17}, // 32 2
	{0x11, 0x15, 0x1F}, // 33 3
	{0x07, 0x04, 0x1F}, // 34 4
	{0x17, 0x15, 0x1D}, // 35 5
	{0x1F, 0x14, 0x1C}, // 36 6
	{0x01, 0x01, 0x1F}, // 37 7
	{0x1F, 0x15, 0x1F}, // 38 8
	{0x07, 0x05, 0x1F}, // 39 9
	{0x00, 0x0A, 0x00}, // 3a :
	{0x10, 0x0A, 0x00}, // 3b ;
	{0x04, 0x0A, 0x11}, // 3c <
	{0x0A, 0x0A, 0x0A}, // 3d =
	{0x11, 0x0A, 0x04}, // 3e >
	{0x01, 0x15, 0x07}, // 3f ?
	{0x1F, 0x17, 0x17}, // 40 @
	{0x1F, 0x05, 0x1F}, // 41 A
	{0x1F, 0x15, 0x1B}, // 42 B
	{0x0E, 0x11, 0x11}, // 43 C
	{0x1F, 0x11, 0x1E}, // 44 D
	{0x1F, 0x15, 0x11}, // 45 E
	{0x1F, 0x05, 0x01}, // 46 F
	{0x1E, 0x11, 0x19}, // 47 G
	{0x1F, 0x04, 0x1F}, // 48 H
	{0x11, 0x1F, 0x11}, // 49 I
	{0x11, 0x1F, 0x01}, // 4a J
	{0x1F, 0x04, 0x1B}, // 4b K
	{0x1F, 0x10, 0x10}, // 4c L
	{0x1F, 0x03, 0x1F}, // 4d M
	{0x1F, 0x01, 0x1E}, // 4e N
	{0x1E, 0x11, 0x0F}, // 4f O
	{0x1F, 0x05, 0x07}, // 50 P
	{0x0E, 0x19, 0x17}, // 51 Q
	{0x1F, 0x05, 0x1B}, // 52 R
	{0x16, 0x15, 0x0D}, // 53 S
	{0x01, 0x1F, 0x01}, // 54 T
	{0x0F, 0x10, 0x1F}, // 55 U
	{0x0F, 0x10, 0x0F}, // 56 V
	{0x1F, 0x18, 0x1F}, // 57 W
	{0x1B, 0x04, 0x1B}, // 58 X
	{0x07, 0x18, 0x07}, // 59 Y
	{0x19, 0x15, 0x13}, // 5a Z
	{0x1F, 0x11, 0x00}, // 5b [
	{0x01, 0x0E, 0x10}, // 5c '\'
	{0x00, 0x11, 0x1F}, // 5d ]
	{0x02, 0x01, 0x02}, // 5e ^
	{0x10, 0x10, 0x10}, // 5f _
	{0x00, 0x01, 0x02}, // 60 `
	{0x1F, 0x05, 0x1F}, // 61 a
	{0x1F, 0x15, 0x1B}, // 62 b
	{0x0E, 0x11, 0x11}, // 63 c
	{0x1F, 0x11, 0x1E}, // 64 d
	{0x1F, 0x15, 0x11}, // 65 e
	{0x1F, 0x05, 0x01}, // 66 f
	{0x1E, 0x11, 0x19}, // 67 g
	{0x1F, 0x04, 0x1F}, // 68 h
	{0x11, 0x1F, 0x11}, // 69 i
	{0x11, 0x1F, 0x01}, // 6a j
	{0x1F, 0x04, 0x1B}, // 6b k
	{0x1F, 0x10, 0x10}, // 6c l
	{0x1F, 0x03, 0x1F}, // 6d m
	{0x1F, 0x01, 0x1E}, // 6e n
	{0x1E, 0x11, 0x0F}, // 6f o
	{0x1F, 0x05, 0x07}, // 70 p
	{0x0E, 0x19, 0x17}, // 71 q
	{0x1F, 0x05, 0x1B}, // 72 r
	{0x16, 0x15, 0x0D}, // 73 s
	{0x01, 0x1F, 0x01}, // 74 t
	{0x0F, 0x10, 0x1F}, // 75 u
	{0x0F, 0x10, 0x0F}, // 76 v
	{0x1F, 0x18, 0x1F}, // 77 w
	{0x1B, 0x04, 0x1B}, // 78 x
	{0x07, 0x18, 0x07}, // 79 y
	{0x19, 0x15, 0x13}, // 7a z
	{0x04, 0x1B, 0x11}, // 7b {
	{0x00, 0x1F, 0x00}, // 7c |
	{0x11, 0x1B, 0x04}, // 7d }
	{0x0C, 0x04, 0x06}, // 7e ~
	{0x00, 0x00, 0x00}, // 7f DEL
};
