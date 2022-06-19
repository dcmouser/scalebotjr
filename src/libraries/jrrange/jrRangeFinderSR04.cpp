#include "jrRangeFinderSR04.h"


#include <arduino.h>


// see https://www.elithecomputerguy.com/2019/06/hc-sr04-ultrasonic-distance-sensor-audio-alert-with-arduino/



// call win mindist of 30 and maxdist of 4500
// by default pulseWidth should be 10 or 20 to be safe
//---------------------------------------------------------------------------
JrRangeFinderSR04::JrRangeFinderSR04(int inechoPin, int intriggerPin, int inPulseWidth, unsigned long inpulseTimeOutMc, float inOptionMinDist, float inOptionMaxDist, float inOptionOutlierDistanceThreshold) : JrRangeFinder() {
	init(inOptionMinDist, inOptionMaxDist, inOptionOutlierDistanceThreshold);
	echoPin = inechoPin;
	triggerPin = intriggerPin;
	pulseWidth = inPulseWidth;
	pulseTimeOutMc = inpulseTimeOutMc;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool JrRangeFinderSR04::begin() {

  // Set pinmodes for sensor connections
  pinMode(echoPin, INPUT);
  pinMode(triggerPin, OUTPUT);

	// it's enabled
	enabled = true;
	
	// base stuff
	JrRangeFinder::begin();

	return enabled;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
float JrRangeFinderSR04::readSensorValue() {

  // Set the trigger pin LOW for 2uS
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
 
  // Set the trigger pin HIGH for 20us to send pulse
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(pulseWidth);
 
  // Return the trigger pin to LOW
  digitalWrite(triggerPin, LOW);
 
  // Measure the width of the incoming pulse
  //unsigned long duration = pulseIn(echoPin, HIGH);
  
  unsigned long duration = pulseIn(echoPin, HIGH, pulseTimeOutMc);
  
  if (duration==0) {
  	// pulse timed out
		lastSensorReadValid = false;
  	return DefBadValueRangeConstant;
  }
  
  lastSensorReadValid = true;
 
  // Determine distance from duration
  // Use 343 metres per second as speed of sound
  // Divide by 1000 as we want millimeters
  float distanceMm = (duration / 2) * 0.343;
  
  return distanceMm;
}
//---------------------------------------------------------------------------



