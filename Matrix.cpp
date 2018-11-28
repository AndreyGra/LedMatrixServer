#include "Matrix.hpp"

 Matrix::Matrix(){

        IO = IOExpander();
        matrixBitPattern  = generateBitPattern();
        matrixBytePattern = generateBytePattern(matrixBitPattern);

        //Configure I2C Bus
        IO.configI2C(400000);
        
        IO.configGPIOA(0x00);
        IO.configGPIOB(0x00);
        
        //Ensure that we start in a turned off position in case
        //the screen goes on before we're ready
        GPIOA = rowSelectA;
        GPIOB = rowSelectB;
    };

    void Matrix::update(){
            //Update rowSelect
            byte rowBits = rowSelect[currentRow];

            //Update ledPattern
            byte ledPattern = matrixBytePattern[currentRow];

            //update currentRow
            if (currentRow < 7) {
                currentRow++;
            }
            else {
                currentRow = 0;
            }

            GPIOA  =   ((rowBits & rowSelectA) | (ledPattern & ledSelectA)) ^ rowSelectA;
            GPIOB  =   ((rowBits & rowSelectB) | (ledPattern & ledSelectB)) ^ rowSelectB;
    }

    void Matrix::turnOn(){
            IO.writeGPIOA(GPIOA);
            IO.writeGPIOB(GPIOB);
    }

    void Matrix::turnOff(){
            IO.writeGPIOA(rowSelectA);
            IO.writeGPIOB(rowSelectB);
    }

    array<byte,8> Matrix::generateBytePattern(array< array<bool,8>,8> &bitPattern){
            array<byte,8> newBytePattern = {{0,0,0,0,0,0,0,0}};
            for( int row = 0; row < 8; row++){
                for( int col = 0; col < 8; col++){
                    if (bitPattern[row][col]){
                        newBytePattern[col] |= ledSelect[row];
                }
            }
        }

        return newBytePattern;
    }

    array< array<bool,8>,8> Matrix::generateBitPattern(){
                array< array<bool,8>,8> matrixPattern = {{
                                        {{1,1,1,1,1,1,1,1}},
                                        {{1,1,1,1,1,1,1,1}},
                                        {{1,1,1,1,1,1,1,1}},
                                        {{1,1,1,1,1,1,1,1}},
                                        {{1,1,1,1,1,1,1,1}},
                                        {{1,1,1,1,1,1,1,1}},
                                        {{1,1,1,1,1,1,1,1}},
                                        {{1,1,1,1,1,1,1,1}},
                                    }};

                    return matrixPattern;

                } 

    void Matrix::changeSingleLED(byte row, byte col){
        if ( row < 0 || col < 0){
          return;
          }
        if (row > 7 || col > 7){
          return;
          }

         matrixBytePattern[row] ^= ledSelect[col];
      
      }
