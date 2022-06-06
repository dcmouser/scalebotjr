#ifndef jrrotaryencoderH
#define jrrotaryencoderH



//

//---------------------------------------------------------------------------
enum JrEncoderRotaryDirection {JrEncoderRotaryDirectionLeft, JrEncoderRotaryDirectionRight, JrEncoderRotaryDirectionNone};
//---------------------------------------------------------------------------



#include <Encoder.h>

class JrEncoder: public Encoder {
protected:
	// ATTN: jr attempt to fix spurious encoder trigger where it thinks we go the right direction followed immediately by one tick in opposite direction;
  // so here we disregard a reverse direction tick if it happens too quickly
  // ATTN: im not sure this is anymore needed if we use interrupt pins
  const unsigned long kludgeIgnoreReverseMs = 200;
  const bool kludgeReversalIgnore = true;
  const unsigned long acelerationIntervalMs = 400;
  const unsigned long acelerationSlowIntervalMs = 800;
  unsigned long lastDirectionClickTime;
  JrEncoderRotaryDirection lastDirection = JrEncoderRotaryDirectionNone;
  int acceleration;
public:
	JrEncoder(uint8_t pin1, uint8_t pin2);
	//virtual ~JrEncoder() {;}
public:
	JrEncoderRotaryDirection update();
public:
	JrEncoderRotaryDirection getLastTurnDirection() { return lastDirection; }
	int getAccelerationJumpSteps();
};


#endif