#include "font.h"

void LCD_PutChar(char c) {
	if (LCD_text_X + LCD_CHAR_WIDTH >= LCD_WIDTH) {
		LCD_text_X = 0;
		LCD_text_Y += LCD_CHAR_HEIGHT + 1;
	}
	if (LCD_text_Y + LCD_CHAR_HEIGHT >= LCD_HEIGHT) {
		LCD_Scroll(0, LCD_HEIGHT - (LCD_text_Y + LCD_CHAR_HEIGHT));
		LCD_text_Y = LCD_HEIGHT - LCD_CHAR_HEIGHT;
	}
	LCD_Blit(LCD_font[c - 0x20], LCD_text_X, LCD_text_Y, LCD_CHAR_WIDTH, LCD_CHAR_HEIGHT, LCD_text_mode);
	LCD_text_X += LCD_CHAR_WIDTH + 1;
}

void LCD_Text(const char *string, int x, int y, LCD_COLOR mode) {
	if (x >= 0) LCD_text_X = x;
	if (y >= 0) LCD_text_Y = y;
	if (mode != UNDEFINED) LCD_text_mode = mode;
	while (*string) {
		LCD_PutChar(*string++);
	}
}