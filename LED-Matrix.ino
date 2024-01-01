#include <FastLED.h>

#define LED_ROWS        16
#define LED_COLS        16
#define NUM_LEDS        (LED_ROWS * LED_COLS)

#define LED_BRIGHTNESS  100

#define SCROLL_IN_OUT 0     // start with black, scroll pattern in and then scroll pattern out
#define SCROLL_FULL   1     // start with black, display the first 16 columns and then scroll
                            // leaving last column of pattern on the right edge of the display


CRGB Leds[NUM_LEDS];


// patterns are defined "sideways" - each row starts at the "bottom" and goes to the "top" of the display
// so the first LED in the pattern is actually the lower left on the matrix display
//
// patterns that are longer than the number of columns will scroll right at ScrollDelay rate
//
// Pattern[COLUMN][ROW]
//
// to prevent memory access issues, all rows must be fully populated

const CRGB Pattern[][LED_ROWS] = {
#include "led_matrix_2.h"
};


int ScrollDelay = 100;      // milliseconds

#define COLUMN_COUNT    (sizeof(Pattern) / sizeof(Pattern[0]))


void  showPattern(int scrollMode);
void  loadColumn(CRGB pattern[][LED_ROWS], int from, int to);


void setup() {
  FastLED.addLeds<NEOPIXEL, 6>(Leds, NUM_LEDS);

  FastLED.setBrightness(LED_BRIGHTNESS);

  FastLED.setTemperature(ColorTemperature::Halogen);      // a little warmer please
}


void loop() {

  fill_solid(Leds, LED_ROWS * LED_COLS, CRGB::Black);
  FastLED.show();

  delay(1000);

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
        loadColumn(Pattern, col, col);        // first col loaded is the first row of pattern and so on
      }

      FastLED.show();

      if (col < (LED_COLS - 1)) {
        return;                               // the pattern did not fill the matrix so we're done
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
  // if SCROLL_FULL, the matrix has been fully populated (with more to go - potentially)
  // if SCROLL_IN_OUT, the last (rightmost) column has been filled (with more to go - potentially)
  // so, let's move the pattern to the right
  //
  // we can just memmove() the LED colors for columns 1-15 to columns 0-14
  // and then fill the last column - we need to use memove() rather than memcpy() because the
  // areas overlap and memmove(0 handles it correctly
  //
  // we continue until the last line of the pattern hits the rightmost column
  //
  // after that, we need to consider the scroll mode to tell us whether we need to scroll
  // it off or not

  for ( ; col < COLUMN_COUNT ; col++) {
    delay(ScrollDelay);                                           // delay first - then slide left
    memmove(Leds, &Leds[LED_ROWS], sizeof(Leds[0])*(LED_ROWS*(LED_COLS-1)));   // slide 15 right cols left 1 col
    loadColumn(Pattern, col, LED_COLS - 1);                       // load the right col
    FastLED.show();
  }

  // at this point, the last row in the pattern is in the rightmost column
  //
  // for SCROLL_FULL, we're done - just return
  //
  // for SCROLL_IN_OUT, we need to scroll things off to the left so we're all dark at the end

  if (scrollMode == SCROLL_FULL) {
    return;
  }

  for (col = 0; col < LED_COLS ; col++) {
    delay(ScrollDelay);
    memmove(Leds, &Leds[LED_ROWS], sizeof(Leds[0])*(LED_ROWS*(LED_COLS-1)));   // slide 15 right cols left 1 col
    fill_solid (&Leds[NUM_LEDS - LED_ROWS - 1], LED_ROWS, CRGB::Black); // but blacken the last column
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

void loadColumn(CRGB pattern[][LED_ROWS], int from, int to) {

  int row, ledIndex;

  // where does this row start
  ledIndex = LED_ROWS * to;

  // the LEDs on the panel are wired in a zig-zag so we need to pull even and odd
  // columns in a different order
  //
  //  even columns (0 is even) in the numeric order 0 to 15
  //  odd columns in reverse order 15 to 0

  if  (from % 2) {    // true if an odd column
    for (row = LED_ROWS - 1 ; row >= 0 ; row--) {
      Leds[ledIndex++] = Pattern[from][row];
    }
  } else {
    for (row = 0 ; row < LED_ROWS ; row++) {
      Leds[ledIndex++] = Pattern[from][row];
    }
  }
}
