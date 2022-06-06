// menu main loop code


//---------------------------------------------------------------------------
void menuDisplayEngage() {
  // ask for a REDRAW of the menu
  LCDML.DISP_update();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void setupMenu() { 
  // LCDMenuLib Setup
  LCDML_setup(_LCDML_DISP_cnt);

  // Some settings which can be used
  // Enable Menu Rollover (wrap around)
  //LCDML.MENU_enRollover();

  // Enable Screensaver (screensaver menu function, time to activate in ms)
  //LCDML.SCREEN_enable(mFunc_screensaver, 10000); // set to 10 seconds
  LCDML.SCREEN_disable();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void loopMenu() {
  LCDML.loop();
}
//---------------------------------------------------------------------------
