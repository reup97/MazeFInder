/**
 * entrance:    '*'
 * exportation: '+'
 * wall:        '#'
 * road:        ' '
 * checked      '~'
 */

#include "mazefinder.h"

Point info::entrance = {-1,  -1};
Point info::exportation = {-1, -1}; 

// initialize maze
extern char maze[info::row][info::col];


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
lcd_image_t wallImage = { "wall.lcd", TFT_WIDTH, TFT_HEIGHT };
lcd_image_t gndImage = { "gnd.lcd", TFT_WIDTH, TFT_HEIGHT };
lcd_image_t entranceImage = { "exitsign.lcd", info::wallWidth, info::wallWidth };   // TODO: find a better pic for entrance
lcd_image_t exitImage = { "exitsign.lcd", info::wallWidth, info::wallWidth };

void setup() 
{
    init();
    Serial.begin(9600);
    Serial.print(F("Initializing TFT display..."));
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(BLACK);
    Serial.println(F("OK!"));   

    Serial.print("Initializing SD card...");
    if (!SD.begin(SD_CS)) 
    {
        Serial.println("Failed!");
        return;
    }
    Serial.println("OK!");

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
    Serial.print(F("Initializing maze map..."));
    for (int i = 0; i < info::row; ++i)
        for (int j = 0; j < info::col; ++j)
            maze[i][j] = ' ';
    Serial.println(F("OK!")); 

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
        Serial
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
    delay(1000);    // delay for 1 seconds

    finder();

    Serial.end();
    return 0;
}

// complexity: O(n), n is the user input
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

// complexity: O(1)
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

// complexity: O(1)
inline void updateCursor(Point& cursor, int horiz, int vert)
{
    //store previous piece
    char prev = maze[cursor.y][cursor.x];
    // pedding
    if ( prev == Road || prev == Checked )  // road
    {
        // tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
        //                  info::wallWidth, info::wallWidth, info::roadColor);
        lcd_image_draw(&gndImage, &tft,
                 cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                 cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                 info::wallWidth, info::wallWidth);
    }
    else if ( prev == Wall)  // wall
    {
        // tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
        //                  info::wallWidth, info::wallWidth, info::wallColor);
        lcd_image_draw(&wallImage, &tft,
                 cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                 cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                 info::wallWidth, info::wallWidth);
    }
    else if ( prev == Entrance)   // entrance
    {
        // tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
        //          info::wallWidth, info::wallWidth, info::entranceColor);
        lcd_image_draw(&entranceImage, &tft,
                 0, 0,
                 cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                 info::wallWidth, info::wallWidth);
    }
    else if ( prev == Exportation)   // exportation
    {
        // tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
        //          info::wallWidth, info::wallWidth, info::exitColor);
        lcd_image_draw(&exitImage, &tft,
                0, 0,
                cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                info::wallWidth, info::wallWidth);
    }
    // new cursor position
    cursor.x = constrain( cursor.x + horiz, 0, info::col - 1);
    cursor.y = constrain( cursor.y + vert, 0, info::row - 1);
    // update cursor
    tft.fillRect(cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                        info::wallWidth, info::wallWidth, info::cursorColor);

}

