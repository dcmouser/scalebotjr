#ifndef jrworkflowH
#define jrworkflowH




//---------------------------------------------------------------------------
//#define JrTestFix true
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// let's make the workflow the boss of the scale
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// lcd
#include <LiquidCrystal_I2C.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// multiscale
#include <HX711-multi.h>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include <jrScale.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <jrlcd.h>
#include <jr7seg.h>
#include <jrLcdMatrix.h>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
enum JrWorkflowModeEnum {JrWorkflowMode_None=0, JrWorkflowMode_SelectCoffee, JrWorkflowMode_Beans, JrWorkflowMode_Espresso, JrWorkflowMode_All, JrWorkflowMode_Done };
enum JrTimerState {JrTimerStateOff=0, JrTimerStateRunning, JrTimerStateDone};
enum JrWorkflowStartModeEnum {JrWorkflowStartMode_Last=0, JrWorkflowStartMode_Scale, JrWorkflowStartMode_Wf};
enum JrEnumIngredientMeasureMode {JrEnumIngredientMeasureMode_TotalWeightByGramBean, JrEnumIngredientMeasureMode_MultiplyGramPerGramEspresso, JrEnumIngredientMeasureMode_MultiplyGramPerGramBean};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// scale 1 and scale 2 what are they used for
#define DefPreferredScaleNone     0
#define DefPreferredScaleBeans    1
#define DefPreferredScaleEspresso 2
#define DefPreferredScaleAll	  0
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define MaxUserMemoryCount 4
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define DefUsernameUnknown "Unknown"
#define DefCoffeenameUnknown "Unknown"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
extern const char* doneWorkflowMessages[];
extern const char* JrModeInstructionLines[];
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// for 7-segment lcds for weight and timer
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#define DefMatrixRotation 3
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
class JrWorkflow {
protected:
	int currentRecipe = -1;
	int currentRecipeStepIndex = -1;
	float currentRecipeStepTargetWeight = 0.0;
	float priorRecipeStepTargetWeight = 0.0;
	char recipeLabel[20];
	char ingredientLabel[20];
	JrEnumIngredientMeasureMode ingredientMeasureMode;
protected:
	VoidEventCallbackFpT eventCallbackFp = NULL;
protected:
	// multiscale manager
	HX711MULTI *scaleSet = NULL;
	bool multiscaleMode = false;
protected:
	uint8_t coffeeId;
	char coffeeName[20];
	char userName[20];
	uint8_t grindSize = 0;
	uint8_t userId;
	uint8_t userLastCoffee[MaxUserMemoryCount] = {0};
protected:
	char warningMsg[20];
protected:
	JrLcd* jrlcdp = NULL;
	Jr7Seg seg7a, seg7b;
	JrLcdMatrix lcdMatrix;
protected:
	JrScale jrscale1;
	JrScale jrscale2;
	//
	JrScale* jrscalep;
	JrScale* iajscalep;
	JrScale* prefscalep;
protected:
	char headerTextLine[24];
public:
	// options; public for now
	bool optionDoActiveScaleSwitching = true;
	int JrStartingScale = 1;
	bool jrOnlyUseOneScale = false;
public:
	int optionDebugLevel = 0;
	bool optionShowTimerMs = true;
	bool optionEnableAutoTimer = true;
	bool optionShowWorkflowModeInstructions = true;
	bool optionStartupTare = false;
	int optionStartingWorkflowMode = JrWorkflowMode_SelectCoffee;
	bool optionCheckWarnings = false;
	int optionScaleMode = 0;
	int optionSoftZero = 0;
	float optionBeanContainerWeight = 100.0;
public:
	// not yet changeable in options
	bool optionStartupCalibrationTweak = true;
	bool optionResumeCalibrationTweak = true;
	bool optionAutoAdvanceWorkflowFromBeans = true;
protected:
	bool wantScheduleCalibrationTweak=false;
protected:
	unsigned long scheduledTareMillis = 0;
	float internalTare = 0.0;
protected:
	unsigned long scheduledCalibrationTweakMillis = 0;
protected:
	// workflow modes and tracking weights
	JrWorkflowModeEnum workflowMode = JrWorkflowMode_None;
	int workflowSubStepIndex = 0;
	float workflowWeights[6];
	float manualBeanWeight = 0;
//
protected:
	JrTimerState timerState = JrTimerStateOff;
	unsigned long timerStartMillis = 0;
	unsigned long timerEndMillis = 0;
	bool timerAsterisk = false;
	//
	unsigned long timerElapsedMs = 0;
	long timerElapsedPreviousValue = 0;
	float tarePreviousValue = 0.0;
	float displayWeightPreviousValue = 0.0;
	float secondWeightPreviousValue = 0.0;
	//
	unsigned long startOfCurrentIncreaseSequenceMs = 0;
protected:
	float lastPositiveBeanWeight = 0.0;
	float lastPositiveWeight = 0.0;
protected:
	float avgUpdateMs = 0;
	unsigned long lastLoopTimeMs;
protected:
	// calibration and tare stuff
	const unsigned long DefNewCalibrationPeriodMs = 1000;
	const int DefTareStabilizeDelayMs = 250;
	const unsigned long calStabilizeDelayMs = 500;
protected:
	const bool blockWorkflowAdvanceOnBadWeights = true;
protected:
	const float paMinTareToShow = 5.0;											// under this value, the tare will not be shown on display (i.e. its not the cup its to 0)
	const bool paModeDisplayTareWeightOneLine = false;			// if true, we will always show tare weight on bottom line with weight (normally only done on finish of workflow)
protected:
	//
	const bool wfEnableAutoAdvance = true;									// whether to auto advance workflow modes
	const float wfMinCupWeightForBeanTare = 5.0;						// min weight to trigger a tare for bean cup
	const float wfMinWeightForBeans = 5.0;									// min weight to trigger detecting beans
	const float wfMinCupWeightForEspressoTare = 50.0;			// min weight to trigger a tare for espresso shot
	const float wfMaxMinimalTareThreshold = 10.0;						// any tare over this means a cup has been torn
	const float wfMinWeightForPositiveStabilized = 5.0;
protected:
	const float minWeightForPreferredScaleToDominate = 5;		// more than this we always rely on preferrred scale
	const float minWeightDeltaForActiveScaleSwitch = 5;
protected:
	// pre 6/6/22
	//const unsigned long wfStableWeightElapsedTimeShort = 500;
	//const unsigned long wfStableWeightElapsedTimeSuperShort = 200;
	//const unsigned long wfStableWeightElapsedTimeNorm = 1000;
	// post 6/6/22
	const unsigned long wfStableWeightElapsedTimeShort = 750;
	const unsigned long wfStableWeightElapsedTimeSuperShort = 400;
	const unsigned long wfStableWeightElapsedTimeNorm = 1000;

