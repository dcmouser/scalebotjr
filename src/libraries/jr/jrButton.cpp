//
#include "JrButton.h"


//---------------------------------------------------------------------------
#define JrLongPressDuration 400
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
JrButton::JrButton(): Button() {
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool JrButton::wasPressLongPress() {
	// return true if the last detected press was a long press
	unsigned long prevDuration = previousDuration();
	if (prevDuration > JrLongPressDuration) {
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
bool JrButton::isUnstableOrIndeterminate() {
	// annoying because functions are marked private
  return((state & 0b00000010) != 0);
}


void JrButton::waitForButtonRelease() {
  // ATTN: this is used because the sleep button triggers when pressed down, and so we need to make sure it is released before we actually go to sleep
  // the reason we dont just trigger on release is because we dont want user to think they have to Hold down the sleep button to sleep
  
  // little bit of delay always?
  delay(500);

  // now make sure its not pressed before returning
  while(true) {
      update();
      
      // if button is down, keep looping
			if (readCurrentState()) {
				continue;
			} else if (isUnstableOrIndeterminate()) {
				continue;
			} else {
				// ok its ok
				break;
			}
    }
    // eat button press?
    update();
    bool bretv1 = pressed();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
JrPressType JrButton::getPressType() {
	if (!pressed()) {
		return JrPressTypeNone;
	}
	if (wasPressLongPress()) {
		return JrPressTypeLong;
	}
	return JrPressTypeShort;
}
//---------------------------------------------------------------------------