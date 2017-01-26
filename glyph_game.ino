// One Way Out Glyph Game
// by Adam Spontarelli
// custom MFRC522 library by Jordan Goulder

#include <Adafruit_NeoPixel.h>
#include <MFRC522.h>

#define LEDPIN 10
#define DOOR 13
#define LIGHT 12


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

bool solved = false;


int solution[4] = {reader_one, reader_two, reader_three, reader_four};
int current_glyphs[4] = {0};
int last_glyphs[4] = {0};
int empty_readers = 0;

void dump_byte_array(byte, byte);
void read_cards(int (&current_glyphs)[4]);
void reset_game();
void heartbeat();

void setup() {

    Serial.begin(57600); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

    mfrc522_1.PCD_Init(); // Init MFRC522 card
    mfrc522_2.PCD_Init(); // Init MFRC522 card
    mfrc522_3.PCD_Init(); // Init MFRC522 card
    mfrc522_4.PCD_Init(); // Init MFRC522 card

    pinMode(DOOR, OUTPUT);
    pinMode(LIGHT, OUTPUT);

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Reader 1 Details"));
    mfrc522_1.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
    Serial.println(F("Reader 2 Details"));
    mfrc522_2.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
    Serial.println(F("Reader 3 Details"));
    mfrc522_3.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
    Serial.println(F("Reader 4 Details"));
    mfrc522_4.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

    strip.begin();

    heartbeat();
}

void heartbeat(){
    strip.setPixelColor(0, 0, 255, 0); // G, R, B
    strip.setPixelColor(1, 0, 255, 0);
    strip.setPixelColor(2, 0, 255, 0);
    strip.setPixelColor(3, 0, 255, 0);
    strip.show();
    delay(500);

    strip.setPixelColor(0, 255, 0, 0); // G, R, B
    strip.setPixelColor(1, 255, 0, 0);
    strip.setPixelColor(2, 255, 0, 0);
    strip.setPixelColor(3, 255, 0, 0);
    strip.show();
    delay(500);

    strip.setPixelColor(0, 0, 0, 255); // G, R, B
    strip.setPixelColor(1, 0, 0, 255);
    strip.setPixelColor(2, 0, 0, 255);
    strip.setPixelColor(3, 0, 0, 255);
    strip.show();
    delay(500);

    // Initialize all pixels to 'off'
    strip.setPixelColor(0, 0, 0, 0); // G, R, B
    strip.setPixelColor(1, 0, 0, 0);
    strip.setPixelColor(2, 0, 0, 0);
    strip.setPixelColor(3, 0, 0, 0);
    strip.show();
}

void loop() {
    int correct = 0;
    int yellow = 0;
    int blue = 0;

    // Read all readers and check solutions
    read_cards( current_glyphs );


    for (int i=0; i<4; i++){
        // glyph in correct position
        if (current_glyphs[i] == solution[i]){
            correct++;
            blue++;
        }
        else if (current_glyphs[i] != solution[i]){
            // check for glyph in wrong position
            for (int j=0; j<4; j++){
                if (current_glyphs[i] == solution[j]){
                    yellow++;
                }
            }
        }
    }

    // Check that all readers have a glyph
    /* Serial.print(current_glyphs[0]); */
    /* Serial.print(", "); */
    /* Serial.print(current_glyphs[1]); */
    /* Serial.print(", "); */
    /* Serial.print(current_glyphs[2]); */
    /* Serial.print(", "); */
    /* Serial.println(current_glyphs[3]);
 */
    empty_readers = 0;
    for (int i=0; i<4; i++){
        if (current_glyphs[i] == 0){
            empty_readers = 1;
        }
    }


    // If all readers have a glyph on them, light the LEDs
    if (empty_readers == 0){

        /* Serial.print(current_glyphs[0]); */
        /* Serial.print(", "); */
        /* Serial.print(current_glyphs[1]); */
        /* Serial.print(", "); */
        /* Serial.print(current_glyphs[2]); */
        /* Serial.print(", "); */
        /* Serial.println(current_glyphs[3]); */


        // Color the LEDs
        // This method is used to remove association of led number with glyph number
        for (int i=0; i<blue; i++){
            strip.setPixelColor(i, 0x00, 0x00, 0xFF);  // blue
        }
        for (int i=blue; i< (blue+yellow); i++){
            strip.setPixelColor(i, 0xFF, 0xAF, 0x00);  // yellow
        }
        for (int i=(blue+yellow); i<4; i++){
            strip.setPixelColor(i, 0x00, 0x00, 0x00);
        }
        strip.show();

        // Check if all four are correct
        if (correct == 4){
            solved = true;
            strip.setPixelColor(0, 0xFF, 0x00, 0x00);
            strip.setPixelColor(1, 0xFF, 0x00, 0x00);
            strip.setPixelColor(2, 0xFF, 0x00, 0x00);
            strip.setPixelColor(3, 0xFF, 0x00, 0x00);
            strip.show();
            digitalWrite(DOOR, HIGH); // open the door
            delay(2000);
            digitalWrite(LIGHT, HIGH); // this is used to turn on the lights for the room
            reset_game();
        }
        else{
            strip.show();
        }
    }
}

