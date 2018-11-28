#include <ESP8266WiFi.h>
#include "IOExpander.hpp"
#include <array>
using namespace std;

class Matrix{
    private:

    //rowSelect[row] gives a binary number that selects a specific row
    IOExpander IO;
    array<byte,8> rowSelect = {{
                                    B00010000,
                                    B00001000,
                                    B00000010,
                                    B10000000,
                                    B00000001,
                                    B01000000,
                                    B00100000,
                                    B00000100,
                                }};

    //ledSelect[column] provide a binary value that selects an LED in a specific column
    array<byte,8> ledSelect = {{
                                    32,128,16,64,1,8,2,4,
                              }};

    //Addresses for the purposes of IO Expander
    // byte ADDR_SELECT  = 0x20; //Address which writes to correct IO expander
    // byte GPIOA_SELECT = 0x12; //Address which writes to GPIOA
    // byte GPIOB_SELECT = 0x13; //Address which writes to GPIOB
    // byte IOA_SELECT   = 0x00; //Address which writes configurations of GPIOA
    // byte IOB_SELECT   = 0x01; //Address which writes configurations of GPIOB

    //Bitmasks
    byte rowSelectA = B11100000;
    byte rowSelectB = B00011111;
    byte ledSelectA = ~rowSelectA;
    byte ledSelectB = ~rowSelectB;

    //Bit pattern
    array< array<bool,8>,8> matrixBitPattern;
    array<byte,8> matrixBytePattern;
    
    byte GPIOA;
    byte GPIOB;
    byte currentRow = 0;

    void setNewMatrix(array< array<bool,8>,8> matrix);
    void changeSingleLEDState(byte col, byte row);
    
    array<byte,8> generateBytePattern(array< array<bool,8>,8> &bitPattern);
    array< array<bool,8>,8> generateBitPattern();

    public:
    void update(); //updates row to be rendered

    void turnOn();  //Enables lights
    void turnOff(); //Disables lights

    void changeSingleLED(byte row, byte col);

    Matrix();
};

/*
This is how logically patterns get written to IO Expander:
LSB   ______ LSB
B0---|      |----A0 
B1---|      |----A1
B2---|      |----A2
B3---|      |----A3
B4---|      |----A4
B5---|      |----A5
B6---|      |----A6
B7---|      |----A7
MSB           MSB

This is how the outputs actually appear:

  MSG   ______ LSB
* B7---|      |----A0 *
* B6---|      |----A1 *
* B5---|      |----A2 *
* B4---|      |----A3 
* B3---|      |----A4 
  B2---|      |----A5
  B1---|      |----A6
  B0---|      |----A7
  LSB           MSB

(*) Denotes active low pins and represent row select pins

How GPIOA and GPIOB 8 bit binary numbers are generated:

              |A|| B | 
 rowSelect = BXXXXXXXX
 rowSelect for GPIOA = BXXXXXXXX AND B11100000;
 rowSelect for GPIOB = BXXXXXXXX AND B00011111;

              |A|| B | 
 ledSelect = BXXXXXXXX;
 ledSelect for GPIOA = BXXXXXXXX AND B00011111;
 ledSelect for GPIOB = BXXXXXXXX AND B11100000;

 Now combined rowSelect and ledSelect:
 A = rowSelectA | ledSelectA
 B = rowSelectB | ledSelectB

 Invert rowSelect Bits
 GPIOA = A ^ rowSelectA
 GPIOB = A ^ rowSelectB

 Note: I turned around rowSelectB because it's difficult to reverse a binary string systematically
       It was easier to just work with reverse string even though it breaks some logic
*/
