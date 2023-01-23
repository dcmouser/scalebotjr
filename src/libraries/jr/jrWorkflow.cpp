#include "jrWorkflow.h"
#include <jrutils.h>

//
#include <EEPROM.h>


//---------------------------------------------------------------------------
//const char* doneWorkflowMessages[] = {"Workflow complete.", "Goodbye.", "Thank you.", "Job complete.", "Enjoy your coffee.","Have a nice day.", "How is the coffee?", 0};
const char* doneWorkflowMessages[] = {"Workflow complete.", 0};
const char* JrModeInstructionLines[] = {"","%s's coffee?", "Weigh beans","Pour shot","Add water/milk",""};
//const char* welcomeMessages[] = {"Hello there.", "Good morning.", "How can I help you?",0};
//const char* welcomeMessages[] = {"Hello there.", 0};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// see https://clivecoffee.com/blogs/learn/espresso-drink-recipes
// format is "name", "# ingredient", "",
// ending with "" as a name
/*
const char* recipes[] = {
	"Namericano", "Water 12t", "",
	"Brevicano", "HnH 0.75m", "Water 9t", "",
	"Americano", "Water 4m", "",
	"Cappucino", "Milk 1m", "Foam 1m", "",
	"Latte", "Milk 4m", "",
	"Flat White", "Milk 2m", "",
	"Breve", "HnH 3m", "Foam 0.5m", "",
	"Mocha", "Choc. 1.5m", "Milk 2m", "Foam 1m", "",
	""
	};
*/
// new 5/31/22
const char* recipes[] = {
	"Namericano", "Water 12t", "",
	"Brevicano", "HnH 0.75m", "Water 9t", "",
	"Americano", "Water 5b", "",
	"Machiato", "Milk 1b", "",
	"Cortado", "Milk 2b", "",
	"Flat White", "Milk 3b", "",
	"Cappucino", "Milk 4b", "Foam 1b", "",
	"Latte", "Milk 5b", "",
	"Breve", "HnH 3m", "Foam 0.5m", "",
	"Mocha", "Choc. 1.5m", "Milk 2m", "Foam 1m", "",
	""
	};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