void reset_game(){
    // reset game on product ID = 9 tag
    while(1){
        read_cards( current_glyphs );

        /* Serial.print("waiting for reset: "); */
        /* Serial.print(current_glyphs[0]); */
        /* Serial.print(", "); */
        /* Serial.print(current_glyphs[1]); */
        /* Serial.print(", "); */
        /* Serial.print(current_glyphs[2]); */
        /* Serial.print(", "); */
        /* Serial.println(current_glyphs[3]); */

        for (int i=0; i<4; i++){
            if (current_glyphs[i] == 9){
               Serial.println("9 DETECTED");
                digitalWrite(DOOR, LOW);
                digitalWrite(LIGHT, LOW);
                heartbeat();
                current_glyphs[0] = 0;
                current_glyphs[1] = 0;
                current_glyphs[2] = 0;
                current_glyphs[3] = 0;
                mfrc522_1.PCD_Init(); // Init MFRC522 card
                mfrc522_2.PCD_Init(); // Init MFRC522 card
                mfrc522_3.PCD_Init(); // Init MFRC522 card
                mfrc522_4.PCD_Init(); // Init MFRC522 card
                solved = false;
                return;
            }
        }
    }
}

void read_cards(int (&current_glyphs)[4]){

    // Look for new cards
    if ( ! mfrc522_1.PICC_IsNewCardPresent() && ! mfrc522_2.PICC_IsNewCardPresent() &&
         ! mfrc522_3.PICC_IsNewCardPresent() && ! mfrc522_4.PICC_IsNewCardPresent()){
        return;
    }

    // If a new card is read and there were 4 solutions, clear them all
    bool nozero = true;
    for (int i=0; i<4; i++){
        if (current_glyphs[i] == 0){
                nozero = false;
            }
    }
    if (nozero){
        current_glyphs[0] = 0;
        current_glyphs[1] = 0;
        current_glyphs[2] = 0;
        current_glyphs[3] = 0;
        mfrc522_1.PCD_Init(); // Init MFRC522 card
        mfrc522_2.PCD_Init(); // Init MFRC522 card
        mfrc522_3.PCD_Init(); // Init MFRC522 card
        mfrc522_4.PCD_Init(); // Init MFRC522 card
    }

    if (solved == false){
        // clear colors
        // need to detect card being removed
        strip.setPixelColor(0, 0, 0, 0);
        strip.setPixelColor(1, 0, 0, 0);
        strip.setPixelColor(2, 0, 0, 0);
        strip.setPixelColor(3, 0, 0, 0);
        strip.show();
    }

    byte sector         = 2;
    byte blockAddr      = 9;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);


    if ( mfrc522_1.PICC_ReadCardSerial())
    {
        Serial.println("READER 1");
        mfrc522_1.PICC_DumpMifareClassicSectorToSerial(&(mfrc522_1.uid), &key, sector);
        status = (MFRC522::StatusCode) mfrc522_1.MIFARE_Read(blockAddr, buffer, &size);

        if ((buffer[0] - 48) <= 10 && (buffer[0]-48) > 0)  {
            current_glyphs[0] = buffer[0] - 48; // convert from dec to ascii
        }

        // prevent same glyph from being placed on multiple readers
        for (int i=1; i<4; i++){
            if (current_glyphs[i] == (buffer[0] - 48)){
                current_glyphs[i] = 0;
            }
        }
        // Halt PICC
        mfrc522_1.PICC_HaltA();
        // Stop encryption on PCD
        mfrc522_1.PCD_StopCrypto1();
    }

    if ( mfrc522_2.PICC_ReadCardSerial())
    {
        Serial.println("READER 2");
        mfrc522_2.PICC_DumpMifareClassicSectorToSerial(&(mfrc522_2.uid), &key, sector);
        status = (MFRC522::StatusCode) mfrc522_2.MIFARE_Read(blockAddr, buffer, &size);

        if ((buffer[0] - 48) <= 10 && (buffer[0]-48) > 0)  {
            current_glyphs[1] = buffer[0] - 48;
        }

        // prevent same glyph from being placed on multiple readers
        if (current_glyphs[0] == (buffer[0] - 48)){
            current_glyphs[0] = 0;
        }
        for (int i=2; i<4; i++){
            if (current_glyphs[i] == (buffer[0] - 48)){
                current_glyphs[i] = 0;
            }
        }
        // Halt PICC
        mfrc522_2.PICC_HaltA();
        // Stop encryption on PCD
        mfrc522_2.PCD_StopCrypto1();
    }

    if ( mfrc522_3.PICC_ReadCardSerial())
    {
        Serial.println("READER 3");
        /* Serial.print(F("Card UID:")); */
        mfrc522_3.PICC_DumpMifareClassicSectorToSerial(&(mfrc522_3.uid), &key, sector);
        status = (MFRC522::StatusCode) mfrc522_3.MIFARE_Read(blockAddr, buffer, &size);

        if ((buffer[0] - 48) <= 10 && (buffer[0]-48) > 0)  {
            current_glyphs[2] = buffer[0] - 48; // convert from dec to ascii
        }

        // prevent same glyph from being placed on multiple readers
        if (current_glyphs[4] == (buffer[0] - 48)){
            current_glyphs[4] = 0;
        }
        for (int i=0; i<2; i++){
            if (current_glyphs[i] == (buffer[0] - 48)){
                current_glyphs[i] = 0;
            }
        }

        // Halt PICC
        mfrc522_3.PICC_HaltA();
        // Stop encryption on PCD
        mfrc522_3.PCD_StopCrypto1();

    }

    if ( mfrc522_4.PICC_ReadCardSerial())
    {
        Serial.println("READER 4");
        /* Serial.print(F("Card UID:")); */
        mfrc522_4.PICC_DumpMifareClassicSectorToSerial(&(mfrc522_4.uid), &key, sector);
        status = (MFRC522::StatusCode) mfrc522_4.MIFARE_Read(blockAddr, buffer, &size);

        if ((buffer[0] - 48) <= 10 && (buffer[0]-48) > 0)  {
            current_glyphs[3] = buffer[0] - 48; // convert from dec to ascii
        }

        for (int i=0; i<3; i++){
            if (current_glyphs[i] == (buffer[0] - 48)){
                current_glyphs[i] = 0;
            }
        }
        // Halt PICC
        mfrc522_4.PICC_HaltA();
        // Stop encryption on PCD
        mfrc522_4.PCD_StopCrypto1();
    }
    return;
}
