// provided with LCDML library
// modified by jr 4/25/22


// =====================================================================
//
// CONTROL v2.2.0
//
// =====================================================================
// *********************************************************************
// *************** (3) CONTROL WITH ENCODER ****************************
// *********************************************************************




//---------------------------------------------------------------------------
void lcdml_menu_control(void)
{
  // If something must init, put in in the setup condition
  if(LCDML.BT_setup())
  {
  }

  // update buttons
  // note we only handle the optionsButton and rotary encoder dial here; other buttons are handled in mainLoop code

  
  optionsButton.update();
  // get rotary turning direction if any
  int pos = rotaryEncoder.update();
  // get state of options dial button press
  JrPressType optionPressType = optionsButton.getPressType();

  if (pos==JrEncoderRotaryDirectionLeft) {
    // turn left/up
    LCDML.BT_up();
    uiInteractionSignify(DefSoundTypeMenuMove);
  } else if (pos==JrEncoderRotaryDirectionRight) {
    // turn right/down
    LCDML.BT_down();
    uiInteractionSignify(DefSoundTypeMenuMove);
  }
  
  if (optionPressType==JrPressTypeShort) {
    // short press detected
    LCDML.BT_enter();          
    uiInteractionSignify(DefSoundTypeMenuSelect);
  } else if (optionPressType==JrPressTypeLong) {
    // long press detected -- this is a GO BACK choice
    LCDML.BT_quit();
    uiInteractionSignify(DefSoundTypeMenuMove);
  }
}
//---------------------------------------------------------------------------
