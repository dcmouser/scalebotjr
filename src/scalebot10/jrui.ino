//---------------------------------------------------------------------------
#include "globals.h"
//
#include <jrneokeys.h>
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// to work around bug in heap allocation (?) we have to create these globally
// see pindefs.h
Adafruit_NeoPixel jrWrappedNeopixelStrip(neoKeypad_Rows * neoKeypad_Cols, neoKeypadPin_Neopixel, NEO_GRB + NEO_KHZ800);
Adafruit_Keypad jrWrappedKeypad( makeKeymap(neoKeypadKeymap), neoKeypadPins_Rows, neoKeypadPins_Cols, neoKeypad_Rows, neoKeypad_Cols);
bool jrWrappedNeoKeymap[neoKeypad_Rows*neoKeypad_Cols] = {0};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
float testWaterLevelVal = 0.0;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void loopButtonsAndKeypad() {
  // update buttons on loop -  UNLESS they are menu specific

  // rotary knob and associated button
  if (mode==DefModeMenu) {
    // menu code UPDATE and handle these buttons
  } else {
    // scale mode
    optionsButton.update();
    int pos = rotaryEncoder.update();
    JrPressType optionPressType = optionsButton.getPressType();
    //
    // options button
    if (optionsButton.pressed()) {
      setMenuMode(DefMenuModeOptions);
      // update interaction time
      uiInteractionSignify(DefSoundTypeMainFunc);
      }

    JrWorkflowModeEnum wfmode = jrworkflow.getWorkflowMode();

    if (false && wfmode == JrWorkflowMode_None) {
      // TEST
      if (pos==JrEncoderRotaryDirectionLeft) {
        // turn left/up
        testWaterLevelVal-=0.3;
        uiInteractionSignify(DefSoundTypeMenuMove);
      } else if (pos==JrEncoderRotaryDirectionRight) {
        // turn right/down
        testWaterLevelVal+=0.3;
        uiInteractionSignify(DefSoundTypeMenuMove);
      }
      jrworkflow.setWaterLevel(testWaterLevelVal);
    }
    
    
    if (wfmode == JrWorkflowMode_All) {
      // in this mode user can rotate through recipes
      if (pos==JrEncoderRotaryDirectionLeft) {
        // turn left/up
        jrworkflow.deltaRecipe(-1);
        uiInteractionSignify(DefSoundTypeMenuMove);
      } else if (pos==JrEncoderRotaryDirectionRight) {
        // turn right/down
        jrworkflow.deltaRecipe(1);
        uiInteractionSignify(DefSoundTypeMenuMove);
      }
    } else if (wfmode==JrWorkflowMode_Beans) {
      if (pos==JrEncoderRotaryDirectionLeft) {
        jrworkflow.adjustManualBeanWeight(-1.0);
        uiInteractionSignify(DefSoundTypeMenuMove);
      } else if (pos==JrEncoderRotaryDirectionRight) {
        jrworkflow.adjustManualBeanWeight(1.0);
        uiInteractionSignify(DefSoundTypeMenuMove);
      }
    }
  }

  // keypad (it will invoke callbacks)
  jrkeypad.loop(true);

  if (justWokeUp) {
    justWokeUp = false;
  }
}
//---------------------------------------------------------------------------



















//---------------------------------------------------------------------------
// ATTN: no longer used except maybe for debugging
void jrTurnOnLed() {
    digitalWrite(pinLed, HIGH);
}

