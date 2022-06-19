// menu test code
// see https://github.com/Jomelo/LCDMenuLib2




//---------------------------------------------------------------------------
// forward declarations
extern void mDynParamSleep(uint8_t line);
extern void mDynParamDebug(uint8_t line);
extern void mDynParamBeep(uint8_t line);
extern void mDynParamRoundUp(uint8_t line);
extern void mDynParamSoftZero(uint8_t line);
extern void mDynParamAutoZero(uint8_t line);
extern void mDynParam7SegBrightness(uint8_t line);
extern void mDynParamStartMode(uint8_t line);

//
extern void mDynParamGrind(uint8_t line);
extern bool mDynParamGrindCond();
//
extern void mDynPageNetInfo(uint8_t param);
extern void mDynPageDiagnosticsGeneral(uint8_t param);
extern void mDynPageAbout(uint8_t param);
extern void mDynPageScaleData1(uint8_t param);
extern void mDynPageScaleData2(uint8_t param);
extern void mDynParamSmoothMode(uint8_t param);
extern void mDynParamScaleMode(uint8_t param);
extern void mDynParamCalibrationWeight(uint8_t param);
extern void mDynParamCalibrationTweakMethod(uint8_t line);
//
extern void mMenuDoScale1Place0Weight(uint8_t param);
extern void mMenuDoScale1Place100gWeight(uint8_t param);
extern void mMenuDoScale2Place0Weight(uint8_t param);
extern void mMenuDoScale2Place100gWeight(uint8_t param);
extern void mDynParamCalibrationWeight(uint8_t param);
extern void mDynParamBeanJarWeight(uint8_t param);
extern void mMenuDoScaleWeighBeanJar(uint8_t param);
//
extern void mFuncRebootNow(uint8_t param);
extern void mFuncSleepNow(uint8_t param);
extern void myMenuMainExit(uint8_t param);
//
extern void mFunc_back(uint8_t param);
extern void mFunc_goToRootMenu(uint8_t param);
//
extern void mDynParamWfStartsCoffeeSelect(uint8_t line);
extern void mDynParamOptionCheckWarnings(uint8_t line);
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// *********************************************************************
// MENU LCDML display settings
// *********************************************************************
  // menu library
  #include <LCDMenuLib2.h>
  
  // settings for LCD
  #define _LCDML_DISP_cols  DefLcdCols
  #define _LCDML_DISP_rows  DefLcdRows

  // enable this line (set to 1) to show a header above the first menu element
  // this function can be changed in LCDML_display_menu tab
  #define _LCDML_DSIP_use_header 0

  #define _LCDML_DISP_cfg_cursor                     0x7E   // cursor Symbol
  #define _LCDML_DISP_cfg_scrollbar                  1      // enable a scrollbar



// *********************************************************************
// Prototypes
// *********************************************************************
  void lcdml_menu_display();
  void lcdml_menu_clear();
  void lcdml_menu_control();

  bool lcdmMenuHide() { return false;} 

