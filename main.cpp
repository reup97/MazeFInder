/**
 * entrance:    '*'
 * exportation: '+'
 * wall:        '#'
 * road:        ' '
 */

#include "mazefinder.h"

namespace info
{
    const int col = 8;
    const int row = 10;
    const int wallWidth = 16;
    Point entrance = { -1,  -1};
    Point exportation = {-1, -1}; 
    const int wallColor = WHITE;
    const int roadColor = BLACK;
    const int cursorColor = BLUE;
    const int entranceColor = RED;
    const int exitColor = GREEN;
}

const int PUSHBUTTON_DELAY = 200;
// initialize maze
char maze[info::row][info::col];

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() 
{
    init();
    Serial.begin(9600);
    Serial.print(F("Initializing TFT display..."));
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(BLACK);
    Serial.println(F("OK!"));   

    // initialize pushbutton
    Serial.print(F("Initializing pushbutton..."));
    pinMode(PUSHBUTTON, INPUT);
    digitalWrite(PUSHBUTTON, HIGH); // enable pull-up resistor   
    Serial.println(F("OK!"));  

    // initialize joystick
    Serial.print(F("Initializing joystick..."));
    pinMode(JOY_SEL, INPUT);
    digitalWrite(JOY_SEL, HIGH); // enable pull-up resistor
    Serial.println(F("OK!"));  



    // initialize maze 
    for (int i = 0; i < info::row; ++i)
        for (int j = 0; j < info::col; ++j)
            maze[i][j] = ' ';
    
}


int main()
{
    setup();
    while (-1 ==info::entrance.x)
    {
        Serial.println(F("Please select an entrance with joystick."));
        drawMaze(Entrance);
    }

    while ( -1 == info::exportation.x)
    {
        Serial.println(F("Please select an exportation with joystick."));
        drawMaze(Exportation);
    }
    // allow user to draw the walls
    Serial.println(F("You can draw the wall now."));
	drawMaze(Wall);
    Serial.println(F("Ready to find the way out!"));
    finder();
    return 0;
}


void drawMaze(Block block)
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
            drawmap(cursor, block);
            if ( Entrance == block || Exportation == block )
                return;
            // test
            test_showMaze();
            // test end
        }

        // finish drawing if user clicks the pushbutton
        if ( LOW == digitalRead(PUSHBUTTON) )
        {
            Serial.println(F("Finish drawing!"));
            delay(PUSHBUTTON_DELAY);
            return;
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
        delay(PUSHBUTTON_DELAY);
    }

    return state;
}

void updateCursor(Point& cursor, int horiz, int vert)
{
    //store previous pic
    char prev = maze[cursor.y][cursor.x];
    // pedding
    if ( prev == Road )  // road
    {
        tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                         info::wallWidth, info::wallWidth, info::roadColor);
    }
    else if ( prev == Wall)  // wall
    {
        tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                         info::wallWidth, info::wallWidth, info::wallColor);
    }
    else if ( prev == Entrance)   // entrance
    {
        tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                 info::wallWidth, info::wallWidth, info::entranceColor);
    }
    else if ( prev == Exportation)   // exportation
    {
        tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                 info::wallWidth, info::wallWidth, info::exitColor);
    }
    // new cursor position
    cursor.x = constrain( cursor.x + horiz, 0, info::col - 1);
    cursor.y = constrain( cursor.y + vert, 0, info::row - 1);
    // update cursor
    tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                        info::wallWidth, info::wallWidth, info::cursorColor);

}

void drawmap(const Point &cursor, Block block)
{
    if ( Wall == block)
    {
        if (isOccupied(cursor))
        {
            Serial.println(F("You cannot set wall here."));
            return;
        }        
        tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                            info::wallWidth, info::wallWidth, info::wallColor);
        mapping(cursor, Wall);
    }
    else if ( Entrance == block)
    {
        if ( !atBoundaries(cursor) || isOccupied(cursor))
        {
            Serial.println(F("You cannot set entrance here."));
            return;
        }
        tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                    info::wallWidth, info::wallWidth, info::entranceColor);
        info::entrance.x = cursor.x;
        info::entrance.y = cursor.y;
        mapping(cursor, Entrance);
        // test
        Serial.print(F("Entrance x:"));
        Serial.println(info::entrance.x);
        Serial.print(F("Entrance y:"));
        Serial.println(info::entrance.y);    
        // test end   
    }
    else if ( Exportation == block)
    {
        if ( !atBoundaries(cursor) || isOccupied(cursor))
        {
            Serial.println(F("You cannot set exportation here."));
            return;
        }
        tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                    info::wallWidth, info::wallWidth, info::exitColor);
        info::exportation.x = cursor.x;
        info::exportation.y = cursor.y;
        mapping(cursor, Exportation);
        // test
        Serial.print(F("Exportation x:"));
        Serial.println(info::exportation.x);
        Serial.print(F("Exportation y:"));
        Serial.println(info::exportation.y);  
        // test end
    }

}

void mapping(const Point &cursor, char ch)
{
    maze[cursor.y][cursor.x] = ch;
}

bool atBoundaries(const Point &cursor)
{
    return ( cursor.x == 0 || cursor.x == info::col - 1 || cursor.y == 0 || cursor.y == info::row - 1 );
}

bool isOccupied(const Point &cursor)
{
    return maze[cursor.y][cursor.x] != ' ';
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
            if ( maze[i][j] == ' ' )    // road
                Serial.print(" ");
            else if ( maze[i][j] == '#' )   // wall
                Serial.print("#");
            else if ( maze[i][j] == '*' )   // entrance
                Serial.print("*");
            else if ( maze[i][j] == '+' )   // exportation
                Serial.print("+");
            if (j == info::col - 1 )
                Serial.println("|");
        }
    }
    Serial.println("----------");
}