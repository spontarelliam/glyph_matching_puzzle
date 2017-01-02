// One Way Out Glyph Game
// by Adam Spontarelli

#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <MFRC522.h>

#define LEDPIN 10
#define BUTTON 11


Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, LEDPIN, NEO_GRB + NEO_KHZ800);

#define RST_1_PIN   6
#define SS_1_PIN    A2
#define MISO_1_PIN  2

#define RST_2_PIN   7
#define SS_2_PIN    A3
#define MISO_2_PIN  3

#define RST_3_PIN   8
#define SS_3_PIN    A4
#define MISO_3_PIN  4

#define RST_4_PIN   9
#define SS_4_PIN    A5
#define MISO_4_PIN  5

#define SCK_PIN   A0
#define MOSI_PIN  A1

MFRC522 mfrc522_1(SS_1_PIN, RST_1_PIN, MISO_1_PIN, MOSI_PIN, SCK_PIN);  // Create MFRC522 instance
MFRC522 mfrc522_2(SS_2_PIN, RST_2_PIN, MISO_2_PIN, MOSI_PIN, SCK_PIN);  // Create MFRC522 instance
MFRC522 mfrc522_3(SS_3_PIN, RST_3_PIN, MISO_3_PIN, MOSI_PIN, SCK_PIN);  // Create MFRC522 instance
MFRC522 mfrc522_4(SS_4_PIN, RST_4_PIN, MISO_4_PIN, MOSI_PIN, SCK_PIN);  // Create MFRC522 instance


MFRC522::MIFARE_Key key;


//-----------------------------------
// Solution:   reader = product ID
int reader_one = 3;
int reader_two = 4;
int reader_three = 8;
int reader_four = 5;
//-----------------------------------


int solution[4] = {reader_one, reader_two, reader_three, reader_four};
int current_glyphs[4] = {0};
int last_glyphs[4] = {0};
int empty_readers = 0;

void dump_byte_array(byte, byte);
void read_cards(int (&current_glyphs)[4]);

void setup() {
    Serial.begin(9600); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522_1.PCD_Init(); // Init MFRC522 card
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

    Serial.println(F("Reader 1 Details"));
    mfrc522_1.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
    Serial.println(F("Reader 2 Details"));
    mfrc522_2.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
    Serial.println(F("Reader 3 Details"));
    mfrc522_3.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
    Serial.println(F("Reader 4 Details"));
    mfrc522_4.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

    strip.begin();

    // Initialize all pixels to 'off'
    strip.setPixelColor(0, 200, 130, 0); // G, R, B
    strip.setPixelColor(1, 0, 200, 0);
    strip.setPixelColor(2, 0, 0, 200);
    strip.setPixelColor(3, 0, 0, 0);
    strip.show();

}


void loop() {
    int correct = 0;
    // Read all readers and check solutions
    read_cards( current_glyphs );
    Serial.print(current_glyphs[0]);
    Serial.print(", ");
    Serial.print(current_glyphs[1]);
    Serial.print(", ");
    Serial.print(current_glyphs[2]);
    Serial.print(", ");
    Serial.println(current_glyphs[3]);

    for (int i=0; i<4; i++){
        // glyph in correct position
        if (current_glyphs[i] == solution[i]){
            strip.setPixelColor(i, 0, 0, 250); // blue
            correct++;
            /* Serial.print("Glyph #"); */
            /* Serial.print(i+1); */
            /* Serial.println(" is correct"); */
        }
        else if (current_glyphs[i] != solution[i]){
            bool found = false;
            // check for glyph in wrong position
            for (int j=0; j<4; j++){
                if (current_glyphs[i] == solution[j]){
                    /* Serial.print("Glyph #"); */
                    /* Serial.print(i+1); */
                    /* Serial.print(" in position"); */
                    /* Serial.println(j+1); */
                    found = true;
                    strip.setPixelColor(i, 200, 130, 0); // yellow
                }
            }
            // glyph not in solution set
            if (found == false){
                strip.setPixelColor(i, 0, 0, 0);
            }
        }
    }

    // Check that all readers have a glyph
    empty_readers = 0;
    for (int i=0; i<4; i++){
        if (current_glyphs[i] == 0){
            empty_readers = 1;
        }
    }

    // If all readers have a glyph on them, light the LEDs
    if (empty_readers == 0){
        // if glyphs haven't changed, break
        int unchanged = 0;
        for (int i=0; i<4; i++){
            if (current_glyphs[i] == last_glyphs[i]){
                unchanged++;
            }
            if (unchanged == 4){
                Serial.println("UNCHANGED");
                return;
            }
        }

        Serial.println("four glyphs have been placed.");

        /* last_glyphs = current_glyphs; */
        memcpy(last_glyphs, current_glyphs, sizeof(last_glyphs));

        // Check if all four are correct
        if (correct == 4){
            Serial.println("YOU WIN!");
            pinMode(BUTTON, HIGH);
            strip.setPixelColor(0, 200, 0, 0);
            strip.setPixelColor(1, 200, 0, 0);
            strip.setPixelColor(2, 200, 0, 0);
            strip.setPixelColor(3, 200, 0, 0);
            strip.show();
            delay(1000);
            pinMode(BUTTON, LOW);
        }
        else{
            Serial.println("you didn't win.");
            strip.show();
            delay(3000);
        }

        // reset board

        // This forces you to remove all glyphs
        current_glyphs[0] = 0;
        current_glyphs[1] = 0;
        current_glyphs[2] = 0;
        current_glyphs[3] = 0;

        // leaves previous readings even if you move the glyph
        // if you take one glyph off, it will calc new solution
        mfrc522_1.PCD_Init(); // Init MFRC522 card
        mfrc522_2.PCD_Init(); // Init MFRC522 card
        mfrc522_3.PCD_Init(); // Init MFRC522 card
        mfrc522_4.PCD_Init(); // Init MFRC522 card

        // need to detect card being removed

        strip.setPixelColor(0, 0, 0, 0);
        strip.setPixelColor(1, 0, 0, 0);
        strip.setPixelColor(2, 0, 0, 0);
        strip.setPixelColor(3, 0, 0, 0);
        strip.show();
    }
}


