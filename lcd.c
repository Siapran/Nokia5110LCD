#include <stdlib.h>
#include <stdio.h>
#include "lcd.h"

#define sgn(x)  (x<0?-1:1)
#define rnd(x)  ((int)(x+0.5))
#define abs(x)  (x<0?-x:x)

//The DC pin tells the LCD if we are sending a command or data
typedef enum {
    COMMAND = 0,
    DATA = 1
} LCD_TYPE;

// screen buffer
// all drawing operations are made internally on the buffer
// the buffer is then sent to the LCD screen via LCD_Display()
static unsigned char LCD_buffer[LCD_WIDTH * LCD_HEIGHT / 8];
LCD_COLOR LCD_PixelGet(int x, int y);

#ifndef LCD_EMULATED

#include <wiringPi.h> // for core GPIO functions
#include <wiringShift.h> // for shiftOut()
/*
    GND > GND
    BL > 3.3V
    VCC > 3.3V

    CLK > SCLK
    DIN > MOSI
    DC > GPIO 23
    SCE > CE0
    RST > GPIO 24
 */
#define PIN_SCE   10 //Pin 3 on LCD -> sync
#define PIN_RESET 24 //Pin 4 on LCD -> reset
#define PIN_DC    23 //Pin 5 on LCD -> type
#define PIN_SDIN  12 //Pin 6 on LCD -> input
#define PIN_SCLK  14 //Pin 7 on LCD -> clock

#define PIN_LIGHT 0

#define LCD_StartTransmit() digitalWrite(PIN_SCE, LOW)
#define LCD_EndTransmit() digitalWrite(PIN_SCE, HIGH)
#define LCD_SetType(type) digitalWrite(PIN_DC, type)
#define LCD_SendByte(byte) shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, byte);

int LCD_Init() {

    wiringPiSetup(); //setup the wiringPi library to use GPIO mapping

    //Configure control pins
    pinMode(PIN_SCE, OUTPUT);
    pinMode(PIN_RESET, OUTPUT);
    pinMode(PIN_DC, OUTPUT);
    pinMode(PIN_SDIN, OUTPUT);
    pinMode(PIN_SCLK, OUTPUT);
    pinMode(PIN_LIGHT, OUTPUT) ;

    //Reset the LCD to a known state
    digitalWrite(PIN_RESET, LOW);
    digitalWrite(PIN_RESET, HIGH);

    LCD_StartTransmit();
    LCD_SetType(COMMAND);

    LCD_SendByte(0x21); //Tell LCD that extended commands follow
    LCD_SendByte(0xB0); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
    LCD_SendByte(0x04); //Set Temp coefficent
    LCD_SendByte(0x14); //LCD bias mode 1:48: Try 0x13 or 0x14

    LCD_SendByte(0x20); //We must send 0x20 before modifying the display control mode
    LCD_SendByte(0x0C); //Set display control, normal mode. 0x0D for inverse

    LCD_EndTransmit();

    return 0;
}

void LCD_Display() {
    size_t i;
    LCD_StartTransmit();

    // reset position to 0,0
    LCD_SetType(COMMAND);
    LCD_SendByte(0x40);
    LCD_SendByte(0x80);

    // send buffer
    LCD_SetType(DATA);
    for (i = 0 ; i < sizeof(LCD_buffer) ; ++i) {
        LCD_SendByte(LCD_buffer[i]);
    }

    LCD_EndTransmit();
}

void LCD_SetBacklight(int on) {
    digitalWrite(PIN_LIGHT, !!on);
}

#else

#include <SDL2/SDL.h>

static SDL_Window *win;
static SDL_Renderer *ren;

int LCD_Init() {

    if (SDL_Init(SDL_INIT_VIDEO))
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    win = SDL_CreateWindow("Nokia 5110 LCD", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, LCD_WIDTH * LCD_PIXEL_SIZE_X, LCD_HEIGHT * LCD_PIXEL_SIZE_Y, SDL_WINDOW_SHOWN);
    if (win == NULL)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL) {
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    return 0;
}