// *********************************************************************
// Objects
// *********************************************************************
  LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
  LCDMenuLib2 LCDML(LCDML_0, _LCDML_DISP_rows-_LCDML_DSIP_use_header, _LCDML_DISP_cols, lcdml_menu_display, lcdml_menu_clear, lcdml_menu_control);

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// *********************************************************************
// LCDML MENU/DISP
// *********************************************************************
  // LCDML_0        => layer 0
  // LCDML_0_X      => layer 1
  // LCDML_0_X_X    => layer 2
  // LCDML_0_X_X_X  => layer 3
  // LCDML_0_...      => layer ...

  // For beginners
  // LCDML_add(id, prev_layer, new_num, lang_char_array, callback_function)
  // LCDMenuLib_addAdvanced(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )

  LCDML_add         (0  , LCDML_0         , 1  , "Information"      , NULL);

    LCDML_add         (1  , LCDML_0_1       , 1  , "Grind size", NULL);

      LCDML_addAdvanced         (2  , LCDML_0_1_1     , 1  , mDynParamGrindCond,"" , mDynParamGrind, 1, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (3  , LCDML_0_1_1     , 2  , mDynParamGrindCond,"" , mDynParamGrind, 2, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (4  , LCDML_0_1_1     , 3  , mDynParamGrindCond,"" , mDynParamGrind, 3, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (5  , LCDML_0_1_1     , 4  , mDynParamGrindCond,"" , mDynParamGrind, 4, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (6  , LCDML_0_1_1     , 5  , mDynParamGrindCond,"" , mDynParamGrind, 5, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (7  , LCDML_0_1_1     , 6  , mDynParamGrindCond,"" , mDynParamGrind, 6, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (8  , LCDML_0_1_1     , 7  , mDynParamGrindCond,"" , mDynParamGrind, 7, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (9  , LCDML_0_1_1     , 8  , mDynParamGrindCond,"" , mDynParamGrind, 8, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (10  , LCDML_0_1_1     , 9  , mDynParamGrindCond,"" , mDynParamGrind, 9, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (11  , LCDML_0_1_1     , 10  , mDynParamGrindCond,"" , mDynParamGrind, 10, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (12  , LCDML_0_1_1     , 11  , mDynParamGrindCond,"" , mDynParamGrind, 11, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (13  , LCDML_0_1_1     , 12  , mDynParamGrindCond,"" , mDynParamGrind, 12, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (14  , LCDML_0_1_1     , 13  , mDynParamGrindCond,"" , mDynParamGrind, 13, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (15  , LCDML_0_1_1     , 14  , mDynParamGrindCond,"" , mDynParamGrind, 14, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (16  , LCDML_0_1_1     , 15  , mDynParamGrindCond,"" , mDynParamGrind, 15, _LCDML_TYPE_dynParam);
      LCDML_addAdvanced         (17  , LCDML_0_1_1     , 16  , mDynParamGrindCond,"" , mDynParamGrind, 16, _LCDML_TYPE_dynParam);

      LCDML_add         (18  , LCDML_0_1_1     , 17  , "Back"             , mFunc_back);  

    LCDML_add         (19  , LCDML_0_1     , 4  , "Diagnostics"      , NULL);
      LCDML_add         (20  , LCDML_0_1_4     , 1  , "General"      , mDynPageDiagnosticsGeneral);
      LCDML_add         (21  , LCDML_0_1_4     , 2  , "NetInfo"      , mDynPageNetInfo);
      LCDML_add         (22  , LCDML_0_1_4     , 3  , "Scale 1 data"      , mDynPageScaleData1);
      LCDML_add         (23  , LCDML_0_1_4     , 4  , "Scale 2 data"      , mDynPageScaleData2);
      LCDML_add         (24  , LCDML_0_1_4     , 5  , "Back"             , mFunc_back);  
    LCDML_add         (25  , LCDML_0_1     , 5  , "About"      , mDynPageAbout);
    LCDML_add         (26  , LCDML_0_1     , 6  , "Back"             , mFunc_back);  
  
  LCDML_add         (27  , LCDML_0         , 2  , "Options"          , NULL);

    LCDML_add         (28  , LCDML_0_2       , 1  , "Calibrate scale 1", NULL);
      LCDML_add         (29  , LCDML_0_2_1     , 1  , "Place 0 weight"         , mMenuDoScale1Place0Weight);
      LCDML_add         (30  , LCDML_0_2_1     , 2  , "Place heavy weight"       , mMenuDoScale1Place100gWeight);

      LCDML_add         (31  , LCDML_0_2_1     , 3  , "Back"             , mFunc_back);

    LCDML_add           (32  , LCDML_0_2       , 2  , "Calibrate scale 2", NULL);
      LCDML_add         (33  , LCDML_0_2_2     , 1  , "Place 0 weight"         , mMenuDoScale2Place0Weight);
      LCDML_add         (34  , LCDML_0_2_2     , 2  , "Place heavy weight"       , mMenuDoScale2Place100gWeight);

      LCDML_add         (35  , LCDML_0_2_2     , 3  , "Back"             , mFunc_back);

    LCDML_addAdvanced   (36  , LCDML_0_2     , 3  , NULL,""         , mDynParamCalibrationWeight,1, _LCDML_TYPE_dynParam);
    LCDML_add         (37  , LCDML_0_2     , 4  , "Weigh bean jar"       , mMenuDoScaleWeighBeanJar);

    LCDML_addAdvanced (38 , LCDML_0_2       , 5  , NULL,          ""                  , mDynParamStartMode,              0,   _LCDML_TYPE_dynParam);
    LCDML_addAdvanced (39 , LCDML_0_2       , 6  , NULL,          ""                  , mDynParamScaleMode,              0,   _LCDML_TYPE_dynParam);
    LCDML_addAdvanced (40 , LCDML_0_2       , 7  , NULL,          ""                  , mDynParamSmoothMode,              0,   _LCDML_TYPE_dynParam);
    LCDML_addAdvanced (41 , LCDML_0_2       , 8  , NULL,          ""                  , mDynParamCalibrationTweakMethod,  0,   _LCDML_TYPE_dynParam);
    //LCDML_addAdvanced (42 , LCDML_0_2       , 9  , lcdmMenuHide,          ""                  , mDynParamAutoZero,            0,   _LCDML_TYPE_dynParam);
    LCDML_addAdvanced (42 , LCDML_0_2       , 9  , NULL,          ""                  , mDynParamAutoZero,            0,   _LCDML_TYPE_dynParam);
    LCDML_addAdvanced (43 , LCDML_0_2       , 10 , NULL,          ""                  , mDynParamCalibrationWeight,            0,   _LCDML_TYPE_dynParam);
    LCDML_addAdvanced (44 , LCDML_0_2       , 11 , NULL,          ""                  , mDynParamSleep,                  0,   _LCDML_TYPE_dynParam);
    LCDML_addAdvanced (45 , LCDML_0_2       , 12 , NULL,          ""                  , mDynParamDebug,                  0,   _LCDML_TYPE_dynParam);
    LCDML_addAdvanced (46 , LCDML_0_2       , 13 , NULL,          ""                  , mDynParamBeep,                   0,   _LCDML_TYPE_dynParam);
    LCDML_addAdvanced (47 , LCDML_0_2       , 14  , NULL,          ""                  , mDynParam7SegBrightness,         0,   _LCDML_TYPE_dynParam);
    LCDML_addAdvanced (48 , LCDML_0_2       , 15  , NULL,          ""                  , mDynParamRoundUp,                0,   _LCDML_TYPE_dynParam);
    LCDML_addAdvanced (49 , LCDML_0_2       , 16  , NULL,          ""                  , mDynParamSoftZero,                0,   _LCDML_TYPE_dynParam);
    LCDML_addAdvanced (50 , LCDML_0_2       , 17  , NULL,          ""                  , mDynParamWfStartsCoffeeSelect,                0,   _LCDML_TYPE_dynParam);
    LCDML_addAdvanced (51 , LCDML_0_2       , 18  , NULL,          ""                  , mDynParamOptionCheckWarnings,                0,   _LCDML_TYPE_dynParam);    
    
    LCDML_add         (52 , LCDML_0_2       , 19  , "Reboot now"   , mFuncRebootNow);
    LCDML_add         (53 , LCDML_0_2       , 20  , "Sleep now"   , mFuncSleepNow);
    LCDML_add         (54 , LCDML_0_2       , 21  , "Back"         , mFunc_back);
      
  LCDML_add         (55, LCDML_0         , 3  , "Exit menu"       , myMenuMainExit);


// ATTN: Getting this next value wrong _LCDML_DISP_cnt will cause a lot of macro compilation errors
// in fact this hellish mess will throw all sorts of crazy compilation errors due to macros even if the error is in another file
  // ***TIP*** Try to update _LCDML_DISP_cnt when you add a menu element.
  // menu element count - last element id
  // this value must be the same as the last menu element

  #define _LCDML_DISP_cnt    55
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// create menu (MACRO)
LCDML_createMenu(_LCDML_DISP_cnt);
//---------------------------------------------------------------------------
