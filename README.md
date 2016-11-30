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
The RFID readers use the SPI interface which requires five digital pins plus Vin and Gnd. Since the digital pins can be shared by all of the readers, an Arduino mini will be used.
