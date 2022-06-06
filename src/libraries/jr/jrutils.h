#ifndef jrutilsH
#define jrutilsH


// F() functions
#include <avr/pgmspace.h>
#include <arduino.h>

//---------------------------------------------------------------------------
// helpers
void jrFloatToStr(char *outBuf, float val, int spaces, int decimals);
void jrRemoveTrailingZeros(char *outBuf);
void jrpadstr(char* str, int targetLen);

const char* jrCopyProgmenStringBuf(char *outbuf, const __FlashStringHelper *instrf, int maxlen);

void jrConvertMsToNiceTimeBufCompact(unsigned long elapsedMs, char* outbuf);
void jrConvertMsToNiceTimeBuf(unsigned long elapsedMs, char* outbuf, bool showMs);


const char* jrChooseRandomStringFromList(const char *slist[]);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int JrFreemem();
//---------------------------------------------------------------------------



#endif

