// One Way Out Glyph Game
// by Adam Spontarelli

#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <MFRC522.h>

#define LEDPIN 6


Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, LEDPIN, NEO_GRB + NEO_KHZ800);


#define SS_PIN1          10          // Configurable, see typical pin layout above
#define RST_PIN1         9           // Configurable, see typical pin layout above
#define SS_PIN2          2          // Configurable, see typical pin layout above
#define RST_PIN2         3           // Configurable, see typical pin layout above
#define SS_PIN3          4          // Configurable, see typical pin layout above
#define RST_PIN3         5           // Configurable, see typical pin layout above
#define SS_PIN4          7          // Configurable, see typical pin layout above
#define RST_PIN4         8           // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN1, RST_PIN1);   // Create MFRC522 instance.
MFRC522 mfrc522_2(SS_PIN2, RST_PIN2);   // Create MFRC522 instance.
MFRC522 mfrc522_3(SS_PIN3, RST_PIN3);   // Create MFRC522 instance.
MFRC522 mfrc522_4(SS_PIN4, RST_PIN4);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;


//-----------------------------------
// Solution:   reader = product ID
int reader_one = 3;
int reader_two = 2;
int reader_three = 8;
int reader_four = 5;
//-----------------------------------


int solution[4] = {reader_one, reader_two, reader_three, reader_four};
int current_glyphs[4] = {0};
int empty_readers = 0;

void dump_byte_array(byte, byte);
void read_cards(int (&current_glyphs)[4]);

void setup() {
    Serial.begin(9600); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card
    mfrc522_2.PCD_Init(); // Init MFRC522 card
    mfrc522_3.PCD_Init(); // Init MFRC522 card
    mfrc522_4.PCD_Init(); // Init MFRC522 card


    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    /* Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write.")); */
    /* Serial.print(F("Using key (for A and B):")); */
    /* /\* dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE); *\/ */
    /* Serial.println(); */

    strip.begin();
    strip.show(); // Initialize all pixels to 'off'

}


void loop() {
    int correct = 0;
    // Read all readers and check solutions
    read_cards( current_glyphs );
    Serial.println(current_glyphs[0]);
    Serial.println("done");

    for (int i=0; i<4; i++){
        // glyph in correct position
        if (current_glyphs[i] == solution[i]){
            strip.setPixelColor(i, 0, 0, 250);
            correct++;
        }
        else if (current_glyphs[i] != solution[i]){
            bool found = false;
            // check for glyph in wrong position
            for (int j=0; j<4; j++){
                if (current_glyphs[i] == solution[j]){
                    found = true;
                    strip.setPixelColor(i, 200, 200, 0);
                }
            }
            // glyph not in solution set
            if (found == false){
                strip.setPixelColor(i, 0, 0, 0);
            }
        }
        // glyph not detected
        else{
            empty_readers++;
        }
    }

    // If all readers have a glyph on them, light the LEDs
    if (empty_readers == 0){
        // Check if all four are correct
        if (correct == 4){
            pinMode(6, HIGH);
            strip.setPixelColor(0, 0, 200, 0);
            strip.setPixelColor(1, 0, 200, 0);
            strip.setPixelColor(2, 0, 200, 0);
            strip.setPixelColor(3, 0, 200, 0);
            strip.show();
            delay(1000);
            pinMode(6, LOW);
        }
        else{
            strip.show();
        }

        // reset board
        strip.setPixelColor(0, 0, 0, 0);
        strip.setPixelColor(1, 0, 0, 0);
        strip.setPixelColor(2, 0, 0, 0);
        strip.setPixelColor(3, 0, 0, 0);
        strip.show();
        empty_readers = 0;
        delay(1000);
    }
}


void read_cards(int (&current_glyphs)[4]){
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent() && ! mfrc522_2.PICC_IsNewCardPresent() &&
         ! mfrc522_3.PICC_IsNewCardPresent() && ! mfrc522_4.PICC_IsNewCardPresent()){
        return;
    }

    byte sector         = 2;
    byte blockAddr      = 9;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    Serial.println("glow");
    // Glow LED when reading
    /* for (int i=0; i<256; i++){ */
    /*     strip.setPixelColor(0, 0, 0, i); */
    /*     strip.setPixelColor(1, 0, 0, i); */
    /*     strip.setPixelColor(2, 0, 0, i); */
    /*     strip.setPixelColor(3, 0, 0, i); */
    /*     strip.show(); */
    /*     delay(1); */
    /* } */
    /* for (int i=0; i<256; i++){ */
    /*     strip.setPixelColor(0, 0, 0, 255-i); */
    /*     strip.setPixelColor(1, 0, 0, 255-i); */
    /*     strip.setPixelColor(2, 0, 0, 255-i); */
    /*     strip.setPixelColor(3, 0, 0, 255-i); */
    /*     strip.show(); */
    /*     delay(1); */
    /* } */



    // Check each card
    if ( mfrc522.PICC_ReadCardSerial())
    {
        Serial.print(F("Card UID:"));
        mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
        current_glyphs[0] = buffer[0] - 48; // convert from dec to ascii
        // Halt PICC
        mfrc522.PICC_HaltA();
        // Stop encryption on PCD
        mfrc522.PCD_StopCrypto1();
        Serial.println("\nreader1 DONE");
    }
    if ( mfrc522_2.PICC_ReadCardSerial())
    {
        mfrc522_2.PICC_DumpMifareClassicSectorToSerial(&(mfrc522_2.uid), &key, sector);
        status = (MFRC522::StatusCode) mfrc522_2.MIFARE_Read(blockAddr, buffer, &size);
        current_glyphs[1] = buffer[0] - 48;
        // Halt PICC
        mfrc522_2.PICC_HaltA();
        // Stop encryption on PCD
        mfrc522_2.PCD_StopCrypto1();
    }
    if ( mfrc522_3.PICC_ReadCardSerial())
    {
        Serial.print(F("Card UID:"));
        mfrc522_3.PICC_DumpMifareClassicSectorToSerial(&(mfrc522_3.uid), &key, sector);
        status = (MFRC522::StatusCode) mfrc522_3.MIFARE_Read(blockAddr, buffer, &size);
        current_glyphs[2] = buffer[0] - 48; // convert from dec to ascii
        // Halt PICC
        mfrc522_3.PICC_HaltA();
        // Stop encryption on PCD
        mfrc522_3.PCD_StopCrypto1();

    }
    if ( mfrc522_4.PICC_ReadCardSerial())
    {
        Serial.print(F("Card UID:"));
        mfrc522_4.PICC_DumpMifareClassicSectorToSerial(&(mfrc522_4.uid), &key, sector);
        status = (MFRC522::StatusCode) mfrc522_4.MIFARE_Read(blockAddr, buffer, &size);
        current_glyphs[3] = buffer[0] - 48; // convert from dec to ascii
        // Halt PICC
        mfrc522_4.PICC_HaltA();
        // Stop encryption on PCD
        mfrc522_4.PCD_StopCrypto1();
    }

    return;
}
