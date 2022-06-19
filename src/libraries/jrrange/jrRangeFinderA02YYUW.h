#ifndef jrrangefinderA02YYUWH
#define jrrangefinderA02YYUWH


//---------------------------------------------------------------------------
#include <jrRangeFinderSR04.h>
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
class JrRangeFinderA02YYUW : public JrRangeFinderSR04 {
protected:
	int mode;
public:
	JrRangeFinderA02YYUW(int inmode, int inechoPin, int intriggerPin, int inPulseWidth, unsigned long inpulseTimeOutMc, float inOptionMinDist, float inOptionMaxDist, float inOptionOutlierDistanceThreshold);
};
//---------------------------------------------------------------------------










#endif
