// One Way Out Glyph Game
// by Adam Spontarelli

#include <Adafruit_NeoPixel.h>

#define PIN 6


Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, PIN, NEO_GRB + NEO_KHZ800);


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

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN1          5          // Configurable, see typical pin layout above
#define SS_PIN2          6          // Configurable, see typical pin layout above
#define SS_PIN3          7          // Configurable, see typical pin layout above
#define SS_PIN4          8          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN1, RST_PIN);   // Create MFRC522 instance.
MFRC522 mfrc522(SS_PIN2, RST_PIN);   // Create MFRC522 instance.
MFRC522 mfrc522(SS_PIN3, RST_PIN);   // Create MFRC522 instance.
MFRC522 mfrc522(SS_PIN4, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

void setup() {
    Serial.begin(9600); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write."));
    Serial.print(F("Using key (for A and B):"));
    dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();

    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    }


void loop() {

    int correct = 0;
    // Read all readers and check solutions
    for (int i=0; i<4; i++){
        int current_glyph = get_glyph(i);

        // glyph in correct position
        if (current_glyph == solution[i]){
            strip.setPixelColor(i, 0, 200, 0);
            correct++;
        }
        else if (current_glyph != solution[i]){
            bool found = false;
            // check for glyph in wrong position
            for (int j=0; j<4; j++){
                if (current_glyph == solution[j]){
                    found = true;
                    strip.setPixelColor(i, 0, 200, 200);
                }
            }
            // glyph not in solution set
            if (found == false){
                strip.setPixelColor(i, 100, 0, 0);
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

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

int get_glyph(int n){
    // Read the glyph (RFID tag) on the nth RFID reader
    int glyph;

    // Glow LED when reading
    for (int i=0; i<256; i++){
        strip.setPixelColor(n, 100, 100, 100, i);
        strip.show();
    }

    // Read RFID
    glyph = read_card();


    for (int i=0; i<256; i++){
        strip.setPixelColor(n, 100, 100, 100, 255-i);
        strip.show();
    }

    return glyph;
}

int read_card(){
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }

    // In this sample we use the second sector,
    // that is: sector #1, covering block #4 up to and including block #7
    byte sector         = 2;
    byte blockAddr      = 9;
    byte dataBlock[]    = {
        0x01, 0x02, 0x03, 0x04, //  1,  2,   3,  4,
        0x05, 0x06, 0x07, 0x08, //  5,  6,   7,  8,
        0x08, 0x09, 0xff, 0x0b, //  9, 10, 255, 12,
        0x0c, 0x0d, 0x0e, 0x0f  // 13, 14,  15, 16
    };
    byte trailerBlock   = 7;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    Serial.println();


    // Read data from the block
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();
    Serial.println();

    int cardID = buffer[0];
    if (cardID == 49){
      Serial.println("Card 1");
    }
    else if (cardID == 50){
      Serial.println("Card 2");
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    return cardID;
}
