#ifndef jrrangefinderAf6180H
#define jrrangefinderAf6180H

#include <jrRangeFinder.h>

//---------------------------------------------------------------------------
#include "Adafruit_VL6180X.h"
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
class JrRangeFinderAf6180 : public JrRangeFinder {
protected:
	Adafruit_VL6180X* loxp = NULL;
public:
	JrRangeFinderAf6180(Adafruit_VL6180X *inloxp, float inOptionMinDist, float inOptionMaxDist, float inOptionOutlierDistanceThreshold);
public:
	virtual bool begin();
public:
	virtual float readSensorValue();
};
//---------------------------------------------------------------------------










#endif
