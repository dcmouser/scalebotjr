



//---------------------------------------------------------------------------
void setupGeneric() {

  // button objects/pins
  setupButtons();

  // neokey ui setuff
  setupNeokeyMatrix();

  // start time of app
  startRunMillis = millis();
 
  // serial init; only be needed if serial control is used 
  if (DefUseSerial) {
    Serial.begin(9600);
    //
    // serial announce
    char str[80];
    sprintf_P(str,PSTR("%S v%S - %S by %S (%S)"), F(DefAppLabelStr), F(DefAppVersionStr), F(DefAppDateStr), F(DefAppAuthorStr), F(DefAppEmailStr));
    Serial.println(str);
  }

  // load eeprom non-volatile data/settings
  readStoredEepromData();

  // lcd setup
  setupLcd();

  // generic led and pins
  pinMode(pinLed, OUTPUT);  

  // buzzer prep
  EasyBuzzer.setPin(pinBuzzer);

  // turn off annoying led
  jrTurnOffLed();
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
void doModeTransition(bool resetLocation) {
  // record the new mode so we can detect next change
  previousMode = mode;
  //
  doResetDisplayForMode(resetLocation);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void doResetDisplayForMode(bool resetLocation) {
  if (mode == DefModeWorkflow) {
    debugDelayModifier = DefDebugDelayModifierWorkflowMode;
    jrworkflow.workflowDisplayEngage();
  } else if (mode == DefModeMenu) {
    debugDelayModifier = DefDebugDelayModifierMenuMode;
    if (menuMode == DefMenuModeOptions) {
      if (resetLocation) {
        // always go to root when switching into menu mode?
        LCDML.MENU_goRoot();
      }
    }
    menuDisplayEngage();
  }
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
void pushOptionsToObjects() {
  jrworkflow.setOptions(optionDebugLevel, optionRoundUp, option7SegmentBrightness, optionWeightSmoothMode, optionScaleMode, optionSoftZero, optionCalibrationTweakMethod);
}

void pushOptionsToObjects2() {
  int optionStartingWorkflowMode = JrWorkflowMode_SelectCoffee;
  if (!optionCoffeeSelect) {
    optionStartingWorkflowMode=JrWorkflowMode_Beans;
  }
  jrworkflow.setOptions2(optionBeanJarWeight, optionStartingWorkflowMode, optionCheckWarnings);
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
int countMaxCoffeeIds() {
  // make sure we even have enough
  int max = 30;
  for (int i=0;i<max;++i) {
    if (CoffeeBeanTypes[i]==0) {
      return i;
    }
  }
  return max;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void doReadyStuff(const char *premsg) {
  // called on startup and ready
  sendAdafruitIoFeedEventOnReady(premsg);
}
//---------------------------------------------------------------------------
