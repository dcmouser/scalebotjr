// settings helper code


//---------------------------------------------------------------------------
bool readStoredEepromData() {
  unsigned int savedDataWriteVersion = jreGetWriteVersion();
  if (savedDataWriteVersion==0) {
    // no saved date
    return;
  }
  if (savedDataWriteVersion < JrEpromWriteVersion) {
    // OLDER data was saved, might want to figure out how to upgrade it
    // or else return and refuse to load it
    // return;
  }

  // read global saved vars
  int addr;
  EEPROM.get(DefJreAddressUserDataStart + 0, optionSleepDelay);
  EEPROM.get(DefJreAddressUserDataStart + 1, optionDebugLevel);
  EEPROM.get(DefJreAddressUserDataStart + 2, option7SegmentBrightness);
  EEPROM.get(DefJreAddressUserDataStart + 3, optionStartMode);
  EEPROM.get(DefJreAddressUserDataStart + 4, optionBeepMode);
  EEPROM.get(DefJreAddressUserDataStart + 5, optionWeightSmoothMode);
  EEPROM.get(DefJreAddressUserDataStart + 6, optionScaleMode);
  EEPROM.get(DefJreAddressUserDataStart + 7, optionSoftZero);
  EEPROM.get(DefJreAddressUserDataStart + 8, optionCalibrationTweakMethod);
  
  
  
  //Serial.print("ATTN: read beep mode: ");
  //Serial.println(optionBeepMode);

  // save array of grind settings
  addr = 16;
  for (int i=0;i<30;++i) {
    if (CoffeeBeanTypes[i]==0) {
      break;
    }
    EEPROM.get(addr, grindSettings[i]);
    ++addr;
  }

  EEPROM.get(DefJreAddressUserDataStart + 50, optionAutoZero);
  EEPROM.get(DefJreAddressUserDataStart + 51, optionRoundUp);

  EEPROM.get(DefJreAddressUserDataStart + 56, optionCoffeeSelect);

  EEPROM.get(DefJreAddressUserDataStart + 57, optionCheckWarnings);

  EEPROM.get(DefJreAddressUserDataStart + 60, optionCalibrationWeight);
  EEPROM.get(DefJreAddressUserDataStart + 64, optionBeanJarWeight);

  // workflow reads its settings
  jrworkflow.readStoredEepromData(DefJreAddressUserDataStart+100);
}





bool writeStoredEepromData() {
  // save global vars
  int addr;
  
  EEPROM.put(DefJreAddressUserDataStart + 0, optionSleepDelay);
  EEPROM.put(DefJreAddressUserDataStart + 1, optionDebugLevel);
  EEPROM.put(DefJreAddressUserDataStart + 2, option7SegmentBrightness);
  EEPROM.put(DefJreAddressUserDataStart + 3, optionStartMode);
  EEPROM.put(DefJreAddressUserDataStart + 4, optionBeepMode);
  EEPROM.put(DefJreAddressUserDataStart + 5, optionWeightSmoothMode);
  EEPROM.put(DefJreAddressUserDataStart + 6, optionScaleMode);
  EEPROM.put(DefJreAddressUserDataStart + 7, optionSoftZero);
  EEPROM.put(DefJreAddressUserDataStart + 8, optionCalibrationTweakMethod);
  //
  //Serial.print("ATTN: wrote beep mode: ");
  //Serial.println(optionBeepMode);

  // save array of grind settings
  addr = 16;
  for (int i=0;i<30;++i) {
    if (CoffeeBeanTypes[i]==0) {
      break;
    }
    EEPROM.put(addr, grindSettings[i]);
    ++addr;
  }

  EEPROM.put(DefJreAddressUserDataStart + 50, optionAutoZero);
  EEPROM.put(DefJreAddressUserDataStart + 51, optionRoundUp);

  EEPROM.put(DefJreAddressUserDataStart + 56, optionCoffeeSelect);

  EEPROM.put(DefJreAddressUserDataStart + 57, optionCheckWarnings);

  EEPROM.put(DefJreAddressUserDataStart + 60, optionCalibrationWeight);
  EEPROM.put(DefJreAddressUserDataStart + 64, optionBeanJarWeight);
  
  // workflow saves its settings
  jrworkflow.writeStoredEepromData(DefJreAddressUserDataStart+100);

  // now update write version and sentinel, marking the saved data as good
  jreUpdateWriteVersion();
}
//---------------------------------------------------------------------------
