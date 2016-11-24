/**
 * entrance:    '*'
 * exportation: '+'
 * wall:        '#'
 * road:        ' '
 */

#include "mazefinder.h"


Point info::entrance = { -1,  -1};
Point info::exportation = {-1, -1}; 


const int PUSHBUTTON_DELAY = 200;
// initialize maze
extern char maze[info::row][info::col];

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

    // initialize LEDs
    Serial.print(F("Initializing LED arrays..."));
    pinMode(RATING_LED_0, OUTPUT);
    pinMode(RATING_LED_1, OUTPUT);
    pinMode(RATING_LED_2, OUTPUT);
    pinMode(RATING_LED_3, OUTPUT);
    pinMode(RATING_LED_4, OUTPUT);
    Serial.println(F("OK!")); 

    // initialize maze 
    for (int i = 0; i < info::row; ++i)
        for (int j = 0; j < info::col; ++j)
            maze[i][j] = ' ';
    
}


int main()
{
    setup();
    // beginning animation
    startGame();

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
    delay(2000);    // delay fir 2 seconds
    finder();

    Serial.end();
    return 0;
}


void drawMaze(Block block)
{
    // allows user to draw the maze on the TFT screen and map the corresponding blocks to maze array
    
    static Point cursor = { 0, 0 };
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
    //store previous piece
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
    return maze[cursor.y][cursor.x] != Road;
}

void finder()
{
	// NOTE: now it will crash when it uses up memory
    Point myFinder = {info::entrance.x, info::entrance.y};
    StackArray<Cell> stack;
    stack.push(Cell(-1, -1));   // dummy b
    stack.push(Cell(info::entrance.y, info::entrance.x));

    Cell nextCell(-1, -1);
    int deltaX = 0;
    int deltaY = 0;
    while ( true )
    {
        Cell& currentCell = stack.peek();
        Cell& prevCell = stack.peekPrev();

        // if exit is found
        if ( currentCell.getRow() == info::exportation.y && currentCell.getCol() == info::exportation.x )
        {
            Serial.println(F("It finds the way out!"));
            blinkFiveLeds();
            return;
        }
        else if ( currentCell.getDirection() < 4 ) // not finish seeking in current cell
        {
            // // test
            // Serial.print("im here: -- ");
            // Serial.println(currentCell.getValue());
            // Serial.print("direction: ");
            // Serial.println(currentCell.getDirection());
            
            // // test end
            
            // get next cell and previous cell
            nextCell = currentCell.getNextCell();
            

            // // test
            // Serial.println("next cell:");
            // Serial.println(nextCell.getValue());
            // Serial.print("current direction:");
            // Serial.println(currentCell.getDirection());
            // Serial.println("previous cell:");
            // Serial.println(prevCell.getValue());
            // delay(500);
            // // test end
    
            // TODO: this one only compare with the previous cell for performance yet may lead to infinite loop
            if ( (nextCell.getValue() == ' ' || nextCell.getValue() == '+' || nextCell.getValue() == '*') && 
                (nextCell.getRow() != prevCell.getRow() || nextCell.getCol() != prevCell.getCol()) )
            {
                stack.push(nextCell);
                // TODO: add animation
                
                deltaX = nextCell.getCol() - myFinder.x;
                deltaY = nextCell.getRow() - myFinder.y;
                updateCursor( myFinder, deltaX, deltaY);
                
                // test
                Serial.print(F("* now is ["));
                Serial.print(nextCell.getCol() );
                Serial.print(F(", "));
                Serial.print(nextCell.getRow() );
                Serial.print(F("]; "));
                Serial.print(F("value: "));
                Serial.println(nextCell.getValue());
                // test end;
                delay(200);
            }
        }
        else
        {
            // test
            Serial.print(F("stack size is "));
            Serial.println(stack.count());
            // test end
            stack.pop();
            // animation
            deltaX = stack.peek().getCol() - myFinder.x;
            deltaY = stack.peek().getRow() - myFinder.y;
            updateCursor( myFinder, deltaX, deltaY);
            delay(200);
            
            
        }
    }
}


void blinkFiveLeds()
{
    int led[] = {RATING_LED_0, RATING_LED_1, RATING_LED_2, RATING_LED_3, RATING_LED_4};
    while ( true )
    {
        for (int i = 0; i < 5; ++i)
        {
            digitalWrite( led[i], HIGH);
        }
        delay(100);
        for (int i = 0; i < 5; ++i)
        {
            digitalWrite( led[i], LOW);
        }
        delay(100);
    }
}

///////////////////////////////
//////////TEST FUNCTIONS///////
///////////////////////////////

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
