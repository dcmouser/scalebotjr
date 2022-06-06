


//---------------------------------------------------------------------------
void loopGenericPre() {
  if (flagWantsSleep) {
    sendDeviceToSleep();
  }
  
  // if we sleep above, we will be suspended.. and will resume here:
  if (wakingCycleCount>0) {
    --wakingCycleCount;
  }

  // just waking up from sleep?
  if (needsWakeupFuncRun) {
    doWakeUpStuff();
  }

  // buzzer helper loop (this used to be at BOTTOM of this func)
  EasyBuzzer.update();
}


void loopGenericPost() {
  // check buttons and keypad presses
  if (!JrSkipControllerLoop) {
    loopButtonsAndKeypad();
  }

  // go to sleep if it's been too long
  sleepIfNoInteractionsForLongTime();
}
//---------------------------------------------------------------------------
