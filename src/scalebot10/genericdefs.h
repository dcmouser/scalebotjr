#ifndef genericdefsH
#define genericdefsH



//---------------------------------------------------------------------------
#define JrSkipControllerLoop false
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define DefUseMultiScaleHx711 true
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define DefSoundTypeMainFunc 1
#define DefSoundTypeMenuSelect 2
#define DefSoundTypeMenuMove 3
#define DefSoundTypeSleep 4
#define DefSoundTypeWorkflow 5
#define DefSoundTypeExit 6
#define DefSoundTypeDone 7
#define DefSoundTypeError 8
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define DefModeSetup 0
#define DefModeWorkflow 1
#define DefModeMenu 2
//
#define DefMenuModeOptions 0
#define DefMenuModeQuestion 1

//
// mode to start up in
#define DefModeStart DefModeWorkflow
//#define DefModeStart DefModeMenu
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// string defs
#define DefStrThenClickButton "[then click button]"
#define DefStrClickButtonToContinue " [click button]"
#define DefStrPleaseWait "  -- PLEASE WAIT -- "
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define DefDebugDelayModifierWorkflowMode 50
#define DefDebugDelayModifierMenuMode 15000
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// for debugging
#define DefUseSerial true
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define DefWakingCycleCountHigh 5
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define DefCoffeeMenuStartingId 2
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// ATTN: jr added when using UNO to try to fix problems
// for rotary encoder
// but we are back to allowing interrupts now on Mega, using pins 2+3
// #define ENCODER_DO_NOT_USE_INTERRUPTS true
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// for workflow buttons
#define DefWfUserName1 "Jesse"
#define DefWfUserName2 "Naoki"
#define DefWfUserNameGuest "Guest"
//---------------------------------------------------------------------------






#endif
