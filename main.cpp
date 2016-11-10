#include "mazefinder.h"

namespace info
{
    const int col = 8;
    const int row = 10;
    const int wallWidth = 16;
    Point entrance = {0, 0};
    Point exportation = {7, 9}; 
    const int wallColor = WHITE;
    const int roadColor = BLACK;
    const int cursorColor = BLUE;
}


// initialize maze
bool maze[info::row][info::col];

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() 
{
    init();
    Serial.begin(9600);
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(BLACK);

    // initialize joystick
    Serial.print("Initializing joystick...");
    pinMode(JOY_SEL, INPUT);
    digitalWrite(JOY_SEL, HIGH); // enable pull-up resistor
    // tft.fillScreen(BLACK);
    Serial.println("OK!");  


    // initialize maze 
    for (int i = 0; i < info::row; ++i)
        for (int j = 0; j < info::col; ++j)
            maze[i][j] = true;
    
}


int main()
{
    setup();
    askInfo();
	drawMaze();
    finder();
    return 0;
}

void askInfo()
{
    Serial.println("Select a point you want as the entrance.");
    Serial.println("Note: This point must on one of the four boundaries.");
    int userInput[2] = {0};
    if ( Serial.available() )
    {
        
    }
}

void drawMaze()
{
    // allows user to draw the maze on the TFT screen and map the corresponding blocks to maze array
    Point cursor = { 0, 0 };
    updateCursor(cursor, 0, 0);
    for (;;)
    {
        State state = scanJoystick();
        if (Up == state)
        {
            updateCursor(cursor, 0, 1);
        }
        else if ( Down == state )
        {
            updateCursor(cursor, 0, -1);
        }
        else if ( Left == state )
        {
            updateCursor(cursor, -1, 0);
        }
        else if ( Right == state )
        {
            updateCursor(cursor, 1, 0);
        }
        else if ( Click == state)
        {
            draw(cursor);
            mapping(cursor);
            // test
            test_showMaze();
            // test end
        }
        delay(250);
    }

}

State scanJoystick()
{
    int vert = analogRead(JOY_VERT_ANALOG);
    int horiz = analogRead(JOY_HORIZ_ANALOG);
    int sel = digitalRead(JOY_SEL); 
    State state = Hover;
    if (abs(horiz - JOY_CENTRE) > JOY_DEADZONE) 
    {
        if ( horiz > JOY_CENTRE)
            state = Right;
        else
            state = Left;
    }

    if (abs(vert - JOY_CENTRE) > JOY_DEADZONE) 
    {
        if (vert > JOY_CENTRE)
            state = Up;
        else
            state = Down;
    }
    if (LOW == sel)
    {
        state = Click;
    }

    return state;
}

void updateCursor(Point& cursor, int horiz, int vert)
{
    //store previous pic
    bool prev = maze[cursor.y][cursor.x];
    // pedding
    if ( prev )
    {
        tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                         info::wallWidth, info::wallWidth, info::roadColor);
    }
    else
    {
        tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                         info::wallWidth, info::wallWidth, info::wallColor);
    }
    // new cursor position
    cursor.x = constrain( cursor.x + horiz, 0, info::col - 1);
    cursor.y = constrain( cursor.y + vert, 0, info::row - 1);
    // update cursor
    tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                        info::wallWidth, info::wallWidth, info::cursorColor);

}

void draw(const Point &cursor)
{
    tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                        info::wallWidth, info::wallWidth, info::wallColor);
}

void mapping(const Point &cursor)
{
    maze[cursor.y][cursor.x] = false;
}

void finder()
{

}

void test_showMaze()
{
    for (int i = 0; i < info::row; ++i)
    {
        Serial.print("|");
        for (int j = 0; j < info::col; ++j)
        {
            if ( maze[i][j] )
                Serial.print(" ");
            else
                Serial.print("#");
            if (j == info::col - 1 )
                Serial.println("|");
        }
    }
    Serial.println("------------");
}