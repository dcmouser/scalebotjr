#include "jrlcd.h"






//---------------------------------------------------------------------------
JrLcd::JrLcd(LiquidCrystal_I2C *inlcdp) {
	setupLcd(inlcdp);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void JrLcd::setupLcd(LiquidCrystal_I2C *inlcdp) {
	lcdp = inlcdp;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void JrLcd::begin(int cols, int rows) {
	  //initialize the lcd
  #ifdef LCD_LIB_NEWCRYSTAL
    // new crystal code
    lcdp->begin(cols, rows);
    lcdp->backlight();
  #else
    // dfrobot code
    lcdp->init();
  #endif
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
void JrLcd::setActive(bool val) {
	active = val;
	updateBrightness();
}

void JrLcd::setBrightness(int val) {
	brightness = val;
	updateBrightness();
}

void JrLcd::updateBrightness() {
	// ATTN: brightness control not possible in software for i2c lq display
	
	if (!active || brightness==0) {
		// turning display off
		if (!isOff) {
			// ATTN: todo - it's not currently off, so save contents for redisplay when we turn back on?
			isOff = true;
		}
		// clear it
		return;
	}

	// non-zero brightness
	if (isOff) {
		// ATTN: todo - restore contents?
		isOff = false;
	}
	// set brightness
	// ATTN: todo
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
void JrLcd::createScrollbarCharsForMenu() {
  const uint8_t scroll_bar[7][8] = {
    {B10001, B10001, B10001, B10001, B10001, B10001, B10001, B10001}, // blank
    {B11111, B11111, B11111, B11111, B11111, B11111, B10001, B10001}, // scrollbar top
    {B10001, B10001, B11111, B11111, B11111, B11111, B11111, B11111}, // scrollbar bottom
    {B11111, B11111, B10001, B10001, B10001, B10001, B10001, B10001}, // scroll state 1
    {B10001, B10001, B11111, B11111, B10001, B10001, B10001, B10001}, // scroll state 2
    {B10001, B10001, B10001, B10001, B11111, B11111, B10001, B10001}, // scroll state 3
    {B10001, B10001, B10001, B10001, B10001, B10001, B11111, B11111}, // scroll state 4
  }; 
    
  lcdp->createChar(0, (uint8_t*)scroll_bar[0]);
  lcdp->createChar(1, (uint8_t*)scroll_bar[1]);
  lcdp->createChar(2, (uint8_t*)scroll_bar[2]);
  lcdp->createChar(3, (uint8_t*)scroll_bar[3]);
  lcdp->createChar(4, (uint8_t*)scroll_bar[4]);
  lcdp->createChar(5, (uint8_t*)scroll_bar[5]);
  lcdp->createChar(6, (uint8_t*)scroll_bar[6]);

  // needed for https://github.com/arduino-libraries/LiquidCrystal/issues/6?
  lcdp->setCursor(0, 0);
  lcdp->clear();      
}
//---------------------------------------------------------------------------
