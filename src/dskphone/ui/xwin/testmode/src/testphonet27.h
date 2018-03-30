#ifndef __TESTMODE_TESTPHONET27_H__
#define __TESTMODE_TESTPHONET27_H__

#include "testphonet66.h"

//和T66大致形同，直接复用T66测试模式
class CTestPhoneT27
    : public CTestPhoneT66
{
public:
    CTestPhoneT27();
    virtual ~CTestPhoneT27();

    virtual void FlushLightIndex();
    virtual void InitCmpString();
    virtual bool ChangeEchoState(int iKey, ECHO_MODE & eMode);
    virtual int  GetLedMaxStep() {
        return 3;
    }

    // 新需求led测试阶段专用
    virtual void TestAllLightOn(int mLineColor, bool bFuncKey = true);

    //全屏提示用字体
    virtual int GetNoteTextFont(int nTextType);

};


#endif //__TESTMODE_TESTPHONET66_H__


