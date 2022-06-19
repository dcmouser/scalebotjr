// menu helper code



//---------------------------------------------------------------------------
// function prototype for callbacks
typedef bool (*FuncHandleDynamicPage)(int);
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
// ATTN: JR MOD see LCDMenuLib2.cpp - called by core LCDmenu2 code
void JR_MENU_QuitFromRoot() {
  doSafelyExitMenuMode(true);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void doSafelyExitMenuMode(bool leavingFromMenu) {
  // end function and go an layer back
  // save data
  writeStoredEepromData();

  // ATTN: WAIT for options button to release? i think this was only needed when we were duplicating processing of optionsButton, but no longer
  // optionsButton.waitForButtonReleaseUp(optionsButton);

  // switch to scale mode
  mode = DefModeWorkflow;

  if (menuMode==DefMenuModeQuestion) {
    MenuModeQuestionDone(leavingFromMenu);
  }

  // enable this in case we were in scroll mode inside a dynamic param
  LCDML.MENU_enScroll();

  // good time to re-check any warnings?
  jrworkflow.checkWarnings();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
int handleDynamicParam(const char *label, uint8_t line, int var, int vmin, int vmax) {
  // check if this function is active (cursor stands on this line)

  // give buzzer a change to update a bit so sound on turning dial isnt too weird
  JrSemiUpdateBuzzer();

  // initial fix of var
  if (var<vmin) {
    var = vmin;
  }
  if (var>vmax) {
    var=vmax;
  }
  
  char editIndicator[3];
  strcpy(editIndicator,"");
  if (line == LCDML.MENU_getCursorPos())
  {
    if (LCDML.BT_checkAny())
    {
      if (LCDML.BT_checkEnter())
      {
        // this function checks returns the scroll disable status (0 = menu scrolling enabled, 1 = menu scrolling disabled)
        if (LCDML.MENU_getScrollDisableStatus() == 0) {
          // disable the menu scroll function to catch the cursor on this point
          // now it is possible to work with BT_checkUp and BT_checkDown in this function
          // this function can only be called in a menu, not in a menu function
          LCDML.MENU_disScroll();
        }
        else {
          // enable the normal menu scroll function
          LCDML.MENU_enScroll();
        }
        // do something..?
      }

      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkDown() || LCDML.BT_checkRight()) {
        var+=rotaryEncoder.getAccelerationJumpSteps();
        if (var>vmax) {
          var=vmax;
        }
      }

      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkUp() || LCDML.BT_checkLeft()) {
        var-=rotaryEncoder.getAccelerationJumpSteps();
        if (var<vmin) {
          var = vmin;
        }
      }
    }

    // highlight if we are editing it
    if(LCDML.MENU_getScrollDisableStatus() != 0) {
      strcpy(editIndicator,"*");
    }
  }

  // give buzzer a change to update a bit so sound on turning dial isnt too weird
  JrSemiUpdateBuzzer();

  char buf[24];
  sprintf(buf, "%s: %d%s", label, var, editIndicator);

  // use the line from function parameters
  jrlcd.setCursor(1, (line + _LCDML_DSIP_use_header));
  jrlcd.print(buf);

  // give buzzer a change to update a bit so sound on turning dial isnt too weird
  JrSemiUpdateBuzzer();

  return var;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
int handleDynamicParam(const char *label, uint8_t line, float var, float vmin, float vmax) {
  // check if this function is active (cursor stands on this line)

  // give buzzer a change to update a bit so sound on turning dial isnt too weird
  JrSemiUpdateBuzzer();

  // initial fix of var
  if (var<vmin) {
    var = vmin;
  }
  if (var>vmax) {
    var=vmax;
  }
  
  char editIndicator[3];
  strcpy(editIndicator,"");
  if (line == LCDML.MENU_getCursorPos())
  {
    if (LCDML.BT_checkAny())
    {
      if (LCDML.BT_checkEnter())
      {
        // this function checks returns the scroll disable status (0 = menu scrolling enabled, 1 = menu scrolling disabled)
        if (LCDML.MENU_getScrollDisableStatus() == 0) {
          // disable the menu scroll function to catch the cursor on this point
          // now it is possible to work with BT_checkUp and BT_checkDown in this function
          // this function can only be called in a menu, not in a menu function
          LCDML.MENU_disScroll();
        }
        else {
          // enable the normal menu scroll function
          LCDML.MENU_enScroll();
        }
        // do something..?
      }

      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkDown() || LCDML.BT_checkRight()) {
        var+=rotaryEncoder.getAccelerationJumpSteps()/10.0;
        if (var>vmax) {
          var=vmax;
        }
      }

      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkUp() || LCDML.BT_checkLeft()) {
        var-=rotaryEncoder.getAccelerationJumpSteps()/10.0;
        if (var<vmin) {
          var = vmin;
        }
      }
    }

    // highlight if we are editing it
    if(LCDML.MENU_getScrollDisableStatus() != 0) {
      strcpy(editIndicator,"*");
    }
  }

  // give buzzer a change to update a bit so sound on turning dial isnt too weird
  JrSemiUpdateBuzzer();

  char buf[24];
  char varbuf[10];
  jrFloatToStr(varbuf, var, 1, 12);
  sprintf(buf, "%s: %s%s", label, varbuf, editIndicator);

  // use the line from function parameters
  jrlcd.setCursor(1, (line + _LCDML_DSIP_use_header));
  jrlcd.print(buf);

  // give buzzer a change to update a bit so sound on turning dial isnt too weird
  JrSemiUpdateBuzzer();

  return var;
}
//---------------------------------------------------------------------------


















