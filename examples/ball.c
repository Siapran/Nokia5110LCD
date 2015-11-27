#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "lcd/lcd.h"
#include "lcd/font.h"

#define FRAMES_PER_SECOND 30
#define BILLION 1000000000L

struct Timer
{
	struct timespec start, stop;
};

long get_msecs(struct timespec *start, struct timespec *end) {
	return (BILLION * (end->tv_sec - start->tv_sec) + end->tv_nsec - start->tv_nsec) / 1000000;
}

void timer_start(struct Timer *self) {
	clock_gettime(CLOCK_MONOTONIC, &self->start);
}

long timer_get_msecs(struct Timer *self) {
	clock_gettime(CLOCK_MONOTONIC, &self->stop);
	return get_msecs(&self->start, &self->stop);
}

typedef struct Buffer {
    int w;
    int h;
    const unsigned char data[];
} Buffer;

Buffer image = {
    .w = 16,
    .h = 16,
    .data = {
        0xC0, 0xF0, 0xFC, 0xFC, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFC, 0xFC, 0xF0, 0xC0,
        0x03, 0x0F, 0x3F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0x3F, 0x0F, 0x03,
    }
};

typedef struct Ball {
    int x;
    int y;
    int vx;
    int vy;
    int r;
} Ball;

static Ball balls[8];

void create_ball(Ball *ball) {
    ball->vx = ((rand() % 2) * 2 - 1) * (rand() % 2 + 1);
    ball->vy = ((rand() % 2) * 2 - 1) * (rand() % 2 + 1);
    ball->r = rand() % 8 + 4;
    ball->x = rand() % (LCD_WIDTH - ball->r * 2) + ball->r;
    ball->y = rand() % (LCD_HEIGHT - ball->r * 2) + ball->r;
}

void update_ball(Ball *ball) {
    ball->x += ball->vx;
    ball->y += ball->vy;
    if (ball->x - ball->r < 0 || ball->x + ball->r >= LCD_WIDTH) {
        ball->vx = -ball->vx;
        ball->x += 2 * ball->vx;
    }
    if (ball->y - ball->r < 0 || ball->y + ball->r >= LCD_HEIGHT) {
        ball->vy = -ball->vy;
        ball->y += 2 * ball->vy;
    }
}

void draw_ball(Ball *ball) {

    LCD_FillCircle(ball->x, ball->y, ball->r , XOR);

}

int main()
{
    struct Timer fps;
	
	int size = sizeof(balls) / sizeof(*balls);
    int i, j;

    const char string[] = "Hello World! How are you? ";
    int len = strlen(string);

    srand(time(NULL));

    for (i = 0; i < size; ++i)
    {
        create_ball(&balls[i]);
    }

    if (LCD_Init() != 0) {
        printf("Error initializing LCD\n");
        return 1;
    }

    LCD_SetBacklight(1);

    j = 0;

    for (;;) {

		timer_start(&fps);

        for (i = 0; i < size; ++i)
        {
            update_ball(&balls[i]);
        }

        LCD_Clear();

        for (i = 0; i < size; ++i)
        {
            draw_ball(&balls[i]);
        }

        LCD_FillRect(0, (LCD_HEIGHT - LCD_CHAR_HEIGHT) / 2 - 2, LCD_WIDTH, (LCD_HEIGHT - LCD_CHAR_HEIGHT) / 2 + LCD_CHAR_HEIGHT + 1, BLACK);
        LCD_FillRect(0, (LCD_HEIGHT - LCD_CHAR_HEIGHT) / 2 - 1, LCD_WIDTH, (LCD_HEIGHT - LCD_CHAR_HEIGHT) / 2 + LCD_CHAR_HEIGHT, WHITE);

        LCD_TextLocate(j, (LCD_HEIGHT - LCD_CHAR_HEIGHT) / 2);
        LCD_Text(string);
        LCD_TextLocate(j + len * 4, (LCD_HEIGHT - LCD_CHAR_HEIGHT) / 2);
        LCD_Text(string);

        j = (j - 1) % (len * 4);

        LCD_Display();

        if (timer_get_msecs(&fps) < 1000 / FRAMES_PER_SECOND) {
			usleep(1000 * ( 1000 / FRAMES_PER_SECOND ) - timer_get_msecs(&fps));
		}
    }

    return 0;
}
