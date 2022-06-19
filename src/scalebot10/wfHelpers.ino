// scale helper code


//---------------------------------------------------------------------------
extern uint8_t option7SegmentBrightness;
extern uint8_t optionMatrix8x8Brightness;
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void setupWorkflowAndScales() {

  // multiscale mode
  jrworkflow.setMultiscaleMode(DefUseMultiScaleHx711);
//  jrworkflow.setMultiscaleMode(false);

  // header text
  char title[24];
  //sprintf_P(title,PSTR("  %S v%S"), F(DefAppLabelStr), F(DefAppVersionStr));
  sprintf_P(title,PSTR("] %S v%S"), F(DefAppLabelStr), F(DefAppVersionStr));
  jrworkflow.setHeaderTextLine(title);

  // 7 segment lcds
  jrworkflow.setup7Segments(seg7I2cAddrA, seg7I2cAddrB);

  // 8x8 matrix
  jrworkflow.setupLcdMatrix(matrix8x8Addr);

  // set lcd
  jrworkflow.setJrLcd(&jrlcd);

  
  // setup scales
  jrworkflow.setupScales(pinLoadcallSck, pinLoadcell1Dout, pinLoadcell2Dout, &gscaleSet);
  // callbacks
  jrworkflow.setEventCallbackFpT(eventCallback);

  // rangefinder
  jrworkflow.setupRangefinder(&jrRangeFinder);
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
void loopScaleAndWorkflow(bool flagUpdateDisplay) {
  // workflow
  jrworkflow.loopWorkflow();
  // workflow info display
  jrworkflow.updateDisplays(false, flagUpdateDisplay);
  }
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void eventCallback(JrEventCallbackEnum etype) {
  // callbacks of various types
  // Serial.println((int)etype);
  if (etype == JrEventCallbackEnum_Interaction) {
    updateNextSleepTimeOnInteract();
  } else if (etype == JrEventCallbackEnum_WfAdvance) {
    uiInteractionSignify(DefSoundTypeWorkflow, true);
  } else if (etype == JrEventCallbackEnum_CoffeeDone) {
    storeFinishedCoffeeDrink();
  } else if (etype==JrEventCallbackEnum_PlaySoundAndWait) {
    uiInteractionSignify(DefSoundTypeWorkflow, true);
  } else if (etype==JrEventCallbackEnum_PlayBackgroundSound) {
    uiInteractionSignify(DefSoundTypeWorkflow, false);
  } else if (etype==JrEventCallbackEnum_PlayErrorSound) {
    uiInteractionSignify(DefSoundTypeError, false);
  } else if (etype==JrEventCallbackEnum_CoffeeSelect) {
    goCoffeeSelectMode();
  }
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void userStartsOrContinuesWorkflow(char* username, int userid) {
  bool bretv = jrworkflow.userStartsWorkflow(username,userid);
  if (!bretv) {
    // canceled
    uiInteractionSignify(DefSoundTypeError);
    return;
  }

  int wfmode = jrworkflow.getWorkflowMode();
  if (wfmode==JrWorkflowMode_None || wfmode == JrWorkflowMode_SelectCoffee) {
    uiInteractionSignify(DefSoundTypeMainFunc); 
  } else if (wfmode==JrWorkflowMode_Done) {
    // not clear why we need TRUE here to wait until sound finishes.. maybe the network sending causes the problem.
    uiInteractionSignify(DefSoundTypeDone, true);
  } else {
    uiInteractionSignify(DefSoundTypeWorkflow);
  }

  if (wfmode == JrWorkflowMode_SelectCoffee) {
    goCoffeeSelectMode();
  }
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void storeFinishedCoffeeDrink() {
  char username[20];
  char coffeeTypeStr[20];
  int coffeeId;
  int grindSize;
  float beanWeight, espressoWeight, finalWeight;
  
  jrworkflow.fillFinishedCoffeeInfo(username, coffeeTypeStr, coffeeId, grindSize, beanWeight, espressoWeight, finalWeight);
  
  char beanWeightfs[12], espressoWeightfs[12], finalWeightfs[12];
  jrFloatToStr(beanWeightfs, beanWeight, 3,2);
  jrFloatToStr(espressoWeightfs, espressoWeight, 3,2);
  jrFloatToStr(finalWeightfs, finalWeight, 3,2);

  char buf[100];
  sprintf(buf, "Coffee made for %s: %s (%d grind): %s/%s/%s",username, coffeeTypeStr, grindSize, beanWeightfs, espressoWeightfs, finalWeightfs);
  if (strcmp(jrworkflow.get_recipeLabel(),"")!=0) {
    strcat(buf," [");
    strcat(buf, jrworkflow.get_recipeLabel());
    strcat(buf, "]");
  }
  //
  //Serial.println(buf);

  // record the drink on our network
  jrnet.sendSimpleFeedSaveRequest(buf);
}
//---------------------------------------------------------------------------
