#include "jrlcdnc.h"






//---------------------------------------------------------------------------
JrLcd::JrLcdNc(LiquidCrystal_I2C *inlcdp) : JrLcd() {
	setupLcd(inlcdp);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void JrLcdNc::setupLcd(LiquidCrystal_I2C *inlcdp) {
	lcdp = inlcdp;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
bool JrLcdNc::begin(int cols, int rows) {
	  //initialize the lcd
	  bool bretv;
  #ifdef LCD_LIB_NEWCRYSTAL
    // new crystal code
    bretv = lcdp->begin(cols, rows);
    backlightOn();
  #else
    // dfrobot code
    lcdp->init();
    bretv = true;
  #endif
  return bretv;
}
//---------------------------------------------------------------------------


