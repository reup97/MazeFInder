#ifndef __MAZE_FINDER_H
#define __MAZE_FINDER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>
#include "lcd_image.h"
#include <StackArray.h>

#define SD_CS 5
#define TFT_CS 6
#define TFT_DC 7
#define TFT_RST 8
#define TFT_WIDTH 128
#define TFT_HEIGHT 160
#define JOY_SEL 9
#define JOY_VERT_ANALOG 0
#define JOY_HORIZ_ANALOG 1
// Only care about joystick movement if
// position is JOY_CENTRE +/- JOY_DEADZONE
#define JOY_DEADZONE 64 
#define JOY_CENTRE 512
#define JOY_SPEED 5
#define MILLIS_PER_FRAME 50 

#define PUSHBUTTON 12
// Color definitions
#define BLACK 0x0000
#define WHITE 0xFFFF
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
// #define CYAN 0x07FF
// #define MAGENTA 0xF81F 
// #define YELLOW 0xFFE0 

struct Point 
{
    int x;
    int y;
};



enum State {Up = 0, Down, Left, Right, Hover, Click, Done};
enum Block {Road = ' ', Wall = '#', Entrance = '*', Exportation = '+'};


void setupGame();
void askInfo();
void drawMaze(Block block);
State scanJoystick();
void updateCursor(Point& cursor, int horiz, int vert);
void drawmap(const Point &cursor, Block block);
void mapping(const Point &cursor, char ch);
bool atBoundaries(const Point &cursor);
bool isOccupied(const Point &cursor);
void finder();

void test_showMaze();


#endif