#include "jrlcdaf.h"






//---------------------------------------------------------------------------
JrLcdAf::JrLcdAf(Adafruit_LiquidCrystal *inlcdp) : JrLcd() {
	setupLcd(inlcdp);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void JrLcdAf::setupLcd(Adafruit_LiquidCrystal *inlcdp) {
	lcdp = inlcdp;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
bool JrLcdAf::begin(int cols, int rows) {
    bool bretv = lcdp->begin(cols, rows);
    backlightOn();
    return bretv;
}
//---------------------------------------------------------------------------