void LCD_Display() {
    int x, y;
    SDL_Event event;
    SDL_Rect pixel = {
        .x = 0, .y = 0,
        .w = LCD_PIXEL_SIZE_X, .h = LCD_PIXEL_SIZE_Y
    };
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderClear(ren);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    for (y = 0; y < LCD_HEIGHT; ++y) {
        for (x = 0; x < LCD_WIDTH; ++x) {
            if (LCD_PixelGet(x, y) == BLACK)
            {
                pixel.x = x * LCD_PIXEL_SIZE_X;
                pixel.y = y * LCD_PIXEL_SIZE_Y;
                SDL_RenderFillRect(ren, &pixel);
            }
        }
    }
    SDL_RenderPresent(ren);

    while ( SDL_PollEvent(&event) ) {
        switch (event.type) {
        case SDL_QUIT: {
            SDL_Quit();
            exit(0);
        }
        break;
        default:
            break;
        }
    }
}

void LCD_SetBacklight(int on) {
    printf("backlight state: %d\n", !!on);
}

#endif

// TODO: improve genericity
#define CLIP_X(pos) (pos < 0 ? 0 : (pos > LCD_WIDTH ? LCD_WIDTH : pos))
#define CLIP_Y(pos) (pos < 0 ? 0 : (pos > LCD_HEIGHT ? LCD_HEIGHT : pos))
#define TEST_X(pos) (pos < 0 ? 0 : (pos >= LCD_WIDTH ? 0 : 1))
#define TEST_Y(pos) (pos < 0 ? 0 : (pos >= LCD_HEIGHT ? 0 : 1))

void LCD_Clear() {
    size_t i;
    for (i = 0 ; i < sizeof(LCD_buffer) ; ++i) {
        LCD_buffer[i] = 0;
    }
}

void LCD_Invert() {
    size_t i;
    for (i = 0 ; i < sizeof(LCD_buffer) ; ++i) {
        LCD_buffer[i] ^= 0xFF;
    }
}

void LCD_Pixel(int x, int y, LCD_COLOR color) {
    unsigned char *ptr;
    if (TEST_X(x) && TEST_Y(y)) {
        ptr = &LCD_buffer[ x + (y / 8 * LCD_WIDTH) ];
        switch (color) {
        case WHITE:
            *ptr &= ~(1 << (y % 8)); // erase pixel
            break;
        case BLACK:
            *ptr |= 1 << (y % 8); // write requested pixel
            break;
        case XOR:
            *ptr ^= 1 << (y % 8); // write requested pixel
            break;
        default:
            break;
        }
    }
}

LCD_COLOR LCD_PixelGet(int x, int y) {
    if (!(TEST_X(x) && TEST_Y(y))) return UNDEFINED;
    return !! // double negation (forces true to 1 and false to 0)
           (
               LCD_buffer[ x + (y / 8 * LCD_WIDTH) ] & // location on buffer (y / 8)
               (1 << (y % 8)) // get the appropriate bit from the byte (y % 8)
           );
}

void LCD_DrawLine(int x1, int y1, int x2, int y2, LCD_COLOR color) {
    int i, x, y, dx, dy, sx, sy, cumul;
    x = x1;
    y = y1;
    dx = x2 - x1;
    dy = y2 - y1;
    sx = sgn(dx);
    sy = sgn(dy);
    dx = abs(dx);
    dy = abs(dy);
    LCD_Pixel(x, y, color);
    if (dx > dy)
    {
        cumul = dx / 2;
        for (i = 1 ; i < dx ; i++)
        {
            x += sx;
            cumul += dy;
            if (cumul > dx)
            {
                cumul -= dx;
                y += sy;
            }
            LCD_Pixel(x, y, color);
        }
    }
    else
    {
        cumul = dy / 2;
        for (i = 1 ; i < dy ; i++)
        {
            y += sy;
            cumul += dx;
            if (cumul > dy)
            {
                cumul -= dy;
                x += sx;
            }
            LCD_Pixel(x, y, color);
        }
    }
}

void LCD_HorizontalLine(int y, int x1, int x2, LCD_COLOR color) {
    int x;
    unsigned char byte;
    if (TEST_Y(y) && (TEST_X(x1) || TEST_X(x2)))
    {
        if (x1 > x2) {
            x = x1;
            x1 = x2;
            x2 = x;
        }
        x1 = CLIP_X(x1);
        x2 = CLIP_X(x2);

        byte = 1 << (y % 8);

        switch (color) {
        case WHITE:
            byte = ~byte;
            for (x = x1; x <= x2; ++x)
                LCD_buffer[ x + (y / 8 * LCD_WIDTH) ] &= byte;
            break;
        case BLACK:
            for (x = x1; x <= x2; ++x)
                LCD_buffer[ x + (y / 8 * LCD_WIDTH) ] |= byte;
            break;
        case XOR:
            for (x = x1; x <= x2; ++x)
                LCD_buffer[ x + (y / 8 * LCD_WIDTH) ] ^= byte;
            break;
        default:
            break;
        }
    }
}