//---------------------------------------------------------------------------
int handleDynamicParamList(const char *label, uint8_t line, int var, const char** listOptions) {
  // min and max
  int vmin=0;
  int vmax=1;
  
  // vmin and vmax
  for (int i=0; i<999 ;++i) {
    if (listOptions[i]==0) {
      vmax=i-1;
      break;
    }
  }
  // initial fix of var
  if (var<vmin) {
    var = vmin;
  }
  if (var>vmax) {
    var=vmax;
  }
  
  char editIndicator[3];
  strcpy(editIndicator,"");
  if (line == LCDML.MENU_getCursorPos())
  {
    // make only an action when the cursor stands on this menu item
    //check Button
    if(LCDML.BT_checkAny())
    {
      if(LCDML.BT_checkEnter())
      {
        // this function checks returns the scroll disable status (0 = menu scrolling enabled, 1 = menu scrolling disabled)
        if(LCDML.MENU_getScrollDisableStatus() == 0)
        {
          // disable the menu scroll function to catch the cursor on this point
          // now it is possible to work with BT_checkUp and BT_checkDown in this function
          // this function can only be called in a menu, not in a menu function
          LCDML.MENU_disScroll();
        }
        else
        {
          // enable the normal menu scroll function
          LCDML.MENU_enScroll();
        }

        // do something
        // ...
      }

      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkDown() || LCDML.BT_checkRight())
      {
        ++var;
        if (var>vmax) {
          var=vmax;
        }
      }

      // This check have only an effect when MENU_disScroll is set
      if(LCDML.BT_checkUp() || LCDML.BT_checkLeft())
      {
        --var;
        if (var<vmin) {
          var = vmin;
        }
      }
    }

    // highlight if we are editing it
    if(LCDML.MENU_getScrollDisableStatus() != 0) {
      strcpy(editIndicator,"*");
    }
}

  char buf[30];
  char* choicep = listOptions[var];
  sprintf (buf, "%s: %s%s", label, choicep, editIndicator);

  // use the line from function parameters
  jrlcd.setCursor(1, (line + _LCDML_DSIP_use_header));
  jrlcd.print(buf);

  return var;
}
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
void handleDynamicPageSetup(uint8_t param, const char *infostr, const char *endline=NULL) {
    // remmove compiler warnings when the param variable is not used:
    LCDML_UNUSED(param);

    int maxlines = _LCDML_DISP_rows;
    int maxcols = _LCDML_DISP_cols;

    // clear it first?
    jrlcd.clear();

    // we would like to handle newlines properly here

    // setup function
    int curline = 0;
    char* charp = strtok(infostr,"\n");
    do {
      while (strlen(charp)>maxcols && curline < maxlines) {
        char *charpn = charp+maxcols;
        char c = *charpn;
        *charpn='\0';
        jrlcd.setCursor(0, curline);
        jrlcd.print(charp);
        ++curline;
        // restore char we set to 0
        *charpn = c;
        charp = charpn;
      }
      if (curline >= maxlines) {
        break;
      }
      //
      jrlcd.setCursor(0, curline);
      jrlcd.print(charp);
      if (++curline >= maxlines) {
        break;
      }
    } while (charp = strtok ( NULL, "\n"));

    while (curline < maxlines) {
      jrlcd.setCursor(0, curline);
      if (curline == maxlines-1) {
        // we have toom for this
        if (endline && endline[0]!=0) {
          jrlcd.print(endline);
        } else {
          jrlcd.print(F("[click to return]"));
        }
      } else {
        jrlcd.print(F(""));
      }
      ++curline;
    }
}