	const int wfConsecutiveRisingWeightWindowTarget = 3;		// see jrscale; the # of consectuive increases in window of size requiredRisingWindowTooCloseDurationMs to be considered a steady increase over time; multiply to get rising time length
	const unsigned long wfStableTimeEndPourMs = 4000;				// how long weight shoudl stabilize to signify end of pour
	const unsigned long wfWaitAfterTimerEndToAdvanceMode = 3000;
	const unsigned wfMaxWeightIncAfterEspressoToUpdateOnRecipe = 5; // as long as not more than this many grams have been added when we switch to a recipe, update espresso weight to this value, to account for dripping
	const unsigned long wfCalStableWeightElapsedTime = 250;
	float wfCalMaxWeightToConsiderZero = 3.0;
	//
	const float wfMinBeanWeightToAssumeInContainer = 3.0;
	const float wfBeanWeightToAssumeInContainerMin = 8.0;
	const float wfBeanWeightToAssumeInContainerMax = 30.0;
protected:
	// tracking update times
	const float DefAvgUpdateMsSmoothFactor = 0.99;
	const unsigned long DefMaxAvgUpdateToConsider = 1000;
public:
	JrWorkflow();
public:
	void setMultiscaleMode(bool in_multiscaleMode) { multiscaleMode = in_multiscaleMode;};
	void setHeaderTextLine(char *str);

public:
	void setupScales(int sckPin, int dout1Pin, int dout2Pin, HX711MULTI* scalesetinp);

public:
	void setJrLcd(JrLcd* injrlcdp) {jrlcdp = injrlcdp;}
	void setup7Segments(uint8_t addrA, uint8_t addrB);
	void setupLcdMatrix(uint8_t addr);

public:
	void begin();
	void setOptions(int in_optionDebugLevel, bool in_optionRoundUp, int in_option7SegmentBrightness, int weightSmoothMode, int inoptionScaleMode, int inoptionSoftZero, int inoptionCalibrationTweakMethod);
	void setOptions2(float inBeanContainerWeight, int in_optionStartingWorkflowMode, bool in_optionCheckWarnings);
	void powerUp();
	void powerDown();
	void powerUpScale(JrScale *scalep);
	void powerDownScale(JrScale *scalep);
public:
	void scheduleTare(unsigned long delayMillis = 0);
	void doScheduledTare();
public:
	void scheduleCalibrationTweak(unsigned long delayMillis = 0);
	void doScheduledCalibrationTweak();
	void doSetCalibrationTweakForScale(JrScale *scalep);
public:
	void loopWorkflow();
	void doLoopNonMultiScale(JrScale *scalep);
public:
	void resetWorkflowWeights();
public:
	void workflowModeChanges();
	void setWorkflowModeEnable(bool val);
	void workflowDisplayEngage();
	void showWorkflowModeInstructions();
	void calcWorkflowDoneMessage(char *outbuf);
	bool advanceWorkflowMode(bool manualHuman);
	void rewindWorkflowMode();
	void resumeSleepForceWorkflowOption(JrWorkflowStartModeEnum resumeMode, bool flagFirstStartup);
	void firstStart(bool val, JrWorkflowStartModeEnum resumeMode);
	bool workflowSafeToAdvance();
public:
	void updateDisplays(bool force);
	void updateRatioDisplay();
	void updateTimerDisplay();
	void updateTareDisplay(float tareval);
	void updateWeightDisplay(float displayWeight);
	void updateTareWeightDisplay(float displayWeight, float tareval);
	void updateWeightDisplayWithSecondaryWeight(float displayWeight, float secondWeight);
	void simpleShowFloatOnDisplay(float val, Jr7Seg *seg7p);
public:
	void showTimerStr(char* str);
	void showTimerStrPad(const char* str);
	void clearTimerStr();
	void clearDupeCheckers(bool clearTimer, bool clearTare, bool clearDisplayWeight);
public:
	void readStoredEepromData(unsigned long EpromDataStartAddress);
	void writeStoredEepromData(unsigned long EpromDataStartAddress);
public:
	void setEventCallbackFpT(VoidEventCallbackFpT fp);
public:
	void manualTimerTriggerToggle();
	void timerOff();
	void stopTimer();
	void startTimer(bool isAsterisked);
	inline bool isTimerRunning() {return (timerState==JrTimerStateRunning);};
	void setTimerStartNow();
	//
	bool readyForAutoTimer();
public:
	void checkAutoWorkflow();
	bool isWeightStabilized(unsigned long duration);
	bool isWeightSteadyIncreasing();
	bool isTornWeightOver(float weightThreshold);
	bool isTornWeightBelow(float weightThreshold);
	bool isTareMinimal();
	bool isTearWeightOver(float weightThreshold);
	void wfDoAutoTare();
	void doClearTare();
	void wfSignifyAutoStep();
	float wfGetLastPositiveBeanWeight();
	unsigned long wfGetStartTimeOfSteadyIncrease();
	unsigned long wfGetStopTimeOfSteadyIncrease();
	void clearTrackingWeights();
public:
	void switchToActiveScale(JrScale* sp);
	void setActiveAndInactiveScale(JrScale* p) {jrscalep = p; iajscalep = calcInactiveScalep();};
	JrScale* getScaleById(int id) {if (id==0) return NULL; if (id==1) return &jrscale1; return &jrscale2;};
	JrScale* calcInactiveScalep() {if (jrscalep==&jrscale1) return &jrscale2; else return &jrscale1;};
	JrScale* getActiveScalep() { return jrscalep; };
	void setPreferredScaleIdForWorkflow();
	void setPreferredScaleId(int id) { prefscalep = getScaleById(id); preferredScaleChanges(); };
	void preferredScaleChanges();
public:
	void updateActiveScaleChoice();
public:
	void updateCalibratedPlatformRawWeight(int scaleId);
	void updateCalibrationFactor(int scaleId, float weightOnScaleInGrams);
	void updateCalibratedPlatformRawWeight(JrScale* scalep);
	void updateCalibrationFactor(JrScale* scalep, float weightOnScaleInGrams);
	float calcSmoothedRawScaleWeightForCalibration(JrScale* scalep);
	void settleMeasureScaleWeight(JrScale *scalep);
	void queryScaleUpdateRawWeight(JrScale* scalep);
public:
	void doMultiLoopScales();
	void doMultiScaleQueryUpdateRawWeight();
public:
	bool storeWorkflowedDrink();
	bool userStartsWorkflow(const char* inUserName, uint8_t inUserId);
	void clearCoffeeSelection() {coffeeId=0; strcpy(coffeeName,DefCoffeenameUnknown);grindSize=0;};
	void clearUserSelection() {strcpy(userName,DefUsernameUnknown);userId=0;};
	JrWorkflowModeEnum getWorkflowMode() { return workflowMode;};
	uint8_t getUsersLastCoffee();
	void setUsersLastCoffee(uint8_t coffeeId, char* inCoffeeLabel, uint8_t ingrindSize);
public:
	void smartGuessBeanWeightInJarIfAppropriate();
	void fillFinishedCoffeeInfo(char* username, char* coffeeTypeStr, int &oucoffeeId, int &outgrindSize, float &beanWeight, float &espressoWeight, float &finalWeight);
public:
	void prepareForSleepEarly();
	void prepareForSleepLate();
	void prepareForWake();
public:
	void updateMatrixImageForWorkflowMode();
public:
	void checkWarnings();
	void setWarningMessage(const char *msg) {strcpy(warningMsg,msg);};
public:
	int countRecipes();
	int getRecipeStartIndex(int recipeIndex);
	bool parseRecipeStep(const char *str, char* labelout, float &amountoff);
	void dislayRecipeStepInstructions();
	void deltaRecipe(int delta);
	void advanceRecipeStep();
	void newRecipeStarts();
	void recipeChanges();
	void clearRecipe();
	void parseAndStoreRecipeIngredientString(const char* str);
	int getRecipeIngredientIndex(int rec, int stepnumber);
	void displayRecipeIngredientStepInfo();
	void autoAdvanceRecipeIngredientStepIfAppropriate();
	char* get_recipeLabel() {return recipeLabel;};
public:
	float getActiveScaleDisplayWeight() { return jrscalep->getDisplayWeight();}
public:
	bool isCupOnEspressoScale();
	void adjustManualBeanWeight(float val) { manualBeanWeight+=val; if (manualBeanWeight<0) manualBeanWeight=0;};
public:
	unsigned long getAvgUpdateMs() {return (unsigned long) avgUpdateMs;};
	bool goBackwardsStep();
};
//---------------------------------------------------------------------------



	


#endif
