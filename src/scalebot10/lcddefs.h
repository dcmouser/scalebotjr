#ifndef lcddefsH
#define lcddefsH

//---------------------------------------------------------------------------
#include <jrlcdaf.h>
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// liquid crystal lib
#include <Wire.h> 
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// these is also referenced in menudefs (the menu system)
#define DefLcdRows 4
#define DefLcdCols 20

#include <Adafruit_LiquidCrystal.h>
Adafruit_LiquidCrystal afLcd(3);
JrLcdAf jrlcd(&afLcd);

/*
// test, shouldnt be needed since its defined in jrlcd.h
// #define LCD_LIB_NEWCRYSTAL
#ifdef LCD_LIB_NEWCRYSTAL
	// from LiquidCrystal_i2C.h for backlight controlled 
	//   LiquidCrystal_I2C(uint8_t lcd_Addr, uint8_t En, uint8_t Rw, uint8_t Rs, uint8_t backlighPin, t_backlightPol pol);   // Constructor with backlight control
	//   LiquidCrystal_I2C(uint8_t lcd_Addr, uint8_t En, uint8_t Rw, uint8_t Rs, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t backlighPin, t_backlightPol pol);  LiquidCrystal_I2C lclcd(pinLcd, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
  // see also jrlcd.begin(LCD_COLS, LCD_ROWS) call below in setup()
  #ifdef LCD_OLD_SUNFOUNDER
  	LiquidCrystal_I2C lclcd(pinLcdOld, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
  #else
  	//LiquidCrystal_I2C lclcd(pinLcdNew, 2, 1, 0, 4, 5, 6, 7, 3, NEGATIVE);  
    LiquidCrystal_I2C lclcd(pinLcdNew, 2, 255, 1, 3, 4, 5, 6, 7, NEGATIVE);
  #endif
  // see also jrlcd.begin(LCD_COLS, LCD_ROWS) call below in setup()
  JrLcd jrlcd(&lclcd);
#else
  LiquidCrystal_I2C lclcd(pinLcd,DefLcdCols,DefLcdRows);
  JrLcd jrlcd(&lclcd);
#endif
*/
//---------------------------------------------------------------------------









#endif