void handleDynamicPageLoop(FuncHandleDynamicPage callbackfp=NULL, int callbackparam=0) {
  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
    // the quit button works in every DISP function without any checks; it starts the loop_end function
    if(LCDML.BT_checkAny()) { // check if any button is pressed (enter, up, down, left, right)
      // LCDML_goToMenu stops a running menu function and goes to the menu
      //check Button
      if(LCDML.BT_checkEnter())
        { 
          // done
          if (callbackfp!=NULL) {
            bool bretv = callbackfp(callbackparam);
            if (bretv) {
              LCDML.FUNC_goBackToMenu();
            } else {
              // nothing
            }
          } else {
            // return
            LCDML.FUNC_goBackToMenu();
          }
        }
    }
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // you can here reset some global vars or do nothing
  }
}


bool checkAndDoDynamicPageSetup() {
  return LCDML.FUNC_setup();
}


void handleDynamicPage(uint8_t param, const char *infostr, const char *endline=NULL, FuncHandleDynamicPage callbackfp=NULL, int callbackparam=0) {
  bool bretv = checkAndDoDynamicPageSetup();
  if (bretv) {
    handleDynamicPageSetup(param, infostr, endline);
  }
  handleDynamicPageLoop(callbackfp, callbackparam);
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
// wrappers for flash based 
int handleDynamicParam(const __FlashStringHelper *labelfp, uint8_t line, int var, int vmin, int vmax) {
  char label[81];
  jrCopyProgmenStringBuf(label, labelfp, 80);
  return handleDynamicParam(label, line, var, vmin, vmax);
}

int handleDynamicParamList(const __FlashStringHelper *labelfp, uint8_t line, int var, const char** listOptions) {
  char label[81];
  jrCopyProgmenStringBuf(label, labelfp, 80);
  return handleDynamicParamList(label, line, var, listOptions);
}

void handleDynamicPage(uint8_t param, const __FlashStringHelper *infostrfp,  const __FlashStringHelper *endlinefp=NULL, FuncHandleDynamicPage callbackfp=NULL, int callbackparam=0) {
  char infostr[81];
  char endline[21];
  jrCopyProgmenStringBuf(infostr, infostrfp, 80);
  jrCopyProgmenStringBuf(endline, endlinefp, 20);
  handleDynamicPage(param, infostr, endline, callbackfp, callbackparam);
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
void MenuModeQuestionDone(bool leavingFromMenu) {

  // record coffee selection
  updateCoffeeSelectionChoice();

  if (leavingFromMenu) {
    // coming back from a question advances workflow
    //uiInteractionSignify(DefSoundTypeWorkflow); 
    // action
    jrworkflow.advanceWorkflowMode(true);
  }
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void hideEmptyCoffeeTypes() {
//  LCDMenuLib2_menu *menuitemp = LCDML.MENU_getCurrentObj();
//  menuitemp->updateCondetion();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool handleDynamicPageWaitForPress(FuncHandleDynamicPage callbackfp=NULL, int callbackparam=0) {
  if(LCDML.FUNC_setup()) {
    return false;
  }
  
  if(LCDML.FUNC_loop()) {
    // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
    // the quit button works in every DISP function without any checks; it starts the loop_end function
    if(LCDML.BT_checkAny()) { // check if any button is pressed (enter, up, down, left, right)
      // LCDML_goToMenu stops a running menu function and goes to the menu
      //check Button
      if(LCDML.BT_checkEnter())
        { 
          // done
          if (callbackfp!=NULL) {
            bool bretv = callbackfp(callbackparam);
            if (bretv) {
              LCDML.FUNC_goBackToMenu();
            } else {
              // nothing
            }
          } else {
            // return
            LCDML.FUNC_goBackToMenu();
          }
        }
    }
  return true;
  }

  return false;
}
//---------------------------------------------------------------------------




bool MenuHelperGoBackParent() {
  int fid = LCDML.FUNC_getID();
  LCDML.OTHER_jumpToID(fid);
  return;  

  LCDMenuLib2_menu *curmenup = LCDML.MENU_getCurrentObj();
  if (!curmenup) {
    return false;
  }
  int curid = curmenup->getID();
  LCDMenuLib2_menu *par = curmenup->getParent();
  if (!par) {
    return false;
  }
  int parid = par->getID();

    
  //LCDML.OTHER_setCursorToID(parid);
  LCDML.OTHER_jumpToID(curid);
  //LCDML.OTHER_setCursorToID(curid);
  return true;
}
