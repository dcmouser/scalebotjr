/* ===================================================================== *
 *                                                                       *
 * Dynamic content                                                       *
 *                                                                       *
 * ===================================================================== *
 */


//---------------------------------------------------------------------------
// from our patch of LCDMenuLib
#include "LCDMenuLib2_menu.h"
extern LCDMenuLib2_menu* jrGlobalLcdMenuItemp;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define DefTimeoutForJrNetReply 5000
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void mDynParamSleep(uint8_t line) {
  int oldval = optionSleepDelay;
  optionSleepDelay = handleDynamicParam(F("Sleep (min)"), line, optionSleepDelay, 0, 99);
  if (optionSleepDelay != oldval) {
    setNextSleepTimeOnWakeup(1);
  }
}
void mDynParamDebug(uint8_t line) {
  optionDebugLevel = handleDynamicParamList(F("Debug mode"), line, optionDebugLevel, DefParamOptionsListOffLowMediumHigh);
  pushOptionsToObjects();
}

void mDynParamBeep(uint8_t line) {
  optionBeepMode = handleDynamicParamList(F("Beep level"), line, optionBeepMode, DefParamOptionsListOffLowMediumHigh);
}

void mDynParamRoundUp(uint8_t line) {
  optionRoundUp = handleDynamicParamList(F("Round num"), line, optionRoundUp, DefParamOptionsListYesNo);
  pushOptionsToObjects();
}


void mDynParamSoftZero(uint8_t line) {
  optionSoftZero = handleDynamicParamList(F("Soft zero"), line, optionSoftZero, DefParamOptionsListYesNo);
  pushOptionsToObjects();
}

void mDynParamAutoZero(uint8_t line) {
  optionAutoZero = handleDynamicParamList(F("Auto zero"), line, optionAutoZero, DefParamOptionsAutoZero);
}

void mDynParam7SegBrightness(uint8_t line) {
  option7SegmentBrightness = handleDynamicParam(F("LED Bright"), line, option7SegmentBrightness, 0, 15);
  pushOptionsToObjects();
}

void mDynParamStartMode(uint8_t line) {
  optionStartMode = handleDynamicParamList(F("Start mode"), line, optionStartMode, DefParamOptionsStartMode);
}

void mDynParamScaleMode(uint8_t line) {
  optionScaleMode = handleDynamicParamList(F("Scale mode"), line, optionScaleMode, DefParamOptionsScaleMode);
}


void mDynParamSmoothMode(uint8_t line) {
  optionWeightSmoothMode = handleDynamicParamList(F("Smooth mode"), line, optionWeightSmoothMode, DefParamOptionsSmoothMode);
  pushOptionsToObjects();
}

void mDynParamCalibrationTweakMethod(uint8_t line) {
  optionCalibrationTweakMethod = handleDynamicParamList(F("Calib. Tweak"), line, optionCalibrationTweakMethod, DefParamOptionsCalibTweak);
  pushOptionsToObjects();
}


//

void mDynParamCalibrationWeight(uint8_t line) {
  optionCalibrationWeight = handleDynamicParam(F("Calib. Wght"), line, optionCalibrationWeight, 50.0, 500.0);
}


void mDynParamBeanJarWeight(uint8_t line) {
  optionBeanJarWeight = handleDynamicParam(F("Jar. Wght"), line, optionBeanJarWeight, 25, 400);
  pushOptionsToObjects2();
}


void mDynParamWfStartsCoffeeSelect(uint8_t line) {
  optionCoffeeSelect = handleDynamicParamList(F("Coffee sel"), line, optionCoffeeSelect, DefParamOptionsListYesNo);
  pushOptionsToObjects2();
}


void mDynParamOptionCheckWarnings(uint8_t line) {
  optionCheckWarnings = handleDynamicParamList(F("Chk. Wat."), line, optionCheckWarnings, DefParamOptionsListCheckWarnings);
  pushOptionsToObjects2();
}

void mDynParamOptionSyncPouredWeights(uint8_t line) {
  optionSyncPouredWeights = handleDynamicParamList(F("Sync Wght."), line, optionSyncPouredWeights, DefParamOptionsListYesNo);
  pushOptionsToObjects2();
}

