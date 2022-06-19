#ifndef jrrangefinderH
#define jrrangefinderH




//---------------------------------------------------------------------------
class JrRangeFinder {
public:
	JrRangeFinder() {;};
public:
	void init(float inOptionMinDist, float inOptionMaxDist, float inOptionOutlierDistanceThreshold);
public:
	virtual bool begin() {;};
	virtual float readSensorValue() { return -1.0; };
protected:
	bool enabled = false;
	float optionMinDist = 0;
	float optionMaxDist = 300;
	float optionOutlierDistanceThreshold = 10;
protected:
	bool lastSensorReadValid = false;
	float rangeSmoothed=0.0;
	float rangeNormalizedSmoothed = 0.0;
	float range = 0.0;
	float rangeLast = 0.0;
	int outlierCount = -1;
	int badReadCount = 0;
public:
	void loop();
protected:
	const float DefAveragingFactor = 0.90;
	const int DefOutlierCountThreshold = 20;
	const float DefBadValueRangeConstant = -12345;
public:
	bool getSensorReadIsValid() { return lastSensorReadValid; }
	float getSmoothedRange() {return rangeSmoothed;};
	float getSmoothedRangeNormalized() { return rangeNormalizedSmoothed; }
};
//---------------------------------------------------------------------------










#endif
