#ifndef __TEST_PHONE_T21_H__
#define __TEST_PHONE_T21_H__

///////////////////////////CTestPhoneT21///////////////////////////////////
#include "testphonet66.h"

class CTestPhoneT21 : public CTestPhoneT66
{
public:
    CTestPhoneT21();
    virtual ~CTestPhoneT21();

    // 全屏提示用字体
    virtual int GetNoteTextFont(int nTextType);

    virtual void FlushLightIndex();
    virtual void InitCmpString();
    virtual bool ChangeEchoState(int iKey, ECHO_MODE & eMode);
    virtual int  GetLedMaxStep() {
        return 1;
    }

};

#endif //__TEST_PHONE_T21_H__
