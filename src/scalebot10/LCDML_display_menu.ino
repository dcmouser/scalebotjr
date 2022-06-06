// provided with LCDML library
// modified by jr 4/25/22


//---------------------------------------------------------------------------
// kludge for dyn menus
int globalLcdmlDynMenuParam = 0;
//---------------------------------------------------------------------------





// =====================================================================
//
// Output function
//
// =====================================================================




//---------------------------------------------------------------------------
/* ******************************************************************** */
void lcdml_menu_clear()
/* ******************************************************************** */
{
  jrlcd.clear();
  jrlcd.setCursor(0, 0);
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
/* ******************************************************************** */
void lcdml_menu_display()
/* ******************************************************************** */
{
  // update content
  // ***************
  if (LCDML.DISP_checkMenuUpdate()) {
    // clear menu
    // ***************
    LCDML.DISP_clear();

    // declaration of some variables
    // ***************
    // content variable
    char content_text[_LCDML_DISP_cols];  // save the content text of every menu element
    // menu element object
    LCDMenuLib2_menu *tmp;
    // some limit values
    uint8_t i = LCDML.MENU_getScroll();
    uint8_t maxi = (_LCDML_DISP_rows - _LCDML_DSIP_use_header) + i;
    uint8_t n = 0;

    // Display a header with the parent element name
    if(_LCDML_DSIP_use_header > 0)
    {
      // only one line
      if(LCDML.MENU_getLayer() == 0)
      {
        // this text is displayed when no header is available
        jrlcd.setCursor(0,0);
        jrlcd.print(F("Root Menu"));       
      }
      else
      {
        // Display parent name
        LCDML_getContent(content_text, LCDML.MENU_getParentID());
        jrlcd.setCursor(0,0);
        jrlcd.print(content_text);       
      }        
    }      

    // check if this element has children
    if ((tmp = LCDML.MENU_getDisplayedObj()) != NULL)
    {
      // loop to display lines
      do
      {
        // ATTN: jr
        // store global param for menu item for use in dynamic functions and conditions
        globalLcdmlDynMenuParam = tmp->getParam();

        // check if a menu element has a condition and if the condition be true
        if (tmp->checkCondition())
        {
          // check the type off a menu element
          if(tmp->checkType_menu() == true)
          {
            // display normal content
            LCDML_getContent(content_text, tmp->getID());
            jrlcd.setCursor(1, (n+_LCDML_DSIP_use_header));
            jrlcd.print(content_text);
          }
          else
          {
            if(tmp->checkType_dynParam()) {
               tmp->callback(n);
            }
          }
          // increment some values
          i++;
          n++;
        }
      // try to go to the next sibling and check the number of displayed rows
      } while (((tmp = tmp->getSibling(1)) != NULL) && (i < maxi));
    }
  }

  if(LCDML.DISP_checkMenuCursorUpdate())
  {
    // init vars
    uint8_t scrollbar_max     = LCDML.MENU_getChilds();
    uint8_t scrollbar_cur_pos = LCDML.MENU_getCursorPosAbs();
    //
    doDrawScrollbar(scrollbar_max, scrollbar_cur_pos);
  }
}
//---------------------------------------------------------------------------





























//---------------------------------------------------------------------------
void doDrawScrollbar(uint8_t scrollbar_max, uint8_t scrollbar_cur_pos) {
    uint8_t n_max             = (scrollbar_max >= (_LCDML_DISP_rows - _LCDML_DSIP_use_header)) ? (_LCDML_DISP_rows - _LCDML_DSIP_use_header) : (scrollbar_max);
    uint8_t scrollbar_min     = 0;
    uint8_t scroll_pos        = ((1.*n_max * (_LCDML_DISP_rows - _LCDML_DSIP_use_header)) / (scrollbar_max - 1) * scrollbar_cur_pos);


    // display rows
    for (uint8_t n = 0; n < n_max; n++)
    {
      //set cursor
      jrlcd.setCursor(0, (n+_LCDML_DSIP_use_header));

      //set cursor char
      if (n == LCDML.MENU_getCursorPos()) {
        jrlcd.write(_LCDML_DISP_cfg_cursor);
      } else {
        jrlcd.write(' ');
      }

      // delete or reset scrollbar
      if (_LCDML_DISP_cfg_scrollbar == 1) {
        if (scrollbar_max > n_max) {
          jrlcd.setCursor((_LCDML_DISP_cols - 1), (n+_LCDML_DSIP_use_header));
          if (false && n==0 && scroll_pos==0) {
            jrlcd.write((uint8_t)1);
          }
          else if (false && n==n_max-1 && scroll_pos+n >= scrollbar_max) {
            jrlcd.write((uint8_t)2);
          }
          else {
            jrlcd.write((uint8_t)0);
          }
        }
        else {
          jrlcd.setCursor((_LCDML_DISP_cols - 1), (n+_LCDML_DSIP_use_header));
          jrlcd.print(' ');
        }
      }
    }

    // display scrollbar
    if (_LCDML_DISP_cfg_scrollbar == 1) {
      if (scrollbar_max > n_max) {
        //set scroll position
        if (scrollbar_cur_pos == scrollbar_min) {
          // min pos
          jrlcd.setCursor((_LCDML_DISP_cols - 1), (0+_LCDML_DSIP_use_header));
          jrlcd.write((uint8_t)1);
        } else if (scrollbar_cur_pos == (scrollbar_max - 1)) {
          // max pos
          jrlcd.setCursor((_LCDML_DISP_cols - 1), (n_max - 1 + _LCDML_DSIP_use_header));
          jrlcd.write((uint8_t)2);
        } else {
          // between
          jrlcd.setCursor((_LCDML_DISP_cols - 1), (scroll_pos / n_max + _LCDML_DSIP_use_header));
          jrlcd.write((uint8_t)(scroll_pos % 4) + 3);
        }
      }
    }
}
//---------------------------------------------------------------------------
