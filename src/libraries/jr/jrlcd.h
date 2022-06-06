#ifndef jrlcdH
#define jrlcdH


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
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
//
#include <LiquidCrystal_I2C.h>
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
class JrLcd {
public:
	LiquidCrystal_I2C *lcdp = NULL;
	uint8_t brightness = 1;
	bool isOff = false;
	bool active = true;
public:
  JrLcd(LiquidCrystal_I2C *inlcdp=NULL);
public:
	void setupLcd(LiquidCrystal_I2C *inlcdp);
	void begin(int cols, int rows);
public:
	void setActive(bool val);
	void setBrightness(int val);
	void updateBrightness();
public:
	void createScrollbarCharsForMenu();
public:
	// wrapping normal lcd functions
	void clear() {lcdp->clear();};
	void setCursor(uint8_t col, uint8_t row) {lcdp->setCursor(col, row);};
	void backlight() {lcdp->backlight();};
	void noBacklight() {lcdp->noBacklight();}
	void print(const char c) {lcdp->print(c);};
	void print(const char* charp) {lcdp->print(charp);};
	void print(const __FlashStringHelper *fcharp) {lcdp->print(fcharp);};
	void write(uint8_t val) {lcdp->write(val);};
};
//---------------------------------------------------------------------------











#endif
