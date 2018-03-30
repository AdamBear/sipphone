#ifndef __TEST_PHONE_T40_H__
#define __TEST_PHONE_T40_H__

///////////////////////////CTestPhoneT40///////////////////////////////////
#include "testphonet42.h"

class CTestPhoneT40 : public CTestPhoneT42
{
public:
    CTestPhoneT40();
    virtual ~CTestPhoneT40();
    // 全屏提示用字体
    virtual int GetNoteTextFont(int nTextType);
    virtual void FlushLightIndex();
    virtual void InitCmpString();
};

#endif //__TEST_PHONE_T40_H__
