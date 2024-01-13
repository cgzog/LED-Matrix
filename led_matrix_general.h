// some defines to make the pattern definition a little easier to handle and easier to
// do in a bulk sort of way

#define LED_BRIGHTNESS  30


#define   BLK_CLR       CRGB::Black

#define   NAP_CLR       CRGB::Blue                // "NAPERVILLE" color
#define   HSK_CLR       CRGB::DarkOrange          // "HUSKIES" color
#define   FRC_CLR       0xABAB9A                  // Team number color (darker Beige)


// an entire columns of black LEDs - good for spacing between letters
#define  ALL_BLK_LEDS   { BLK_CLR, BLK_CLR, BLK_CLR, BLK_CLR, BLK_CLR, BLK_CLR, BLK_CLR, BLK_CLR, BLK_CLR, BLK_CLR, BLK_CLR, BLK_CLR, BLK_CLR, BLK_CLR, BLK_CLR, BLK_CLR }