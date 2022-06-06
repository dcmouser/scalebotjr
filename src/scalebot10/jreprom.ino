// eprom wrapper
// https://docs.arduino.cc/learn/built-in-libraries/eeprom

// expects a global #define for JrEpromWriteVersion (unsigned int) that user can check to see what version of their non-volatile options are stored in eprom via jreGetWriteVersion()
// a value 
// #define JrEpromWriteVersion 1
// #define JrEpromAppSentinelId 2



#include <EEPROM.h>


//---------------------------------------------------------------------------
// defines
#define DefJreAddressAppSentinelId 0
#define DefJreAddressWriteVersion 2
#define DefJreAddressUserDataStart 4
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
unsigned int jreGetWriteVersion() {
  // get the write version, stored at location 0
  // used so we can know when interpret saved vars
  unsigned int writeVersion;
  unsigned int sentinalAppIdValue = 0;
  //
  EEPROM.get(DefJreAddressAppSentinelId, sentinalAppIdValue);
  if (sentinalAppIdValue != JrEpromAppSentinelId) {
    // sentinel is incorrect, so return 0 meaning no write version found
    return 0;
  }
  return EEPROM.get(DefJreAddressWriteVersion, writeVersion);
}


void jreUpdateWriteVersion() {
  // save write version in eeprom
  EEPROM.put(DefJreAddressWriteVersion, (unsigned int) JrEpromWriteVersion);
  // update sentinel value
  EEPROM.put(DefJreAddressAppSentinelId, (unsigned int) JrEpromAppSentinelId);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
// sample code
/*
bool readStoredEepromData() {
  unsigned int savedDataWriteVersion = jreGetWriteVersion();
  if (savedDataWriteVersion==0) {
    // no saved date
    return;
  }
  if (savedDataWriteVersion < JrEpromWriteVersion) {
    // OLDER data was saved, might want to figure out how to upgrade it
  }
}


bool writeStoredEepromData() {
  // write first var
  int testvar1 = 1;
  EEPROM.put(DefJreAddressUserDataStart + 0, testvar1);
  // now update write version and sentinel, marking the saved data as good
  jreUpdateWriteVersion();
}
*/
//---------------------------------------------------------------------------
