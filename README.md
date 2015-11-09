# Nokia5110LCD

A simple graphic library to use or emulate a [Nokia 5110 LCD screen](http://www.sparkfun.com/products/10168).
You can either run it on a raspberry pi with wiringPi, or emulate the display using SDL2.

![demo](https://dl.dropboxusercontent.com/u/40487730/Pictures/gif/lcd.gif)

## Usage

This code is meant to run on a raspberri pi wired up to a Nokia 5110 LCD. Please make sure you know what you're doing when you wire the LCD to your pi.

If you want to test a project directly on your computer, define LCD\_EMULATED in lcd.h, and link against the SDL2 libraries. This will set up an emulated LCD Screen in a SDL window. The only functions changed are LCD\_Init() and LCD_Display(). All graphic functions write in a buffer, and the buffer is sent to the LCD (or rendered with SDL) via LCD\_Display()

LCD\_Blit() takes a buffer using the same format as the screen buffer. You can generate these buffers using [this utility](https://github.com/Siapran/Nokia5110LCD-Image-Encoder).

## Modules

* **lcd.h**: Core display functionalities and graphic primitives
* **font.h**: Text rendering utilities

## Authors

* Siapran Candoris ([Retroactive](retroactive.me) 2015)
* VynDragon

Inspired by the work of:

* Nathan Seidle (Spark Fun Electronics 2011), published under the "Beerware licence"
* Pierre "PierrotLL" Le Gall, published as "free software"

## Licence

This code is public domain.
