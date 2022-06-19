#include "jrRangeFinder.h"

// others
#include <jrutils.h>


//---------------------------------------------------------------------------
void JrRangeFinder::init(float inOptionMinDist, float inOptionMaxDist, float inOptionOutlierDistanceThreshold) {
	optionMinDist = inOptionMinDist;
	optionMaxDist = inOptionMaxDist;
	optionOutlierDistanceThreshold = inOptionOutlierDistanceThreshold;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void JrRangeFinder::loop() {
	// get measurement
	if (!enabled) {
		return;
	}

	// read sensor, which is EXPECTED in range from optionMinDist to optionMaxDist, or -1 on error
	float newRange = readSensorValue();	

	// valid?
	if (newRange == DefBadValueRangeConstant) {
		// means value out of range (phase failure?)
		++badReadCount;
		// just leave our last range as our current setting
		return rangeNormalizedSmoothed;
	}

	// good value; store current reading
	rangeLast = range;
	range = newRange;
	float deltaAbs = abs(range-rangeLast);

	// it is an outlier?
	if (deltaAbs>optionOutlierDistanceThreshold) {
		if (outlierCount!=-1 && ++outlierCount > DefOutlierCountThreshold) {
			// looks like a weird outlier; don't incorporate it yet
			return;
		}
	}

	// valid value; reset outlier count
	outlierCount = 0;
	badReadCount = 0;
	
	// smooth it average it in
	rangeSmoothed = (DefAveragingFactor*rangeSmoothed) + ((1-DefAveragingFactor)*range);
	
	// normalized to 0-1 range
	rangeNormalizedSmoothed = ((rangeSmoothed - optionMinDist) / (optionMaxDist-optionMinDist));
	if (rangeNormalizedSmoothed<0.0) {
		rangeNormalizedSmoothed = 0.0;
	} else if (rangeNormalizedSmoothed>1.0) {
		rangeNormalizedSmoothed = 1.0;		
	}
	
	return;
}
//---------------------------------------------------------------------------



