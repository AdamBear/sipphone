#ifndef __TEST_PHONE_T42_H__
#define __TEST_PHONE_T42_H__

#include "testphonet66.h"

///////////////////////////CTestPhoneT42///////////////////////////////////
class CTestPhoneT42 : public CTestPhoneT66
{
public:
    CTestPhoneT42();
    virtual ~CTestPhoneT42();

    virtual void FlushLightIndex();
    virtual void InitCmpString();
    virtual bool ChangeEchoState(int iKey, ECHO_MODE & eMode);
    virtual void FillFullRect(bool bBlack);
    virtual void DrawAllIcon(bool bShow);
    virtual int  GetLedMaxStep() {
        return 3;
    }

    // 新需求led测试阶段专用
    virtual void TestAllLightOn(int mLineColor, bool bFuncKey = true);

    // 对图标的控制
    void DrawIcon(LCD_ICON_TYPE iConType, bool bShow);
};

#endif //__TEST_PHONE_T42_H__
