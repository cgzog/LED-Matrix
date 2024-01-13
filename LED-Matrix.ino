#include <FastLED.h>

#define LED_ROWS        16
#define LED_COLS        16
#define NUM_LEDS        (LED_ROWS * LED_COLS)

#define LED_BRIGHTNESS  65

#define SCROLL_IN_OUT 0     // start with black, scroll pattern in and then scroll pattern out
#define SCROLL_FULL   1     // start with black, display the first 16 columns and then scroll
                            // leaving last column of pattern on the right edge of the display

#define SCROLL_PRE_DELAY    1000    // ms - for SCROLL_FULL mode - wait this long before scrolling
#define SCROLL_POST_DELAY   1000    // ms - for SCROLL_FULL mode - wait this long after scrolling


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

#define SET_CLR       HSK_CLR           // make all the test "Husky" color

#include "words/led_matrix_NAPERVILLE.h"

ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,

#define SET_CLR       NAP_CLR

#include "words/led_matrix_HUSKIES.h"

ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,

#define SET_CLR       FRC_CLR

#include "letters/led_matrix_F.h"

ALL_BLK_LEDS,
ALL_BLK_LEDS,

#include "letters/led_matrix_R.h"

ALL_BLK_LEDS,
ALL_BLK_LEDS,

#include "letters/led_matrix_C.h"

ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,
ALL_BLK_LEDS,

#include "letters/led_matrix_hash.h"

ALL_BLK_LEDS,
ALL_BLK_LEDS,

#include "numbers/led_matrix_3.h"

ALL_BLK_LEDS,
ALL_BLK_LEDS,

#include "numbers/led_matrix_0.h"

ALL_BLK_LEDS,
ALL_BLK_LEDS,

#include "numbers/led_matrix_1.h"

ALL_BLK_LEDS

};


int ScrollDelay = 75;      // milliseconds

#define COLUMN_COUNT    (sizeof(Pattern) / sizeof(Pattern[0]))


void  showPattern(int scrollMode);
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
  
#ifdef NEVER
  Leds[0] = CRGB::Red;
  Leds[15] = CRGB::Blue;
  Leds[16] = CRGB::Green;
  Leds[31] = CRGB::Cyan;
  Leds[224] = CRGB::Green;
  Leds[230] = CRGB::DarkOrange;
  Leds[233] = CRGB::Green;
  Leds[239] = CRGB::Yellow;
  Leds[240] = CRGB::Blue;
  Leds[243] = CRGB::Cyan;
  Leds[246] = CRGB::DarkOrange;
  Leds[249] = CRGB::Green;
  Leds[252] = CRGB::Purple;
  Leds[255] = CRGB::Red;

  FastLED.show();
  delay(1000);

int i;

for (i = 0 ; i < LED_ROWS ; i++) {
  scrollLEDs();

  FastLED.show();
  delay(600);
}
#endif

  showPattern(SCROLL_IN_OUT);

  delay(5000);
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
//
//  two display modes - both scroll if the pattern is longer than the number of columns in the matrix
//
//  SCROLL_IN_OUT - start with black and scroll entire pattern in on right and off on left
//  SCROLL_FULL - start with full matrix and scroll right until last column in pattern is on right column

void showPattern(int scrollMode) {

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

  switch (scrollMode) {

    case SCROLL_FULL:     // preload the first 16 columns (or less if it's shorter)

      for (col = 0 ; col < ((COLUMN_COUNT > LED_COLS) ? LED_COLS : COLUMN_COUNT) ; col++) {
        loadColumn(Pattern, col, col);        // first col of the LED matrix is loaded is the
                                              // first col of the pattern and so on
      }

      FastLED.show();

      delay(SCROLL_PRE_DELAY);                // if the display doesn't fill, we only delay once

      if (col < (LED_COLS - 1)) {
        return;                               // the pattern did not fill the entire matrix so we're done
      }

      break;

    case SCROLL_IN_OUT:
      loadColumn(Pattern, 0, LED_COLS - 1);   // right col loaded from the first row of pattern
      FastLED.show();

      col = 1;                                // we only showed one column so far

      break;
  }

  // once we're here, we've handled the first set of operations regardless of the scroll mode
  //
  // if SCROLL_FULL, the matrix has been fully populated - all 16 columns (with more to go
  // potentially if there are more than 16 cols in the pattern)
  //
  // if SCROLL_IN_OUT, the last (rightmost) column has been filled (with more to go
  // if there is more than one column in the pattern)
  //
  // so, let's move the pattern to the right
  //
  // we continue until the last line of the pattern hits the rightmost column
  //
  // after that, we need to consider the scroll mode to tell us whether we need to scroll
  // it off or not

  // we start col from where we left off so no intialization

  for ( ; col < COLUMN_COUNT ; col++) {
    delay(ScrollDelay);                                           // delay first - then slide left
    scrollLEDs();                                                 // scroll everything left one column
    loadColumn(Pattern, col, LED_COLS - 1);                       // load the right col
    FastLED.show();
  }

  // at this point, the last row in the pattern is in the rightmost column
  //
  // for SCROLL_FULL, we're done - just return
  //
  // for SCROLL_IN_OUT, we need to scroll things off to the left so we're all dark at the end

  if (scrollMode == SCROLL_FULL) {
    delay(SCROLL_POST_DELAY);
    return;
  }

  // we're in SCROLL_IN_OUT mode

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
