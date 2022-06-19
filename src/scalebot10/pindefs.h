#ifndef pindefsH
#define pindefsH

//---------------------------------------------------------------------------
// multiscale
#include <HX711-multi.h>
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// sleep button
//
// ATTN: IMPORTANT
// PIN 2 should work for waking from sleep with an interrupt for both UNO and MEGA but it doesn't seem to
// so we use PIN 2 for UNO and PIN 18 for MEGA
/*
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  const int buttonPinSleep = 19;
#else
  const int buttonPinSleep = 2;
#endif
//#define buttonPinInterrupt buttonPinSleep
//#define buttonInterruptState HIGH
*/

#define buttonPinInterrupt 18

#define buttonInterruptState HIGH // SEESAW_KEYPAD_EDGE_FALLING  // HIGH
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// neokeys keypad
#define neoKeypad_Cols 4
#define neoKeypad_Rows 2
uint8_t neoKeypadPins_Cols[] = {24, 25, 26, 27};
uint8_t neoKeypadPins_Rows[] = {28, 29};
#define neoKeypadPin_Neopixel 30
// not used but needed
char neoKeypadKeymap[neoKeypad_Rows][neoKeypad_Cols] = {
    {'1','2','3','4'},
    {'5','6','7','8'},
  };
//---------------------------------------------------------------------------

//
#define neoKeypadColorOption true
#define neoKeyValUser1 3
#define neoKeyValUser2 2
#define neoKeyValUserGuest 1
#define neoKeyValEsc 0
#define neoKeyValZeroTare 4
#define neoKeyValTimer 5
#define neoKeyValBackSpace 6
#define neoKeyValSleep 7
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// buzzer for sounds
const int pinBuzzer = 8;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// generic led
const int pinLed =  13;      // the number of the LED pin
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
const int wakeHelperBoardPin = 48;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// pins used by the HX711 scale connector
// set to 0 to disable a scale
//#define pinLoadcell1Dout  0
#define pinLoadcell1Dout  6
//#define pinLoadcell1Dout  6
//
#define pinLoadcell2Dout  5
//#define pinLoadcell2Dout  0
//#define pinLoadcell2Dout  5
//
#define pinLoadcallSck  4

// attempt to fix memory problem by doing this globally
byte GDOUTS[2] = {pinLoadcell1Dout, pinLoadcell2Dout};
HX711MULTI gscaleSet(2, GDOUTS , pinLoadcallSck);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// pins for rotary encoder
const int pinRotaryA = 2;
const int pinRotaryB = 3;
const int pinRotaryButton = 9;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// 20x4 lcd
#define pinLcd 0x27
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// 7 segment lcds using i2c
#define seg7I2cAddrA 0x70
#define seg7I2cAddrB 0x71
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define matrix8x8Addr 0x72
//---------------------------------------------------------------------------



#endif
