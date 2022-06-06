#ifndef lcddefsH
#define lcddefsH

//---------------------------------------------------------------------------
#include <jrlcd.h>
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// liquid crystal lib
#include <Wire.h> 
//---------------------------------------------------------------------------

// test, shouldnt be needed since its defined in jrlcd.h
// #define LCD_LIB_NEWCRYSTAL


//---------------------------------------------------------------------------
// these is also referenced in menudefs (the menu system)
#define DefLcdRows 4
#define DefLcdCols 20

#ifdef LCD_LIB_NEWCRYSTAL
  LiquidCrystal_I2C lclcd(pinLcd, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
  // see also jrlcd.begin(LCD_COLS, LCD_ROWS) call below in setup()
  JrLcd jrlcd(&lclcd);
#else
  LiquidCrystal_I2C lclcd(pinLcd,DefLcdCols,DefLcdRows);
  JrLcd jrlcd(&lclcd);
#endif
//---------------------------------------------------------------------------









#endif
