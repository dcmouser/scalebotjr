#ifndef jrrangefinderSR04H
#define jrrangefinderSR04H

#include <jrRangeFinder.h>

//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
class JrRangeFinderSR04 : public JrRangeFinder {
protected:
	int echoPin, triggerPin;
	int pulseWidth;
	unsigned long pulseTimeOutMc;
public:
	JrRangeFinderSR04(int inechoPin, int intriggerPin, int inPulseWidth, unsigned long inpulseTimeOutMc, float inOptionMinDist, float inOptionMaxDist, float inOptionOutlierDistanceThreshold);
public:
	virtual bool begin();
public:
	virtual float readSensorValue();
};
//---------------------------------------------------------------------------










#endif
