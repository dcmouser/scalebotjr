#ifndef jrscaleH
#define jrscaleH


//---------------------------------------------------------------------------
//#define JrTestFix true
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// hx711 lib
#include "HX711.h"
// lcd
#include <LiquidCrystal_I2C.h>
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// callback typedefs
enum JrEventCallbackEnum {JrEventCallbackEnum_Interaction, JrEventCallbackEnum_WfAdvance, JrEventCallbackEnum_CoffeeDone, JrEventCallbackEnum_PlaySoundAndWait, JrEventCallbackEnum_PlayBackgroundSound, JrEventCallbackEnum_PlayErrorSound, JrEventCallbackEnum_CoffeeSelect};
typedef void (*VoidVoidCallbackFpT)();
typedef void (*VoidEventCallbackFpT) (JrEventCallbackEnum);
enum JrWeightChangeSensitivityMode {JrWeightChangeSensitivityMode_VeryHigh, JrWeightChangeSensitivityMode_High, JrWeightChangeSensitivityMode_Low};
//enum JrWeightSmoothingMode { JrWeightSmoothingModeInstant=0, JrWeightSmoothingModeSlow, JrWeightSmoothingModeAdaptive, JrWeightSmoothingModeNoiseReject1, JrWeightSmoothingModeWindowedOld, JrWeightSmoothingModeWindowed, JrWeightSmoothingModeWindowMost, JrWeightSmoothingModeGap};
enum JrWeightSmoothingMode { JrWeightSmoothingModeInstant=0, JrWeightSmoothingModeSlow, JrWeightSmoothingModeGap};

enum JrCalibrationTweakMethod {JrCalibrationTweakMethod_None, JrCalibrationTweakMethod_Additive, JrCalibrationTweakMethod_Multiplicative};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
class JrScale {
public:
	// library class for the hx711 scale helper
	HX711 scale;
	bool enabled = false;
protected:
	JrWeightSmoothingMode weightSmoothMode = JrWeightSmoothingModeGap;
	bool newTare = true;
	float rawWeight=0.0;
	float rawWeightSmoothed=0.0;
	float lastRawWeight=0.0;
	float lastRawWeightSmoothed=0.0;
	float tornWeight=0.0;
	float untornUntweakedWeight=0.0;
	float untornWeight=0.0;	
	float displayWeight=0.0;
	float internalTare = 0.0;
protected:
	unsigned long timeThisWeightIncorporate = 0;
	unsigned long timeLastWeightIncorporate = 0;
protected:
	//
	int windPullCountLower = 0;
	int windPullCountHigher = 0;
	int windPullCountSame = 0;
protected:
	float significantWeightChangeForTimeTrackingUp, significantWeightChangeForTimeTrackingDown;
	float tornWeightForTracking = 0.0;
	float windowedTornWeightForTracking = 0.0;
	float highestTornWeightSinceTare = 0.0;
	unsigned long timeOfLastSignificantWeightIncrease = 0;
	unsigned long timeOfLastSignificantWeightDecrease = 0;
  unsigned long lastWindowedWeightIncreaseMillis = 0; 
  unsigned long firstWindowedWeightIncreaseMillis = 0;
	int trackingRisingWeightConsecutives = 0;
  float windowedDisplayWeightForTracking = 0.0;
	float windowedLowestHigherWeight;
	float windowedHighestLowerWeight;
public:
	// calibration factors, saved between runs and calibrated from options
	float calibrationFactor = 100;
	float calibratedPlatformRawWeight = 0;
	float calibrationTweakAdditive = 0.0;
	float calibrationTweakMultiplicative = 1.0;
public:
	// options; public for now
	bool optionHideSmallChanges = false;
	int optionDebugLevel=0;
	bool optionRoundUp=false;
	bool optionRoundUpForce = false;
	int optionSoftZero = 0;
	JrCalibrationTweakMethod optionCalibrationTweakMethod = JrCalibrationTweakMethod_Additive;
protected:
	VoidEventCallbackFpT eventCallbackFp = NULL;

protected:
	// new smoothing algorithm constants
	const float DefAvgSmoothingModeWeightAveragingFactorVerySlow = 0.97;
	const int DefModeGapMaxDirectionCount = 15; // track count of this many samples in one direction more than another
	const int DefModeGapCountTrigger = 8; // must be less than above
protected:
	// displayweight tweaks
	const float thresholdLowestWeightAsZero = -0.9;     // between low and high near 0 we show 0
	const float thresholdHighestWeightAsZero = 0.2;     // between low and high near 0 we show 0
	//
	const float weightBackwardsHideThreshold = -0.7;
	const float weightForwardsHideThreshold = 0.5;
	const float weightDerivAveragingFactor = 0.99;
protected:
	// calibration tweak stuff
	const float DefMinimalTareToConvertToInternalAdditiveFix = 1.0;

protected:
	// auto timer tracking stuff for workflow
	//const float averagingFactorDelta = 0.95;
	const float significantWeightChangeForTimeTrackingUp_High = 1.5;			// it may be that what we consider sig weight change may depend on the workflow
	const float significantWeightChangeForTimeTrackingDown_High = -1.5;	//
	const float significantWeightChangeForTimeTrackingUp_Low = 1.5;			// it may be that what we consider sig weight change may depend on the workflow
	const float significantWeightChangeForTimeTrackingDown_Low = -1.5;	//

