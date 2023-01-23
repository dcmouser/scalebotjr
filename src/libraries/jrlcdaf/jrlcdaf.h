#ifndef jrlcdAf
#define jrlcdAf


//---------------------------------------------------------------------------
#include <Adafruit_LiquidCrystal.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "jrlcdb.h"
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
class JrLcdAf : public JrLcd {
public:
	Adafruit_LiquidCrystal *lcdp = NULL;
public:
  JrLcdAf(Adafruit_LiquidCrystal *inlcdp=NULL);
public:
	// wrapping normal lcd functions
	virtual void setupLcd(Adafruit_LiquidCrystal *inlcdp);
	virtual bool begin(int cols, int rows);
public:
	virtual void clear() {lcdp->clear();};
	virtual void setCursor(uint8_t col, uint8_t row) {lcdp->setCursor(col, row);};
	virtual void backlightOn() {lcdp->setBacklight(HIGH);};
	virtual void backlightOff() {lcdp->setBacklight(LOW);}
	virtual void print(unsigned long val) {lcdp->print(val);};
	virtual void print(long val) {lcdp->print(val);};
	virtual void print(const char c) {lcdp->print(c);};
	virtual void print(const char* charp) {lcdp->print(charp);};
	virtual void print(const __FlashStringHelper *fcharp) {lcdp->print(fcharp);};
	virtual void write(uint8_t val) {lcdp->write(val);};
public:
	virtual void doCreateChar(int index, uint8_t* rowdatap) {lcdp->createChar(index, rowdatap);};
};
//---------------------------------------------------------------------------











#endif