void jrTurnOffLed() {
    digitalWrite(pinLed, LOW);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void jrPlaySound(int beepType, bool flagWaitForCompletion) {
  if (optionBeepMode==0) {
    return;
  }

  if (optionDebugLevel>0) {
    Serial.print("ATTN: Playing sound: ");
    Serial.println(beepType);
  }
  

  EasyBuzzer.stopBeep();
  if (beepType==DefSoundTypeMenuMove) {
    if (optionBeepMode>=3) {
      EasyBuzzer.singleBeep(100, 20);
    }
  }  else if (beepType==DefSoundTypeMenuSelect) {
    if (optionBeepMode>=2) {
      EasyBuzzer.singleBeep(200, 50);
    }
  } else if (beepType==DefSoundTypeSleep) {
    EasyBuzzer.singleBeep(300, 80);
  } else if (beepType==DefSoundTypeWorkflow) {
    EasyBuzzer.singleBeep(150, 70);
  } else if (beepType==DefSoundTypeExit) {
    EasyBuzzer.singleBeep(200, 80);
  } else if (beepType==DefSoundTypeDone) {
    EasyBuzzer.singleBeep(85, 100);
  } else if (beepType==DefSoundTypeError) {
    EasyBuzzer.singleBeep(600, 80);
  } else {
    EasyBuzzer.singleBeep(400, 80);
  }

  if (flagWaitForCompletion) {
    // this might be called right before going to sleep for example, where we can't rely on loop to stop
    for (int i=0;i<20;++i) {  
      // buzzer helper
      EasyBuzzer.update();
      // we sleep ourselves (do we need to instead loop calling easy buzzer update?
      delay(10);
    }
    // now stop sound
    EasyBuzzer.stopBeep();
    // buzzer helper
    EasyBuzzer.update();
  }
}


void JrSemiUpdateBuzzer() {
  EasyBuzzer.update();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void jrStopSoundsBeforeDelay() {
    // buzzer helper
    EasyBuzzer.update();
    // now stop sound
    EasyBuzzer.stopBeep();
    // buzzer helper
    EasyBuzzer.update();
}
//---------------------------------------------------------------------------


















//---------------------------------------------------------------------------
void debugPrintln(char *buf) {
  Serial.println(buf);
}

void debugPrint(char *buf) {
  Serial.print(buf);
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
void uiInteractionSignify(uint8_t itype, bool waitForSound=false) {
  // sound
  jrPlaySound(itype, waitForSound);
  // update interaction time
  updateNextSleepTimeOnInteract();  
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
void myCallbackNeoKeyPress(JrKeyCallbackType eventType, int keynum) { 
    // && (eventType != JrKeyCallbackType_Up || !justWokeUp)) {
  if (eventType != JrKeyCallbackType_Down) {
    // we only deal with keydown
    return;
  }

  //Serial.print("ATTN:myCallbackNeoKeyPress ");
  //Serial.println(keynum);
  //return;

   if (mode==DefModeMenu) {
    // in menu mode, any button just exits menu mode
    if (keynum == neoKeyValBackSpace) {
      uiInteractionSignify(DefSoundTypeMenuMove);
      MenuHelperGoBackParent();
      return;
    }
    if (menuMode==DefMenuModeQuestion) {
      //uiInteractionSignify(DefSoundTypeExit);
    } else {
      //uiInteractionSignify(DefSoundTypeWorkflow);
    }
    doSafelyExitMenuMode(false);
    if (keynum!=neoKeyValUser1 && keynum!=neoKeyValUser2 && keynum!=neoKeyValUserGuest && keynum!=neoKeyValEsc) {
      // should we make a beep here and rewing workflow?
      uiInteractionSignify(DefSoundTypeExit);
      jrworkflow.rewindWorkflowMode();
      return;
    } else {
      // drop down to trigger new workflow
    }
  } 

  if (keynum==neoKeyValZeroTare) {
    uiInteractionSignify(DefSoundTypeMainFunc);
    // action
    jrworkflow.scheduleTare();
  } else if (keynum==neoKeyValTimer) {
    uiInteractionSignify(DefSoundTypeMainFunc);
    // action
    jrworkflow.manualTimerTriggerToggle();
  } else if (keynum==neoKeyValSleep && !justWokeUp) {
    uiInteractionSignify(DefSoundTypeSleep, true);   
    // action
    sendDeviceToSleep();
  } else if (keynum==neoKeyValUser1) {
    userStartsOrContinuesWorkflow(DefWfUserName1,1);
  } else if (keynum==neoKeyValUser2) {
    userStartsOrContinuesWorkflow(DefWfUserName2,2);
  } else if (keynum==neoKeyValUserGuest) {
    userStartsOrContinuesWorkflow(DefWfUserNameGuest,0);
  } else if (keynum==neoKeyValEsc && !justWokeUp) {
    // if almost done with workflow mode, should escape advances to done?
    if (false && jrworkflow.getWorkflowMode() == JrWorkflowMode_All) {
      jrworkflow.advanceWorkflowMode(true);
      uiInteractionSignify(DefSoundTypeWorkflow,true);
    } else {
      jrworkflow.setWorkflowModeEnable(false);
      uiInteractionSignify(DefSoundTypeMainFunc);
    }
    jrworkflow.updateWaterLevelIfAppropriate();
  } else if (keynum==neoKeyValEsc) {
    jrworkflow.updateWaterLevelIfAppropriate();
  } else if (keynum == neoKeyValBackSpace) {
    handleUiBackButtonInContext();
  }
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void setupNeokeyMatrix() {
  //jrkeypad.setup(neoKeypadRows, neoKeypadCols, neoKeypadPins_Rows, neoKeypadPins_Cols, neoKeypadPin_Neopixel, neoKeypadColorOption);
  jrkeypad.setup(neoKeypad_Rows, neoKeypad_Cols, neoKeypadPins_Rows, neoKeypadPins_Cols, &jrWrappedKeypad, &jrWrappedNeopixelStrip, jrWrappedNeoKeymap, true, false);
  jrkeypad.setCallbackFp(myCallbackNeoKeyPress);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// we use Bounce2 library to properly debounce buttons and wrap low-level button code


void setupBounceButton(Bounce2::Button &buttonref, int pin, int debounceInterval, int mode, int triggerState) {
  buttonref.attach( pin, mode );
  // DEBOUNCE INTERVAL IN MILLISECONDS
  buttonref.interval(debounceInterval); 
  // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON
  buttonref.setPressedState(triggerState); 
}


void setupButtons() {

  // bounce helpes for buttons
  const int debounceInterval = 5;

   // rotary push button
  pinMode(pinRotaryButton, INPUT_PULLUP);
  setupBounceButton(optionsButton, pinRotaryButton, debounceInterval, INPUT_PULLUP, HIGH);

  // rotary encoder (using Encoder library)
  pinMode(pinRotaryA, INPUT_PULLUP);
  pinMode(pinRotaryB, INPUT_PULLUP);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void setMenuMode(int inMenuMode) {
  // set the menu mode and submode
  mode = DefModeMenu;
  menuMode = inMenuMode;
}
//---------------------------------------------------------------------------






























//---------------------------------------------------------------------------
void goCoffeeSelectMode() {
  int lastCoffeeId = jrworkflow.getUsersLastCoffee();
  //
  //Serial.print("ATTN: in testCoffeeSelectMode.");
  //Serial.println(lastCoffeeId);
  //

  if (lastCoffeeId<1 || lastCoffeeId>countMaxCoffeeIds()) {
    lastCoffeeId = 1;
  }
  
  LCDML.OTHER_setCursorToID(DefCoffeeMenuStartingId + (lastCoffeeId-1));

  setMenuMode(DefMenuModeQuestion);

  //// force transition manually so we dont reset position
  //doModeTransition(false);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void updateCoffeeSelectionChoice() {
  uint8_t optionId = LCDML.FUNC_getID();
  uint8_t coffeeId = (optionId-DefCoffeeMenuStartingId) + 1;
  //Serial.print("ATTN: Leaving question mode from menu option: ");
  //Serial.println(coffeeId);

  // get the menu element name and copy the name in the "var" variable 
  //Serial.print("Label chosen: ");
  //Serial.println(CoffeeBeanTypes[coffeeId]);

  // save their last coffee item
  if (coffeeId>=1 && coffeeId<=countMaxCoffeeIds()) {
    jrworkflow.setUsersLastCoffee(coffeeId, CoffeeBeanTypes[coffeeId-1], grindSettings[coffeeId-1]);
  } else {
    jrworkflow.clearCoffeeSelection();
  }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void handleUiBackButtonInContext() {
  if (jrworkflow.getWorkflowMode()==JrWorkflowMode_None) {
    // nothing to do
    uiInteractionSignify(DefSoundTypeError);    
    return;
  }
  bool bretv = jrworkflow.goBackwardsStep();
  if (bretv) {
    uiInteractionSignify(DefSoundTypeMenuMove);
  } else {
    uiInteractionSignify(DefSoundTypeError);
  }
}
//---------------------------------------------------------------------------
