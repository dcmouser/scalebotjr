#ifndef jrlcdNcH
#define jrlcdNcH


//---------------------------------------------------------------------------
// Which LiquidCrystal lib to use?
// new-liquid-crystal https://github.com/fmalpartida/New-LiquidCrystal
// sample https://www.nikolaus-lueneburg.de/2016/02/new-liquidcrystal-library/
#define LCD_LIB_NEWCRYSTAL

// OR undef the above and use this version: https://github.com/johnrickman/LiquidCrystal_I2C
// note that they use same file names so only one lib dir should be in Arduino libs folder
// One advantage of using lib_newcrystal is that it is used by some menu system?
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
#include "jrlcdb.h"
#include <LiquidCrystal_I2C.h>
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
class JrLcdNc : public JrLcd {
public:
	LiquidCrystal_I2C *lcdp = NULL;
public:
  JrLcdNc(LiquidCrystal_I2C *inlcdp=NULL);
public:
	// wrapping normal lcd functions
	virtual void setupLcd(LiquidCrystal_I2C *inlcdp);
	virtual bool begin(int cols, int rows);
public:
	virtual void clear() {lcdp->clear();};
	virtual void setCursor(uint8_t col, uint8_t row) {lcdp->setCursor(col, row);};
	virtual void backlightOn() {lcdp->backlight();};
	virtual void backlightOff() {lcdp->noBacklight();}
	virtual void print(long val) {lcdp->print(val);};
	virtual void print(const char c) {lcdp->print(c);};
	virtual void print(const char* charp) {lcdp->print(charp);};
	virtual void print(const __FlashStringHelper *fcharp) {lcdp->print(fcharp);};
	virtual void write(uint8_t val) {lcdp->write(val);};
	virtual void doCreateChar(int index, uint8_t* rowdatap) {lcdp->createChar(index, rowdatap);};
};
//---------------------------------------------------------------------------











#endif
