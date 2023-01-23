#ifndef jrlcdBH
#define jrlcdBH


//---------------------------------------------------------------------------
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
class JrLcd {
public:
	uint8_t brightness = 1;
	bool isOff = false;
	bool active = true;
public:
  JrLcd() {;};
public:
	void setActive(bool val);
	void setBrightness(int val);
	void updateBrightness();
public:
	void createScrollbarCharsForMenu();
public:
	// wrappers
	virtual bool begin(int cols, int rows) {return false;};
	virtual void clear() {;};
	virtual void setCursor(uint8_t col, uint8_t row) {;};
	virtual void backlightOn() {;};
	virtual void backlightOff() {;}
	virtual void print(unsigned long val) {;};
	virtual void print(long val) {;};
	virtual void print(const char c) {;};
	virtual void print(const char* charp) {;};
	virtual void print(const __FlashStringHelper *fcharp) {;};
	virtual void write(uint8_t val) {;};
	virtual void doCreateChar(int index, uint8_t* rowdatap) {;};
};
//---------------------------------------------------------------------------











#endif
