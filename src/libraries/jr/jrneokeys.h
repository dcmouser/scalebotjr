#ifndef jrneokeysH
#define jrneokeysH

//---------------------------------------------------------------------------
#include <Adafruit_NeoPixel.h>
#include "Adafruit_Keypad.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// function prototype for callbacks
enum JrKeyCallbackType {JrKeyCallbackType_Down, JrKeyCallbackType_Up};
typedef void (*FuncPointerKeypressCallback)(JrKeyCallbackType eventType, int keynum);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
class JrNeoKeyMatrixKeypad {
public:
	FuncPointerKeypressCallback keypressCallbackfp;
	int rows, cols;
	uint8_t * colPins;
	uint8_t * rowPins;
	//
	byte *userKeymap = NULL;
	bool *keydown = NULL;
	//
	bool debug = true;
	bool optionShowColor = true;
	int colorOffset = 0;
	bool flagWeAllocated = false;
public:
	Adafruit_Keypad *customKeypadp = NULL;
	Adafruit_NeoPixel *neoPixelStripp = NULL;
public:
	JrNeoKeyMatrixKeypad();
	virtual ~JrNeoKeyMatrixKeypad();
public:
	void setup(int inrows, int incols, uint8_t* inRowPins, uint8_t* inColPins, Adafruit_Keypad* existingkp, Adafruit_NeoPixel* existingnp, bool* inkeydownp, bool inoptionShowColor, bool indebug);
	// ATTN: this call bugs arduino, possibly due to heap space limitations?
	void setupMEMORYLEAK(int inrows, int incols, uint8_t* rowPins, uint8_t* colPins, int neopixelPin,  bool inoptionShowColor, bool indebug);
	//
	void setCallbackFp(FuncPointerKeypressCallback infp);
	void loop(bool processCallbacks);
	void justTick();
	void updateNeopixelColors();
	uint32_t wheelColor(byte WheelPos);
public:
	bool isAnyKeyDown();
	void waitForNoKeysPressed();
public:
	void prepareForSleep();
	void prepareForWake(bool waitForNoKeys);
};
//---------------------------------------------------------------------------





#endif