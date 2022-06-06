#include "jrutils.h"

// F() functions
#include <avr/pgmspace.h>
#include <math.h>




//---------------------------------------------------------------------------
// helpers
void jrFloatToStr(char *outBuf, float val, int spaces, int decimals) {
  dtostrf(val,spaces,decimals,outBuf);
}

void jrRemoveTrailingZeros(char *outBuf) {
  int slen = strlen(outBuf)-1;
  while (slen>0 && outBuf[slen]=='0') {
    --slen;
  }
  if (slen>0 && outBuf[slen]=='.') {
    outBuf[slen]='\0';
  }
}


void jrpadstr(char* str, int targetLen) {
  int slen = strlen(str);
  str[targetLen] = '\0';
  while (slen<targetLen) {
    str[slen]=' ';
    ++slen;
  }
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
const char* jrCopyProgmenStringBuf(char *outbuf, const __FlashStringHelper *instrf, int maxlen) {
  if (instrf == NULL) {
    outbuf[0]='\0';
    return NULL;
  }
  int slen = strlen_PF((PGM_P)instrf)+1;
  if (slen>maxlen) {
    slen = maxlen;
  }
  strlcpy_PF(outbuf, (PGM_P)instrf, slen);
  outbuf[slen]='\0';
  return outbuf;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
void jrConvertMsToNiceTimeBufCompact(unsigned long elapsedMs, char* outbuf) {
  unsigned long seconds = floor((float)elapsedMs / 1000.0);
  unsigned long oseconds = seconds;
  unsigned int minutes = floor((float)seconds / 60.0);
  seconds = seconds - (minutes*60);
  unsigned int hours = floor((float)minutes / 60.0);
  minutes -= hours*60;
  //
  bool showMs = false;
  //
  if (hours>0) {
    sprintf(outbuf,"%02d:%02d:%02lu",hours,minutes,seconds);
  } else if (minutes>0) {
    sprintf(outbuf,"%02d:%02lu",minutes,seconds);
  } else if (seconds>0) {
    if (showMs) {
      int fsec = floor((elapsedMs%1000) / 100);
      sprintf(outbuf,"%02lu.%ds",seconds,fsec);
    } else {
      sprintf(outbuf,"%02lus",seconds);
    }
  } else {
    strcpy(outbuf,"");
  }
}


void jrConvertMsToNiceTimeBuf(unsigned long elapsedMs, char* outbuf, bool showMs) {
  unsigned long seconds = floor((float)elapsedMs / 1000.0);
  unsigned int minutes = floor((float)seconds / 60.0);
  seconds = seconds - (minutes*60);
  unsigned int hours = floor((float)minutes / 60.0);
  minutes -= hours*60;
  //
  if (hours>0) {
    sprintf(outbuf,"%dh,%dm,%02lus",hours,minutes,seconds);
  } else if (minutes>0) {
    sprintf(outbuf,"%dmin,%02lusec",minutes,seconds);
  } else if (seconds>0 || showMs) {
    if (showMs) {
    	char f1[10];
    	jrFloatToStr(f1, ((float)elapsedMs/1000.0), 2, 1);
      sprintf(outbuf,"%0s",f1);
    } else {
      sprintf(outbuf,"%02lus",seconds);
    }
  } else {
    strcpy(outbuf,"");
  }
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
const char* jrChooseRandomStringFromList(const char* slist[]) {
	int i;
	for (i=0;slist[i]!=0;++i) {
		;
	}
	int index = random(0,i);
	return slist[index];
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// see https://forum.arduino.cc/t/what-is-the-default-stack-size-for-function-calls/81357
// this function will return the number of bytes currently free in RAM      
extern int  __bss_end; 
extern int  *__brkval; 
int JrFreemem()
{ 
 int free_memory; 
 if((int)__brkval == 0) 
   free_memory = ((int)&free_memory) - ((int)&__bss_end); 
 else 
   free_memory = ((int)&free_memory) - ((int)__brkval); 
 return free_memory; 
}
//---------------------------------------------------------------------------