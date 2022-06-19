//#ifndef JrNetH
//#define JrNetH

#include <ArduinoJson.h>

#define JrnTestDelayMultiplier 10
#define DefMaxReplyLen 160


//---------------------------------------------------------------------------
typedef StaticJsonDocument<400> JrTypedefStaticJsonDocument;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define JrSerialSpeedMonitor 9600
//#define JrSerialSpeed 9600
#define JrSerialSpeed 115200 
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
typedef bool (*TriggerCallbackFpT) (JrTypedefStaticJsonDocument &doc);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// helper class that is used to listen to serial port, take instructions, and send info via wifi
template<class T>
class JrNet {
public:
	T* serialp = NULL;
	String clientName;
	int clientId=0;
	String versionIdStr;
	int debugLevel = 1;
	unsigned long sentCount = 0;
	unsigned long recvCount = 0;
protected:
	TriggerCallbackFpT triggerfp = NULL;
public:
	char expectedReplyKey[20];
	char lastReply[DefMaxReplyLen];
public:
	JrNet(T* inSerialp, const String inClientName, int inClientId, String inVersionIdStr, int indebugLevel) {serialp=inSerialp; clientName = inClientName; clientId=inClientId; versionIdStr = inVersionIdStr; debugLevel = indebugLevel;};
	~JrNet() {;};
public:
	virtual void setup(bool waitForSerial=false);
	void setTriggerCallbackfp(TriggerCallbackFpT fp) { triggerfp = fp; }
	void debugSerialBuffer();
public:
	bool loop();
public:
	virtual void loopBackground() {;};
public:
	bool loopProcessReadLine(const String &str);
	void sendJsonDocument(JrTypedefStaticJsonDocument &doc);
public:
	void handleError(const String &etype, const String &rawmsg, const char * err);
public:
	virtual bool processJsonCommand(JrTypedefStaticJsonDocument &doc);
	virtual bool triggerCallbackOnUnhandledJsonCommand(JrTypedefStaticJsonDocument &doc);
public:
	int getClientId() {return clientId;}
	const char* getVersionIdStrAsCharp() { return versionIdStr.c_str();};
	void testDelay(int multiplier) {delay(multiplier*JrnTestDelayMultiplier);}
public:
	bool sendTestInfoRequest();
	bool sendTestFeedSaveRequest();
public:
	bool sendSimpleFeedSaveRequest(const char* msg);
	bool sendSimpleCommand(const char *commandstr, const char* msg=NULL);
public:
	virtual void goToSleep() {;};
	virtual void wakeFromSleep() {;};
public:
	void flush() { serialp->flush(); };
public:
	bool requestStatusInfoWithTimeout(char* strbuf, int timeoutMs);	
	bool sendSimpleCommandWaitForReply(char* command, char* strbuf, int timeoutMs);
	bool waitForReply(char* inExpectedReplyKey, char *strbuf, int timeoutMs);
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
template<class T>
void JrNet<T>::setup(bool waitForSerial) {
  
  // put your setup code here, to run once:
  // Initialize serial
  if (debugLevel>0) {
  	Serial.begin(JrSerialSpeedMonitor);
 		// wait for serial port to connect. Needed for native USB port only
  	if (waitForSerial) {
	  	while (!Serial) {
	    	;
	  	}
	  } else {
	  	delay(500);
	  }
	}

  // Print a welcome message
  if (debugLevel>1) {
  	Serial.print(clientName);
	Serial.print(F(" v"));
	Serial.print(versionIdStr);
  	Serial.println(F(" is alive."));
  }
	
	// init the serial we communicate over
  serialp->begin(JrSerialSpeed);
	while (!*serialp) {
	    // wait
	  }
  
  if (debugLevel>1) {
  	Serial.print(clientName);
  	Serial.println(F(" serial port is active."));
  }
  if (debugLevel>1) {
  	debugSerialBuffer();
  }
  
  strcpy(expectedReplyKey, "");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template<class T>
void JrNet<T>::debugSerialBuffer() {
	Serial.println(F("Dbg serialbuf"));
	
	#ifdef SERIAL_RX_BUFFER_SIZE
		Serial.print(F("RXSIZE: "));
		Serial.println(SERIAL_RX_BUFFER_SIZE);
		Serial.print(F("TXSIZE: "));
		Serial.println(SERIAL_TX_BUFFER_SIZE);
	#endif
	
	#ifdef SERIAL_BUFFER_SIZE
		Serial.print(F("SIZE: "));
		Serial.println(SERIAL_BUFFER_SIZE);
	#endif
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
template<class T>
void JrNet<T>::handleError(const String &etype, const String &rawmsg, const char * err) {
	if (debugLevel>0) {
		Serial.print(F("* E "));
		Serial.print(etype);
		Serial.print(F(" r "));
		Serial.print(rawmsg);
		Serial.print(F(" e "));
  	Serial.println(err);
  }
}
//---------------------------------------------------------------------------



















//---------------------------------------------------------------------------
template<class T>
bool JrNet<T>::loop() {
	//
	// call to do any background processing by derived classes
	loopBackground();
	
	if (!this->serialp->available()) {
		return false;
	}
	
	// read in a string line
	bool bretv = false;
	String str;

	// any data ready?
  while (this->serialp->available()) {
		str =  this->serialp->readStringUntil('\n');
		//str =  this->serialp->readString();
		//str = str.trim()
		str.trim();
		if (str=="") {
			continue;
		}

  	if (debugLevel>1) {
	  	Serial.println(F("-"));
  		Serial.print(clientName);
  		Serial.print(F(" r1: "));
  		Serial.println(str);
  	}
	
	// ATTN: TEST do nothing
	//break;

		// ATTN: test to see if split is broken
		if (false) {
			loopProcessReadLine(str);
			bretv = bretv | true;
			continue;
		}
	
		// if messages built up while we are busy, we need to split them by newlines to separate them into separate messages
		String delim = "\n";
		int slen = delim.length();
		while (true) {
			int  pos = str.indexOf(delim);
			if (pos==-1) {
				bretv = bretv | loopProcessReadLine(str);
				break;
			}
			String oneline = str.substring(0,pos);
			str = str.substring(pos+slen);
			bretv = bretv | loopProcessReadLine(oneline);
		}
	}

	return bretv;
}



template<class T>
bool JrNet<T>::loopProcessReadLine(const String &str) {
	if (str=="") {
		return true;
	}
  if (this->debugLevel>1) {
  	Serial.print(clientName);
  	Serial.print(F(" r2: "));
  	Serial.println(str);
  }

	// deserialize it
  JrTypedefStaticJsonDocument doc;
  DeserializationError err = deserializeJson(doc, str);
  //
  if (err!=DeserializationError::Ok) {
  	// error deserializing
 		this->handleError("json", str, err.c_str());
 		return false;
  }
  
	// pretty print for debugging
  if (this->debugLevel>1) {
  	// decoded json:
  	String prettyOut;
  	serializeJsonPretty(doc, prettyOut);
  	Serial.print(F("D: "));
    Serial.println(prettyOut);
  }
  
  // process it
  this->processJsonCommand(doc);

	// pretty print for debugging
  if (this->debugLevel>1) {
  	Serial.println(F("-="));
  }

  // success
  return true;
}
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
template<class T>
void JrNet<T>::sendJsonDocument(JrTypedefStaticJsonDocument &doc) {
	//
 	String outstr;
	serializeJson(doc, outstr);
	//	
  if (this->debugLevel>1) {
		Serial.print(F("Snd: "));
   	Serial.println(outstr);
  }
  // send it
  //serializeJson(doc, *serialp);
  serialp->println(outstr);

  // ATTN: disabling 6/16/22
  // needed?
  //serialp->flush();
}
//---------------------------------------------------------------------------




























//---------------------------------------------------------------------------
template<class T>
bool JrNet<T>::processJsonCommand(JrTypedefStaticJsonDocument &doc) {
	// process the command, return true on success

	if (doc["c"]=="req") {
		// status request
  	JrTypedefStaticJsonDocument rdoc;
		rdoc["f"] = getClientId();
  	rdoc["ri"] = doc["id"];
  	rdoc["c"] = "res";
  	rdoc["t"] = "vanilla";
		// send it
  	sendJsonDocument(rdoc);
		return true;
	} else if (doc["c"]=="dbg") {
		// display something
  		if (debugLevel>1) {
			const char* msg = doc["m"];
			Serial.print(F("recv: "));
			Serial.println(msg);
		}
		return true;
	} else if (doc["c"]=="reply") {
		// display something
		const char* req = doc["req"];
		if (strcmp(req,expectedReplyKey)==0) {
			// this is the reply we were waiting for
			const char* msg = doc["m"];
			strcpy(lastReply,msg);
  		if (debugLevel>1) {
  			Serial.print(F("erply: "));
  			Serial.println(lastReply);
  		}
  		// handled
  		return true;
		} else {
			Serial.print(F("unex1: "));
			Serial.println(req);
			Serial.print(F("ex: "));
			Serial.println(expectedReplyKey);
  		// drop down
		}
	}
	
	// trigger callback?
	if (triggerCallbackOnUnhandledJsonCommand(doc)) {
		return true;
	}
	
	return false;
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
template<class T>
bool JrNet<T>::sendTestInfoRequest() {
	++sentCount;
  if (debugLevel>1) {
  	Serial.print(sentCount);
  	Serial.println(F("] sendc."));
  }
   
  JrTypedefStaticJsonDocument doc;
	doc["f"] = getClientId();
  doc["id"] = sentCount;
  doc["c"] = "req";

  sendJsonDocument(doc);

  return true;
}

template<class T>
bool JrNet<T>::sendTestFeedSaveRequest() {
	++sentCount;
  if (debugLevel>1) {
  	Serial.print(sentCount);
  	Serial.println(F("] sendf."));
  }
   
  JrTypedefStaticJsonDocument doc;
	doc["m"] = "123";
  doc["id"] = sentCount;
  doc["c"] = "fsave";

  sendJsonDocument(doc);

  return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template<class T>
bool JrNet<T>::sendSimpleFeedSaveRequest(const char* msg) {
	++sentCount;
	/*
  if (debugLevel>1) {
  	Serial.print(sentCount);
  	Serial.print("] sending fsave req to friend: ");
  	Serial.println(msg);
  }
  */
   
  JrTypedefStaticJsonDocument doc;
	doc["m"] = msg;
  doc["id"] = sentCount;
  doc["c"] = "fsave";

  sendJsonDocument(doc);
  return true;
}


template<class T>
bool JrNet<T>::sendSimpleCommand(const char *commandstr, const char* msg) {
  JrTypedefStaticJsonDocument doc;
  doc["id"] = sentCount;
  doc["c"] = commandstr;
  if (msg!=NULL && strcmp(msg,"")!=0) {
  	doc["m"] = msg;
  }
	//
  sendJsonDocument(doc);
  return true;
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
template<class T>
bool JrNet<T>::requestStatusInfoWithTimeout(char* strbuf, int timeoutMs) {
	// send request and wait for reply
	bool bretv = sendSimpleCommandWaitForReply("reqstatus", strbuf, timeoutMs);
	if (!bretv) {
		strcpy(strbuf, "Could not connect\n to jrnet.");
	}
	return bretv;
}


template<class T>
bool JrNet<T>::sendSimpleCommandWaitForReply(char* command, char* strbuf, int timeoutMs) {
	// send request
	sendSimpleCommand(command);
	// wait for reply
	bool bretv = waitForReply(command, strbuf, timeoutMs);
	return bretv;
}


template<class T>
bool JrNet<T>::waitForReply(char* inExpectedReplyKey, char *strbuf, int timeoutMs) {
	unsigned long startTime = millis();
	unsigned long curtime;
	
	// set key we are expecting and clear result
	//Serial.println("ATTN: Waiting for reply..");
	strcpy(expectedReplyKey, inExpectedReplyKey);
	strcpy(lastReply,"");
	
	while (true) {
		// loop network
		//Serial.println("Looping...");
		loop();
			//Serial.print("LastReply2: ");
  			//Serial.println(lastReply);
		if (strcmp(lastReply,"")!=0) {
			// we got a reply!
			//Serial.println("ATTN: got expected reply: ");
			//Serial.println(lastReply);
			strcpy(strbuf, lastReply);
			strcpy(expectedReplyKey, "");
			return true;
		}
		curtime = millis();
		if (timeoutMs>0 && curtime-startTime > timeoutMs) {
			break;
		}
	}

	// failed to get reply
	strcpy(expectedReplyKey, "");
	return false;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
template<class T>
bool JrNet<T>::triggerCallbackOnUnhandledJsonCommand(JrTypedefStaticJsonDocument &doc) {
	// process the command, return true on success
	if (triggerfp) {
		return triggerfp(doc);
	}
	return false;
}
//---------------------------------------------------------------------------







//#endif