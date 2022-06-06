//

#include "JrRotaryEncoder.h"


//---------------------------------------------------------------------------
JrEncoder::JrEncoder(uint8_t pin1, uint8_t pin2) : Encoder(pin1, pin2) {
	lastDirection=JrEncoderRotaryDirectionNone;
	lastDirectionClickTime=0;
	acceleration=1;
};
//---------------------------------------------------------------------------
	
	
//---------------------------------------------------------------------------
JrEncoderRotaryDirection JrEncoder::update() {
	//
  int32_t pos = read();
  uint8_t decodedTurn = JrEncoderRotaryDirectionNone;
  unsigned long millisTime;

  if (pos <= -3) {
    millisTime = millis();
    if (!kludgeReversalIgnore || lastDirection!=-1 || millisTime-lastDirectionClickTime > kludgeIgnoreReverseMs) {
      // valid turn
      decodedTurn = JrEncoderRotaryDirectionRight;
      //Serial.println("down");
      } else {
      //Serial.println("ignore-down");
      }
    // init encoder for the next step
    write(pos+4);
  }
  else if (pos >= 3) {
    millisTime = millis();
    if (!kludgeReversalIgnore || lastDirection!=1 || millisTime-lastDirectionClickTime > kludgeIgnoreReverseMs) {
      // valid turn
      decodedTurn = JrEncoderRotaryDirectionLeft;
      //Serial.println("up");
      } else {
      //Serial.println("ignore-up");
      }
    // init encoder for the next step
    write(pos-4);
   }

  // accelaration?
	if (decodedTurn != JrEncoderRotaryDirectionNone) {
    if (lastDirection==decodedTurn && millis()-lastDirectionClickTime < acelerationIntervalMs) {
     	// fast rotation in same direction increases acceleration
      ++acceleration;
    } else if (acceleration>=4 && lastDirection==decodedTurn && millis()-lastDirectionClickTime < acelerationSlowIntervalMs) {
   		// slower rotation in same direction slows down acceleration
   		acceleration /= 2;
    } else {
    	// too slow or reverse direction resets acceleration
   	  acceleration = 1;
    }
    // update
    lastDirectionClickTime=millisTime;
 	}

  // track
  lastDirection = decodedTurn;

	return lastDirection;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
int JrEncoder::getAccelerationJumpSteps() {
	//return 1;
	if (acceleration<5) {
		return 1;
	}
	// this delays the kicking in of acceleration and makes it work in jumps of 5/10/15/etc.
	return ceil(acceleration/5)*5;
}
//---------------------------------------------------------------------------

