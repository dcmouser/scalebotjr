
 
 void setupLcd() {
  jrlcd.begin(DefLcdCols, DefLcdRows);
  //
  // lcd custom chars needed for menu?
  jrlcd.createScrollbarCharsForMenu();
}
