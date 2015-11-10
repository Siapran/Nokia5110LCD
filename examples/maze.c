#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "lcd/lcd.h"
#include "lcd/font.h"

#define FRAMES_PER_SECOND 30
static const unsigned char lines[][4] = {
	{9, 12, 6, 3}, {9, 3, 6, 12}
};

static unsigned char line[21];

struct Timer
{
	clock_t start, stop;
};

void timer_start(struct Timer *self) {
	self->start = clock();
}

long int timer_get_ticks(struct Timer *self) {
	return (clock() - self->start) * 1000 / CLOCKS_PER_SEC;
}

long int timer_stop(struct Timer *self) {
	self->stop = clock();
	return (self->stop - self->start) * 1000 / CLOCKS_PER_SEC;
}

void update_line() {
	for (int i = 0; i < sizeof(line); ++i)
	{
		line[i] = rand() % 2;
	}
}

void draw_line(int y) {
	for (int i = 0; i < sizeof(line); ++i)
	{
		LCD_Blit(lines[line[i]], i * 4, y, 4, 4, OR);
	}
}

int main()
{
	struct Timer fps;
	int offset = 0;

	srand(time(NULL));

	if (LCD_Init() != 0) {
		printf("Error initializing LCD\n");
		return 1;
	}

	LCD_SetBacklight(1);

	for (;;) {

		timer_start(&fps);

		offset = (offset) % 4 + 1;
		if (offset == 1)
		{
			update_line();
		}
		LCD_Scroll(0, -1);

		draw_line(LCD_HEIGHT - offset);

		LCD_Display();

		if (timer_get_ticks(&fps) < 1000 / FRAMES_PER_SECOND)
		{
			usleep(1000 * ( 1000 / FRAMES_PER_SECOND ) - timer_get_ticks(&fps));
		}

	}

	return 0;
}