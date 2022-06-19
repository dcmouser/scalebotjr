//---------------------------------------------------------------------------
// app stuff for about box and info
#define DefAppLabelStr "ScaleBot Jr"
#define DefAppVersionStr "3.49"
#define DefAppDateStr "6/17/22"
#define DefAppAuthorStr "J. Reichler"
#define DefAppEmailStr "mouser@donationcoder.com"

// for our jreprom helper saving settings to eeprom memory
#define JrEpromAppSentinelId 01
#define JrEpromWriteVersion 6
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include "genericdefs.h"
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// for sleep support
#include <LowPower.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// button debouncing
#include <jrButton.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// for rotary encoder
#include <jrRotaryEncoder.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// for buzzer
#include <EasyBuzzer.h>
extern EasyBuzzerClass EasyBuzzer;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// our header stuff
//

#include "pindefs.h"
//
// include globals into global space
#include "globals.h"
//
#include "lcddefs.h"
#include "menudefs.h"
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
extern void uiInteractionSignify(uint8_t itype, bool waitForSound=false);
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
// called by arduino code

void setup() {
  setupGeneric();
  setupMenu();
  setupWorkflowAndScales();

  // network for talking to linked wifi board
  setupJrNet();

  // initialize optoins of scales, etc.
  pushOptionsToObjects();
  pushOptionsToObjects2();

  // initial tare
  jrworkflow.firstStart(optionStartMode);
  //
  //// start in workflow mode?
  //jrworkflow.setWorkflowModeEnable(false);

  // reset sleep timeout
  setNextSleepTimeOnWakeup(2);

  // state machine transition
  if (mode==DefModeSetup) {
    // switch to startup (this will cause a mode transition in the first loop)
    mode = DefModeStart;
  }

  // tell workflow we are ready
  jrworkflow.begin();

  // called on startup and wake
  doReadyStuff("startup");
}




void loop() {
  if (mode != previousMode) {
    // we have a change in mode
    doModeTransition(true);
  }


  // generic loop stuff before scale/menu mode
  loopGenericPre();

  // doesnt no anything but maybe one day
  loopJrNet();

  // scale mode
  if (!sleepingMode) {
    if (mode==DefModeWorkflow) {
      loopScaleAndWorkflow(true);
    }
    // menu mode
    if (mode==DefModeMenu) {
      loopMenu();
      // scale also even in menu mode?
      if (false) {
        loopScaleAndWorkflow(false);
      }
    }
  }

  // generic stuff AFTER scale/menu
  loopGenericPost();
}
//---------------------------------------------------------------------------
