#include <ESP8266WiFi.h>
#include "Wire.h"

class IOExpander{
    private:
    byte ADDR_SELECT; //Address which writes to correct IO expander

    byte GPIOA_SELECT = 0x12; //Address which writes to GPIOA
    byte GPIOB_SELECT = 0x13; //Address which writes to GPIOB
    byte IOA_SELECT   = 0x00; //Address which writes configurations of GPIOA
    byte IOB_SELECT   = 0x01; //Address which writes configurations of GPIOB

    void startTransmission();
    void endTransmission();

    public:
    void ICACHE_RAM_ATTR writeGPIOA(byte value);
    void ICACHE_RAM_ATTR writeGPIOB(byte value);
    void configGPIOA(byte value);
    void configGPIOB(byte value);
    void configI2C(int clock);
    IOExpander();
};
