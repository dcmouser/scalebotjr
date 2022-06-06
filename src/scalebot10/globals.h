// globals
#ifndef scalebotGlobalsH
#define scalebotGlobalsH


#include <jrWorkflow.h>
#include <jrneokeys.h>



//---------------------------------------------------------------------------
// workflow manager
JrWorkflow jrworkflow;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// buttons
JrButton optionsButton;
// rotary encoder
JrEncoder rotaryEncoder(pinRotaryA, pinRotaryB);
// neokey matrix
JrNeoKeyMatrixKeypad jrkeypad;
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
// tracking current mode (menu or scale)
int mode = DefModeSetup;
int previousMode = mode;
int menuMode = DefMenuModeOptions;
bool justWokeUp=false;

// slowing down debug messages
int debugDelayModifier = 1;

// sleep stuff
bool sleepingMode = false;
bool needsWakeupFuncRun = false;
bool flagWantsSleep = false;
unsigned long nextSleepTimeOnNonInteraction = 0;

// keep track of start app clock time (or can we assume this will always be 0?)
unsigned long startRunMillis = 0;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// options
uint8_t optionBeepMode = 1;
uint8_t optionSleepDelay = 5;
uint8_t optionDebugLevel = 0;
uint8_t optionRoundUp = 0;
uint8_t optionTareOnStart = 0;
uint8_t optionWeightSmoothMode = 6; // window mode
uint8_t optionScaleMode = 0;
uint8_t optionSoftZero = 0;
uint8_t optionCalibrationTweakMethod = 1;
//
uint8_t optionGrind1 = 10;
uint8_t optionGrind2 = 8;
uint8_t optionGrind3 = 9;
uint8_t optionGrind4 = 11;
uint8_t optionGrind5 = 7;
uint8_t optionGrind6 = 8;
//
uint8_t option7SegmentBrightness = 1;
uint8_t optionMatrix8x8Brightness = 1;
uint8_t optionStartMode = 1;
//
int optionCalibrationWeight = 100;
int optionBeanJarWeight = 121;
//
uint8_t optionCoffeeSelect = 1;
//
bool optionCheckWarnings = true;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int wakingCycleCount = 0;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
const char* DefParamOptionsListYesNo[] = { "OFF", "ON", 0 };
const char* DefParamOptionsListDebugLevel[] = { "OFF", "Low", "Med", "High", 0 };
const char* DefParamOptionsListBeepLevel[] = { "OFF", "Low", "Med", "High", 0 };
const char* DefParamOptionsStartMode[] = {"Last","Scale","WF",0};
const char* DefParamOptionsSmoothMode[] = {"Inst.", "Slow", "Gap",0};
const char* DefParamOptionsScaleMode[] = {"Auto","Dual", 0};
const char* DefParamOptionsCalibTweak[] = {"None","Add","Mul",0};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
const char* CoffeeBeanTypes[] = {"Papa N.G.", "Decaf Mex", "Guatemala", "Pg.Columb.", "Mex. Chiapas", "Kenya AA", "Ethiopian", "Unknown", "", "", 0};
uint8_t grindSettings[30] = {0};
//---------------------------------------------------------------------------






#endif
