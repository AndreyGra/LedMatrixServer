#include "IOExpander.hpp"

IOExpander::IOExpander(){
    ADDR_SELECT = 0x20;
}

void IOExpander::configI2C(int clock){
  Wire.setClock(clock);
  Wire.begin(D1,D2);
}

void IOExpander::startTransmission(){
    Wire.beginTransmission(ADDR_SELECT);
}
void IOExpander::endTransmission(){
    Wire.endTransmission();
}

void IOExpander::writeGPIOA(byte value){
    startTransmission();
    Wire.write(GPIOA_SELECT); 
    Wire.write(value);
    endTransmission();  
}
void IOExpander::writeGPIOB(byte value){
    startTransmission();
    Wire.write(GPIOB_SELECT); 
    Wire.write(value);   
    endTransmission(); 
};

void IOExpander::configGPIOA(byte value){
    startTransmission();
    Wire.write(IOA_SELECT);
    Wire.write(value);
    endTransmission(); 
};
void IOExpander::configGPIOB(byte value){
    startTransmission();
    Wire.write(IOB_SELECT);
    Wire.write(value);
    endTransmission(); 
};
