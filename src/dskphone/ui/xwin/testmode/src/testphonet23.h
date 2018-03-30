#ifndef __TEST_PHONE_T23_H__
#define __TEST_PHONE_T23_H__

///////////////////////////CTestPhoneT23///////////////////////////////////
#define T23_LIGHT_NUM 10

#include "testphonet66.h"

class CTestPhoneT23 : public CTestPhoneT66
{
public:
    CTestPhoneT23();
    virtual ~CTestPhoneT23();

    //全屏提示用字体
    virtual int GetNoteTextFont(int nTextType);

    virtual void FlushLightIndex();
    virtual void InitCmpString();
    virtual bool ChangeEchoState(int iKey, ECHO_MODE & eMode);
    virtual int  GetLedMaxStep() {
        return 1;
    }

    // 新需求led测试阶段专用
    virtual void TestAllLightOn(int mLineColor, bool bFuncKey = true);

};

#endif //__TEST_PHONE_T23_H__
