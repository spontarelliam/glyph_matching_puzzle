Glyph Game for One Way Out.

# Overview

There are 4 RFID readers and 8 glyphs. The player must place one glyph on each reader. After the player places all four glyphs, four LEDs will illuminate. If a glyph is in the correct spot, an LED is blue, if a glyph is in the set of four correct glyphs but in the wrong location, an LED is yellow. If all four glyphs are placed correctly, a pin is set high.


# Example

For this example, we’ll call the 8 glyphs A, B, C, D, E, F, G, & H.  We’ll call the readers location slots 1, 2, 3 & 4.  In this example, the correct ‘combination’ is:

Slot 1		Slot 2		Slot 3		Slot 4
   ‘E’		   ‘B’		   ‘H’		   ‘F’

Let’s say that the players place the following glyphs in these slots:

Slot 1		Slot 2		Slot 3		Slot 4
   ‘A’		   ‘B’		   ‘C’		   ‘D’

In this example, since only ‘B’ is in the correct location, LED1BLUE output is made high.  None of the other LEDs are lit, because none of the remaining glyphs are part of the desired combination.

Now, the players swap out ‘D’ in location 4 with ‘H’.  When ‘D’ is pulled, the tally LED extinguishes, because there aren’t glyphs present in all 4 slots.  When ‘H’ is placed in slot 4, the tally LEDs illuminate as such:  LED1BLUE (for ‘B’ which is still in slot 2), and LED2YELLOW (for ‘H’ in slot 4).  This lets the players know one of the glyphs is correctly placed, and one of the glyphs is the correct one, but in the wrong slot.

Now, players pull ‘A’ out of slot 1, the LEDs extinguish, and it gets replaced with ‘F’.  The LEDs illuminate, with LED1BLUE on, as well as LED2YELLOW & LED3YELLOW.  This represents that one of the glyphs is in the correct slot, and now there are two correct glyphs in the wrong slots.

As players start logically swapping and replacing the glyphs, they will eventually get all four correct glyphs in the correct slots.  At this point, LED1BLUE, LED2BLUE, LED3BLUE, and LED4BLUE will illuminate, and TRIGGEROUT goes HIGH.


# Design
The RFID readers use the SPI interface which requires five digital pins plus Vin and Gnd. However, multiple RFID readers can't be used on the same circuit. This is annoying, so someone awesome wrote a custom MFRC522 library that gets around this issue.

https://github.com/VectorSpaceHQ/rfid

## RFID

/*
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 */


Product ID is stored in Sector 2, block 9.
|dec| hex| num|
|-|-|
|49|	31|	1|
|50|	32|	2|
|51|	33|	3|
|52|	34|	4|
|53|	35|	5|
|54|	36|	6|
|55|	37|	7|
|56|	38|	8|

reading should reset to zero when glyph is removed.


Do not accept reading that isn't in the solution set.


## LEDs
LEDs used in this project are from Sparkfun.
https://www.sparkfun.com/products/12999

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)


# Wiring
There are four RFID readers in this particular project. So one RFID reader connects SDA to A2, while the next connects to A3, and so on.

| RFID pin | Wire color | Arduino Pin |
|--|--|--|
| SDA | GRN | A2, A3, A4, A5 |
| SCK | GRN/WHT | A0 |
| MOSI | BRN | A1 |
| MISO | BRN/WHT | D2, D3, D4, D5 |
| GND | ORG/WHT | GND|
| RST | BLUE | D6, D7, D8, D9 |
| 3.3V | BLUE/WHT | 3.3V |
