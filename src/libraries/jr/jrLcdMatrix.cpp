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
  jrb_frown_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100 },

  jrb_wifigood_bmp[] =
  { B11111111,
    B11000011,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B11000011,
    B11111111 },
  jrb_wifibad_bmp[] =
  { B11111111,
    B11000011,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B11000011,
    B11111111 };

/*  jrb_neutral_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10111101,
    B10000001,
    B01000010,
    B00111100 },
*/
/*
  jrb_wifigood_bmp[] =
  { B00000011,
		B00000011,
		B00000110,
		B00001100,
		B11001100,
		B11111000,
		B01110000,
		B00110000
 },
  jrb_wifibad_bmp[] =
  { B10111101,
    B11000011,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B11000011,
    B10111101 };
*/
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
	setupFont();
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

void JrLcdMatrix::setupFont() {
  matrix.setFont(&muHeavy8ptBold);
  //matrix.setFont(&FreeMono7pt7b);
  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
}


void JrLcdMatrix::printLetter(char c) {
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


void JrLcdMatrix::drawBitmapWifiGood() {
	drawBitmap(jrb_wifigood_bmp);
}
void JrLcdMatrix::drawBitmapWifiBad() {
	drawBitmap(jrb_wifibad_bmp);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
int JrLcdMatrix::jrRescaleRange(float min, float max, float val, int outMin, int outMax) {
	// get 0-1 value

	// clamp in range
	if (val<min) {
		val = min;
	}
	if (val>max) {
		val=max;
	}

	float relpos = (val-min) / (max-min);
	float outpos = outMin + ((outMax-outMin) * relpos);

	return (int) outpos;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void JrLcdMatrix::drawLevel(float val, float min, float max, int displayMode, bool flagWarn) {
	int width = 8;
	int height = 8;
	int dots = (width-2)*(height-2);
	
	matrix.clear();
	
	if (displayMode==1 && !flagWarn) {
		// only cares about warnings
		matrix.writeDisplay();
		return;
	}
	
	if (val==-1) {
		// this means CLEAR
		matrix.writeDisplay();
		return;
	}
	
	bool flagAsBars = true;
	if (displayMode==3) {
		flagAsBars=false;
	}

	// draw box around
	//matrix.drawRect(0,0,width,height, LED_ON);
	matrix.drawFastVLine(0,0,height-1, LED_ON);
	matrix.drawFastVLine(width-1,0,height-1, LED_ON);
	// bottom
	//matrix.drawFastHLine(1,height-1,width-2, LED_ON);
	matrix.drawFastHLine(0,height-1,width, LED_ON);

	if (flagWarn) {
		static int blinkerCount=0;
		if (++blinkerCount==height) {
			blinkerCount=0;
		}

		int y = blinkerCount+1;
		if (y<=height-1) {
			matrix.fillRect(1,y,width-2,height-1, LED_ON);
		}
/*
		// draw W
		matrix.setCursor(0,7);
		matrix.print('W');
		//matrix.drawRect(0,0,width,height, LED_ON);
*/
		if (flagAsBars || y<height-1) {
			matrix.writeDisplay();
			return;
		} else {
			// drop down to let the final value show through at bottom
		}
	}


	
	//Serial.print("Water level test: ");
	//Serial.print(val);

	if (flagAsBars) {
		// just number of lines
		int lineNum = jrRescaleRange(min,max,val,0,height-2);
		//Serial.println(lineNum);
		for (int i=0;i<=lineNum;++i) {
			matrix.drawFastHLine(1,height-(i+1),width-2, LED_ON);
		}
	} else {
		// dots from bottom to top
		int dotCount = jrRescaleRange(min,max,val,0,dots);
		// draw right to left
		int dotsLeft = dotCount;
		int y=height-2;
		int rowDots;
		while (dotsLeft>0) {
			rowDots = min(dotsLeft,width-2);
			matrix.drawFastHLine(1,y,rowDots, LED_ON);
			dotsLeft-=rowDots;
			--y;
		}
	}
	
  matrix.writeDisplay();
}
//---------------------------------------------------------------------------


