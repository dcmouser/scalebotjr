#ifndef jrLcdMatrixH
#define jrLcdMatrixH


#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>


//---------------------------------------------------------------------------
// fonts
#include <muHeavy8ptBold.h>
//#include <FreeMonoBold7pt7b.h>
//#include <FreeMono7pt7b.h>

//---------------------------------------------------------------------------



// #define JrLBufferBitmapByteSize(w,h) ((w + 7) / 8) * h


//---------------------------------------------------------------------------
class JrLcdMatrix {
public:
	Adafruit_8x8matrix matrix;
	int i2cAddress;
	uint8_t brightness = 1;
	bool isOff = false;
	bool active = true;
	byte displaybuffer[16];
public:
  JrLcdMatrix();
public:
	void setup(int in_i2cAddress, uint8_t rotation);
protected:
	void setupFont() ;
public:
	void setActive(bool val);
	void setBrightness(int val);
	void updateBrightness();
public:
	void clear();
	void printLetter(char c);
	void drawBitmapSmiley();
	void drawBitmapFrown();
	void drawBitmapWifiGood();
	void drawBitmapWifiBad();
	void drawLevel(float val, float min, float max, int displayMode, bool flagWarn);
	void drawBitmap(const uint8_t PROGMEM* bmpp);
public:
	int jrRescaleRange(float min, float max, float val, int outMin, int outMax);
};
//---------------------------------------------------------------------------














#endif
