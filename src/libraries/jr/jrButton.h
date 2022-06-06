#ifndef jrbuttonH
#define jrbuttonH


//
#include <Bounce2.h>

//---------------------------------------------------------------------------
enum JrPressType {JrPressTypeNone, JrPressTypeShort, JrPressTypeLong};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
class JrButton : public Bounce2::Button{
public:
  JrButton();
  //virtual ~JrButton() {};
public:
	bool wasPressLongPress();
	bool isUnstableOrIndeterminate();
	void waitForButtonRelease();
public:
	JrPressType getPressType();
};
//---------------------------------------------------------------------------




#endif