void LCD_VerticalLine(int x, int y1, int y2, LCD_COLOR color) {
    int y;
    ++y2;
    if (TEST_X(x) && (TEST_Y(y1) || TEST_Y(y2)))
    {
        if (y1 > y2) {
            y = y1;
            y1 = y2;
            y2 = y;
        }
        y1 = CLIP_Y(y1);
        y2 = CLIP_Y(y2);

        switch (color) {
        case WHITE:
            if (y1 / 8 != y2 / 8) {
                LCD_buffer[ x + (y1 / 8 * LCD_WIDTH) ] &= ~(0xFF << (y1 % 8));
                LCD_buffer[ x + (y2 / 8 * LCD_WIDTH) ] &= (0xFF << (y2 % 8));
                for (y = (y1 / 8) + 1; y < (y2 / 8); y++) {
                    LCD_buffer[ x + (y * LCD_WIDTH) ] = 0;
                }
            }
            else LCD_buffer[ x + (y1 / 8 * LCD_WIDTH) ] &= ~((0xFF << (y1 % 8)) & ~(0xFF << (y2 % 8)));
            break;
        case BLACK:
            if (y1 / 8 != y2 / 8) {
                LCD_buffer[ x + (y1 / 8 * LCD_WIDTH) ] |= 0xFF << (y1 % 8);
                LCD_buffer[ x + (y2 / 8 * LCD_WIDTH) ] |= ~(0xFF << (y2 % 8));
                for (y = (y1 / 8) + 1; y < (y2 / 8); y++) {
                    LCD_buffer[ x + (y * LCD_WIDTH) ] = 0xFF;
                }
            }
            else LCD_buffer[ x + (y1 / 8 * LCD_WIDTH) ] |= (0xFF << (y1 % 8)) & ~(0xFF << (y2 % 8));
            break;
        case XOR:
            if (y1 / 8 != y2 / 8) {
                LCD_buffer[ x + (y1 / 8 * LCD_WIDTH) ] ^= 0xFF << (y1 % 8);
                LCD_buffer[ x + (y2 / 8 * LCD_WIDTH) ] ^= ~(0xFF << (y2 % 8));
                for (y = (y1 / 8) + 1; y < (y2 / 8); y++) {
                    LCD_buffer[ x + (y * LCD_WIDTH) ] ^= 0xFF;
                }
            }
            else LCD_buffer[ x + (y1 / 8 * LCD_WIDTH) ] ^= (0xFF << (y1 % 8)) & ~(0xFF << (y2 % 8));
            break;
        default:
            break;
        }
    }
}

void LCD_Blit(const unsigned char *buffer, int x1, int y1, int w, int h, LCD_COLOR mode) {
    int x, y;
    unsigned int index = 0;
    unsigned char byte = 0;
    unsigned char buffa, buffb;

    for (y = 0; y * 8 < h; ++y) {
        for (x = 0; x < w; ++x) {
            if (TEST_X(x + x1))
            {
                byte = buffer[index];
                if (y >= h / 8)
                {
                    switch (mode & MODE) {
                    case AND:
                        byte |= ~(0xFF >> (8 - (h % 8)));
                        break;
                    default:
                        byte &= 0xFF >> (8 - (h % 8));
                        break;
                    }
                }
                buffa = byte << ((y1 % 8 + 8) % 8);
                buffb = byte >> (8 - (y1 % 8 + 8) % 8);
                switch (mode & MODE) {
                case OR:
                    if (TEST_Y(y1 + y * 8))
                        LCD_buffer[x1 + x + (y * 8 + y1) / 8 * LCD_WIDTH] |= buffa;
                    if (TEST_Y(y1 + y * 8 + 8))
                        LCD_buffer[x1 + x + (y * 8 + y1 + 8) / 8 * LCD_WIDTH] |= buffb;
                    break;
                case AND:
                    buffa |= 0xFF >> (8 - y1 % 8);
                    buffb |= 0xFF << (y1 % 8);
                    if (TEST_Y(y1 + y * 8))
                        LCD_buffer[x1 + x + (y * 8 + y1) / 8 * LCD_WIDTH] &= buffa;
                    if (TEST_Y(y1 + y * 8 + 8))
                        LCD_buffer[x1 + x + (y * 8 + y1 + 8) / 8 * LCD_WIDTH] &= buffb;
                    break;
                case XOR:
                    if (TEST_Y(y1 + y * 8))
                        LCD_buffer[x1 + x + (y * 8 + y1) / 8 * LCD_WIDTH] ^= buffa;
                    if (TEST_Y(y1 + y * 8 + 8))
                        LCD_buffer[x1 + x + (y * 8 + y1 + 8) / 8 * LCD_WIDTH] ^= buffb;
                    break;
                default:
                    break;
                }
                if (mode & NOT)
                {
                    byte = 0xFF;
                    if (y >= h / 8)
                    {
                        byte = byte >> (8 - (h % 8));
                    }
                    if (TEST_Y(y1 + y * 8))
                        LCD_buffer[x1 + x + (y * 8 + y1) / 8 * LCD_WIDTH] ^= byte << (y1 % 8);
                    if (TEST_Y(y1 + y * 8 + 8))
                        LCD_buffer[x1 + x + (y * 8 + y1 + 8) / 8 * LCD_WIDTH] ^= byte >> (8 - y1 % 8);
                }
            }
            ++index;
        }
    }
}



