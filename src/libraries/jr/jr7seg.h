#ifndef jr7segH
#define jr7segH


#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>



//---------------------------------------------------------------------------
class Jr7Seg {
public:
	Adafruit_7segment lcdseg;
	int i2cAddress;
	uint8_t brightness = 1;
	bool isOff = false;
	bool active = true;
	uint16_t displaybuffer[10];
public:
  Jr7Seg();
public:
	void setup(int in_i2cAddress);
public:
	void setActive(bool val);
	void setBrightness(int val);
	void updateBrightness();
	void printFloat(float val, int digits);
	void printElapsedTime(unsigned long elapsedMs);
	bool isLit() { return (active && brightness>0); }
};
//---------------------------------------------------------------------------














#endif
