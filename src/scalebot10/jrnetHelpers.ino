//---------------------------------------------------------------------------
// Talking with feather wifi
//#include <ArduinoJson.h>
//
#include <jrNet.h>
//
#define DefJrnClientName "Scalebot"
#define DefJrnClientId 2
#define DefClientVersionStr "1.0"
#define DefJrnDebugLevel 0 //2
//
#define JrfNetDelayBeforeSleep 250
#define JrfNetDelayBetweenInterruptPin 250
//
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// forward declaration needed
bool jrnetResultTriggerCalback(JrTypedefStaticJsonDocument &doc);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// use serial port 3 (pins 14+15)
JrNet<HardwareSerial> jrnet(&Serial3, "Mega", DefJrnClientId, DefClientVersionStr,  DefJrnDebugLevel);
//JrNet<HardwareSerial> jrnet(&Serial1, "Mega", DefJrnClientId, DefClientVersionStr,  DefJrnDebugLevel);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//typedef StaticJsonDocument<300> JrTypedefStaticJsonDocument;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void setupJrNet() {
  // init
  pinMode(wakeHelperBoardPin, OUTPUT);
  digitalWrite(wakeHelperBoardPin, LOW);
  //
  jrnet.setup();
  jrnet.setTriggerCallbackfp(jrnetResultTriggerCalback);
}


void loopJrNet() {
  // nothing to do
  jrnet.loop();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void jrNetGoToSleep() {

  jrnet.goToSleep();
  // send command to go to sleep
  // ATTN: test disabling this so we can watch interrupt pin
  if (true) {
    if (true) {
      char buf[20];
      strcpy(buf, "Scalebot sleeping.");
      // record the drink on our network
      jrnet.sendSimpleFeedSaveRequest(buf);
      delay(JrfNetDelayBeforeSleep);
    }
    //
    jrnet.sendSimpleCommand("sleep");
    // ATTN: we need to delay a bit before we fall asleep.. but how long?
    delay(JrfNetDelayBeforeSleep);
  }
}

void jrNetWakeFromSleep() {
  sendWakeInterruptToJrNetBoard();
  // our LOCAL element has nothing to do
  jrnet.wakeFromSleep();
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
void sendWakeInterruptToJrNetBoard() {
  if (optionDebugLevel > 0) {
    Serial.println("Sending wake interrupt signal to helper jrnet board!");
  }

  // send pin high
  // ATTN: this delay slows down wake, let's figure out a better way to do it
  digitalWrite(wakeHelperBoardPin, HIGH);
  delay(JrfNetDelayBetweenInterruptPin);
  digitalWrite(wakeHelperBoardPin, LOW);
  // then low
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void sendAdafruitIoFeedEventOnReady(const char *premsg) {
  char buf[80];
  sprintf(buf, "Scalebot %s; ready and online.", premsg);
  // record the drink on our network
  jrnet.sendSimpleFeedSaveRequest(buf);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool jrnetResultTriggerCalback(JrTypedefStaticJsonDocument &doc) {
  if (doc["c"] == "feedresult") {
    if (jrworkflow.getWorkflowMode()==JrWorkflowMode_Done) {
      String msg = doc["m"];
      if (msg.indexOf("ok:")>=0) {
        JrLcdMatrix* lcdmatrixp = jrworkflow.getLcdMatrix();
        lcdmatrixp->drawBitmapWifiGood();
      }
    }
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------