void LCD_FillRect(int x1, int y1, int x2, int y2, LCD_COLOR color) {
    int x;
    if (x1 > x2) {
        x = x1;
        x1 = x2;
        x2 = x;
    }
    for (x = x1; x <= x2; ++x) {
        LCD_VerticalLine(x, y1, y2, color);
    }

}

void LCD_DrawRect(int x1, int y1, int x2, int y2, LCD_COLOR color) {
    LCD_VerticalLine(x1, y1 + 1, y2, color);
    LCD_VerticalLine(x2, y1, y2 - 1, color);
    LCD_HorizontalLine(y1, x1, x2 - 1, color);
    LCD_HorizontalLine(y2, x1 + 1, x2, color);
}

void LCD_DrawCircle(int x, int y, int radius, LCD_COLOR color) {
    int plot_x, plot_y, d;

    if (radius < 0) return;
    plot_x = 0;
    plot_y = radius;
    d = 1 - radius;

    LCD_Pixel(x, y + plot_y, color);
    if (radius)
    {
        LCD_Pixel(x, y - plot_y, color);
        LCD_Pixel(x + plot_y, y, color);
        LCD_Pixel(x - plot_y, y, color);
    }
    while (plot_y > plot_x)
    {
        if (d < 0)
            d += 2 * plot_x + 3;
        else
        {
            d += 2 * (plot_x - plot_y) + 5;
            plot_y--;
        }
        plot_x++;
        if (plot_y >= plot_x)
        {
            LCD_Pixel(x + plot_x, y + plot_y, color);
            LCD_Pixel(x - plot_x, y + plot_y, color);
            LCD_Pixel(x + plot_x, y - plot_y, color);
            LCD_Pixel(x - plot_x, y - plot_y, color);
        }
        if (plot_y > plot_x)
        {
            LCD_Pixel(x + plot_y, y + plot_x, color);
            LCD_Pixel(x - plot_y, y + plot_x, color);
            LCD_Pixel(x + plot_y, y - plot_x, color);
            LCD_Pixel(x - plot_y, y - plot_x, color);
        }
    }
}

void LCD_FillCircle(int x, int y, int radius, LCD_COLOR color) {
    int plot_y, plot_x, d;

    if (radius < 0) return;
    plot_y = 0;
    plot_x = radius;
    d = 1 - radius;

    LCD_VerticalLine(x, y - plot_x, y + plot_x, color);
    while (plot_x > plot_y)
    {
        if (d < 0)
            d += 2 * plot_y + 3;
        else {
            d += 2 * (plot_y - plot_x) + 5;
            plot_x--;
            LCD_VerticalLine(x + plot_x + 1, y - plot_y, y + plot_y, color);
            LCD_VerticalLine(x - plot_x - 1, y - plot_y, y + plot_y, color);
        }
        plot_y++;
        if (plot_x >= plot_y)
        {
            LCD_VerticalLine(x + plot_y, y - plot_x, y + plot_x, color);
            LCD_VerticalLine(x - plot_y, y - plot_x, y + plot_x, color);
        }
    }
}

void LCD_Scroll(int x, int y) {
    unsigned char buffer[LCD_WIDTH * LCD_HEIGHT / 8];
    int i;
    for (i = 0; i < LCD_WIDTH * LCD_HEIGHT / 8; ++i)
    {
        buffer[i] = LCD_buffer[i];
    }
    LCD_Clear();
    LCD_Blit(buffer, x, y, LCD_WIDTH, LCD_HEIGHT, OR);
}