void read_cards(int (&current_glyphs)[4]){

    /* // check if card removed */
    /* // look for 2 successive false values */
    /* bool reading; */
    /* bool present; */

    /* reading = mfrc522_1.PICC_ReadCardSerial(); */
    /* Serial.println(reading); */
    /* present = mfrc522_1.PICC_IsNewCardPresent(); //Does RequestA */
    /* Serial.println(present); */
    /* reading = mfrc522_1.PICC_ReadCardSerial(); */
    /* Serial.println(reading); */

    /* delay(3000); */

    /* if (! reading){ */
    /*     Serial.println("No card on 1"); */
    /*     current_glyphs[0] = 0; */
    /*     present = mfrc522_1.PICC_IsNewCardPresent(); //Does RequestA */
    /*     Serial.println(present); */
    /*     delay(1000); */
    /* } */
    /* else{ */

    /* } */


    // Look for new cards
    /* if ( ! mfrc522_1.PICC_IsNewCardPresent() && ! mfrc522_2.PICC_IsNewCardPresent() && */
    /*      ! mfrc522_3.PICC_IsNewCardPresent() && ! mfrc522_4.PICC_IsNewCardPresent()){ */
    /*     return; */
    /* } */


    byte sector         = 2;
    byte blockAddr      = 9;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    // Check each card
    Serial.println(mfrc522_1.PICC_ReadCardSerial());
    Serial.println(mfrc522_1.PICC_IsNewCardPresent());
    /* Serial.println(mfrc522_1.PICC_ReadCardSerial()); */
    /* Serial.println(mfrc522_1.PICC_IsNewCardPresent()); */
    delay(3000);
    if ( mfrc522_1.PICC_ReadCardSerial() ){
        Serial.println(mfrc522_1.PICC_IsNewCardPresent());
        if ( mfrc522_1.PICC_IsNewCardPresent() ){ // new card is present

            /* Serial.print(F("Card UID:")); */
            mfrc522_1.PICC_DumpMifareClassicSectorToSerial(&(mfrc522_1.uid), &key, sector);
            status = (MFRC522::StatusCode) mfrc522_1.MIFARE_Read(blockAddr, buffer, &size);
            current_glyphs[0] = buffer[0] - 48; // convert from dec to ascii
            // Halt PICC
            mfrc522_1.PICC_HaltA();
            // Stop encryption on PCD
            mfrc522_1.PCD_StopCrypto1();
        }
    }
    else{
        current_glyphs[0] = 0;
    }

    if ( mfrc522_2.PICC_ReadCardSerial())
    {
        /* Serial.println("READER 2"); */
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
        /* Serial.println("READER 3"); */
        /* Serial.print(F("Card UID:")); */
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
        /* Serial.println("READER 4"); */
        /* Serial.print(F("Card UID:")); */
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
