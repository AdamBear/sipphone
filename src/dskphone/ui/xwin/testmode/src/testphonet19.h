#ifndef __TEST_PHONE_T19_H__
#define __TEST_PHONE_T19_H__

///////////////////////////CTestPhoneT19///////////////////////////////////
#include "testphonet66.h"

class CTestPhoneT19 : public CTestPhoneT66
{
public:
    CTestPhoneT19();
    virtual ~CTestPhoneT19();

    // 全屏提示用字体
    virtual int GetNoteTextFont(int nTextType);

    virtual void FlushLightIndex();
    virtual void InitCmpString();
    virtual bool ChangeEchoState(int iKey, ECHO_MODE & eMode);

};
#endif //__TEST_PHONE_T19_H__
