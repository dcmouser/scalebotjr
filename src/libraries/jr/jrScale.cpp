// header
#include "jrScale.h"

// others
#include <jrutils.h>


// F() functions
#include <avr/pgmspace.h>




//---------------------------------------------------------------------------
JrScale::JrScale() {
	
	// init tracking
	resetLastChangeTimes();
	//
	// note this will be set my workflow based on mode, normally high except when watching for espresso which shakes the machine
	setWeightChangeSensitivity(JrWeightChangeSensitivityMode_High);
}
//---------------------------------------------------------------------------







































//---------------------------------------------------------------------------
void JrScale::setupScale(int doutPin, int sckPin) {
  // scale stuff
  scale.begin(doutPin, sckPin);
  enabled = true;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
void JrScale::loopScale() {
  // get current weight
  if (!enabled) {
  	return;
  }
  
  // we have rawWeight, which is directly from scale and was set by our caller
  // from that we have our configurable sophisticated smoothing algorithm which gives us rawWeightSmoothed
  // from rawWeightSmoothed we can use our calibration factors and tweaks to get untornWeight (smoothed)
  // and from that tornWeight (smoothed)
  // and finally displayweight (smoothed and massaged)
  // REMINDER: turnweight, untornweight, and displayweight only exist in smoothed form based on rawWeightSmoothed

  // from rawWeight get a smoothed more slowly changing rawWeightSmoothed using our algorithm of choice
  applySmoothRawWeightAlgorithm();
  
  // compute torn and untorn weights from raw weights
  computeTornUntornWeightFromRawWeightSmoothed();

  // clear any newtare flag checked in updateWeightDisplayCalcAndAveraging
  newTare = false;

	// save last displayWeight
	float lastDisplayWeight = displayWeight;

	// display weight defaults to torn weight
  displayWeight = tornWeight;
  
  // now any tweaks on that
  // how much is this new value a change from the last DISPLAY
  float thisChangeInDisplayWeight = displayWeight - lastDisplayWeight;
  
  // rounding to 0.5 resolution
  if (optionRoundUp || optionRoundUpForce) {
  	long val = displayWeight*2.0;
  	displayWeight = val / 2.0;
  }
  
	// tweaks
  if (optionSoftZero>0) {
	  if (displayWeight < thresholdHighestWeightAsZero && displayWeight > thresholdLowestWeightAsZero) {
		// close enough to 0 to report as 0
		displayWeight = 0.0;
	  } else if (optionHideSmallChanges && thisChangeInDisplayWeight < 0 && thisChangeInDisplayWeight > weightBackwardsHideThreshold) {
		// if its too small a backward move, don't report it -- this is for user comfort
		// refuse to update
		displayWeight = lastDisplayWeight;
	  } else if (optionHideSmallChanges && thisChangeInDisplayWeight > 0 && thisChangeInDisplayWeight < weightForwardsHideThreshold) {
		// if its too small a backward move, don't report it -- this is for user comfort
		// refuse to update
		displayWeight = lastDisplayWeight;
	  }
  } else {
  	// even if optionSoftZero is not set, we dont want -0 shown
		if (displayWeight < 0 && displayWeight > -0.1) {
			// close enough to 0 to report as 0
			displayWeight = 0.0;
	  	}
  }

  if (false) {
  	Serial.print("reading from scale ");
  	Serial.print(tornWeight);
  	Serial.print(" shown as ");
  	Serial.println(displayWeight);
  }
  
  if (displayWeight < -999) {
  	displayWeight = -999;
  } else if (displayWeight > 999) {
  	displayWeight = 999;
  }
  
  // track changes for workflow
  doTrackingUpdate();
}




void JrScale::selfQueryScaleUpdateRawWeight() {
	// not used in workflow code
	rawWeight = scale.get_units();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void JrScale::triggerDelaySleepCallback() {
	// trigger the callback to say the weight change has been significant enough to count as an interaction that should delay the next sleep
	if (eventCallbackFp) {
		eventCallbackFp(JrEventCallbackEnum_Interaction);
	}
}
//---------------------------------------------------------------------------

















































//---------------------------------------------------------------------------
void JrScale::tareChangesUpdate() {
	// recompute torn weight
	computeTornUntornWeightFromRawWeightSmoothed();
	// tracking resets
	tornWeightForTracking = tornWeight;
	windowedTornWeightForTracking = tornWeight;
	clearHighestTornWeightSinceTare();
	// set this which says to restart any smoothing windows etc
	newTare = true;
}


void JrScale::clearHighestTornWeightSinceTare() {
	highestTornWeightSinceTare=tornWeight;
}

void JrScale::setInternalTare(float val) {
	
	if (val<DefMinimalTareToConvertToInternalAdditiveFix) {
		// when a tare is this tiny, we assume its a scale 0 calibration fix
		internalTare=0;
		doSetCalibrationTweakForScaleFromRawWeight();
		// this will be called by above func
		// tareChangesUpdate();
		return;
	}
	
  internalTare = val;
  tareChangesUpdate();
}


void JrScale::doClearTare() {
  // reset last reported weight so it can be compared to new weight in updateWeightAveraging(); so this should now be 0
	internalTare=0.0;
	tareChangesUpdate();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void JrScale::doTrackingUpdate() {
	// this is used for workflow weight tracking (ie to test when coffee is pouring, etc.)

  // tracking of last time weight increased or decreased.. this is used for stability stuff
	unsigned long ms = millis();

  // see call below to getElapsedTimeSinceLastSignificantWeightChange() from jrworkflow
  float trackingDelta = tornWeight-tornWeightForTracking;
  if (trackingDelta > significantWeightChangeForTimeTrackingUp) {
  	// reset last times
  	timeOfLastSignificantWeightIncrease = ms;
  	// update
  	tornWeightForTracking = tornWeight;
  } else if (trackingDelta < significantWeightChangeForTimeTrackingDown) {
		timeOfLastSignificantWeightDecrease = ms;
		// update
		tornWeightForTracking = tornWeight;
  }
  
  // update for next time
  // ATTN: do we really want to update this EVEN if we fail to achieve delta? that would mean a slow rise or fall under significantWeightChangeForTimeTrackingUp would not be detected
  // I think what we really need to do is reset this only when we go up or down
  //displayWeightForTracking = computedVisibleWeight;
  //
  // now a more advanced case to help us track when there is a steady rising of weight over time
  // the end result of this is the value trackingRisingWeightConsecutives, which tells us how many (requiredRisingWindowTooCloseDurationMs) consecutive windows of time the weight has been increaseing
  trackingDelta = tornWeight - windowedTornWeightForTracking;
  unsigned long trackingDeltaTime = ms - lastWindowedWeightIncreaseMillis;
  if (trackingDelta > significantWeightIncreaseForWindowedTimeTrackingUp) {
  	// it's a significant increase, let's see if we should increase our counter
  	if (trackingDeltaTime > requiredRisingWindowTooCloseDurationMs) {
  		// enough time has passed that we do want to consider this another consecutive rising event.
  		if (trackingRisingWeightConsecutives < 1000) {
  			++trackingRisingWeightConsecutives;
  			if (trackingRisingWeightConsecutives==1) {
  				// first time increasing in this series, record this time
  				firstWindowedWeightIncreaseMillis = ms;
  			}
  		}
  		// record the time of this event for the next check
  		lastWindowedWeightIncreaseMillis = ms;
  		windowedTornWeightForTracking = tornWeight;
  	} else {
  		// it's gone up enough, but not enough time has passed, do nothing
  	}
  } else {
  	// hasn't gone up enough.. but
  	// has enough time passed for us to consider this a failure to keep increasing in the window
  	if (trackingRisingWeightConsecutives>0) {
  		// we are tracking some consecutive increases
  		// ATTN: tracking a decrease in weight is problematic, possibly due to spikes
  		if (trackingDeltaTime > requiredRisingWindowTooLongFailDurationMs || (false && trackingDelta < significantWeightDecreaseorWindowedTimeTrackingDown)) {
  			// yes too long as passed (or its too big a drop) since we had a rising event so we consider it no longer rising; so we reset our count
  			trackingRisingWeightConsecutives = 0;
  			}
  	} else {
  		// we arent tracking any increases so we should reset our starting point for next time
  		windowedTornWeightForTracking = min(tornWeight,windowedTornWeightForTracking);
  	}
  }
  
  // track highest weight seen since tare
  highestTornWeightSinceTare = max(highestTornWeightSinceTare, tornWeight);
}




unsigned long JrScale::getElapsedTimeSinceLastSignificantWeightChange() {
	// so we want the shortest time since last change up or down
	unsigned long ms = millis();
	return min(ms-timeOfLastSignificantWeightIncrease, ms-timeOfLastSignificantWeightDecrease);
}


int JrScale::getConsecutiveWindowedIncreases() {
	return trackingRisingWeightConsecutives;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void JrScale::setWeightChangeSensitivity(JrWeightChangeSensitivityMode smode) {
	if (smode == JrWeightChangeSensitivityMode_High) {
		significantWeightChangeForTimeTrackingUp = significantWeightChangeForTimeTrackingUp_High;
		significantWeightChangeForTimeTrackingDown = significantWeightChangeForTimeTrackingDown_High;	
	} else if (smode == JrWeightChangeSensitivityMode_Low) {
		significantWeightChangeForTimeTrackingUp = significantWeightChangeForTimeTrackingUp_Low;
		significantWeightChangeForTimeTrackingDown = significantWeightChangeForTimeTrackingDown_Low;
	} else if (smode == JrWeightChangeSensitivityMode_VeryHigh) {
		significantWeightChangeForTimeTrackingUp = significantWeightChangeForTimeTrackingUp_VeryHigh;
		significantWeightChangeForTimeTrackingDown = significantWeightChangeForTimeTrackingDown_VeryHigh;
	}
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
void JrScale::doSetCalibrationTweakForScaleFromRawWeight() {
	// ok so a minimal weight has been stable for enough time, we are going to do a calibration tweak which is very similar to an internal tare
	// essentially this is a value to add to the pre-calibrated platform weight
	// it will account for very small drifts at startup maybe temp differences
	// though note that this is an additive tweak, one could ALSO make an assumption about a multiplicative adjustment (temp related?) 
	
	float rawWeightToMakeZero = getRawWeightSmoothed();
	
	// additive
	if (optionCalibrationTweakMethod == JrCalibrationTweakMethod_Additive) {
		float calAdditiveTweak = getCalibratedPlatformRawWeight() - rawWeightToMakeZero;
		setCalibrationTweakAdditive(calAdditiveTweak);
		setCalibrationTweakMultiplicative(1.0);
	} else if (optionCalibrationTweakMethod == JrCalibrationTweakMethod_Multiplicative && abs(rawWeightToMakeZero)>0.001) {
		float calMultiplicativeTweak = getCalibratedPlatformRawWeight() / rawWeightToMakeZero;
		setCalibrationTweakAdditive(0.0);
		setCalibrationTweakMultiplicative(calMultiplicativeTweak);
	} else {
		// none
		resetCalibrationTweaks();
	}

	// recompute untorn weight etc
	tareChangesUpdate();
	
	if (optionDebugLevel>0) {
		Serial.print("ATTN: Additive tweak computed as: ");
		Serial.println(calibrationTweakAdditive);
		Serial.print("ATTN: Multiplicative tweak computed as: ");
		Serial.print(getCalibrationTweakMultiplicativeForDisplay());
		Serial.println(" from raw weight of ");
		Serial.print(rawWeightToMakeZero);
		Serial.print(" and stored platform weight of ");
		Serial.println(getCalibratedPlatformRawWeight());
	}
}
//---------------------------------------------------------------------------



































//---------------------------------------------------------------------------
void JrScale::applySmoothRawWeightAlgorithm() {

  // this is called AFTER rawWeight is updated, but we track lastRawWeight where it is set; however we will track smoothed raw weight at top of this function
	if (newTare) {
		lastRawWeight = rawWeight;
		lastRawWeightSmoothed = rawWeight;
	} else {
		lastRawWeightSmoothed = rawWeightSmoothed;
	}

  // difference between immediate raw weight obervation and our last SMOOTHED value
  float deltaRawFromLastSmoothedRawWeight = rawWeight-lastRawWeightSmoothed;

  // keep track of how long between updates for stats reporting
  timeLastWeightIncorporate = timeThisWeightIncorporate;
  timeThisWeightIncorporate = millis();
  unsigned long deltaTimeIncorporate = timeThisWeightIncorporate - timeLastWeightIncorporate;


	if (weightSmoothMode==JrWeightSmoothingModeGap) {
		if (newTare) {
			// just use current values
			rawWeightSmoothed = rawWeight;
			windPullCountLower	= 0;
			windPullCountHigher = 0;
			windPullCountSame = 0;	
		} else {
			// get dif between reading and our currently reported value
			if (deltaRawFromLastSmoothedRawWeight<0.0) {
				if (windPullCountLower<DefModeGapMaxDirectionCount) {
					++windPullCountLower;
				} else {
					if (windPullCountHigher>0) {
						--windPullCountHigher;
					}
					if (windPullCountSame>0) {
						--windPullCountSame;
					}
				}
				if (rawWeight > windowedHighestLowerWeight || windPullCountLower==1) {
					windowedHighestLowerWeight = rawWeight;
				}
			} else if (deltaRawFromLastSmoothedRawWeight>0.0) {
				if (windPullCountHigher<DefModeGapMaxDirectionCount) {
					++windPullCountHigher;
				} else {
					if (windPullCountLower>0) {
						--windPullCountLower;
					}
					if (windPullCountSame>0) {
						--windPullCountSame;
					}
				}
				if (rawWeight < windowedLowestHigherWeight || windPullCountHigher==1) {
					windowedLowestHigherWeight = rawWeight;
				}
			} else {
				// unchanged
				if (windPullCountHigher<DefModeGapMaxDirectionCount) {
					++windPullCountSame;
				} else {
					if (windPullCountLower>0) {
						--windPullCountLower;
					}
					if (windPullCountHigher>0) {
						--windPullCountHigher;
					}
				}
			}
			//
			bool triggerWeightChange=false;
			float newWeight;
			if (windPullCountLower-windPullCountHigher>DefModeGapCountTrigger && windPullCountLower-windPullCountSame>DefModeGapCountTrigger) {
				// enough lower to trigger
				triggerWeightChange = true;
				newWeight = windowedHighestLowerWeight;
			}
			else if (windPullCountHigher-windPullCountLower>DefModeGapCountTrigger && windPullCountHigher-windPullCountSame>DefModeGapCountTrigger) {
				// enough higher to trigger
				triggerWeightChange = true;
				newWeight = windowedLowestHigherWeight;
			} else {
				//nothing
			}
			if (triggerWeightChange) {
				// adjust weight
				rawWeightSmoothed = newWeight;
				// reset
				windPullCountLower	= 0;
				windPullCountHigher = 0;
				windPullCountSame = 0;
			}
		}
	}
  
  // other algorithms
  
  else if (weightSmoothMode == JrWeightSmoothingModeInstant) {
  	// simplest most noisy way
  	rawWeightSmoothed = rawWeight;
  }
  
  else if (weightSmoothMode == JrWeightSmoothingModeSlow) {
  	// very slow changing, we blend new values into old (weightAveragingFactorSlow will be >0.90)
  	rawWeightSmoothed = (DefAvgSmoothingModeWeightAveragingFactorVerySlow * rawWeightSmoothed) + ((1.0-DefAvgSmoothingModeWeightAveragingFactorVerySlow) * rawWeight);
  }
  
  else {
  	// error
  	rawWeightSmoothed = rawWeight;
  }

  return;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
void JrScale::computeTornUntornWeightFromRawWeightSmoothed() {
	if (!enabled) {
		return;
	}
	
	// note these are all based on rawWeightSmoothed (we only care about smoothed for everything other than initial raw weight observation)
	
	// compute untorn weight
  untornWeight = rawWeightSmoothed * calibrationTweakMultiplicative;
  // calibration stuff
  // subtract calibrated platform weight
  untornWeight -= calibratedPlatformRawWeight;
  // calibration tweaks based on startup values
  untornWeight += calibrationTweakAdditive;
  // now scale it as per calibration
  untornWeight = untornWeight / calibrationFactor;

  // compute untorn weight
  untornUntweakedWeight = rawWeightSmoothed;
  // subtract calibrated platform weight
  untornUntweakedWeight -= calibratedPlatformRawWeight;
  // now scale it as per calibration
  untornUntweakedWeight = untornUntweakedWeight / calibrationFactor;

  // tornweight
  tornWeight = untornWeight - internalTare;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void JrScale::resetLastChangeTimes() {
	unsigned long ms = millis();
	timeOfLastSignificantWeightIncrease=ms;
	timeOfLastSignificantWeightDecrease=ms;
}
//---------------------------------------------------------------------------

