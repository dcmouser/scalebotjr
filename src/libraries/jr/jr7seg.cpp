#include "jr7seg.h"



//---------------------------------------------------------------------------
Jr7Seg::Jr7Seg() {
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void Jr7Seg::setup(int in_i2cAddress) {
	i2cAddress = in_i2cAddress;
	lcdseg.begin(i2cAddress);
	// initial dim brightness (but not off)
	setBrightness(1);
	
  lcdseg.print("0000");
  lcdseg.writeDisplay();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void Jr7Seg::setActive(bool val) {
	active = val;
	updateBrightness();
}
	
	
	
void Jr7Seg::setBrightness(int val) {
	brightness = val;
	updateBrightness();
}


void Jr7Seg::updateBrightness() {
	// 15 is max brightness? (see adafruit file for led backpack)
	if (!active || brightness==0) {
		// turning display off
		if (!isOff) {
			// it's not currently off, so save contents for redisplay when we turn back on 
			memcpy(displaybuffer, lcdseg.displaybuffer, 16);
			isOff = true;
		}
		// turn off display
		lcdseg.setBrightness(0);
		lcdseg.clear();
		lcdseg.writeDisplay();
		// done
		return;
	}

	// non-zero brightness
	if (isOff) {
		// going from off to on restore last display
		lcdseg.clear();
		memcpy(lcdseg.displaybuffer, displaybuffer, 16);
		lcdseg.writeDisplay();
		isOff = false;
		// drop down
	}

	// set actual brightness
	lcdseg.setBrightness(brightness-1);
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
void Jr7Seg::printFloat(float val, int digits) {
	// use library float print
	if (!isLit()) {
		return;
	}

	lcdseg.print(val, digits);
	lcdseg.writeDisplay();
}


void Jr7Seg::printElapsedTime(unsigned long elapsedMs) {
	if (!isLit()) {
		return;
	}

	//elapsedMs += 60000;
	if (elapsedMs==0) {
		lcdseg.clear();
		lcdseg.drawColon(false);
		lcdseg.writeDisplay();
		return;
	}
	if (elapsedMs<60000) {
  	int seconds = floor((float)elapsedMs / 1000.0);
  	int ms = elapsedMs%1000;
  	int msf = floor((float)ms/100.0);
		char outbuf[12];
    sprintf(outbuf," %2d%d",seconds,msf);
    //
    // we need to manually print the decimal point because we only want 1 digit after it
		lcdseg.println();
		lcdseg.print(outbuf);
		lcdseg.drawColon(false);
		lcdseg.writeDigitAscii(3, outbuf[2],true);
		lcdseg.println();
		lcdseg.writeDisplay();
    return;
  }

	// minutes and seconds
	char outbuf[12];
  unsigned long seconds = floor((float)elapsedMs / 1000.0);
  unsigned long oseconds = seconds;
  unsigned int minutes = floor((float)seconds / 60.0);
  seconds = seconds - (minutes*60);
  unsigned int hours = floor((float)minutes / 60.0);
  minutes -= hours*60;
  bool flagColon = false;
  if (hours>0) {
    sprintf(outbuf,"%02d%02d",hours,minutes);
    flagColon = true;
  } else if (minutes>0) {
    sprintf(outbuf,"%02d%02lu",minutes,seconds);
    flagColon = true;
  } 

	lcdseg.println();
	lcdseg.print(outbuf);
	lcdseg.drawColon(flagColon);
	lcdseg.println();
	lcdseg.writeDisplay();
}
//---------------------------------------------------------------------------