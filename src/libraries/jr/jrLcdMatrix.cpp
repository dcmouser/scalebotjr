#include "jrLcdMatrix.h"


//---------------------------------------------------------------------------
const uint8_t PROGMEM
  jrb_smile_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
  jrb_neutral_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10111101,
    B10000001,
    B01000010,
    B00111100 },
  jrb_frown_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100 };
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
JrLcdMatrix::JrLcdMatrix() {
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void JrLcdMatrix::setup(int in_i2cAddress, uint8_t rotation) {
	i2cAddress = in_i2cAddress;
	matrix.begin(i2cAddress);
	matrix.setRotation(rotation);
	clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void JrLcdMatrix::setActive(bool val) {
	active = val;
	updateBrightness();
}

void JrLcdMatrix::setBrightness(int val) {
	brightness = val;
	updateBrightness();
}


void JrLcdMatrix::updateBrightness() {
	// 15 is max brightness? (see adafruit file for led backpack)
	if (!active || brightness==0) {
		// turning display off
		if (!isOff) {
			// ATTN: todo - it's not currently off, so save contents for redisplay when we turn back on?
			memcpy(displaybuffer, matrix.displaybuffer, 16);
			isOff = true;
		}
		matrix.setBrightness(0);
		clear();
		return;
	}

	// non-zero brightness
	if (isOff) {
		// ATTN: todo - restore contents?
		matrix.clear();
		memcpy(matrix.displaybuffer, displaybuffer, 16);
		matrix.writeDisplay();
		isOff = false;
		// drop down
	}

	// set brightness
	matrix.setBrightness(brightness-1);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void JrLcdMatrix::clear() {
	matrix.clear();
	matrix.writeDisplay();
}


void JrLcdMatrix::printLetter(char c) {
	matrix.setFont(&muHeavy8ptBold);
	//matrix.setFont(&FreeMono7pt7b);
	
  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
  //
  matrix.clear();
  //matrix.setCursor(1,1);
  matrix.setCursor(0,7);
  matrix.print(c);
  matrix.writeDisplay();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void JrLcdMatrix::drawBitmap(const uint8_t PROGMEM* bmpp) {
  matrix.clear();
  matrix.drawBitmap(0, 0, bmpp, 8, 8, LED_ON);
  matrix.writeDisplay();
}

void JrLcdMatrix::drawBitmapSmiley() {
	drawBitmap(jrb_smile_bmp);
}

void JrLcdMatrix::drawBitmapFrown() {
	drawBitmap(jrb_frown_bmp);
}
//---------------------------------------------------------------------------
