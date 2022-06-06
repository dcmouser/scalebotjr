#ifndef JrNetFeatherH
#define JrNetFeatherH


//---------------------------------------------------------------------------
#include <jrNet.h>

#include <WiFi101.h>
#include <SPI.h>
#include <driver/source/nmasic.h>
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
#define DefJrfnWakeInterruptPin 6
#define JrfnDefSleepBlinkLights false
#define JrfnDefDebugEicIsr false
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// delay from time a friend tells us to sleep and when we really shut down
#define JrfnScheduleSleepDelay 1000
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
volatile bool jrFnInterruptTriggered = false;

// global callback for waking from sleep, just sets flags
// do we really need any code in here? i don't think so
void EIC_ISR() {
	jrFnInterruptTriggered = true;
  if (true /*needswake*/) {
  	if (JrfnDefDebugEicIsr) {
    	Serial.println("IN EIC_ISR waking up.");    
    }
  } else {
  	if (JrfnDefDebugEicIsr) {
    	Serial.println("IN EIC_ISR not asleep!");
    }
  }
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
// helper class that is used to listen to serial port, take instructions, and send info via wifi
template<class T>
class JrFeatherNet : public JrNet<T> {
public:
	//WiFiClient client;
	AdafruitIO* iop = NULL;
	AdafruitIO_Feed* feedp = NULL;
public:
	bool needsWake = false;
	unsigned long scheduledSleepTimeMs = 0;
public:
	bool useLowPowerMode = true;
public:
	int status = WL_IDLE_STATUS;
public:
	JrFeatherNet(T* inSerialp, String inClientName, int inClientId, int indebugLevel) : JrNet<T>(inSerialp, inClientName, inClientId, indebugLevel) {;};
	~JrFeatherNet() {;};
public:
	virtual void setup(bool waitForSerial=false);
public:
	void setAdafruitIop(AdafruitIO* iniop) {iop = iniop;};
	void setFeedp(AdafruitIO_Feed* infeedp) {feedp = infeedp;}
public:
	bool connectIo();
	void disconnectWifi();
	void getNetStatus(char* statusStr);
public:
	virtual void loopBackground();
public:
	virtual bool processJsonCommand(const String &str, JrTypedefStaticJsonDocument &doc);
public:
	bool doSaveFeedItem(JrTypedefStaticJsonDocument &doc);
public:
	void sleepWifi();
	void wakeWifi();
	void setPowerModeWifi();
public:
	void boardSetupSleepFunctions();
	void boardGoToSleep();
	void boardWakeFromSleep();
	void doAttachInterrupt();
	void scheduleSleep();
public:
	void loopSleepStuff();
	void doWakeStuff();
	void doGoSleepStuff();
};
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
template<class T>
void JrFeatherNet<T>::setup(bool waitForSerial) {
	// base setup
	JrNet<T>::setup(waitForSerial);
	// 
	boardSetupSleepFunctions();
	setPowerModeWifi();
	// init
	scheduledSleepTimeMs = 0;
	needsWake = false;
}


template<class T>
void JrFeatherNet<T>::setPowerModeWifi() {
	if (useLowPowerMode) {
		// see https://learn.adafruit.com/adafruit-feather-m0-wifi-atwinc1500/power-management
		WiFi.lowPowerMode();
	}
}


template<class T>
void JrFeatherNet<T>::sleepWifi() {
	// this just sleeps the WIFI not the board(!)
	m2m_wifi_set_sleep_mode(M2M_PS_MANUAL, 1);
	//WiFi.setSleepMode(M2M_PS_MANUAL, 1);
	// tell it to sleep for a day? but really if a day works, then 10 days is better? ie it can be woken up
	//m2m_wifi_request_sleep(0);
	//WiFi.requestSleep(864000000);
	m2m_wifi_request_sleep(864000000);
}

template<class T>
void JrFeatherNet<T>::wakeWifi() {
	// reset chosen power mode
	setPowerModeWifi();
}
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
// see https://github.com/ProjectsByJRP/M0_Sleep_External_Int/blob/master/sleep_with_ext_int_pin6.ino



template<class T>
void JrFeatherNet<T>::doAttachInterrupt() {
  attachInterrupt(digitalPinToInterrupt(DefJrfnWakeInterruptPin), EIC_ISR, CHANGE);  // Attach interrupt to pin 6 with an ISR and when the pin state CHANGEs
  //attachInterrupt(digitalPinToInterrupt(DefJrfnWakeInterruptPin), EIC_ISR, HIGH);  // Attach interrupt to pin 6 with an ISR and when the pin state CHANGEs
}


template<class T>
void JrFeatherNet<T>::boardSetupSleepFunctions() {
	
	// setup interrupt pin that will wake us up
	doAttachInterrupt();

	// see https://github.com/ProjectsByJRP/M0_Sleep_External_Int/blob/master/sleep_with_ext_int_pin6.ino
  SYSCTRL->XOSC32K.reg |=  (SYSCTRL_XOSC32K_RUNSTDBY | SYSCTRL_XOSC32K_ONDEMAND); // set external 32k oscillator to run when idle or sleep mode is chosen
  REG_GCLK_CLKCTRL  |= GCLK_CLKCTRL_ID(GCM_EIC) |  // generic clock multiplexer id for the external interrupt controller
                       GCLK_CLKCTRL_GEN_GCLK1 |  // generic clock 1 which is xosc32k
                       GCLK_CLKCTRL_CLKEN;       // enable it
  while (GCLK->STATUS.bit.SYNCBUSY);              // write protected, wait for sync

  EIC->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN4;        // Set External Interrupt Controller to use channel 4 (pin 6)

  
  PM->SLEEP.reg |= PM_SLEEP_IDLE_CPU;  // Enable Idle0 mode - sleep CPU clock only
  //PM->SLEEP.reg |= PM_SLEEP_IDLE_AHB; // Idle1 - sleep CPU and AHB clocks
  //PM->SLEEP.reg |= PM_SLEEP_IDLE_APB; // Idle2 - sleep CPU, AHB, and APB clocks

  // It is either Idle mode or Standby mode, not both. 
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;   // Enable Standby or "deep sleep" mode

  // Built-in LED set to output and high
  if (JrfnDefSleepBlinkLights) {
  	PORT->Group[g_APinDescription[LED_BUILTIN].ulPort].DIRSET.reg = (uint32_t)(1<<g_APinDescription[LED_BUILTIN].ulPin); // set pin direction to output
  	PORT->Group[g_APinDescription[LED_BUILTIN].ulPort].OUTSET.reg = (uint32_t)(1<<g_APinDescription[LED_BUILTIN].ulPin); // set pin mode to high
  }
}





template<class T>
void JrFeatherNet<T>::boardWakeFromSleep() {
	if (JrfnDefSleepBlinkLights) {
		PORT->Group[g_APinDescription[LED_BUILTIN].ulPort].OUTTGL.reg = (uint32_t)(1<<g_APinDescription[LED_BUILTIN].ulPin);  // toggle output of built-in LED pin
	}
}
//---------------------------------------------------------------------------























//---------------------------------------------------------------------------
template<class T>
bool JrFeatherNet<T>::connectIo() {
  if (this->debugLevel>0) {
  	Serial.println("Connecting to AdafruitIO");
  }

  // connect to io.adafruit.com
	iop->connect();

  // wait for a connection
  while(iop->status() < AIO_CONNECTED) {
  	if (this->debugLevel>0) {
	    Serial.print("AdafruitIO status: ");
    	Serial.println(iop->status());
    }
    delay(500);
  }

  // we are connected
  if (this->debugLevel>0) {
	  Serial.println();
	  Serial.println(iop->statusText());
	}
  
  return true;
}


template<class T>
void JrFeatherNet<T>::disconnectWifi() {
  if (this->debugLevel>0) {
  	Serial.println("Disconnecting from Wifi/Adafruit IO");
  }

  // connect to io.adafruit.com
	iop->wifi_disconnect();
}
//---------------------------------------------------------------------------



































//---------------------------------------------------------------------------
template<class T>
void JrFeatherNet<T>::loopBackground() {
	// wake or sleep
	loopSleepStuff();
	
	// io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  iop->run();
  
	if (false) {
	  // test interrupt pin
	  int ival = digitalRead(DefJrfnWakeInterruptPin);
	  Serial.print("Int pin val: ");
	  Serial.println(ival);
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
template<class T>
bool JrFeatherNet<T>::processJsonCommand(const String &str, JrTypedefStaticJsonDocument &doc) {
	//
	// test
	/*
	if (doc["c"]=="req") {
		// status request
  	JrTypedefStaticJsonDocument rdoc;
		rdoc["f"] = this->getClientId();
  	rdoc["ri"] = doc["id"];
  	rdoc["c"] = "res";
  	rdoc["m"] = "fnok";//getWifiStatusStr();
		// send it
		this->sendJsonDocument(rdoc);
		return true;
	}
	*/

	if (doc["c"]=="fsave") {
		// request to save some data to feed
		doSaveFeedItem(doc);
		return true;
	}

	if (doc["c"]=="sleep") {
		// send device to sleep
		scheduleSleep();
		return true;
	}
	
	if (doc["c"]=="reqstatus") {
		// caller is asking for our status
		// status request
  	JrTypedefStaticJsonDocument rdoc;
  	rdoc["c"] = "reply";
		//rdoc["f"] = this->getClientId();
  	//rdoc["ri"] = doc["id"];
  	rdoc["req"] = "reqstatus";
  	char statusStr[80];
  	getNetStatus(statusStr);
  	rdoc["m"] = statusStr;
		// send it
		this->sendJsonDocument(rdoc);
		return true;
	}

	// fall back on base class
	return JrNet<T>::processJsonCommand(str, doc);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
template<class T>
void JrFeatherNet<T>::scheduleSleep() {
	scheduledSleepTimeMs = millis() + JrfnScheduleSleepDelay;
	jrFnInterruptTriggered = false;
}







//---------------------------------------------------------------------------
template<class T>
bool JrFeatherNet<T>::doSaveFeedItem(JrTypedefStaticJsonDocument &doc) {
	// save raw "m" field string
	char buf[80];
	strcpy(buf, (const char*)doc["m"]);
	// save it to feed
	(this->feedp)->jrsaveRaw(buf);

	// some feedback debug info
  if (this->debugLevel>1) {
		Serial.print("Feed save triggered from received command: ");
		Serial.println(buf);
	}
	// send to main app because we lose serial when we go to sleep
	this->sendSimpleCommand("dbg","sent feed item");

	return true;
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
template<class T>
void JrFeatherNet<T>::loopSleepStuff() {

  if (needsWake) {
  	doWakeStuff();
  }
  

  if (scheduledSleepTimeMs>0) {
  	// sleep is scheduled, is it time to go to sleep?
  	if (true && jrFnInterruptTriggered) {
	  	// interrupt trisggered since we tried to go to sleep, so cancel scheduled sleep
  		jrFnInterruptTriggered = false;
  		scheduledSleepTimeMs = 0;
  	} else if (millis()>scheduledSleepTimeMs) {
			doGoSleepStuff();
		}
  }

	if (JrfnDefSleepBlinkLights) {
  	PORT->Group[g_APinDescription[LED_BUILTIN].ulPort].OUTTGL.reg = (uint32_t)(1<<g_APinDescription[LED_BUILTIN].ulPin);  // toggle output of built-in LED pin
  }
       
}



template<class T>
void JrFeatherNet<T>::doWakeStuff() {
	// clear flag
  needsWake = false;

  // disconnect and reconnect
  this->disconnectWifi();
  this->connectIo();

  // trying to re-establish serial
  // ATTN: NOTE THIS DOES *NOT* WORK AND CAN HANG BOARD
  if (false) {
  	USBDevice.init();
  	USBDevice.attach();
  	USBDevice.detach();
  	Serial.end(); 
  	delay(1000);
  	Serial.begin(9600);
  	while (!Serial) {
  	   ;
  	}
  }

	// we never see this because serial is disconnected after first sleep
  if (this->debugLevel>0) {
  	Serial.println("JrFeatherNet is awake");
  }

	if (JrfnDefSleepBlinkLights) {
  	PORT->Group[g_APinDescription[LED_BUILTIN].ulPort].OUTCLR.reg = (uint32_t)(1<<g_APinDescription[LED_BUILTIN].ulPin); // set pin mode to low
  	//PORT->Group[g_APinDescription[LED_BUILTIN].ulPort].OUTTGL.reg = (uint32_t)(1<<g_APinDescription[LED_BUILTIN].ulPin);  // toggle output of built-in LED pin
  }

  this->sendSimpleCommand("dbg","awake");  
}



template<class T>
void JrFeatherNet<T>::doGoSleepStuff() {
	// clear flag
	scheduledSleepTimeMs=0;
	//
	// disconnect from wifi/io while asleep
	this->disconnectWifi();
	// do we need to delay here?

	if (JrfnDefSleepBlinkLights) {
  	PORT->Group[g_APinDescription[LED_BUILTIN].ulPort].OUTCLR.reg = (uint32_t)(1<<g_APinDescription[LED_BUILTIN].ulPin); // set pin mode to low
  }
  
  if (this->debugLevel>0) {
  	Serial.println("JrFeatherNet going to sleep.");
  }

  // trying to re-establish serial
  // ATTN: NOTE THIS DOES *NOT* WORK AND CAN HANG BOARD
  if (false) {
  	// test
  	Serial.flush();  //if this line is commented out, only one cycle of "sleep" will show
  	Serial.end();  
  	USBDevice.detach();      
  }

	// set flag so when we wake up and start looping we know we need to do wakeup stuff
	// ATTN: note this could be set AFTER _WFI()
  needsWake = true;

	// ACTUALLY PUTS BOARD TO SLEEP RIGHT HERE AND STOPS EXECUTING CODE BEFORE NEXT LINE      
  __WFI();  // wake from interrupt
  
  // WILL RESUME HERE

  return;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
template<class T>
void JrFeatherNet<T>::getNetStatus(char* statusStr) {
	sprintf(statusStr,"Status: %s.", iop->statusText());
}
//---------------------------------------------------------------------------






#endif