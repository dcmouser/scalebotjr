
// more info on using pin as interrupt to wake from sleep:
// https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
// for UNO we used pin 2
// THIS FAILS for mega; we need to use a different pin (18)
// doesn't seem to be working for mega..
//
// see https://github.com/rocketscream/Low-Power
// see https://forum.arduino.cc/t/problem-with-sleep-mode-interrupt-working-only-with-low-mode-solved/559727


#include <jrutils.h>







//---------------------------------------------------------------------------
void sendDeviceToSleep() {
  // send system and devices to sleep

  // clear this flag
  flagWantsSleep = false;

  // show sleep message
  showSleepOnScreen(false);

  jrworkflow.prepareForSleepEarly();

  // dont need this, now we can let the net shutdown cause this delay
  if (false) {
    // sleep for a little bit to give time for screen to show
    delay(500);
  }

  // net helper board
  jrNetGoToSleep();

  // important, make sure button is released before sleeping, other wise it will wake immediately
  jrkeypad.prepareForSleep();

  // send scale to sleep (note if you try to read scale when it is down it will HANG!)
  jrworkflow.powerDown();

  //lcd.setCursor ( 0, 3 );            // go to the fourth row
  jrlcd.noBacklight(); // turn off backlight

  // lcds off, etc.
  jrworkflow.prepareForSleepLate();

  // interrupt for waking
  enableWakeInterrupt();

  // ATTN: we set these AFTER we enableWakeInterrupt since there appears to be some kind of bug triggering it as soon as we set it, or some floating pin signal or something weird.. (see https://forum.arduino.cc/t/interrupt-triggering-by-itself/49000)
  sleepingMode = true;
  needsWakeupFuncRun = true;
  wakingCycleCount = DefWakingCycleCountHigh;

  // system board sleep
  // Enter power down state with ADC and BOD module disabled.
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  // ??
  // ATTN: trying to disable 4/29/22 for mega, doesn't help, but it might break UNO commenting this?
  //disableWakeInterrupt();
}




void wakeupDevice() { 

  // clear this flag
  flagWantsSleep = false;

  // try to catch the key being pressed
  jrkeypad.justTick();
  
  // ATTN: 5/6/22 putting this here
  sleepingMode = false;
  
  //Serial.println("Attn: in wakeupDevice.");
  //Serial.flush();

  // turn off interrupt
  disableWakeInterrupt();

  // show sleep message
  showSleepOnScreen(true);

  // lcd
  jrlcd.backlight();  //open the backlight 

  // sound before manually going to sleep
  jrPlaySound(DefSoundTypeMainFunc, true);

  // important, make sure button is released before sleeping, other wise it will re-sleep immediately
  bool flagWaitForNoKeys = false;
  jrkeypad.prepareForWake(flagWaitForNoKeys);
  
  // scale stuff
  jrworkflow.powerUp();

  // resume from sleep
  jrworkflow.resumeFromSleepOrStart(optionStartMode);

  // lcds, etc.
  //jrworkflow.prepareForWake();
  
  // net helper board
  jrNetWakeFromSleep();

  // swallow any pending keypresses and wait for all keys up
  // ATTN: i think doing this above in prepareForWake is enough..
  //  jrkeypad.waitForNoKeysPressed();

  // mode
  sleepingMode = false;
  justWokeUp = true;

  // reset display
  doResetDisplayForMode(false);

  // reset next wake ime
  setNextSleepTimeOnWakeup(2);

  // we are now ready
  doReadyStuff("wakeup");
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void enableWakeInterrupt() {
  // setup interrupt on sleep pin
  if (optionDebugLevel>2) {
    char buf[80];
    sprintf(buf,"setting wake interrupt on pin %d (interrupt %d).", buttonPinInterrupt, digitalPinToInterrupt(buttonPinInterrupt));
    Serial.println(buf);
    delay(500);
  }
  //
  attachInterrupt(digitalPinToInterrupt(buttonPinInterrupt), triggerWakeInterrupt, buttonInterruptState);
}


void disableWakeInterrupt() {
  // remove interrupt pin
  detachInterrupt(digitalPinToInterrupt(buttonPinInterrupt));
}


void triggerWakeInterrupt() {
  if (!sleepingMode) {
    // spurious wakeup trigger interrupt
    if (optionDebugLevel>2) {
      Serial.println(F("intq"));
    }
    return;
  }
  // ATTN: let's try to clear this now
  sleepingMode = false;
  
  if (optionDebugLevel>2) {
    Serial.println(F("INTQ"));
  }
  
  // wake up!
  // code here is not actually executed? we set flag so on loop() resume it will wake up
  needsWakeupFuncRun = true;
}

void doWakeUpStuff() {
  needsWakeupFuncRun = false;
  // turn off sleep mode
  wakeupDevice();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void updateNextSleepTimeOnInteract() {
  if (optionSleepDelay<1) {
    nextSleepTimeOnNonInteraction = 0;
  } else {
    nextSleepTimeOnNonInteraction = max(nextSleepTimeOnNonInteraction, millis() + (optionSleepDelay * 60000L));
    //nextSleepTimeOnNonInteraction = millis() + (optionSleepDelay * 60000L);

    if (optionDebugLevel>1) {
      debugPrintln("ATTN: resetting sleep in updateNextSleepTimeOnInteract.");
    }
  }
}



void sleepIfNoInteractionsForLongTime() {
  if (nextSleepTimeOnNonInteraction!=0) {
    unsigned long m = millis();
    //
    if (optionDebugLevel>1) {
      static int dcounter = 0;
      if (++dcounter>debugDelayModifier) {
        dcounter = 0;
        //
        // ATTN: 5/11/22
        // ATTN: there is some memory fault or similar bug causing this code to break things all over
        //
        char bufd[80];
        char floatbuf[10];
        jrFloatToStr(floatbuf, (nextSleepTimeOnNonInteraction-m) / 1000.0, 5,2);
        //sprintf(floatbuf,"%d",RAMEND - SP);
        //sprintf(floatbuf,"%d",JrFreemem());
        sprintf(bufd,"ATTN: sec to sleep = %s (%d min)",floatbuf,optionSleepDelay);
        Serial.println(bufd);
      }
    }
    //
    if (m>nextSleepTimeOnNonInteraction) {
      // set flag saying to go to sleep on next loop
      flagWantsSleep = true;
    }
  }
}


void setNextSleepTimeOnWakeup(unsigned long multiplier) {
  // we can treat this just like a next sleep interact, or we can make it a little longer on manual start
  if (optionSleepDelay<1) {
    nextSleepTimeOnNonInteraction = 0;
  } else {
    // NOTE: this doesnt really work, because it will be overridden with normal sleep time when weight sleep reset triggers
    nextSleepTimeOnNonInteraction = millis() + (optionSleepDelay * 60000L * multiplier);
  }
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void showSleepOnScreen(bool isWaking) {
  jrlcd.clear();
  jrlcd.setCursor ( 0, 0 );            // go to the top left corner
  char title[24];
  sprintf_P(title,PSTR("  %S v%S"), F(DefAppLabelStr), F(DefAppVersionStr));
  jrlcd.print(title);
  jrlcd.setCursor ( 0, 2 );
  if (isWaking) {
    jrlcd.print("  ---- waking ----  ");
  } else {
    jrlcd.print("  --- sleeping ---  ");
  }
}
//---------------------------------------------------------------------------
