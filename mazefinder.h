#ifndef __MAZE_FINDER_H
#define __MAZE_FINDER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>
#include "lcd_image.h"
#include "StackArray.h"
#include "cell.h"

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
#define RED 0x001F
#define BLUE 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F 
#define YELLOW 0xFFE0 


#define RATING_LED_0 2      // rating leds 0-4
#define RATING_LED_1 3
#define RATING_LED_2 4
#define RATING_LED_3 10
#define RATING_LED_4 11
#define LED_TOTAL    5

const int PUSHBUTTON_DELAY = 200;

struct Point 
{
    int x;
    int y;
};

namespace info
{
    const int col = 8;
    const int row = 10;
    const int wallWidth = 16;
    const int wallColor = WHITE;
    const int roadColor = BLACK;
    const int cursorColor = RED;
    const int entranceColor = BLUE;
    const int exitColor = GREEN;
    extern Point entrance;
    extern Point exportation; 
}

enum State {Up = 0, Down, Left, Right, Hover, Click, Done};
enum Block {Road = ' ',
            Wall = '#', 
            Entrance = '*', 
            Exportation = '+',
            Checked = '~'};

void drawMaze(Block block);
State scanJoystick();
inline void updateCursor(Point& cursor, int horiz, int vert);
void drawmap(const Point &cursor, Block block);
inline void mapping(const Point &cursor, char ch);
inline bool atBoundaries(const Point &cursor);
inline bool isOccupied(const Point &cursor);
int finder();
void blinkFiveLeds();
inline int getDistance(const Point &a, const Point &b);
inline void indicateDistance( const int dist );

// addition features
void startGame();
// test function
void test_showMaze();


#endif