JrWorkflow::JrWorkflow() {
	// constructor
	resetWorkflowWeights();
	strcpy(headerTextLine,"[appinfo_missing]");
	clearCoffeeSelection();
	clearUserSelection();
	setWarningMessage("");
	
	setWaterLevelLimits(0.0, 1.0, 0.10);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void JrWorkflow::begin() {
	// called after app is ready to go
	checkWarnings();
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
void JrWorkflow::setupScales(int sckPin, int dout1Pin, int dout2Pin, HX711MULTI* scalesetinp) {
	
	if (multiscaleMode && dout1Pin && dout2Pin) {
		// new multiscale system
		byte DOUTS[2] = {dout1Pin, dout2Pin};

//		scaleSet = new HX711MULTI(2, DOUTS, sckPin);
		scaleSet = scalesetinp;

		// note that even though we use the multi hx711 class, we still use instances of the jrscale class
		jrscale1.enabled = true;
		jrscale2.enabled = true;
		//
		if (JrStartingScale==1) {
			setActiveAndInactiveScale(&jrscale1);
		} else {
			setActiveAndInactiveScale(&jrscale2);
		}
	} else {
		// old scale class
		multiscaleMode = false;
		if (dout2Pin>0) {
			jrscale2.setupScale(dout2Pin, sckPin);
			if (dout1Pin<=0) {
					setActiveAndInactiveScale(&jrscale2);
			}
		}
		if (dout1Pin>0) {
			jrscale1.setupScale(dout1Pin, sckPin);
			setActiveAndInactiveScale(&jrscale1);
		}
	}

	prefscalep = NULL;
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void JrWorkflow::setHeaderTextLine(char *str) {
	strcpy(headerTextLine, str);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void JrWorkflow::resetWorkflowWeights() {
	for (int i=0;i<=4;++i) {
		workflowWeights[i]=0.0;
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void JrWorkflow::powerUp() {
	if (multiscaleMode) {
		scaleSet->power_up();
	} else {
		powerUpScale(&jrscale1);
		powerUpScale(&jrscale2);
	}

	// recheck warnings
	checkWarnings();
}


void JrWorkflow::powerDown() {
	if (multiscaleMode) {
		scaleSet->power_down();
	} else {
		powerDownScale(&jrscale1);
		powerDownScale(&jrscale2);
	}
}


		
//---------------------------------------------------------------------------
void JrWorkflow::powerUpScale(JrScale *scalep) {
	if (!scalep || !scalep->enabled) {
		return;
	}
	//
	if (multiscaleMode) {
		
	} else {
		scalep->scale.power_up();
	}
}

void JrWorkflow::powerDownScale(JrScale *scalep) {
	if (!scalep || !scalep->enabled) {
		return;
	}
	if (multiscaleMode) {
		
	} else {
		scalep->scale.power_down();
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void JrWorkflow::setOptions(int in_optionDebugLevel, bool in_optionRoundUp, int in_option7SegmentBrightness, int weightSmoothMode, int inoptionScaleMode, int inoptionSoftZero, int inoptionCalibrationTweakMethod, int in_optionAutoZero) {
	optionDebugLevel = in_optionDebugLevel;
	optionScaleMode = inoptionScaleMode;
	optionSoftZero = inoptionSoftZero;
	optionAutoZero = in_optionAutoZero;
	jrscale1.optionDebugLevel = in_optionDebugLevel;
	jrscale1.optionRoundUp = in_optionRoundUp;
	jrscale2.optionDebugLevel = in_optionDebugLevel;
	jrscale2.optionRoundUp = in_optionRoundUp;
	jrscale1.optionSoftZero = inoptionSoftZero;
	jrscale2.optionSoftZero = inoptionSoftZero;
	jrscale1.setWeightSmoothMode(weightSmoothMode);
	jrscale2.setWeightSmoothMode(weightSmoothMode);
	jrscale1.optionCalibrationTweakMethod = inoptionCalibrationTweakMethod;
	jrscale2.optionCalibrationTweakMethod = inoptionCalibrationTweakMethod;
	seg7a.setBrightness(in_option7SegmentBrightness);
	seg7b.setBrightness(in_option7SegmentBrightness);
	lcdMatrix.setBrightness(in_option7SegmentBrightness);
}

void JrWorkflow::setOptions2(float inoptionBeanJarWeight, int in_optionStartingWorkflowMode, int in_optionCheckWarnings, int in_optionWeightSync) {
	optionBeanJarWeight = inoptionBeanJarWeight;
	optionStartingWorkflowMode = in_optionStartingWorkflowMode;
	optionCheckWarnings = in_optionCheckWarnings;
	optionWeightSync = in_optionWeightSync;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void JrWorkflow::readStoredEepromData(unsigned long EpromDataStartAddress) {
  EEPROM.get(EpromDataStartAddress + 0, jrscale1.calibratedPlatformRawWeight);
  EEPROM.get(EpromDataStartAddress + 4, jrscale1.calibrationFactor);
  EEPROM.get(EpromDataStartAddress + 8, jrscale2.calibratedPlatformRawWeight);
  EEPROM.get(EpromDataStartAddress + 12, jrscale2.calibrationFactor);
  // saving users favorite coffees
  int addr = EpromDataStartAddress + 16;
  for (int i=0;i<MaxUserMemoryCount;++i) {
  	EEPROM.get(addr, userLastCoffee[i]);
  	addr+=1;
  }
}


void JrWorkflow::writeStoredEepromData(unsigned long EpromDataStartAddress) {
  EEPROM.put(EpromDataStartAddress + 0, jrscale1.calibratedPlatformRawWeight);
  EEPROM.put(EpromDataStartAddress + 4, jrscale1.calibrationFactor);
  EEPROM.put(EpromDataStartAddress + 8, jrscale2.calibratedPlatformRawWeight);
  EEPROM.put(EpromDataStartAddress + 12, jrscale2.calibrationFactor);
  // saving users favorite coffees
  int addr = EpromDataStartAddress + 16;
  for (int i=0;i<MaxUserMemoryCount;++i) {
  	EEPROM.put(addr, userLastCoffee[i]);
  	addr+=1;
  }
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void JrWorkflow::setEventCallbackFpT(VoidEventCallbackFpT fp) {
	eventCallbackFp = fp;
	jrscale1.setEventCallbackFpT(fp);
	jrscale2.setEventCallbackFpT(fp);
}
//---------------------------------------------------------------------------
	
	
	
	





















//---------------------------------------------------------------------------
void JrWorkflow::loopWorkflow() {

	// tare?
	// elapsed time for vary stuff
  unsigned long ms = millis();

	// track avg update time
  unsigned long loopIntervalMs = ms-lastLoopTimeMs;
  if (loopIntervalMs<DefMaxAvgUpdateToConsider) {
	  avgUpdateMs = DefAvgUpdateMsSmoothFactor*avgUpdateMs + (1.0-DefAvgUpdateMsSmoothFactor)*loopIntervalMs;
  }
  lastLoopTimeMs = ms;
  



  // tare
  if (scheduledTareMillis> 0 && ms > scheduledTareMillis) {
    doScheduledTare();
  }


  
  
  // loop the scales to update and read weights on them
  
  // ATTN: in future we might want to loop the non-active scall less frequently
  doMultiLoopScales();


  // startup calibration tweak
  if (wantScheduleCalibrationTweak) {
	scheduleCalibrationTweak();
  }
  if (scheduledCalibrationTweakMillis> 0 && ms > scheduledCalibrationTweakMillis) {
    doScheduledCalibrationTweak();
} else if (optionAutoZero==3 || (optionAutoZero==2 && ms<lastWokeTime+DefContinuous1AutoZeroTime)) {
	// CONTINUALLY try calibrating small weights
	doContinuousCalibrationTweakForScaleIfStableSmall(&jrscale1);
	doContinuousCalibrationTweakForScaleIfStableSmall(&jrscale2);
  }

  
  // decide if we want to change what our ACTIVE scale is (jrscalep)
  // ATTN: we might not want to do this EVERY cycle
  updateActiveScaleChoice();
	
  // update weight stored for current workflow
  float displayWeight = jrscalep->getDisplayWeight();

  // manually override bean weight?
  if ((displayWeight<=0 && manualBeanWeight>0) && workflowMode==JrWorkflowMode_Beans) {
	displayWeight = manualBeanWeight;
	}


	// update workfloweights
	if (workflowMode == JrWorkflowMode_Espresso && workflowSubStepIndex == 4) {
		// this is a more delicate time when we are waiting for espresso drops to finish flowing..
		// and we dont want to update when cup is being MOVED to other scale or removed from platform
		if (displayWeight>workflowWeights[workflowMode]) {
			workflowWeights[workflowMode] = displayWeight;
		}
	} else {
		workflowWeights[workflowMode] = displayWeight;
	}
  
  // update timer
  if (isTimerRunning()) {
  	timerElapsedMs = millis() - timerStartMillis;
  }
  
  // see if any auto workflow stuff should happen
  checkAutoWorkflow();

	// water level meter occasional update
	updateWaterLevelOccasionally();


	if (justWoke) {
		if (wokeTime()>DefJustWokeMs) {
			justWoke = false;
		}
	}
 }
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void JrWorkflow::workflowModeChanges() {
	// sub-step index for auto workflow
	workflowSubStepIndex = 0;
	manualBeanWeight = 0;
  // force redisplay of info?
  clearDupeCheckers(true, true, true);
  // set/change preferred scale for workflow
  setPreferredScaleIdForWorkflow();
  // update instructions
  showWorkflowModeInstructions();
  
  if (workflowMode==JrWorkflowMode_Espresso) {
  	jrscale1.setWeightChangeSensitivity(JrWeightChangeSensitivityMode_Low);
  	jrscale2.setWeightChangeSensitivity(JrWeightChangeSensitivityMode_Low);  	
  } else {
  	jrscale1.setWeightChangeSensitivity(JrWeightChangeSensitivityMode_High);
  	jrscale2.setWeightChangeSensitivity(JrWeightChangeSensitivityMode_High);
  }
  
  // water level
  if (workflowMode == JrWorkflowMode_None || justWoke) {
  	// display water level
	updateWaterLevelIfAppropriate();
  }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool JrWorkflow::advanceWorkflowMode(bool manualHuman) {
	// return true on success; false on error to cause beep
	
	// wrap around to start
  if (workflowMode == JrWorkflowMode_Done) {
    // reset to start
    setWorkflowModeEnable(false);
    doClearTare();
    return true;
  } else {
    // advance mode
    if (workflowMode == JrWorkflowMode_None) {
			clearCoffeeSelection();
    	workflowMode = optionStartingWorkflowMode;
    } else {
    	if (workflowMode == JrWorkflowMode_Beans) {
    		// moving on from beans should clear any tare if done manually
    		smartGuessBeanWeightInJarIfAppropriate();
    		if (manualHuman) {
	    		// clear the bean cup tare
					doClearTare();
    		}
    	}

    	if (blockWorkflowAdvanceOnBadWeights) {
	    	// make sure we are allowed to advance
	    	if (!workflowSafeToAdvance()) {
					//eventCallbackFp(JrEventCallbackEnum_PlayErrorSound);
	    		return false;
	    	}
    	}
   
    workflowMode = ((int)workflowMode) + 1;
    }
  }
  
	if (workflowMode == JrWorkflowMode_All) {
		// when switching into all mode, clear recipe
		clearRecipe();
	}

	// clear some stuff
	workflowModeChanges();

	if (optionDebugLevel>0) {
  	Serial.print("ATTN: Advanced to workflow mode: ");
  	Serial.print(workflowMode);
  	Serial.println("");
  }
  
  // did user just finish a drink?
  if (workflowMode == JrWorkflowMode_Done) {
		// fixup weight if they removed cup before they were done
		if (isTornWeightBelow(0)) {
			workflowWeights[JrWorkflowMode_All] = lastPositiveWeight;
		}
  	storeWorkflowedDrink();
  }
  
  return true;
}


void JrWorkflow::rewindWorkflowMode() {
	if (workflowMode>=JrWorkflowMode_SelectCoffee) {
		workflowMode = (int)workflowMode-1;
	}
	workflowModeChanges();
}


void JrWorkflow::setWorkflowModeEnable(bool val) {
	timerOff();
	if (val) {
		workflowMode = optionStartingWorkflowMode;
	} else {
		if (workflowMode!=JrWorkflowMode_None) {
	    doClearTare();
			workflowMode = JrWorkflowMode_None;
		}
	}
	//
	workflowModeChanges();
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
void JrWorkflow::resumeFromSleepOrStart(JrWorkflowStartModeEnum resumeMode) {
	if (true || optionAutoZero>0) {
		wantScheduleCalibrationTweak = true;
	}

	if (resumeMode == JrWorkflowStartMode_Last) {
		// do nothing
		return;
	} else if (resumeMode == JrWorkflowStartMode_Scale) {
		setWorkflowModeEnable(false);
	} else if (resumeMode == JrWorkflowStartMode_Wf) {
		setWorkflowModeEnable(true);
	} else {
		// shouldnt happen
	}
	
	prepareForWake();
}

void JrWorkflow::firstStart(JrWorkflowStartModeEnum resumeMode) {
	if (resumeMode==JrWorkflowStartMode_Last) {
		resumeMode = JrWorkflowStartMode_Scale;
	}
	resumeFromSleepOrStart(resumeMode);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
void JrWorkflow::prepareForSleepEarly() { 
  if (workflowMode == JrWorkflowMode_All) {
    advanceWorkflowMode(true);
    delay(500);
  }
}

void JrWorkflow::prepareForSleepLate() { 
	seg7a.setActive(false);
	seg7b.setActive(false);
	lcdMatrix.setActive(false);
	//
	jrscale1.prepareForSleep();
	jrscale2.prepareForSleep();
}



void JrWorkflow::prepareForWake() { 
	seg7a.setActive(true);
	seg7b.setActive(true);
	lcdMatrix.setActive(true);
	jrscale1.prepareForWake();
	jrscale2.prepareForWake();
	// record last time we woke up
	lastWokeTime = millis();
	justWoke = true;
	//
	updateWaterLevelIfAppropriate();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void JrWorkflow::updateDisplays(bool force, bool flagUpdateControlScreen) {
	if (force) {
		clearDupeCheckers(true, true, true);
	}
	
	// active scale info
	float displayWeight;
	bool dualScaleDisplay = false;
	
	if (optionScaleMode==1 && workflowMode == JrWorkflowMode_None && timerState == JrTimerStateOff) {
		// display 1 shows scale 1
		dualScaleDisplay = true;
		// this mode shows each scale on its on lcd, purely
		displayWeight = jrscale1.getDisplayWeight();
	} else {
		displayWeight = jrscalep->getDisplayWeight();
	}

	float internalTare = jrscalep->getInternalTare();

  // manually override bean weight?
  if ((displayWeight<1 && manualBeanWeight>0) && workflowMode==JrWorkflowMode_Beans) {
	displayWeight = manualBeanWeight;
	}

	if (workflowMode == JrWorkflowMode_Done) {
		// all in one (with ratio) is not as nice, but good for end info when ratio is where tare would be on its own
		updateWeightDisplayWithSecondaryWeight(workflowWeights[JrWorkflowMode_All], workflowWeights[JrWorkflowMode_Beans]);
	} else {
		if (paModeDisplayTareWeightOneLine) {
			updateTareWeightDisplay(displayWeight, internalTare);
		} else {
			// tare
			updateTareDisplay(internalTare);
			
			if (displayWeight<0 && workflowMode == JrWorkflowMode_All && workflowWeights[JrWorkflowMode_All]>0) {
				// they removed their cup, so show them last good value
				displayWeight = workflowWeights[JrWorkflowMode_All];
			}
	 		// scale weight (and ratio)
			updateWeightDisplay(displayWeight);
		}
	}

	if (dualScaleDisplay) {
		// display 2 shows scale 2
		displayWeight = jrscale2.getDisplayWeight();
		simpleShowFloatOnDisplay(displayWeight, &seg7a);
	} else {
		// other workflow info
		updateTimerDisplay();
	}
	
	// new recipe info
	if (workflowMode == JrWorkflowMode_All && currentRecipe>-1) {
		displayRecipeIngredientStepInfo();
		autoAdvanceRecipeIngredientStepIfAppropriate();
	}
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
void JrWorkflow::showTimerStr(char* str) {
  // display
  jrlcdp->setCursor (0,2);
  jrlcdp->print(str);
}


void JrWorkflow::showTimerStrPad(const char* str) {
  // display
  char buf[24];
  strcpy(buf, str);
  jrpadstr(buf, 20);
  showTimerStr(buf);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
void JrWorkflow::manualTimerTriggerToggle() {
	if (timerState == JrTimerStateRunning) {
		// stop the timer
		stopTimer();
	} else if (timerState == JrTimerStateDone) {
		// it's stopped, now hide it
		timerOff();
	} else {
		// start timer
		startTimer(false);
	}
}

void JrWorkflow::timerOff() {
	// its not enough to just stop timer, that will still display it; we need to clear it to 0
	timerState = JrTimerStateOff;
	clearTimerStr();
}


void JrWorkflow::stopTimer() {
	timerState = JrTimerStateDone;
	timerEndMillis = millis();
}

void JrWorkflow::startTimer(bool isAsterisked) {
	timerState = JrTimerStateRunning;
	timerAsterisk = isAsterisked;
	setTimerStartNow();
	//showTimerStrPad("");
}


void JrWorkflow::clearTimerStr() {
	// set timer to 0 to not show it
	timerElapsedMs=0;
	// clear the line
	showTimerStr("");
	clearDupeCheckers(true, false, false);
}

void JrWorkflow::clearDupeCheckers(bool clearTimer, bool clearTare, bool clearDisplayWeight) {
	// clear this so timer value is forced to refresh
	if (clearTimer) {
		timerElapsedPreviousValue=-1;
	}
	if (clearTare) {
		// ditto for tare
		tarePreviousValue = -98765;
	}
	if (clearDisplayWeight) {
		// ditto for displayweight
		displayWeightPreviousValue = -98765;
		secondWeightPreviousValue = -98765;
	}
}


void JrWorkflow::setTimerStartNow() {
	// mark start time
  timerStartMillis = millis();
  // set it to have run a tiny bit in case we want to show it immediately?
  // timerElapsedMs = 1;
  // clear previous value so it shows
	clearDupeCheckers(true, false, false);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool JrWorkflow::readyForAutoTimer() {
	if (!optionEnableAutoTimer) {
		// this option is disabled
		return false;
	}
	if (workflowMode==JrWorkflowMode_None || workflowMode==JrWorkflowMode_Espresso) {
		// not in workflow mode, or we are in the one workflow mode that supports timiing (espresso)
		return true;
	}
	// nope
	return false;
}
//---------------------------------------------------------------------------


/*
//---------------------------------------------------------------------------
// timer functions
void JrWorkflow::timerMaybeBegins() {
	// timer MAY have begun
}

void JrWorkflow::timerMaybeEnds() {
	// timer MAY have ended
}
//---------------------------------------------------------------------------
*/












//---------------------------------------------------------------------------
void JrWorkflow::workflowDisplayEngage() {
  jrlcdp->clear();
  updateDisplays(true, true);
	showWorkflowModeInstructions();
}



void JrWorkflow::showWorkflowModeInstructions() {
	  
  if (workflowMode==JrWorkflowMode_All && currentRecipe>-1) {
  	// show recipe instead of workflow instructions
  	dislayRecipeStepInstructions();
  	return;
  }


	char str[24];
  if (workflowMode == JrWorkflowMode_None) {
  	strcpy(str, headerTextLine);
  } else if (workflowMode == JrWorkflowMode_SelectCoffee || workflowMode == JrWorkflowMode_Beans || workflowMode == JrWorkflowMode_Espresso || workflowMode == JrWorkflowMode_All) {
  	if (strcmp(JrModeInstructionLines[workflowMode],"") == 0) {
  		strcpy(str, "");
  	} else if (strstr(JrModeInstructionLines[workflowMode],"%s")) {
  		//char str2[24];
  		//sprintf(str2, "]%d. %s", (workflowMode-(int)optionStartingWorkflowMode)+2, JrModeInstructionLines[workflowMode]);
  		//sprintf(str,str2,userName);
  		sprintf(str, JrModeInstructionLines[workflowMode], userName);
  	} else {
  		sprintf(str, "]%d. %s", (workflowMode-(int)optionStartingWorkflowMode)+1, JrModeInstructionLines[workflowMode]);
  	}

  	// extra info in dif modes
  	if (workflowMode==JrWorkflowMode_Beans) {
  		char gsizebuf[6];
  		if (grindSize>0) {
  			if (workflowSubStepIndex>0) {
	  			sprintf(gsizebuf," [%d]",grindSize);
  			} else {
	  			sprintf(gsizebuf," (%d)",grindSize);  			
  			}
  		strcat(str, gsizebuf);
  		} else {
  			if (workflowSubStepIndex>0) {
	  			strcat(str, " (*)");
  			}
  		}

  	}	else if (workflowMode==JrWorkflowMode_Espresso) {
  		float bweight = workflowWeights[JrWorkflowMode_Beans];
  		if (bweight>0.0) {
  			char addstr[12];
  			char beanweightstr[8];
				jrFloatToStr(beanweightstr, bweight, 1, 1);
  			sprintf(addstr," (%s)", beanweightstr);
  			strcat(str, addstr);
  		}
  	}
  } else if (workflowMode == JrWorkflowMode_Done) {
			calcWorkflowDoneMessage(str);
  } else {
  	strcpy(str,"");
  }
  
	// display instructions
  jrpadstr(str, 20);
  jrlcdp->setCursor ( 0, 0 );            // go to the top left corner
	jrlcdp->print(str);
	
	// clear third (timer line) -- we do this for sake of the recipe line
	strcpy(str,"");
	jrpadstr(str, 20);
  jrlcdp->setCursor ( 0, 2 );
	jrlcdp->print(str);
	clearDupeCheckers(true,false,false);
	
  // image
  updateMatrixImageForWorkflowMode();
}



void JrWorkflow::calcWorkflowDoneMessage(char *outbuf) {
	strcpy(outbuf,"] ");
	strcat(outbuf, jrChooseRandomStringFromList(doneWorkflowMessages));
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
void JrWorkflow::updateTareDisplay(float tareval) {
		if (tareval == tarePreviousValue) {
		// no change, just return
		return;
	}
	tarePreviousValue = tareval;
	
  //Serial.print("Showing tare ");
  //Serial.println(tareval);
	
  char str[24];
  //
  if (tareval<1) {
  	// show any warning message where tare would go
		strcpy(str,warningMsg);
    //strcpy(str,"");
  } else {
    char weightStr[10];
    jrFloatToStr(weightStr, tareval, 0, 1);
    sprintf(str, "Tare:  %sg", weightStr);
  }

  // display
  jrpadstr(str, 20);
  jrlcdp->setCursor (0,1);
  jrlcdp->print(str);
}


void JrWorkflow::simpleShowFloatOnDisplay(float val, Jr7Seg *seg7p) { 
  // 7 segment
  seg7p->printFloat(val,1);
}





void JrWorkflow::updateWeightDisplay(float displayWeight) {
  if (displayWeight == displayWeightPreviousValue) {
  	// no change, just return
  	return;
  }
  displayWeightPreviousValue = displayWeight;

  char str[24];
  char weightStr[12];
  //
  jrFloatToStr(weightStr, displayWeight, 1, 1);
  sprintf(str, "Grams: %s", weightStr);    
  jrpadstr(str,20);

  // show it
  jrlcdp->setCursor ( 0, 3 );
  jrlcdp->print(str);
  
  // 7 segment
  seg7b.printFloat(displayWeight,1);

  // now ratio
  updateRatioDisplay();
}



void JrWorkflow::updateTareWeightDisplay(float displayWeight, float tareval) {
	if (tareval == tarePreviousValue && displayWeight == displayWeightPreviousValue) {
		// no change, just return
		return;
	}
	tarePreviousValue = tareval;
  displayWeightPreviousValue = displayWeight;

  char str[24];
  char weightStr[12];
  
  jrFloatToStr(weightStr, displayWeight, 1, 1);
  if (tareval>=paMinTareToShow) {
  	char tareStr[12];
  	jrFloatToStr(tareStr, tareval, 1, 1);
  	sprintf(str, "Grams: %s (%s)", weightStr,tareStr);
  } else {
  	sprintf(str, "Grams: %s", weightStr);    
  }
  jrpadstr(str,20);

  // show it
  jrlcdp->setCursor ( 0, 3 );
  jrlcdp->print(str);
  
  // now ratio
  updateRatioDisplay();
}


void JrWorkflow::updateWeightDisplayWithSecondaryWeight(float displayWeight, float secondWeight) {
	if (secondWeight == secondWeightPreviousValue && displayWeight == displayWeightPreviousValue) {
		// no change, just return
		return;
	}
	secondWeightPreviousValue = secondWeight;
  displayWeightPreviousValue = displayWeight;

  char str[24];
  char weightStr[12];

  jrFloatToStr(weightStr, displayWeight, 1, 1);
  if (secondWeight>=paMinTareToShow) {
  	char secondWeightStr[12];
  	jrFloatToStr(secondWeightStr, secondWeight, 1, 1);
  	sprintf(str, "Grams: %s (%s)", weightStr,secondWeightStr);
  } else {
  	sprintf(str, "Grams: %s", weightStr);    
  }
  jrpadstr(str,20);

  // show it
  jrlcdp->setCursor ( 0, 3 );
  jrlcdp->print(str);

  // 7 segment
  seg7b.printFloat(displayWeight,1);

  // now ratio
  updateRatioDisplay();
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
void JrWorkflow::updateRatioDisplay() {
  // show ratios for weights in workflow
  
 
  // ATTN: like the others we would like to avoid updating if values unchanged
  // for now we handle this by updating this iff weight display updates
  if (workflowMode == JrWorkflowMode_None) {
  	return;
  }

 
  // yes we want to display ratio
  char ratioStr[24];  
  char str[24];
  if (workflowMode < JrWorkflowMode_Espresso || workflowWeights[JrWorkflowMode_Beans] < 1.0 || (workflowWeights[JrWorkflowMode_Espresso] < workflowWeights[JrWorkflowMode_Beans]/2.0) || (workflowMode == JrWorkflowMode_Espresso && !isTearWeightOver(wfMinCupWeightForEspressoTare) )) {
    // display nothing
    // ATTN: UNFINISHED we share this space with TARE display, so we should tell scale to show tare here
    // the problem here is that we have have displayed a ratio for a moment, and then want to fall back to tare? 
    if (workflowMode == JrWorkflowMode_Espresso) {
    	if (!isTearWeightOver(wfMinCupWeightForEspressoTare)) {
    		strcpy(str, "] Awaiting cup...");
    	} else {
  	  	strcpy(str,"");
  	  }
  	} else {
  		strcpy(str,"");
  	}
  } else {
  	if (workflowMode == JrWorkflowMode_Espresso || (workflowMode >= JrWorkflowMode_All && workflowWeights[JrWorkflowMode_Espresso] < 1.0) || (workflowWeights[JrWorkflowMode_All] < workflowWeights[JrWorkflowMode_Espresso])) {
    	// show ratio of beans to espresso
    	char f1[10];
    	jrFloatToStr(f1, (workflowWeights[JrWorkflowMode_Espresso] / workflowWeights[JrWorkflowMode_Beans]), 0, 1);
    	jrRemoveTrailingZeros(f1);
    	sprintf(ratioStr, "1:%s", f1);
  	} else if (workflowMode >= JrWorkflowMode_All) {
	    // show ratio of beans to espresso to full vol
    	char f1[10], f2[10];
    	jrFloatToStr(f1, (workflowWeights[JrWorkflowMode_Espresso] / workflowWeights[JrWorkflowMode_Beans]), 0, 1);
    	jrFloatToStr(f2, (workflowWeights[JrWorkflowMode_All] / workflowWeights[JrWorkflowMode_Espresso]), 0, 1);
    	jrRemoveTrailingZeros(f1);
    	jrRemoveTrailingZeros(f2);
    	sprintf(ratioStr,"1:%s:%s", f1,f2);
  	}
  
  	// build string for display
  	if (strcmp(ratioStr,"")==0) {
	    strcpy(str,"");
	  } else {
    	sprintf(str, "Ratio: %s", ratioStr);
  	}
  }
 
  // display
  // for now we put this on same row as (instead of) Tare info
  jrpadstr(str, 20);
  jrlcdp->setCursor (0,1);
  jrlcdp->print(str);
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
void JrWorkflow::updateTimerDisplay() {
	// update timer display

	if (timerElapsedMs==0 && timerElapsedPreviousValue==0) {
		return;
	}

	// same value as before?
	unsigned long elapsedLeastVal;
  if (optionShowTimerMs && timerElapsedMs < 60000) {
  	// we will show ms
		elapsedLeastVal = timerElapsedMs/100;
	} else {
		// seconds is all that matters
		elapsedLeastVal = timerElapsedMs/1000;		
	}
	if (elapsedLeastVal == timerElapsedPreviousValue) {
   	// value unchanged, nothing to show
   	return;
 	} else {
 		timerElapsedPreviousValue = elapsedLeastVal;
 	}

  char str[24];
  if (!isTimerRunning() && timerElapsedMs<500) {
    strcpy(str,"");
    // 7 segment timer
  	seg7a.printElapsedTime(timerElapsedMs);
  } else {
    char timeStr[16];
    jrConvertMsToNiceTimeBuf(timerElapsedMs, timeStr, optionShowTimerMs);
    // 7 segment
  	seg7a.printElapsedTime(timerElapsedMs);

    if (timerAsterisk) {
    	sprintf(str, "Timer: %s*", timeStr);    	
    } else {
    	sprintf(str, "Timer: %s", timeStr);
    }
  }
  
  // show it
  jrpadstr(str, 20);
  showTimerStr(str);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
void JrWorkflow::checkAutoWorkflow() {
	// the things we might automatically do are:
	// 1. auto tare
	// 2. auto start timer
	// 3. auto-advance workflow mode
	
	if (!wfEnableAutoAdvance) {
		return;
	}
	
	//Serial.print("ATTN: workflow mode = ");
	//Serial.println((int)workflowMode);
	//Serial.print("workflowSubStepIndex=");
	//Serial.println(workflowSubStepIndex);

	// in bean mode:
	//  1. auto tare on stable placement of initial bean cup weight
	//  2. auto advance to espresso mode on REMOVAL of torn bean cup removal back to near 0
	if (workflowMode==JrWorkflowMode_Beans) {
		// this is quite complicated for the following reasons:
		// 1. autotaring the bean cup is easy by
		// 2. after user puts in beans they need to REMOVE the beans and cup
		// 3. and we want to REMEMBER the last stable weight we saw in the cup if so, and then auto advance if that option is enabled
		// 4. unlike pouring espresso, we cannot just wait until weight stabilizies, since adding beans might be added in bits and spurts
		// 5. if user did not give beans enough time to settle in weight, we'd like to make a good guess about what we saw
		// 6. and lastly if user wants to not auto advance from bean weight mode, a manual advance should use either weight on the scale, or last good weight we saw if they lifted cup before manual advance
		if (workflowSubStepIndex==0 && isWeightStabilized(wfStableWeightElapsedTimeShort) && isTornWeightOver(wfMinCupWeightForBeanTare) && isTareMinimal()) {
			// yes, we can auto tare
			wfDoAutoTare();
			// provide some feedback
			wfSignifyAutoStep();
			// sub step index
			workflowSubStepIndex = 1;
			// update instructions
			showWorkflowModeInstructions();
		}	else if (workflowSubStepIndex < 2 && isWeightStabilized(wfStableWeightElapsedTimeShort) && isTornWeightOver(wfMinWeightForBeans) && !isTareMinimal()) {
			// ok cup is torn (by auto or user) and there are beans; so mark that cup is torn, and ready to move to next step
			workflowSubStepIndex = 2;
			clearTrackingWeights();
			// update instructions
			showWorkflowModeInstructions();
		}
		if (workflowSubStepIndex >= 2 && workflowSubStepIndex <=6) {
			if (!isTareMinimal() && isTornWeightOver(wfMinWeightForPositiveStabilized) && isTornWeightBelow(wfBeanWeightToAssumeInContainerMax)) {
				// we know the cup is still on the scale at this point
				// track bean weight without requiring it to stabilize fully
				// weight stabilized, should we record the weight
				// note that we can kick back into this mode from substep 5 if person puts cup BACK on scale before advancing
				if (isWeightStabilized(wfStableWeightElapsedTimeShort)) {
					// track this, and remember it even if we later oscilatted unstabilized
					lastPositiveWeight = jrscalep->getDisplayWeight();
					lastPositiveBeanWeight = jrscalep->getDisplayWeight();
					//Serial.print("ATTN: 1 forcing last pos bean to ");
					//Serial.println(lastPositiveBeanWeight);
					workflowWeights[JrWorkflowMode_Beans] = wfGetLastPositiveBeanWeight();
					jrscalep->clearHighestTornWeightSinceTare();
					// advance to step 4 since we have a long enough stable beans to use; once this happens we dont pay attention to shorter stabilizations
					workflowSubStepIndex = 4;
				} else if (workflowSubStepIndex<4 && isWeightStabilized(wfStableWeightElapsedTimeSuperShort)) {
					// we only update this until we get a longer stabilization -- in this way we will get reasonable bean weight even if we never see a longer stabilization
					// if beans stabilize even briefly, and we haven't gotten a nice long stabilization, remember it
					// but stay in workflowstep2, meaning less than ideal weight stabilization
					lastPositiveWeight = jrscalep->getDisplayWeight();					
					lastPositiveBeanWeight = jrscalep->getDisplayWeight();
					workflowWeights[JrWorkflowMode_Beans] = wfGetLastPositiveBeanWeight();
					//Serial.print("ATTN: 2 forcing last pos bean to ");
					//Serial.println(lastPositiveBeanWeight);
					jrscalep->clearHighestTornWeightSinceTare();
					// advance to step 3, meaning we have suboptimal estimation of bean weight; this will be overwritten if it stabilized again minimally
					workflowSubStepIndex = 3;
				}
			}
			// smartguess
			if (workflowSubStepIndex>=3 && isWeightStabilized(wfStableWeightElapsedTimeNorm) && isTornWeightBelow(-1*wfMinCupWeightForBeanTare)) {
				// ok weight is stable after cup REMOVED because tare value is negative now
				// so this is a sign to move to the next workflow mode where user can weigh cup
				// ATTN: note one problem is that we are advancing here when beans have been removed.. but we want to guess weight of beans
				// note that even if we advanced to this stage, we still trigger some code above to adjust; we can even go backwards above to step 4
				//Serial.println("ATTN: new test 0");
				if (workflowSubStepIndex<5) {
					workflowWeights[JrWorkflowMode_Beans] = wfGetLastPositiveBeanWeight();
					workflowSubStepIndex = 5;
					//Serial.println("ATTN: new test A");
					//Serial.print("beanweight store ");
					//Serial.println(workflowWeights[JrWorkflowMode_Beans]);
				}
				if (optionAutoAdvanceWorkflowFromBeans) {
					// only do this if we see a weight on the espresso scale
					//Serial.println("ATTN: new test 1");
					if (isCupOnEspressoScale() && workflowSubStepIndex==6) {
						// we wont autoadvance if cup just left on coffee scale
						//Serial.println("ATTN: new test 2");
						workflowWeights[JrWorkflowMode_Beans] = wfGetLastPositiveBeanWeight();
						// clear the bean cup tare
						doClearTare();
						// advance to espresso mode
						advanceWorkflowMode(false);
						// provide some feedback
						wfSignifyAutoStep();
						// done
						return;
					} else if (!isCupOnEspressoScale() && workflowSubStepIndex==5) {
						// we require coffee cup OFF espresso scale after bean jar remove before we will advance to stage 6 and therefore auto advance
						//Serial.println("ATTN: new test 3");
						workflowSubStepIndex = 6;
					}
				}
			}
		}
	}

	// in espresso mode:
	//  1. auto tare on stable placement of espresso cup weight
	//  2. auto start timer as weight starts to increase
	//  3. auto stop timer as weight stabilizes
	//  4. auto advance to milk/water/add mode after stop of timer
	if (workflowMode==JrWorkflowMode_Espresso) {
		if (workflowSubStepIndex==0 && isWeightStabilized(wfStableWeightElapsedTimeShort) && isTornWeightOver(wfMinCupWeightForEspressoTare) && isTareMinimal()) {
			// yes, we can auto tare
			wfDoAutoTare();
			workflowSubStepIndex = 1;
			// provide some feedback
			wfSignifyAutoStep();
			// note that this should kick us to auto advance once the tare gets set
		}	else if (workflowSubStepIndex <= 1 && isWeightStabilized(wfStableWeightElapsedTimeShort) && !isTareMinimal()) {
			// ok cup is torn (by auto or user) and hopefully no espresso yet, just advance to next SUB step, waiting for espresso
			workflowSubStepIndex = 2;
		} else if (workflowSubStepIndex == 2 && isWeightSteadyIncreasing()) {
			// weight is increasing due to espresso flow
			// start timer and back-date it
			startTimer(true);
			// backdate timer
			timerStartMillis = wfGetStartTimeOfSteadyIncrease();
			// provide some feedback
			wfSignifyAutoStep();
			// advance SUB step
			workflowSubStepIndex = 3;
		} else if (workflowSubStepIndex == 3 && isWeightStabilized(wfStableTimeEndPourMs)) {
			// weight of espresso flow stopped steady increasing, we are done and ready to move on
			// note we could test for isWeightStabilized() here but the intention is that we want to wait LONGER than that to be sure
			// start timer and back-date it
			stopTimer();
			// backdate timer
			timerElapsedMs = wfGetStopTimeOfSteadyIncrease() - timerStartMillis;
			// mark timer as now precide
			timerAsterisk = false;
			// force update of backdated time
			clearDupeCheckers(true, false, false);
			workflowSubStepIndex = 4;
			// provide some feedback
			wfSignifyAutoStep();
			// we would like to swtich to a more sensitive mode to detect end of espresso
			jrscale1.setWeightChangeSensitivity(JrWeightChangeSensitivityMode_VeryHigh);
			jrscale2.setWeightChangeSensitivity(JrWeightChangeSensitivityMode_VeryHigh);
			jrscale1.resetLastChangeTimes();
			jrscale2.resetLastChangeTimes();
		} else if (workflowSubStepIndex == 4 && (isWeightStabilized(wfStableTimePostRealEndPourMs) || (millis()-timerEndMillis > wfWaitAfterTimerEndToAdvanceMode) || (getMostWeightedScalep()==&jrscale1) || (isTornWeightBelow(0)))) {
			// ok espresso is truly done (cup removed or very stabilized)
			// advance to all mode, which will record the end weight, etc.
			// ATTN: note there is some bug that puts it here and beepign incessantly
			advanceWorkflowMode(false);
			// provide some feedback?
			// should we will skip providing feedback for this since its just a little after the timer stop feedback?
			wfSignifyAutoStep();
			return;
		}
	}

	// in milk/water/all mode don't auto advance, because user may add dif stuff
	
	// BUT we need to track stable weights in case they remove their cup before they say they are done
	if (workflowMode==JrWorkflowMode_All) {

		// NEW 10/20/22 we want to detect when cup is moved from scale 1 to scale 2 and coordinate the torn weight 
		if (workflowSubStepIndex==0) {
			// stage 0 is the handover from scale 1 to scale 2
			//if (true || optionDebugLevel>0) {
			//	Serial.println("In workflow alla subindex 0.");
			//}
			if (isWeightStabilized(wfStableWeightElapsedTimeSuperShort) && isTearWeightOver(wfMinCupWeightForEspressoTare) && isTornWeightOver(0)) {
				// ok we have weight stabilized and non-trivial
				if (optionDebugLevel>0) {
					Serial.println("In workflow 3b subindex 0b.");
				}				
				if (getMostWeightedScalep()==&jrscale2) {
					// still on scale 2, do nothing but track weight?
					// since weve already moved on to JrWorkflowMode_All we should stick with whatever measurement we mad 
					if (optionDebugLevel>0) {
						Serial.println("In workflow 3 subindex 0c.");
					}
					if (true) {
						lastPositiveWeight = jrscalep->getDisplayWeight();
						workflowWeights[JrWorkflowMode_All] = lastPositiveWeight;
					}
				} else {
					if (!optionWeightSync || jrscalep->getDisplayWeight() - lastPositiveWeight > wfMaxWeightDifferentialToSkipWorkflowFinalScaleSync) {
						// abort synnc
						workflowSubStepIndex = 1;
					} else {
						// we are stable on main scale, we can coordinate weights and advance
						// tell scale 1 that current weight should match the last weight we had on scale 2
						workflowWeights[JrWorkflowMode_All] = lastPositiveWeight;
						if (optionDebugLevel>0) {
							Serial.println("Caling forceWeightSync with target weight.");
							Serial.println(workflowWeights[JrWorkflowMode_All]);
						}		
						forceWeightSync(&jrscale1, workflowWeights[JrWorkflowMode_All]);
							// and now advance
						workflowSubStepIndex = 1;
						// TEST
						if (true) {
							wfSignifyAutoStep();
							//eventCallbackFp(JrEventCallbackEnum_PlayErrorSound);
						}
					}
				}
			} else {
				// weight unstabilized, nothing to do
			}
			return;
		}
		
		// dropping down here means we are in all worlflow and subindex >0 
		
		if (optionDebugLevel>0) {
			Serial.println("In workflow all subindex >0.");
		}

		// keep track of latest stable weight
		if (isWeightStabilized(wfStableWeightElapsedTimeShort) && isTornWeightOver(workflowWeights[JrWorkflowMode_Espresso]-1)) {
			// weight of drink is stable so remember it
			lastPositiveWeight = jrscalep->getDisplayWeight();
		} else if (isTornWeightBelow(0)) {
			// cup removed
			workflowWeights[JrWorkflowMode_All] = lastPositiveWeight;
			if (isWeightStabilized(wfStableWeightElapsedTimeCupRemovedAtEnd) && lastPositiveWeight>0.0 && (currentRecipe==-1 || isRecipeIndexDone) ) {
				// cup has been gone long enough let's mark it as DONE
				advanceWorkflowMode(false);
				// provide some feedback?
				// should we will skip providing feedback for this since its just a little after the timer stop feedback?
				wfSignifyAutoStep();
				return;
			}
		}
	}
	
	return;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
void JrWorkflow::forceWeightSync(JrScale* scalep, float weight) {
	// for now this is EXACTLy like a calibration step, EXCEPT that we want it to be temporary like a startup tare
	scalep->doSetCalibrationTweakForCorrectedWeight(weight + scalep->getInternalTare());
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
bool JrWorkflow::isTornWeightOver(float weightThreshold) {
	float weight = jrscalep->getDisplayWeight();
	if (weight > weightThreshold) {
		return true;
	}
	return false;
}

bool JrWorkflow::isTornWeightBelow(float weightThreshold) {
	float weight = jrscalep->getDisplayWeight();
	if (weight < weightThreshold) {
		return true;
	}
	return false;
}


bool JrWorkflow::isTareMinimal() {
	if (jrscalep->getInternalTare() <= wfMaxMinimalTareThreshold) {
		return true;
	}
	return false;
}

bool JrWorkflow::isTearWeightOver(float weightThreshold) {
	return (jrscalep->getInternalTare() > weightThreshold);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void JrWorkflow::wfDoAutoTare() {
	scheduleTare();
}

void JrWorkflow::doClearTare() {
	jrscale1.doClearTare();
	jrscale2.doClearTare();
}


void JrWorkflow::clearTrackingWeights() {
	//Serial.println("Clearing tracking weihts.");
	lastPositiveBeanWeight = 0.0;
	lastPositiveWeight = 0.0;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void JrWorkflow::wfSignifyAutoStep() {
	// play sound, etc.
	eventCallbackFp(JrEventCallbackEnum_WfAdvance);
}
//---------------------------------------------------------------------------














// the "hard" auto functions



//---------------------------------------------------------------------------
bool JrWorkflow::isWeightStabilized(unsigned long duration) {
	// we want to ask that weight has been relatively stable for "some" amount of time
	// a reasonable stat to look at is SMOOTHED abs deltaweights (where deltaweight is the difference between consecutive weight readings)
	// for that stat we need a smoothing parameter

	bool bretv = (jrscalep->getElapsedTimeSinceLastSignificantWeightChange() > duration);
	
//	Serial.print("ATTN: isWeightStabilized rettting: ");
//	Serial.println(bretv);
	
	return bretv;
}



bool JrWorkflow::isWeightSteadyIncreasing() {
	// we want to ask that weight is in the process of steadily increading (we know this will have a delay in turning on and a delay in turning off, so it's not exact)
	// this is a lot harder than testing for stability of delta weights
	// do we want to be looking at 2nd derivitive (ie rate of change of delta weights)?
	// part of the problem here is that depending on sampling rates things couuld look quite differently
	// when we transition from yes to no with this, we want to be SURE.. so there should be quite a bit of time of no gain before we say NO.. it's ok to say YES for a while after we've stopped increasing
	
	int consecIncreases = jrscalep->getConsecutiveWindowedIncreases();
	/*
	// ATTN: debug
	Serial.print("Consec increase count: ");
	Serial.println(consecIncreases);
	*/
	//
	if (consecIncreases > wfConsecutiveRisingWeightWindowTarget) {
		// yes it is steady increasing!
		return true;
	}
	
	// no it's not
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
float JrWorkflow::wfGetLastPositiveBeanWeight() {
	// here we want the the last positive weight that became stable; even if we later stabilize at a lower weight (which we may be allowed to assume will typically be negative tare value)
	if (lastPositiveBeanWeight > 0)
		return lastPositiveBeanWeight;
	// we didn't find a stabilized weight, so we can return hightest non-stabilized
	float hweight = jrscalep->getHighestTornWeightSinceTare();
	if (hweight>0.0) {
		return hweight;
	}
	return 0.0;
}

unsigned long JrWorkflow::wfGetStartTimeOfSteadyIncrease() {
	// we want the backdated time when we began a steady increase of weight; understanding that this may be be guessable until we've started increasing for some time; must be valid when isWeightSteadyIncreasing()
	return jrscalep->getFirstWindowedWeightIncreaseMillis();
}

unsigned long JrWorkflow::wfGetStopTimeOfSteadyIncrease() {
	// we want the backdates time when we STOPPED steady increase of weight; understanding this may not be guessable until after; only needs to be valid when we switch to !isWeightSteadyIncreasing()
	
	// just resturn time of last increase
	return jrscalep->getTimeOfLastSignificantWeightIncrease();
	
	// we could use this, but this could reset and happen many times before we detect end of shot
	// return jrscalep->getLastWindowedWeightIncreaseMillis();
}
//---------------------------------------------------------------------------








































































//---------------------------------------------------------------------------
void JrWorkflow::scheduleTare(unsigned long delayMillis) {
	if (scheduledTareMillis!=0) {
		// already one scheduled for sooner presumably, so ignore
		//Serial.println("ATTN: asking to schedule a tare but one is already scheduled so ignoreing new request.");
		return;
	}

	// first we perform an INSTANT tare - this might be premature (before its stable) but gives quick feedback
	// ATTN: i have some concern this might mess with workflow auto advance if it creates a bad tare
	if (false) {
		doScheduledTare();
	}

	// and then schedule another after we settle
	//Serial.println("ATTN: trying to Schedule tare for future.");
	if (delayMillis==0) {
		delayMillis = DefTareStabilizeDelayMs;
	}
  scheduledTareMillis = millis() + delayMillis;
}


void JrWorkflow::doScheduledTare() {
  // clear tare timer
  scheduledTareMillis = 0;
  
  // set tare
  internalTare = jrscalep->getUntornWeight();
  
  // all scales share internal tare
  jrscale1.setInternalTare(internalTare);
  jrscale2.setInternalTare(internalTare);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void JrWorkflow::updateActiveScaleChoice() {
	
	// ATTN: test - disable active scale switching
	if (!optionDoActiveScaleSwitching) {
		return;
	}
	
	
	// ok we have our CURRENT active jrscalep and inactive scales iajscalep
	// and we want to, with some hysteresis, decide whether we should switch active scales
	// let's think about strategies
	// easiest would be, make the active one the scale with the heaviest current weight
	//   one problem here would be the oscillating when there is nothing on either scale
	//   another would be that while working on one scale if we lift up object, we will suddenly switch away
	//   another problem would be if we place empty cup on scale 2 and we want to weigh lighter beans on scale 2
	// another consideration is that depending on workflow we may want to switch
	//
	// another natureal strategy is to switch scales when the other scale experiences a dramatic CHANGE (or Increase only?) on a scale that is not a momentary blip
	//
	// approach:
	//   1. Start by always letting workflow FORCE scale choice (this reflects the idea that beans are always on scale1 and espreso on scale2)
	//   2. Where if workflow is ambivelent (at startup in scale mode, or on completion of espresso pour), use strategy B1 (or B2)
	//   3. Strategy B1: Follow whatever scale has the most weight on it, unless we are near 0 on both in which case dont change
	//   4. or Strategy B2 alternate: When a scale experiences a delta (not an outlier blip) more than N [5] grams, switch to it; this should be trackable by scale.getElapsedTimeSinceLastSignificantWeightChange() being small
	//   5. Maybe let user option control strategy B1 vs B2
	//   Q: Should we allow B1 or B2 to override workflow force after an INIITAL switch to workflow preference?
	//       That is, in bean mode of workflow, scale switched to #1,
	//       But user than places bean container on scale 2 -- can we now switch to scale 2?
	//       The problem with this is if user puts empty cup on scale 2, we don't want to switch
	//   A: So my suggestion is that workflow change dictates the bias scale to use (X), which is immediately switched to, and considered the PREEFERRED scale for the workflow
	//      If after that, strategy B1 or B2 dictate a change of scale, allow the scale, AS LONG AS there is 0 weight on the preferred scale; as soon as preferred scale has > 0 weight, switch back to it
	
	// if activescale = preferred scale, and there is any weight on it, then there is nothing to do
	
	bool flagNeverLeavePreferrred = true;

	if (prefscalep != NULL) {
		// there is a preferred scale
		// does it have any weight on it?
		float prefUntornWeight = prefscalep->getUntornWeight();
		if (prefUntornWeight > minWeightForPreferredScaleToDominate) {
			// we want preferred scale to be active scale
			if (prefscalep != jrscalep) {
				// need to switch to preferred scale
				switchToActiveScale(prefscalep);
				return;
			} else {
				// already set to it, nothing to do
			}
			return;
		}
		// there IS a preferred scale, but there is no weight on it, so we might be willing to switch based on strategy B1 or B2
		if (flagNeverLeavePreferrred) {
			return;
		}
	} else {
		// there is no preferred scale, so we are willing to switch based on strategy B1 or B2
	}
	
	int strategyMode = 1;
	//
	if (strategyMode == 1) {
		float weight1 = jrscale1.getUntornWeight();
		float weight2 = jrscale2.getUntornWeight();
		float deltaWeight = weight1 - weight2;

		// switch to heavier weighted scale
		if (deltaWeight > minWeightDeltaForActiveScaleSwitch) {
			// want scale1
			if (jrscalep !=& jrscale1) {
				// switch to scale 1
				switchToActiveScale(&jrscale1);
			}
		} else if (deltaWeight < minWeightDeltaForActiveScaleSwitch*-1.0) {
			// want scale 2
			if (jrscalep !=& jrscale2) {
				// switch to scale 1
				switchToActiveScale(&jrscale2);
			}
		}
	} else if (strategyMode == 2) {
		unsigned long e1 = jrscale1.getElapsedTimeSinceLastSignificantWeightChange();
		unsigned long e2 = jrscale2.getElapsedTimeSinceLastSignificantWeightChange();
		// switch to changing scale
		if (e1 < e2) {
			// want scale1
			if (jrscalep !=& jrscale1) {
				// switch to scale 1
				switchToActiveScale(&jrscale1);
			}
		} else if (e2 > e1) {
			// want scale 2
			if (jrscalep !=& jrscale2) {
				// switch to scale 1
				switchToActiveScale(&jrscale2);
			}
		}
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
void JrWorkflow::switchToActiveScale(JrScale* sp) {
	setActiveAndInactiveScale(sp);
	
}


void JrWorkflow::setPreferredScaleIdForWorkflow() {
	if (workflowMode==JrWorkflowMode_Beans) {
		setPreferredScaleId(DefPreferredScaleBeans);
		return;
	}	else if (workflowMode==JrWorkflowMode_Espresso) {
		setPreferredScaleId(DefPreferredScaleEspresso);
		return;
	}	else if (workflowMode==JrWorkflowMode_All) {
		setPreferredScaleId(DefPreferredScaleAll);
		return;
	}
	// no preference
	setPreferredScaleId(DefPreferredScaleNone);
}


void JrWorkflow::preferredScaleChanges() {
	// called whenever preferred scale is set
	if (prefscalep!=NULL && !prefscalep->get_enabled()) {
		// set to disabled scale pointer
		prefscalep = NULL;
	}
	if (prefscalep!=NULL) {
		setActiveAndInactiveScale(prefscalep);
	}
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
void JrWorkflow::updateCalibratedPlatformRawWeight(int scaleId) {
	JrScale* scalep = getScaleById(scaleId);
	updateCalibratedPlatformRawWeight(scalep);
}

void JrWorkflow::updateCalibrationFactor(int scaleId, float weightOnScaleInGrams) {
	JrScale* scalep = getScaleById(scaleId);
	updateCalibrationFactor(scalep, weightOnScaleInGrams);
}



void JrWorkflow::updateCalibratedPlatformRawWeight(JrScale* scalep) {
	if (!scalep) {
		return;
	}
  float val = calcSmoothedRawScaleWeightForCalibration(scalep);
  scalep->calibratedPlatformRawWeight = val;
  scalep->resetCalibrationTweaks();
}


void JrWorkflow::updateCalibrationFactor(JrScale* scalep, float weightOnScaleInGrams) {
	if (!scalep) {
		return;
	}
  float weight = calcSmoothedRawScaleWeightForCalibration(scalep) * scalep->getCalibrationTweakMultiplicative();
  // subtract calibrated platform weight
  weight -= scalep->calibratedPlatformRawWeight;
  weight += scalep->getCalibrationTweakAdditive();
  // now calibration scale
  scalep->calibrationFactor = weight / weightOnScaleInGrams;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void JrWorkflow::queryScaleUpdateRawWeight(JrScale* scalep) {
	if (!scalep) {
		return;
	}
	scalep->setRawWeight((scalep->scale).get_units());
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void JrWorkflow::doLoopNonMultiScale(JrScale *scalep) {
	if (!scalep) {
		return;
	}
	// we will do the query of the scale hardware, rather than encapsulate in scale class, so that we can handle multiscale system
	queryScaleUpdateRawWeight(scalep);
	// now loop
	scalep->loopScale();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void JrWorkflow::doMultiLoopScales() {
	if (multiscaleMode) {
		// query both scale hx711 multi hardware
		doMultiScaleQueryUpdateRawWeight();
		// now iterate them
		jrscalep->loopScale();
		
		if (!jrOnlyUseOneScale) {
			iajscalep->loopScale();
		}
  } else {
  	// individual scales do their own thing
  	doLoopNonMultiScale(jrscalep);
  	doLoopNonMultiScale(iajscalep);
  }
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void JrWorkflow::doMultiScaleQueryUpdateRawWeight() {
	// query both scales and set their values via scalep->rawWeight=
	long resultData[3];
	
	scaleSet->readRaw(resultData);
	jrscale1.setRawWeight(resultData[0] * -1.0);
	jrscale2.setRawWeight(resultData[1] * -1.0);
	
	if (false) {
		// debugging
		Serial.print("Raw Weights: ");
		Serial.print(resultData[0]);
		Serial.print(" , ");
		Serial.println(resultData[1]);
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void JrWorkflow::scheduleCalibrationTweak(unsigned long delayMillis) {
	wantScheduleCalibrationTweak = false;
	if (delayMillis==0) {
		delayMillis = calStabilizeDelayMs;
	}
  //Serial.print("ATTN: scheduling calibration tweak in: ");
  //Serial.println(delayMillis);
  scheduledCalibrationTweakMillis = millis() + delayMillis;
}


void JrWorkflow::doScheduledCalibrationTweak() {
  //Serial.println("ATTN: setting calibration tweaks");
  scheduledCalibrationTweakMillis = 0;
  
  doSetCalibrationTweakForScale(&jrscale1);
  doSetCalibrationTweakForScale(&jrscale2);
}


void JrWorkflow::doSetCalibrationTweakForScale(JrScale *scalep) {
	// ATTN: this broke 5/28/22 -- attempting to fix
	if (!scalep) {
		return;
	}
	
	if (optionAutoZero==0) {
		scalep->resetCalibrationTweaks();
		return;
	}
	
	
	// IFF the scale has nominal weight on it (near 0), then we are going to tweak the 0 point to be truly 0
	bool isStable = (scalep->getElapsedTimeSinceLastSignificantWeightChange() > wfCalStableWeightElapsedTime);
	//float weight = scalep->getUntornWeight();
	float weight = scalep->getUntornUnTweakedWeight();
	if (!isStable || abs(weight)>wfCalMaxWeightToConsiderZero) {
		// give up and don't reschedule(!)
		if (false || optionDebugLevel>0) {
			Serial.println("Giving up trying to set calibration wake weight tweak.");
		}
		// should we reset if we cant?
		if (false) {
			scalep->resetCalibrationTweaks();
		}
		return;
	}
	scalep->doSetCalibrationTweakForScaleFromRawWeight();
}



void JrWorkflow::doContinuousCalibrationTweakForScaleIfStableSmall(JrScale *scalep) {
	// ATTN: this broke 5/28/22 -- attempting to fix
	if (!scalep) {
		return;
	}

	float weight = scalep->getUntornWeight();
	if (abs(weight)<0.05) {
		// already close enough to 0 no need to tweak
		return;
	}

	// IFF the scale has nominal weight on it (near 0), then we are going to tweak the 0 point to be truly 0
	bool isStable = (scalep->getElapsedTimeSinceLastSignificantWeightChange() > wfCalStableWeightElapsedTime);
	if (!isStable) {
		return;
	}

	float weightut = scalep->getUntornUnTweakedWeight();
	if (abs(weightut)>wfCalMaxWeightToConsiderZeroContinuous) {
		return;
	}
	//Serial.print("untorn untweaked weight: ");
	//Serial.println(weight);
	// tweak it
	scalep->doSetCalibrationTweakForScaleFromRawWeight();
}
//---------------------------------------------------------------------------

















//---------------------------------------------------------------------------
bool JrWorkflow::userStartsWorkflow(const char* inUserName, uint8_t inUserId) {
	// advance or start
	if (strcmp(userName,inUserName)==0) {
		// they were already set on this, so we treat this like an advance, regardless of their current stage of workflow
		return advanceWorkflowMode(true);
	}
	// new user restarts
	strcpy(userName,inUserName);
	userId = inUserId;
	workflowMode = optionStartingWorkflowMode;
	clearCoffeeSelection();
	workflowModeChanges();
	return true;
}


uint8_t JrWorkflow::getUsersLastCoffee() {
	return userLastCoffee[userId];
}

void JrWorkflow::setUsersLastCoffee(uint8_t inCoffeeId, char* inCoffeeLabel, uint8_t ingrindSize) {
	coffeeId = inCoffeeId;
	strcpy(coffeeName, inCoffeeLabel);
	grindSize = ingrindSize;
	//
	userLastCoffee[userId] = inCoffeeId;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool JrWorkflow::smartGuessBeanWeightInJarIfAppropriate() {
	if (workflowWeights[JrWorkflowMode_Beans] > wfMinBeanWeightToAssumeInContainer) {
		// not needed
		return false;
	}

	// minimal bean weight detected when they advanced mode.. let's see if maybe we can guess that they started with beans already in jar instead of taring
	float allWeight = jrscalep->getInternalTare();
	float guessedBeanWeight = allWeight - optionBeanJarWeight;
	if (guessedBeanWeight>wfBeanWeightToAssumeInContainerMin && guessedBeanWeight<wfBeanWeightToAssumeInContainerMax) {
		// ok!
		// let's show it
		//Serial.print("ATTN: In smartguess 1 ");
		jrscalep->setInternalTare(optionBeanJarWeight);
		//Serial.println(guessedBeanWeight);
		workflowWeights[JrWorkflowMode_Beans] = guessedBeanWeight;
		// force show and delay
		updateDisplays(true, true);
		eventCallbackFp(JrEventCallbackEnum_PlaySoundAndWait);
		//delay(500);
		//jrPlaySound(DefSoundTypeMenuMove,false);
		return true;
	}
	
	// fall back on last good weight seen
	workflowWeights[JrWorkflowMode_Beans] = wfGetLastPositiveBeanWeight();
	true;
}
//---------------------------------------------------------------------------































//---------------------------------------------------------------------------
bool JrWorkflow::storeWorkflowedDrink() {
	// record data for the finished workflow drink -- perhaps to sd card, internet, receipt printer, internal stats, etc.
	// workflowWeights[JrWorkflowMode_Beans], workflowWeights[JrWorkflowMode_Espresso], workflowWeights[JrWorkflowMode_All]
	eventCallbackFp(JrEventCallbackEnum_CoffeeDone);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void JrWorkflow::fillFinishedCoffeeInfo(char* username, char* coffeeTypeStr, int &outcoffeeId, int &outgrindSize, float &beanWeight, float &espressoWeight, float &finalWeight, unsigned long &shotTime) {
	strcpy(username, userName);
	strcpy(coffeeTypeStr, coffeeName);
	outcoffeeId = coffeeId;
	outgrindSize = grindSize;
	beanWeight = workflowWeights[JrWorkflowMode_Beans];
	espressoWeight = workflowWeights[JrWorkflowMode_Espresso];
	finalWeight = workflowWeights[JrWorkflowMode_All];
	shotTime = timerElapsedMs;
}
//---------------------------------------------------------------------------
































































//---------------------------------------------------------------------------
// 7 segment standalone i2c displays
void JrWorkflow::setupLcdMatrix(uint8_t addr) {
	lcdMatrix.setup(addr, DefMatrixRotation);
	// initial dim brightness (but not off)
	lcdMatrix.setBrightness(1);
}


// 7 segment standalone i2c displays
void JrWorkflow::setup7Segments(uint8_t addrA, uint8_t addrB) {
	seg7a.setup(addrA);
	seg7b.setup(addrB);
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
void JrWorkflow::updateMatrixImageForWorkflowMode() {
	unsigned char stepStartChar = '0';
	uint8_t offset = JrWorkflowMode_SelectCoffee - optionStartingWorkflowMode;
	//
  if (workflowMode==JrWorkflowMode_None) {
		// shouldnt exit
		if (strcmp(warningMsg,"")==0) {
			lcdMatrix.clear();
		} else {
		lcdMatrix.drawBitmapFrown();
		}
  } else if (workflowMode==JrWorkflowMode_Done) {
		// shouldnt exit
		lcdMatrix.drawBitmapSmiley();
  } else if (workflowMode == optionStartingWorkflowMode) {
	// show letter of name on startup mode
		lcdMatrix.printLetter(userName[0]);
  } else if (workflowMode==JrWorkflowMode_SelectCoffee) {
		lcdMatrix.printLetter(stepStartChar+offset+1);		
	} else if (workflowMode==JrWorkflowMode_Beans) {
		lcdMatrix.printLetter(stepStartChar+offset+2);		
	} else if (workflowMode==JrWorkflowMode_Espresso) {
		lcdMatrix.printLetter(stepStartChar+offset+3);
	} else if (workflowMode==JrWorkflowMode_All) {
		int letterOffset;
		letterOffset = stepStartChar+offset+4;
		letterOffset += currentRecipeStepIndex;
		lcdMatrix.printLetter(letterOffset);
	} else {
		// shouldnt exit
		lcdMatrix.clear();
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void JrWorkflow::checkWarnings() {

	// force refresh
	clearDupeCheckers(false,true,false);

	if (optionCheckWarnings==0) {
		setWarningMessage("");
		return;
	}

	// this is now done elsewhere.. any other warnings to check?
	//setWarningMessage("!LOW WATER!");
	updateWaterLevelIfAppropriate();
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
int JrWorkflow::countRecipes() {
	int recipeCount = 0;
	for (int i=0;i<99;++i) {
		if (strcmp(recipes[i],"")==0) {
			// at end of one
			++recipeCount;
			if (strcmp(recipes[i+1],"")==0) {
				break;
			}
		}
	}
	return recipeCount;
}


int JrWorkflow::getRecipeStartIndex(int recipeIndex) {
	if (recipeIndex==0) {
		return 0;
	}
	int recipeCount = 0;
	for (int i=0;i<99;++i) {
		if (strcmp(recipes[i],"")==0) {
			// at end of one
			++recipeCount;
			if (recipeCount==recipeIndex) {
				return i+1;
			}
			if (strcmp(recipes[i+1],"")==0) {
				break;
			}
		}
	}
	return -1;
}


bool JrWorkflow::parseRecipeStep(const char *str, char* labelout, float &targetWeight) {
	// parse "# label" where # could be float
	char *spacepos = strchr(str,' ');
	if (spacepos == NULL) {
		strcpy(labelout,"");
		targetWeight = 0.0;
		return false;
		}
	// label
	strncpy(labelout, str, (spacepos-str));
	labelout[spacepos-str]='\0';
	//
	// now amount
	char amountstr[10];
	strcpy(amountstr, spacepos+1);
	// amount TYPE (last char of amount is signifier)
	char m = amountstr[strlen(amountstr)-1];
	amountstr[strlen(amountstr)-1] = '\0';
	if (m=='m') {
		// multiply by espresso weight
		ingredientMeasureMode = JrEnumIngredientMeasureMode_MultiplyGramPerGramEspresso;
	} else if (m=='b') {
		// multiply by bean weight
		ingredientMeasureMode = JrEnumIngredientMeasureMode_MultiplyGramPerGramBean;
	} else if (m=='t') {
		// multiply be bean weight as TOTAL
		ingredientMeasureMode = JrEnumIngredientMeasureMode_TotalWeightByGramBean;
	} else {
		Serial.println("ERROR in format of amount, should end in m or t.");
		targetWeight = 0;
		strcpy(labelout,"amounterr");
		return false;
	}
	// amount value
	float amountValRaw = atof(amountstr);
	
	// ok now, we need to convert amountValRaw to targetWeightAddTo
	// so this final value is the target weight of liquid on scale to stop at
	if (ingredientMeasureMode == JrEnumIngredientMeasureMode_MultiplyGramPerGramEspresso) {
		float addWeight = (workflowWeights[JrWorkflowMode_Espresso] * amountValRaw);
		// our target weight should be this addWeight plus the LAST targetweight
		targetWeight = priorRecipeStepTargetWeight + addWeight;
	} else if (ingredientMeasureMode == JrEnumIngredientMeasureMode_MultiplyGramPerGramBean) {
		float addWeight = (workflowWeights[JrWorkflowMode_Beans] * amountValRaw);
		// our target weight should be this addWeight plus the LAST targetweight
		targetWeight = priorRecipeStepTargetWeight + addWeight;
	} else if (ingredientMeasureMode == JrEnumIngredientMeasureMode_TotalWeightByGramBean) {
		// specifies target weight to add to, in terms of bean weight
		targetWeight = workflowWeights[JrWorkflowMode_Beans] * amountValRaw;
	}
	
	return true;
}




void JrWorkflow::dislayRecipeStepInstructions() {
	// we have the top instruction line which should show name of the recipe
	// then we have the current recipe step ingredient, shown on its own line in form of: "INGREDIENT #A of #B"
	// where #A starts at 0 and shows how much added so far
	// and #B is the target amount to add
	// as soon as #A >= #B the ingredient is auto completed and moved on to next ingredient
	// when final ingredient is complete, the final recipe line shoulds say RECIPENAME done
	// and the last ingredient line can remain live? maybe if its a one ingredient recipe otherwise clear it?
	// display instructions
	char str[20];
	if (strcmp(recipeLabel,"")==0) {
		strcpy(str,"");
	} else {
		sprintf(str,"] %s", recipeLabel);
	}
  jrpadstr(str, 20);
  jrlcdp->setCursor ( 0, 0 );            // go to the top left corner
	jrlcdp->print(str);
	//
	displayRecipeIngredientStepInfo();
}





void JrWorkflow::deltaRecipe(int delta) {
	
	// first time they switch to recipe, record updates espresso weight
	if (workflowMode==JrWorkflowMode_All && workflowSubStepIndex==1) {
		// update espresso weight to current weight
		workflowSubStepIndex = 2;
		if (abs(workflowWeights[JrWorkflowMode_All] - workflowWeights[JrWorkflowMode_Espresso]) < wfMaxWeightIncAfterEspressoToUpdateOnRecipe) {
			workflowWeights[JrWorkflowMode_Espresso] = workflowWeights[JrWorkflowMode_All];
		}
	}	
	
	// loop around through -1 (meaning off)
	currentRecipe+=delta;
	if (currentRecipe >= countRecipes()) {
		currentRecipe = -1;
	} else if (currentRecipe<-1) {
		currentRecipe = countRecipes()-1;
	}
	// and set currentRecipeStepIndex when changing
	newRecipeStarts();
}


void JrWorkflow::newRecipeStarts() {
	currentRecipeStepIndex = 0;
	isRecipeIndexDone = false;
	// set prior target weight to current weight
	priorRecipeStepTargetWeight = workflowWeights[JrWorkflowMode_Espresso];
	strcpy(ingredientLabel,"");
	// now update
	recipeChanges();
}


void JrWorkflow::advanceRecipeStep() {
	if (isRecipeIndexDone) {
		// already done nothing more to do
		return;
	}

	++currentRecipeStepIndex;
	int recipeStepStringIndex = getRecipeIngredientIndex(currentRecipe, currentRecipeStepIndex);
	if (recipeStepStringIndex==-1) {
		// all done!
		isRecipeIndexDone = true;
		// ATTN: unfinished
	} else {
		// we have advanced; store current met target as previous
		priorRecipeStepTargetWeight = currentRecipeStepTargetWeight;
	}
	// now update
	recipeChanges();
}





void JrWorkflow::recipeChanges() {
	// update labels
	if (currentRecipe==-1) {
		strcpy(recipeLabel,"");
		strcpy(ingredientLabel,"");
	} else {
		int sindex = getRecipeStartIndex(currentRecipe);
		strcpy(recipeLabel,recipes[sindex]);
		//
		sindex = getRecipeIngredientIndex(currentRecipe, currentRecipeStepIndex);
		if (sindex!=-1) {
			parseAndStoreRecipeIngredientString(recipes[sindex]);
		}
	}
	
	// trigger redisplay
  showWorkflowModeInstructions();
}




void JrWorkflow::clearRecipe() {
	strcpy(recipeLabel,"");
	strcpy(ingredientLabel,"");
	currentRecipe = -1;
	currentRecipeStepIndex = 0;
	isRecipeIndexDone = true;
	recipeChanges();
}


void JrWorkflow::parseAndStoreRecipeIngredientString(const char* buf) {
	bool bretv = parseRecipeStep(buf, ingredientLabel, currentRecipeStepTargetWeight);
}


int JrWorkflow::getRecipeIngredientIndex(int rec, int stepnumber) {
	int spos = getRecipeStartIndex(rec);
	if (spos==-1) {
		return -1;
	}
	++spos;
	for (int i=0;;++i) {
		if (strcmp(recipes[i+spos],"")==0) {
			// hit end
			break;
		}
		if (i==stepnumber) {
			return i+spos;
		}
	}
	
	return -1;
}



void JrWorkflow::displayRecipeIngredientStepInfo() {
	char str[30];
	char fbuf1[10],fbuf2[10];
	// first number is how much we have ADDED of ingredient so far
	// second number is how much we NEED to add (currentRecipeStepTargetWeight-priorRecipeStepTargetWeight)
	float dw = jrscalep->getDisplayWeight();
	float sofar = (dw-priorRecipeStepTargetWeight);
	if (sofar<0 && sofar>-2.0) {
		// kludge to prevent neg dips
		sofar=0.0;
	}
	
	//jrFloatToStr(fbuf1, sofar, 1, 0);
	//jrFloatToStr(fbuf2, (currentRecipeStepTargetWeight-priorRecipeStepTargetWeight), 1, 0);
	jrFloatToStr(fbuf1, (currentRecipeStepTargetWeight-priorRecipeStepTargetWeight), 1, 0);
	jrFloatToStr(fbuf2, currentRecipeStepTargetWeight, 1, 0);
	char suffix[5];
	if (isRecipeIndexDone) {
		strcpy(suffix,"!");
	} else {
		strcpy(suffix,"");
	}
	sprintf(str,"Add %s: %s to %s%s", ingredientLabel, fbuf1, fbuf2, suffix);
	if (strlen(str)>20) {
		sprintf(str,"+%s: %s to %s%s", ingredientLabel, fbuf1, fbuf2, suffix);
	}
	
  jrpadstr(str, 20);
  jrlcdp->setCursor ( 0, 2 );
	jrlcdp->print(str);
	
	// update workflow display matrix image
	updateMatrixImageForWorkflowMode();
}


void JrWorkflow::autoAdvanceRecipeIngredientStepIfAppropriate() {
	if (isRecipeIndexDone) {
		// already done nothing more to do
		return;
	}
	float dw = jrscalep->getDisplayWeight();
	if (dw >= currentRecipeStepTargetWeight) {
		// we hit target!
		advanceRecipeStep();
		// play sound
		eventCallbackFp(JrEventCallbackEnum_PlayBackgroundSound);
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool JrWorkflow::workflowSafeToAdvance() {
	
	if (workflowMode == JrWorkflowMode_Beans) {
		//Serial.print("Checking bean weight: ");
		//Serial.print(workflowWeights[JrWorkflowMode_Beans]);
		if (workflowWeights[JrWorkflowMode_Beans] > wfMinBeanWeightToAssumeInContainer) {
			// not needed
			//Serial.println("Passed first check ok.");
			return true;
		}

		// minimal bean weight detected when they advanced mode.. let's see if maybe we can guess that they started with beans already in jar instead of taring
		// this is from smartGuessBeanWeightInJarIfAppropriate
		float allWeight = jrscalep->getInternalTare();
		float guessedBeanWeight = allWeight - optionBeanJarWeight;
		//Serial.print(guessedBeanWeight);
		if (guessedBeanWeight>wfBeanWeightToAssumeInContainerMin && guessedBeanWeight<wfBeanWeightToAssumeInContainerMax) {
			// ok!
			//Serial.println("Passed 2nd check ok.");
			return true;
		}
	// no good
	//Serial.println("Failed final check.");
	return false;
	}
	
	if (workflowMode == JrWorkflowMode_Espresso) {
		if (workflowWeights[JrWorkflowMode_Espresso]>wfBeanWeightToAssumeInContainerMin) {
			return true;
		}
	// no espresso?
	return false;
	}
	
	// other steps have no limit
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool JrWorkflow::isCupOnEspressoScale() {
	JrScale* scalep = getScaleById(DefPreferredScaleEspresso);
	float weight = scalep->getUntornWeight();
	//Serial.print("Checking espresso cup: ");
	//Serial.println(weight);
	if (weight>wfMinCupWeightForEspressoTare) {
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
bool JrWorkflow::goBackwardsStep() {
	// return false if we cant go back, true if we did
	bool bretv = false;
	if (workflowMode==JrWorkflowMode_Beans) {
		// how can we go back? its just like an exit
		//clearCoffeeSelection();
		workflowMode = JrWorkflowMode_SelectCoffee;
		doClearTare();
		bretv = true;
	} else if (workflowMode==JrWorkflowMode_Espresso) {
		// straigtforward case?
		workflowMode=JrWorkflowMode_Beans;
		doClearTare();
		timerOff();
		bretv = true;
	} else if (workflowMode==JrWorkflowMode_All) {
		workflowMode=JrWorkflowMode_Espresso;
		timerOff();		
		bretv = true;		
	} else if (workflowMode==JrWorkflowMode_Done) {
		// strightforward case
		workflowMode=JrWorkflowMode_All;
		bretv = true;
	}
	if (bretv) {
		if (workflowMode==JrWorkflowMode_SelectCoffee) {
			// we need caller to do this
			eventCallbackFp(JrEventCallbackEnum_CoffeeSelect);
		} else {
			workflowModeChanges();
		}
	}
	return bretv;
}
//---------------------------------------------------------------------------




































//---------------------------------------------------------------------------
/*
// ATTN: right now this is redundant ugly code from jrscalep; unify this please
float JrWorkflow::calcUntornWeightOnScale(JrScale* scalep) {
	if (!scalep) {
		return 0.0;
	}
	// update calm settled raw weight (we can ignore the returned value; it will set untorn weight on scale)
	calcSmoothedRawScaleWeightForCalibration(scalep);
  // and now as an untorn weight
  return scalep->getUntornWeight();
}
*/
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
// this will delay for a bit as it gathers a calm settled RAW weight
float JrWorkflow::calcSmoothedRawScaleWeightForCalibration(JrScale* scalep) {
	if (!scalep || !scalep->enabled) {
		return 0.0;
	}
	
	// settle and update measurements from scale
	settleMeasureScaleWeight(scalep);
	
	// and now take the last value we computed, which should have whatever smoothing method we've decided on
	return scalep->getRawWeightSmoothed();
}


void JrWorkflow::settleMeasureScaleWeight(JrScale* scalep) {
	// NOTE we used to do raw querying of scale weight here, but now we are trying to unify the smoothing of raw weight values, etc.
	// so run weight updates for some period of time
	unsigned long finishTime = millis() + DefNewCalibrationPeriodMs;
	while (millis()<finishTime) {
		doMultiLoopScales();
	}
}
//---------------------------------------------------------------------------



	
	






//---------------------------------------------------------------------------
void JrWorkflow::setWaterLevelLimits(float min, float max, float lowThreshold) {
	waterLevelmin = min;
	waterLevelmax = max;
	waterLevelLowThreshold = lowThreshold;
}


void JrWorkflow::displayWaterLevelGraphic() {
  bool flagWarn = (waterLevel < waterLevelLowThreshold);

  lcdMatrix.drawLevel(waterLevel, waterLevelmin, waterLevelmax, optionCheckWarnings, flagWarn);
  
  // ATTN: TEST
  if (false) {
	Serial.print("ATTN: water level: ");
	Serial.println(waterLevel);
  }
}
//---------------------------------------------------------------------------

	
	
//---------------------------------------------------------------------------
void JrWorkflow::updateWaterLevelOccasionally() {
  if (optionCheckWarnings==0) {
	  // they have disabled water check
	  return;
  }

	unsigned long updateInterval = DefWaterLevelUpdateFrequencyNormal;

	if (waterCheckCycleCountdown>0) {
		updateInterval = DefWaterLevelUpdateFrequencyVeryFastWithinCycle;
	} else if (waterLevel < waterLevelLowThreshold) {
		updateInterval = DefWaterLevelUpdateFrequencyLowWater;
	} else if (workflowMode != JrWorkflowMode_None) {
		// only update in non-workflow mode
		updateInterval = DefWaterLevelUpdateFrequencyWorkflow;
	}
	
	if (millis()-waterLevelLastUpdateTime > updateInterval) {
		// update water
		updateWaterLevel();
		// either decrease cycle count or reset it
		if (waterCheckCycleCountdown>0) {
			--waterCheckCycleCountdown;
		} else {
			// first call of new cycle window -- this will happen only if we FINISH a window and THEN a new interval passed and new window starts
			if (updateInterval>DefWaterLevelUpdateFrequencyVeryFastWithinCycle) {
				// this test should not be needed
				waterCheckCycleCountdown = DefWaterCheckCyclesPerWindow;
			}
		}
  }

}


void JrWorkflow::updateWaterLevelIfAppropriate() {
  if (optionCheckWarnings==0) {
	  // they have disabled water check
	  return;
  }
  startWaterLevelCheckingCycle();
  updateWaterLevel();
}


void JrWorkflow::startWaterLevelCheckingCycle() {
	waterCheckCycleCountdown = DefWaterCheckCyclesPerWindow;
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
void JrWorkflow::setupRangefinder(JrRangeFinder* injrRangeFinderp) {
	jrRangeFinderp = injrRangeFinderp;
	jrRangeFinderp->begin();
};


void JrWorkflow::updateWaterLevel() {
	// update time
	waterLevelLastUpdateTime = millis();

	// update value from sensor
	// set value to -1 to say NOTHING TO DISPLAY (vs 0 for empty water)
	jrRangeFinderp->loop();

	// get water level
	waterLevel = 1.0 - jrRangeFinderp->getSmoothedRangeNormalized();


  if (waterLevel < waterLevelLowThreshold) {
	  setWarningMessage("!LOW WATER!");
  } else {
	  setWarningMessage("");
  }

	// now display
  if (workflowMode == JrWorkflowMode_None || waterLevel < waterLevelLowThreshold) {
  	// display water level
  	displayWaterLevelGraphic();
  }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
unsigned long JrWorkflow::wokeTime() {
	return millis()-lastWokeTime;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
JrScale* JrWorkflow::getMostWeightedScalep() {
	float w1 = jrscale1.getUntornWeight();
	float w2 = jrscale2.getUntornWeight();
	float delta = w1-w2;
	if (delta>DefWeightDeltaThresholdMostWeightedScale) {
		return &jrscale1;
	} else if (delta<-1*DefWeightDeltaThresholdMostWeightedScale) {
		return &jrscale2;
	}
	return NULL;
}
//---------------------------------------------------------------------------
