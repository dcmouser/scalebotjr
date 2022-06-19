#include "jrRangeFinderAf6180.h"




//---------------------------------------------------------------------------
JrRangeFinderAf6180::JrRangeFinderAf6180(Adafruit_VL6180X *inloxp, float inOptionMinDist, float inOptionMaxDist, float inOptionOutlierDistanceThreshold) : JrRangeFinder() {
	init(inOptionMinDist, inOptionMaxDist, inOptionOutlierDistanceThreshold);
	loxp = inloxp;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool JrRangeFinderAf6180::begin() {
	if (loxp->begin()) {
		enabled = true;
	} else {
		enabled = false;
	}
	Serial.print("JrRangeFinderAf begin result = ");
	Serial.println(enabled);

	// base stuff
	JrRangeFinder::begin();

	return enabled;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
float JrRangeFinderAf6180::readSensorValue() {
	uint8_t range = loxp->readRange();
	return (float)range;
}
//---------------------------------------------------------------------------



