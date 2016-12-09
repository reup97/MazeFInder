# Maze Finder
It allows the user to draw a maze and the maze finder will find the way out.
## Description
Traditionally, computer will give a maze to a human to solve, however, in this game our users will draw the maze
in his(her) own fashion and let Arduino to figure the way out. 
## Usage
After wiring arduino and download whole folder, simply use `make upload` to upload the code to Arduino.
## Contributing
1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request :D

## Credits
All knowledge I learned to solve the problem is from: Depth-first search(wikipedia)https://en.wikipedia.org/wiki/Depth-first_search
The algorithm is totally developed and implemented by myself.
## Accessories
* Arduiono Mega Board (AMG)
* Adafruit LCD display
* Sparkfun Thumb Joystick
* LED * 5
## Wiring instructions
Adafruit LCD display
* GND to BB GND bus
* VCC to BB positive bus
* RESET to Pin 8
* D/C (Data/Command) to Pin 7
* CARD_CS (Card Chip Select) to Pin 5
* TFT_CS (TFT/screen Chip Select) to Pin 6
* MOSI (Master Out Slave In) to Pin 51
* SCK (Clock) to Pin 52
* MISO (Master In Slave Out) to 50
* LITE (Backlite) to BB positive bus

Sparkfun Thumb Joystick
* VCC to BB positive bus
* VERT to Pin A0
* HOR to Pin A1
* SEL to Pin 9
* GND to BB GND bus
* [Specific instructions](http://ugweb.cs.ualberta.ca/~c274/web/ConcreteComputing/section/adafruit_lcd.htm)