	const float significantWeightChangeForTimeTrackingUp_VeryHigh = 0.3;			// it may be that what we consider sig weight change may depend on the workflow
	const float significantWeightChangeForTimeTrackingDown_VeryHigh = -0.3;	//
	//
  const float significantWeightIncreaseForWindowedTimeTrackingUp = 0.5;		// should go up this much in each window of requiredRisingWindowTooCloseDurationMs time to be counted consecutive
  const float significantWeightDecreaseorWindowedTimeTrackingDown = -10;			// should not go down this much within a requiredRisingWindowTooLongFailDurationMs time window
	unsigned long requiredRisingWindowTooCloseDurationMs = 500;								// see above
  unsigned long requiredRisingWindowTooLongFailDurationMs = 2000;						// see above
  //
  unsigned long maxTimeBetweenWeightDecrease = 250;

public:
	JrScale();
public:
	void setupScale(int doutPin, int sckPin);
	void loopScale();
	void selfQueryScaleUpdateRawWeight();
public:
	void updateCalibratedPlatformRawWeight();
	void updateCalibrationFactor(float weightOnScaleInGrams);
public:
	void setEventCallbackFpT(VoidEventCallbackFpT fp) {eventCallbackFp = fp;}
	void triggerDelaySleepCallback();
public:
	void doTrackingUpdate();
	unsigned long getElapsedTimeSinceLastSignificantWeightChange();
	int JrScale::getConsecutiveWindowedIncreases();
public:
	void setCalibrationTweakAdditive(float ina) {	calibrationTweakAdditive = ina;};
	float getCalibrationTweakAdditive() { return calibrationTweakAdditive;};
	void setCalibrationTweakMultiplicative(float ina) {	calibrationTweakMultiplicative = ina;};
	float getCalibrationTweakMultiplicative() { return calibrationTweakMultiplicative;};
	float getCalibrationTweakMultiplicativeForDisplay() { return (calibrationTweakMultiplicative-1.0)*10000.0;};
	void resetCalibrationTweaks() {calibrationTweakAdditive=0.0; calibrationTweakMultiplicative=1.0; };
public:
	void setWeightChangeSensitivity(JrWeightChangeSensitivityMode smode);
	void setWeightSmoothMode(int mode) {weightSmoothMode = mode;};
public:
	// tare and calibration
	void setInternalTare(float val);
	void doClearTare();
	void tareChangesUpdate();
	void clearHighestTornWeightSinceTare();
	float getDisplayWeight() { return displayWeight; };
	float getInternalTare() { return internalTare; };
	void doSetCalibrationTweakForScaleFromRawWeight();
public:
	bool get_enabled() {return enabled;}
public:
	void setRawWeight(float val) {lastRawWeight = rawWeight; rawWeight=val;};
public:
	// accessors
	float getRawWeight() {return rawWeight;}
	float getRawWeightSmoothed() { return rawWeightSmoothed;};
	float getTornWeight() { return tornWeight;};
	float getUntornWeight() { return untornWeight;}
	float getUntornUnTweakedWeight() { return untornUntweakedWeight;}
	float getCalibratedPlatformRawWeight() { return calibratedPlatformRawWeight;};
	float getCalibrationFactor() { return calibrationFactor;};

public:
	// for tracking
	unsigned long getFirstWindowedWeightIncreaseMillis() {return firstWindowedWeightIncreaseMillis;};
	unsigned long getTimeOfLastSignificantWeightIncrease() { return timeOfLastSignificantWeightIncrease; };
	unsigned long getLastWindowedWeightIncreaseMillis() { return lastWindowedWeightIncreaseMillis; }
	float getHighestTornWeightSinceTare() { return highestTornWeightSinceTare; };
public:
	void prepareForSleep() {;};
	void prepareForWake() {newTare=true;};
public:
	void applySmoothRawWeightAlgorithm();
	void computeTornUntornWeightFromRawWeightSmoothed();
public:
	void resetLastChangeTimes();
};
//---------------------------------------------------------------------------



#endif

