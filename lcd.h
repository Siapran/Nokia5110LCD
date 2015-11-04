#ifndef LCD_H
#define LCD_H

#define LCD_EMULATED // to emulate LCD display using SDL

//You may find a different size screen, but this one is 84 by 48 pixels
#define LCD_X     84
#define LCD_Y     48

#define LCD_PIXEL_SIZE_X 6
#define LCD_PIXEL_SIZE_Y 6

typedef enum {
    WHITE = 0,  // 0000
    BLACK = 1,  // 0001
    XOR   = 2,  // 0010
    OR    = 3,  // 0011
    AND   = 4,  // 0100
    NXOR  = 10, // 1010
    NOR   = 11, // 1011
    NAND  = 12, // 1100
    // bitmasks
    MODE  = 7,  // 0111
    NOT   = 8,  // 1000 
} LCD_COLOR;

int LCD_Init();
void LCD_Display();
void LCD_SetBacklight(int on);
void LCD_Clear();
void LCD_Invert();
void LCD_Pixel(int x, int y, LCD_COLOR color);
LCD_COLOR LCD_PixelGet(int x, int y);
void LCD_DrawLine(int x1, int y1, int x2, int y2, LCD_COLOR color);
void LCD_HorizontalLine(int y, int x1, int x2, LCD_COLOR color);
void LCD_VerticalLine(int x, int y1, int y2, LCD_COLOR color);
void LCD_FillRect(int x1, int y1, int x2, int y2, LCD_COLOR color);
void LCD_DrawRect(int x1, int y1, int x2, int y2, LCD_COLOR color);
void LCD_DrawCircle(int x, int y, int radius, LCD_COLOR color);
void LCD_FillCircle(int y, int x, int radius, LCD_COLOR color);
void LCD_Blit(const unsigned char *buffer, int x1, int y1, int w, int h, LCD_COLOR mode);

#endif
