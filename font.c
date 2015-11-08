#include "font.h"

void LCD_Wrap() {
	if (LCD_text_X + LCD_CHAR_WIDTH >= LCD_WIDTH) {
		LCD_text_X = 0;
		LCD_text_Y += LCD_CHAR_HEIGHT + 1;
	}
	if (LCD_text_Y + LCD_CHAR_HEIGHT >= LCD_HEIGHT) {
		LCD_Scroll(0, LCD_HEIGHT - (LCD_text_Y + LCD_CHAR_HEIGHT));
		LCD_text_Y = LCD_HEIGHT - LCD_CHAR_HEIGHT;
	}
}

void LCD_PutChar(char c) {
	LCD_Blit(LCD_font[c - 0x20], LCD_text_X, LCD_text_Y, LCD_CHAR_WIDTH, LCD_CHAR_HEIGHT, LCD_text_mode);
	LCD_text_X += LCD_CHAR_WIDTH + 1;
}

void LCD_Text(const char *string) {
	while (*string) {
		LCD_PutChar(*string++);
	}
}

void LCD_TextN(const char *string, size_t n) {
	while (*string && n--) {
		LCD_PutChar(*string++);
	}
}

void LCD_Print(const char *string) {
	while (*string) {
		LCD_Wrap();
		LCD_PutChar(*string++);
	}
}

void LCD_PrintN(const char *string, size_t n) {
	while (*string && n--) {
		LCD_Wrap();
		LCD_PutChar(*string++);
	}
}

void LCD_TextMode(LCD_COLOR mode) {
	LCD_text_mode = mode;
}

void LCD_TextLocate(int x, int y) {
	LCD_text_X = x;
	LCD_text_Y = y;
}