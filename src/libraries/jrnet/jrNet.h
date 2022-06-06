//#ifndef JrNetH
//#define JrNetH

#include <ArduinoJson.h>

#define JrnTestDelayMultiplier 10


//---------------------------------------------------------------------------
typedef StaticJsonDocument<300> JrTypedefStaticJsonDocument;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//#define JrSerialSpeed 9600
#define JrSerialSpeed 115200 
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// helper class that is used to listen to serial port, take instructions, and send info via wifi
template<class T>
class JrNet {
public:
	T* serialp = NULL;
	String clientName;
	int clientId=0;
	int debugLevel = 1;
	unsigned long sentCount = 0;
	unsigned long recvCount = 0;
public:
	char expectedReplyKey[20];
	String lastReply;
public:
	JrNet(T* inSerialp, const String inClientName, int inClientId, int indebugLevel) {serialp=inSerialp; clientName = inClientName; clientId=inClientId; debugLevel = indebugLevel;};
	~JrNet() {;};
public:
	virtual void setup(bool waitForSerial=false);
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
	virtual bool processJsonCommand(const String &str, JrTypedefStaticJsonDocument &doc);
public:
	int getClientId() {return clientId;}
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
  	Serial.begin(9600);
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
  	Serial.println(" is alive.");
  }
	
	// init the serial we communicate over
  serialp->begin(JrSerialSpeed);
	while (!*serialp) {
	    // wait
	  }
  
  if (debugLevel>1) {
  	Serial.print(clientName);
  	Serial.println(" serial port is active.");
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
	Serial.println("Debugging serial target buffer.");
	
	#ifdef SERIAL_RX_BUFFER_SIZE
		Serial.print("SERIAL_RX_BUFFER_SIZE: ");
		Serial.println(SERIAL_RX_BUFFER_SIZE);
		Serial.print("SERIAL_TX_BUFFER_SIZE: ");
		Serial.println(SERIAL_TX_BUFFER_SIZE);
	#endif
	
	#ifdef SERIAL_BUFFER_SIZE
		Serial.print("SERIAL_BUFFER_SIZE: ");
		Serial.println(SERIAL_BUFFER_SIZE);
	#endif
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
template<class T>
void JrNet<T>::handleError(const String &etype, const String &rawmsg, const char * err) {
	if (debugLevel>0) {
		Serial.print("* ERROR ");
		Serial.print(etype);
		Serial.print(" raw message  ");
		Serial.print(rawmsg);
		Serial.print(" error = ");
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
		
		//str = str.trim()
		str.trim();
		if (str=="") {
			continue;
		}

  	if (debugLevel>1) {
	  	Serial.println("--------------------");
  		Serial.print(clientName);
  		Serial.print(" read1 of data: ");
  		Serial.println(str);
  	}

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
  	Serial.print(" read2 of data: ");
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
  	Serial.print("Decoded: ");
    Serial.println(prettyOut);
  }
  
  // process it
  this->processJsonCommand(str, doc);

	// pretty print for debugging
  if (this->debugLevel>1) {
  	Serial.println("--------------------\n");
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
		Serial.print("Sending out: ");
   	Serial.println(outstr);
  }
  // send it
  //serializeJson(doc, *serialp);
  serialp->println(outstr);
  // needed?
  serialp->flush();
}
//---------------------------------------------------------------------------




























//---------------------------------------------------------------------------
template<class T>
bool JrNet<T>::processJsonCommand(const String &str, JrTypedefStaticJsonDocument &doc) {
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
		const char* msg = doc["m"];
		Serial.print("Debug message received from friend: ");
		Serial.println(msg);
		return true;
	} else if (doc["c"]=="reply") {
		// display something
		const char* req = doc["req"];
		if (strcmp(req,expectedReplyKey)==0) {
			// this is the reply we were waiting for
			const char* msg = doc["m"];
			lastReply = String(msg);
  		if (debugLevel>1) {
  			Serial.print("Received expected reply: ");
  			Serial.println(msg);
  		}
		} else {
  		Serial.print("Received UNexpected reply for key: ");
  		Serial.println(req);
		}
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
  	Serial.println("] sending cmd msg to friend.");
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
  	Serial.println("] sending fsave req to friend.");
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
	strcpy(expectedReplyKey, inExpectedReplyKey);
	lastReply = "";
	
	while (true) {
		// loop network
		loop();
		if (lastReply!="") {
			// we got a reply!
			strcpy(strbuf, lastReply.c_str());
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













//#endif