void mDynParamOptionBlankOnSleep(uint8_t line) {
  optionBlankSleep = handleDynamicParamList(F("Blank Sleep."), line, optionBlankSleep, DefParamOptionsListYesNo);
  pushOptionsToObjects2();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void mDynParamGrind(uint8_t line) {
  int coffeeId = globalLcdmlDynMenuParam;//line+1;
  //Serial.print("Coffee index = ");
  //Serial.println(line);
  doHandleDynParamGrind(line, coffeeId);
}


void doHandleDynParamGrind(uint8_t line, int coffeeId) {
  int grindSize = grindSettings[coffeeId-1];
  grindSize = handleDynamicParam(CoffeeBeanTypes[coffeeId-1], line, grindSize, 4, 15);
  if (grindSize>-1) {
    // save grind setting
    grindSettings[coffeeId-1] = grindSize;
  }
}


bool mDynParamGrindCond() {

  // ATTN: we had to kludge a fix into LCDMenu to catch the item being checked (see LCDMenuLib2_menu.cpp) for:
  // // ATTN: jr modified to support
  // // LCDMenuLib2_menu* jrGlobalLcdMenuItemp = NULL;
  // LINE 198 in updateCondetion()() added:
  //  // ATTN: jr need to have a way to let condition checker know what item is being checked
  //  jrGlobalLcdMenuItemp = this;
  
  LCDMenuLib2_menu *tmp = jrGlobalLcdMenuItemp;
  uint8_t param = tmp->getParam();
  int coffeeId = param;
  if (coffeeId<1) {
    return true;
  }

  if (coffeeId>countMaxCoffeeIds()) {
    // we dont have an entry for this
    return false;
  }
  
  //Serial.println(CoffeeBeanTypes[coffeeId-1]);
  //int coffeeId = line+1;
  if ((strcmp(CoffeeBeanTypes[coffeeId-1],"") == 0)) {
    return false;
  }
  return true;
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void showMessageBrieflyOnScreen(const char* msg) {
  jrlcd.setCursor(0, 3); // set cursor
  jrlcd.print(msg);
}
//---------------------------------------------------------------------------


  

//---------------------------------------------------------------------------
void mDynPageNetInfo(uint8_t param) {
  // this dynamic page presentation code is split into a setup and a display section so that we don't generate request twice when leaving function
  bool bretv = checkAndDoDynamicPageSetup();
  if (bretv) {
    showMessageBrieflyOnScreen("Waiting for reply...");
    jrStopSoundsBeforeDelay();
    char strbuf[100];
    jrnet.requestStatusInfoWithTimeout(strbuf, DefTimeoutForJrNetReply);
    handleDynamicPageSetup(param, strbuf);
  }
  handleDynamicPageLoop();
}




void mDynPageDiagnosticsGeneral(uint8_t param) {
  char strbuf[80];
  sprintf(strbuf,"Loop time (ms): %ld\n",jrworkflow.getAvgUpdateMs());
  handleDynamicPage(param, strbuf);
}


void mDynPageAbout(uint8_t param) {
  char strbuf[80];
  //sprintf(strbuf,"%s\n%s\nby %s",appLabel, appDate, appAuthor);
  sprintf_P(strbuf,PSTR("%S\nv%S (%S)\nby %S"),F(DefAppLabelStr), F(DefAppVersionStr), F(DefAppDateStr), F(DefAppAuthorStr));
  handleDynamicPage(param, strbuf);
}



void mDynPageScaleDataGeneric(uint8_t param, JrScale* scalep, char* scaleid) {
  char strbuf[120];
  char f1[8],f2[8],f2b[8],f3[8],f4[8];



  float cfactor = scalep->getCalibrationFactor();
  jrFloatToStr(f1, scalep->getInternalTare(), 1, 1);
  jrFloatToStr(f2, scalep->getCalibrationTweakAdditive() / cfactor, 1, 2);
  jrFloatToStr(f2b, scalep->getCalibrationTweakMultiplicativeForDisplay(), 1, 2);
  jrFloatToStr(f3, scalep->getCalibratedPlatformRawWeight() / cfactor, 1, 1);
  jrFloatToStr(f4, cfactor /100.0, 1, 1);
  sprintf(strbuf,"%s:\nT: %s\nA:%s|M:%s\nP:%s | C:%s", scaleid, f1, f2, f2b, f3, f4);
  handleDynamicPage(param, strbuf);
}

void mDynPageScaleData1(uint8_t param) {
  mDynPageScaleDataGeneric(param, jrworkflow.getScaleById(1), "Scale 1");
}

void mDynPageScaleData2(uint8_t param) {
  mDynPageScaleDataGeneric(param, jrworkflow.getScaleById(2),"Scale 2");
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void mMenuDoScale1Place0Weight(uint8_t param) {
  handleDynamicPage(param, F("Clear scale #1\nof everything."), F(DefStrThenClickButton), callbackDoScale1Place0Weight, 0);
}

void mMenuDoScale1Place100gWeight(uint8_t param) {
  char buf[40];
  char weightbuf[20];
  jrFloatToStr(weightbuf, optionCalibrationWeight, 1, 1);
  sprintf(buf,"Place %sg on\nscale #1.",weightbuf, optionCalibrationWeight);
  handleDynamicPage(param, buf, DefStrThenClickButton, callbackDoScale1Place100gWeight, 1);
}

void mMenuDoScale2Place0Weight(uint8_t param) {
  handleDynamicPage(param, F("Clear scale #2\nof everything."), F(DefStrThenClickButton), callbackDoScale2Place0Weight, 0);
}

void mMenuDoScale2Place100gWeight(uint8_t param) {
  char buf[40];
  char weightbuf[20];
  jrFloatToStr(weightbuf, optionCalibrationWeight, 1, 1);
  sprintf(buf,"Place %sg on\nscale #2.",weightbuf, optionCalibrationWeight);
  handleDynamicPage(param, buf, DefStrThenClickButton, callbackDoScale2Place100gWeight, 1);
}

//

bool callbackDoScale1Place0Weight(int param) {
  pleaseWaitForScaleToSettle();
  jrworkflow.updateCalibratedPlatformRawWeight(1);
  return true;
}

bool callbackDoScale1Place100gWeight(int param) {
  pleaseWaitForScaleToSettle();
  jrworkflow.updateCalibrationFactor(1, (float)optionCalibrationWeight);
  return true;
}

bool callbackDoScale2Place0Weight(int param) {
  pleaseWaitForScaleToSettle();
  jrworkflow.updateCalibratedPlatformRawWeight(2);
  return true;
}

bool callbackDoScale2Place100gWeight(int param) {
  pleaseWaitForScaleToSettle();
  jrworkflow.updateCalibrationFactor(2, (float)optionCalibrationWeight);
  return true;
}

void pleaseWaitForScaleToSettle() {
  showMessageBrieflyOnScreen(DefStrPleaseWait);
  jrStopSoundsBeforeDelay();
  delay(500);
}




void mMenuDoScaleWeighBeanJar(uint8_t param) {
  char buf[80];
  char weightbuf[20];
  jrFloatToStr(weightbuf, optionBeanJarWeight, 1, 1);
  sprintf(buf,"Place bean jar\non scale (%s).", weightbuf);
  //handleDynamicPage(param, F("Place bean jar\non scale."), F(DefStrThenClickButton), callbackDoScaleWeighBeanJar, 0);
  handleDynamicPage(param, buf, DefStrThenClickButton, callbackDoScaleWeighBeanJar, 0);
}


bool callbackDoScaleWeighBeanJar(int param) {
  pleaseWaitForScaleToSettle();
  JrScale* scalep = jrworkflow.getActiveScalep();
  jrworkflow.settleMeasureScaleWeight(scalep);
  optionBeanJarWeight = scalep->getUntornWeight();
  pushOptionsToObjects2();
  return true;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void mFuncSleepNow(uint8_t param) {
  // go to sleep
  if (LCDML.FUNC_setup()) {
    // go back to top of top menu for next time user enters menu mode? or will we call this whenever we ENTER menu mode?
    LCDML.MENU_goRoot();  

    // save settings and exit switch modes
    doSafelyExitMenuMode(false);

    // beep
    jrPlaySound(DefSoundTypeSleep, true);   

    // set flag saying we want to sleep now
    flagWantsSleep=true;
  }
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void myMenuMainExit(uint8_t param) {
  // this is called when user explicitly chooses EXIT main manu item
  if(LCDML.FUNC_setup()) {
    // this all hapens in Setup? 
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    // go back to top of top menu for next time user enters menu mode? or will we call this whenever we ENTER menu mode?
    LCDML.MENU_goRoot();  

    // save settings and exit switch modes
    doSafelyExitMenuMode(true);
  }
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// reboot
void(* resetFunc) (void) = 0; //declare reset function @ address 0
void mFuncRebootNow(uint8_t param) {
  // go to sleep
  if (LCDML.FUNC_setup()) {
    // save data first
    writeStoredEepromData();
    // now reboot
    resetFunc();
  }
}
//---------------------------------------------------------------------------
