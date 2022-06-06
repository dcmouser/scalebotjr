//---------------------------------------------------------------------------
#include "jrneokeys.h"
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
JrNeoKeyMatrixKeypad::JrNeoKeyMatrixKeypad() {
	// constructor
}

JrNeoKeyMatrixKeypad::~JrNeoKeyMatrixKeypad() {
	if (flagWeAllocated) {
		delete[] keydown;
		delete[] userKeymap;
		delete customKeypadp;
		delete neoPixelStripp;
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void JrNeoKeyMatrixKeypad::setup(int inrows, int incols, uint8_t* inRowPins, uint8_t* inColPins, Adafruit_Keypad* existingkp, Adafruit_NeoPixel* existingnp, bool* inkeydownp, bool inoptionShowColor, bool indebug) {
	// setup details
	rows = inrows;
	cols = incols;
	optionShowColor = inoptionShowColor;
	debug = indebug;
	colPins = inColPins;
	rowPins = inRowPins;
	
	// use the values passed in
	neoPixelStripp = existingnp;
	customKeypadp = existingkp;

	// the keydown list
	keydown = inkeydownp;

	// init hardware
  neoPixelStripp->begin();
  neoPixelStripp->setBrightness(40);
  neoPixelStripp->show(); // Initialize all pixels to 'off'

  // init keypad hardware
  customKeypadp->begin();
  
  if (debug) {
  	Serial.println("JrNeoKeyMatrixKeypad setup complete.");
	}
}



void JrNeoKeyMatrixKeypad::setupMEMORYLEAK(int inrows, int incols, uint8_t* rowPins, uint8_t* colPins, int neopixelPin,  bool inoptionShowColor, bool indebug) {
	// this function SHOULD work but seems to trigger some bug in arduino -- running out of heap?
	// ATTN: causes MEMORY problems using NEW (!!!)
/*
	flagWeAllocated = true;

	keydown = new bool[rows*cols];
	for (int i=0;i<rows*cols;++i) {
		keydown[i]=false;
	}

	// neo pixels
	neoPixelStripp = new Adafruit_NeoPixel(rows*cols, neopixelPin, NEO_GRB + NEO_KHZ800);
	//
	// make the annoying keymap which needs to stay valid
	userKeymap = new byte[rows*cols];
	for (int i=0;i<rows*cols;++i) {
		userKeymap[i]=i+1;
	}
	// adafruit keypad
	customKeypadp = new Adafruit_Keypad( makeKeymap(userKeymap), rowPins, colPins, rows, cols);
	//
	setup(inrows, incols, rowPins, colPins, customKeypadp, neoPixelStripp, inoptionShowColor, indebug);
*/
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void JrNeoKeyMatrixKeypad::setCallbackFp(FuncPointerKeypressCallback infp) {
	keypressCallbackfp = infp;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void JrNeoKeyMatrixKeypad::justTick() {
  // tick the library code update
  customKeypadp->tick();
}
	
	
void JrNeoKeyMatrixKeypad::loop(bool processCallbacks) {
  // tick the library code update
  customKeypadp->tick();

  //Serial.println("L");
  
  bool needsColorUpdate = false;

  // now swallow all waiting "events"
  while(customKeypadp->available()) {
		// Serial.println("E");
  	// get event
    keypadEvent e = customKeypadp->read();

		// location in matrix
    uint8_t row = e.bit.ROW;
    uint8_t col = e.bit.COL;
    // compute keynum
    int keynum;
    if (row % 2 == 0) { // even row
      keynum = row * cols + (3-col);
    } else { // odd row the neopixels go BACKWARDS!
      keynum = row * cols + col;
    }
    
		if (debug) {
     	Serial.print(" ATTN: JrNeoKeyMatric event ");
     	Serial.print(" event: ");
     	Serial.print((int)e.bit.EVENT);
     	Serial.print(" row: "); Serial.print(row);
     	Serial.print(" col: "); Serial.print(col);
     	Serial.print(" : #");
     	Serial.println(keynum);
     }
    
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      // triggers on press down (not release)
      
			// mark key as down
      keydown[keynum] = true;
      needsColorUpdate = true;

      // calback keypress
      if (processCallbacks) {
      	keypressCallbackfp(JrKeyCallbackType_Down, keynum);
      }
    }
    else if(e.bit.EVENT == KEY_JUST_RELEASED) {
			// mark key as up
			//Serial.print("Marking key up, it was ");
			//Serial.println(keydown[keynum]);
			//
      keydown[keynum] = false;
      needsColorUpdate = true;

    	if (processCallbacks) {
      	// calback keypress
      	keypressCallbackfp(JrKeyCallbackType_Up, keynum);
      }
    }
  }
  
  // color update oif neopixels
  if (true || (optionShowColor && needsColorUpdate)) {
  	updateNeopixelColors();
  }
}


void JrNeoKeyMatrixKeypad::updateNeopixelColors() {
  for (int i=0; i< neoPixelStripp->numPixels(); i++) {
    if (keydown[i]) {
      neoPixelStripp->setPixelColor(i, wheelColor(((i * 256 / neoPixelStripp->numPixels()) + colorOffset) & 255));
    } else {
      neoPixelStripp->setPixelColor(i, 0);
    }
  }
  neoPixelStripp->show();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t JrNeoKeyMatrixKeypad::wheelColor(byte WheelPos) {
  if(WheelPos < 85) {
   return neoPixelStripp->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return neoPixelStripp->Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return neoPixelStripp->Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
bool JrNeoKeyMatrixKeypad::isAnyKeyDown() {
	for (int i=0;i<rows*cols;++i) {
		if (keydown[i]) {
			return true;
		}
	}
	return false;
}

void JrNeoKeyMatrixKeypad::waitForNoKeysPressed() {
	loop(false);
	while (isAnyKeyDown() || customKeypadp->available()) {
		loop(false);
		delay(10);
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void JrNeoKeyMatrixKeypad::prepareForSleep() {
	waitForNoKeysPressed();
	// run all col pins low to respond to keypress like an interrupt
	for (int c=0;c<cols;++c) {
		digitalWrite(colPins[c], LOW);
	}
}


void JrNeoKeyMatrixKeypad::prepareForWake(bool waitForNoKeys) {
	if (waitForNoKeys) {
		waitForNoKeysPressed();
	}
}
//---------------------------------------------------------------------------
