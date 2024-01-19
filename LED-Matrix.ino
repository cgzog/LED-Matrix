#include <FastLED.h>

#include "led_matrix_general.h"

#define LED_ROWS        16
#define LED_COLS        16
#define NUM_LEDS        (LED_ROWS * LED_COLS)


int ScrollDelay = LED_SCROLL_SPEED;     // variable in case we make it controllable later


CRGB Leds[NUM_LEDS];


// patterns are defined "sideways" - each row starts at the "bottom" and goes to the "top" of the display
// so the first LED in the pattern is actually the lower left on the matrix display
//
// patterns that are longer than the number of columns will scroll right at ScrollDelay rate
//
// Pattern[COLUMN][ROW]
//
// to prevent memory access issues, all rows must be fully populated

#include "led_matrix_general.h"

const CRGB Pattern[][LED_ROWS] = {

#define SET_CLR       NAP_CLR           // make text "Naperville" color

#include "words/led_matrix_NAPERVILLE.h"

ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,

#define SET_CLR       HSK_CLR

#include "words/led_matrix_HUSKIES.h"

ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,

#define SET_CLR       FRC_CLR

#include "words/led_matrix_FIRST.h"

ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,

#include "words/led_matrix_ROBOTICS.h"

ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,

#include "words/led_matrix_COMPETITION.h"

ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,

#include "words/led_matrix_TEAM.h"

ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,

#include "words/led_matrix_3061.h"

ALL_BLK_LEDS

};


#define COLUMN_COUNT    (sizeof(Pattern) / sizeof(Pattern[0]))


void  showPattern();
void  loadColumn(const CRGB pattern[][LED_ROWS], int from, int to);
void  scrollLEDs(CRGB LEDs[], int columns, int rows);



void setup() {
  FastLED.addLeds<NEOPIXEL, 6>(Leds, NUM_LEDS);

  FastLED.setBrightness(LED_BRIGHTNESS);

  FastLED.setTemperature(ColorTemperature::Halogen);      // a little warmer please

}


void loop() {

  fill_solid(Leds, LED_ROWS * LED_COLS, CRGB::Black);
  FastLED.show();

  showPattern();

  delay(INTER_MSG_DELAY);
}


//  showPattern
//
//  take a 16 wide array and display each column on the display
//
//  we need to accomodate the zig-zag wiring of the LEDs
//
//  for us "0" starts in the top left position
//  "15" is the bottom left
//  "16" is one to the right of the bottom left
//  "31" is one to the right of the top left
//  "32" is one to the right of "31" - and so on...
void showPattern() {

  fill_solid(Leds, LED_ROWS * LED_COLS, CRGB::Black);
  FastLED.show();

  // scrolling is pretty much the same for each mode aside from the starting / ending conditions
  //
  // once the starting condition is in place, we can scroll through the remainder of the
  // pattern until we reach the first potential "end" - that's where the last column of
  // the pattern has been displayed on the right edge
  //
  // if we're in SCROLL_FULL, we're done
  //
  // if we're in SCROLL_IN_OUT, we need to scroll that part off the screen as well

  int col;

  
  loadColumn(Pattern, 0, LED_COLS - 1);   // right col loaded from the first row of pattern
  FastLED.show();

  // the last (rightmost) column has been filled (with more to go
  // if there is more than one column in the pattern)
  //
  // so, let's move the pattern to the right
  //
  // we continue until the last line of the pattern hits the rightmost column
 
  for ( col = 1; col < COLUMN_COUNT ; col++) {
    delay(ScrollDelay);                                           // delay first - then slide left
    scrollLEDs();                                                 // scroll everything left one column
    loadColumn(Pattern, col, LED_COLS - 1);                       // load the right col
    FastLED.show();
  }

// at this pont, we've loaded and shown everything
//
// the next section just scrolls the pattern off the matrix

  for (col = 0; col < LED_COLS ; col++) {
    delay(ScrollDelay);
    scrollLEDs();                                                   // scroll everything left one column
    fill_solid (&Leds[NUM_LEDS - LED_ROWS], LED_ROWS, CRGB::Black); // but blacken the last column
    FastLED.show();
  }
}


// loadColumn
//
// load an LED column from the pattern to the matrix
//
// we need to know get where in the pattern we want to get it from and where to
// in the LED matrix
//
// all references are in "columns" whether the pattern or the LED matrix
//
// even / odd handling is taken care of here based on where the data is going in
// the LED matrix

void loadColumn(const CRGB pattern[][LED_ROWS], int from, int to) {

  int row, ledIndex;

  // where does this row start
  ledIndex = LED_ROWS * to;

  // the LEDs on the panel are wired in a zig-zag so we need to pull even and odd
  // columns in a different order
  //
  //  even columns (0 is even) in the numeric order 0 to 15
  //  odd columns in reverse order 15 to 0

//  if  (from % 2) {    // true if an odd column
  if  (0) {    // true if an odd column
    for (row = LED_ROWS - 1 ; row >= 0 ; row--) {
      Leds[ledIndex++] = Pattern[from][row];
    }
  } else {
    for (row = 0 ; row < LED_ROWS ; row++) {
      Leds[ledIndex++] = Pattern[from][row];
    }
  }
}


// scrollLEDs
//
// when scrolling, we can't just copy a set of columns more than one column at a time because
// of the zig zag wiring of the LEDs
//
// each column needs to be "flipped" top to bottom as it scrolls to the left (direction actually
// isn't important, it's the fact that we are scrolling one column at a time - if we scrolled two
// columns at a time, we could just memmove since the order of the wiring would be the same for
// each of the two columns but we want to go one column at a time)
//
// since every row needs to flip as it moves to the left, we can treat them all the same - we flip
// every single LED color

void scrollLEDs() {

  int col, ledFromIndex, ledToIndex;
    
  // we start at the second column which is LED_ROWS into the list and go to the end
  
  for (col = 1 ; col < LED_COLS ; col++) {
      
    ledToIndex = (col - 1) * LED_ROWS + LED_ROWS - 1;
      
    for (ledFromIndex = col * LED_ROWS ;
          ledFromIndex < col * LED_ROWS + LED_ROWS ;
          ledFromIndex++, ledToIndex--) {
              
            Leds[ledToIndex] = Leds[ledFromIndex];
    }
  }
}