// complexity: O(1)
void drawmap(const Point &cursor, Block block)
{
    if ( Wall == block)
    {
        if (isOccupied(cursor))
        {
            Serial.println(F("You cannot set wall here."));
            return;
        }
        lcd_image_draw(&wallImage, &tft,
                 cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                 cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                 info::wallWidth, info::wallWidth);
        mapping(cursor, Wall);
    }
    else if ( Entrance == block)
    {
        if ( !atBoundaries(cursor) || isOccupied(cursor))
        {
            Serial.println(F("You cannot set entrance here."));

            return;
        }
        lcd_image_draw(&entranceImage, &tft,
                 0, 0,
                 cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                 info::wallWidth, info::wallWidth);
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
        lcd_image_draw(&exitImage, &tft,
                0, 0,
                cursor.x * info::wallWidth, cursor.y * info::wallWidth,
                info::wallWidth, info::wallWidth);
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

// complexity: O(1)
inline void mapping(const Point &cursor, char ch)
{
    maze[cursor.y][cursor.x] = ch;
}

// complexity: O(1)
inline bool atBoundaries(const Point &cursor)
{
    return ( cursor.x == 0 || cursor.x == info::col - 1 || cursor.y == 0 || cursor.y == info::row - 1 );
}

// complexity: O(1)
inline bool isOccupied(const Point &cursor)
{
    return maze[cursor.y][cursor.x] != Road;
}

// TODO: which is better? 
// complexity: O(n), n is roughly the complexity of the given maze
// OR,
// best case: O(1), worst case: O(1) , average: O(1), since the size of maze is fixed, 
// the time it takes will not exceeds the worst case as user increase the complexity.
int finder()
{
    // one Cell takes 7 bytes and the worse case for the finder to find the way out takes
    //  8 (cols)* 10 (rows) * 7 = 560 bytes of memory, which is much less than the total memory
    // of Arduino mega 2560. Thus it allows me to implement a stack to store cells the finder have  
    // visited and makes the algorithm cleaner
    
    Point myFinder = {info::entrance.x, info::entrance.y};
    StackArray<Cell> stack;
    stack.push(Cell(-1, -1));   // dummy "buttom" to eliminate awkward exceptions
    stack.push(Cell(info::entrance.y, info::entrance.x));


    Cell nextCell(-1, -1);
    int deltaX = 0;
    int deltaY = 0;
    while ( true )
    {
        Cell& currentCell = stack.peek();
        // Cell& prevCell = stack.peekPrev();

        // if exit is found
        if ( currentCell.getRow() == info::exportation.y && currentCell.getCol() == info::exportation.x )
        {
            Serial.println(F("It finds the way out!"));
            blinkFiveLeds();
            return 1;
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

            if ( (nextCell.getValue() == ' ' || nextCell.getValue() == '+' ) ) //  || nextCell.getValue() == '*') )
                // (nextCell.getRow() != prevCell.getRow() || nextCell.getCol() != prevCell.getCol()) )
            {
                stack.push(nextCell);
                
                
                deltaX = nextCell.getCol() - myFinder.x;
                deltaY = nextCell.getRow() - myFinder.y;
                updateCursor( myFinder, deltaX, deltaY);
                
                maze[nextCell.getRow()][nextCell.getCol()] = 
                                    (maze[nextCell.getRow()][nextCell.getCol()] != Entrance ) ? Checked : Entrance;
                // test
                // test_showMaze();
                Serial.print(F("Going forward! "));
                Serial.print(F("now is ["));
                Serial.print(nextCell.getCol() );
                Serial.print(F(", "));
                Serial.print(nextCell.getRow() );
                Serial.println(F("]; "));
                // test end;
                delay(100);
            }
        }
        else
        {
            stack.pop();
            // test
            Serial.print(F("Going back! stack size is "));
            Serial.println(stack.count());
            // test end
            
            // animation
            deltaX = stack.peek().getCol() - myFinder.x;
            deltaY = stack.peek().getRow() - myFinder.y;
            updateCursor( myFinder, deltaX, deltaY);

            // if stack size is one ( dummy ), it means that the exit is somehow unreachable.
            // the finder will goes all the way back to the extrance.
            if ( 1 == stack.count())
            {
                Serial.println(F("The exit is not reachable, finder goes back to the entrance point."));
                return -1;
            }
            delay(100);
        }

        // indicate distance using LEDs
        int distance = getDistance(myFinder, info::exportation);
        indicateDistance(distance);
    }
}

// Complexity: O(1)
void blinkFiveLeds()
{
    int led[] = {RATING_LED_0, RATING_LED_1, RATING_LED_2, RATING_LED_3, RATING_LED_4};
    while ( HIGH == digitalRead(PUSHBUTTON) ) 
    {
        for (int i = 0; i < LED_TOTAL; ++i)
        {
            digitalWrite( led[i], HIGH);
        }
        delay(100);
        for (int i = 0; i < LED_TOTAL; ++i)
        {
            digitalWrite( led[i], LOW);
        }
        delay(100);
    }
}


//////////////////////////////////////////////
////////////Additional features///////////////
//////////////////////////////////////////////

// date: Nov 24 2016
// complexity: O(1)
void startGame()
{
    const int showTimeInMillis = 500;
    const int blockSize = 3;
    const uint16_t colour[] = {  BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW, WHITE };
    const int startTime = millis();

    tft.setCursor(0, 40);
    tft.setTextColor(WHITE, BLACK);
    tft.setTextSize(3);

    randomSeed(analogRead(7));
    
    uint16_t currColor = 0;
    Point drawPt = { 0, 0 };
    
    while ( millis() - startTime < showTimeInMillis )
    {
         currColor = colour[ random(7) ];
         drawPt.x = constrain( random(TFT_WIDTH) , 0, TFT_WIDTH - blockSize  );
         drawPt.y = constrain( random(TFT_HEIGHT), 0, TFT_HEIGHT - blockSize );
         // tft.fillRect(drawPt.x, drawPt.y,
         //            blockSize, blockSize, currColor);
         tft.fillCircle( drawPt.x, drawPt.y, blockSize, currColor);
    }
    
    tft.println(F("Maze"));
    tft.println(F("Finder"));
    tft.setTextSize(1);
    tft.print(F("   By Paul and Joanne"));
    tft.println();
    tft.setCursor(0, 140);
    tft.setTextColor( WHITE, RED);
    tft.println(F("click pushbutton to continue..."));

    blinkFiveLeds();    // interally it will enter a loop until user click pushbutton

    lcd_image_draw(&gndImage, &tft,
                 0, 0,
                 0, 0,
                 TFT_WIDTH, TFT_HEIGHT);   
}

// date Nov 26 2016
// complexity: O(1)
inline int getDistance(const Point &a, const Point &b)
{
    return abs( a.x - b.x) + abs(a.y - b.y);
}
// complexity: O(1)
inline void indicateDistance( const int dist )
{

    int distLevel = LED_TOTAL + 1 - (LED_TOTAL * dist / (info::row + info::col - 2));
    int led[] = {RATING_LED_0, RATING_LED_1, RATING_LED_2, RATING_LED_3, RATING_LED_4};
    for (int i = 0; i < distLevel; ++i)
    {
        digitalWrite(led[i], HIGH);
    }
    for (int i = distLevel; i < LED_TOTAL; ++i)
    {
        digitalWrite(led[i], LOW);
    }
}

///////////////////////////////
//////////TEST FUNCTIONS///////
///////////////////////////////

// complexity: O(1) since maze size is fixed. 
// However, if maze size [m, n] are variables,
// the complexity should be O(mn)
void test_showMaze()
{
    Serial.println();
    Serial.println("----------");
    for (int i = 0; i < info::row; ++i)
    {
        Serial.print("|");
        for (int j = 0; j < info::col; ++j)
        {
            if ( maze[i][j] == Road )    // road
                Serial.print(char(Road));
            else if ( maze[i][j] == Wall )   // wall
                Serial.print(char(Wall));
            else if ( maze[i][j] == Entrance )   // entrance
                Serial.print(char(Entrance));
            else if ( maze[i][j] == Exportation )   // exportation
                Serial.print(char(Exportation));
            else if ( maze[i][j] == Checked)    // checked
                Serial.print(char(Checked));
            if (j == info::col - 1 )
                Serial.println("|");
        }
    }
    Serial.println("----------");
}
