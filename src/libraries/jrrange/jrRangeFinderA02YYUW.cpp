#include "jrRangeFinderA02YYUW.h"


// see https://dronebotworkshop.com/waterproof-ultrasonic/



// call win mindist of 30 and maxdist of 4500
// by default pulseWidth should be 10 or 20 to be safe
//---------------------------------------------------------------------------
JrRangeFinderA02YYUW::JrRangeFinderA02YYUW(int inmode, int inechoPin, int intriggerPin, int inPulseWidth, unsigned long inpulseTimeOutMc, float inOptionMinDist, float inOptionMaxDist, float inOptionOutlierDistanceThreshold)
	 : JrRangeFinderSR04(inechoPin, intriggerPin, inPulseWidth, inpulseTimeOutMc, inOptionMinDist, inOptionMaxDist, inOptionOutlierDistanceThreshold) {
	mode = inmode;
}
//---------------------------------------------------------------------------
