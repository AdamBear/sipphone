#ifndef __TEST_PHONE_T54_H__
#define __TEST_PHONE_T54_H__

#if IS_COLOR
#include "testphonet66.h"

///////////////////////////CTestPhoneT54///////////////////////////////////
class CTestPhoneT54 : public CTestPhoneT66
{
public:
    CTestPhoneT54();
    virtual ~CTestPhoneT54();

    virtual void TestSFPInitial();

    virtual void FlushLightIndex();
    virtual void InitCmpString();
    virtual bool ChangeEchoState(int iKey, ECHO_MODE & eMode);

    virtual void DrawRpsString(bool bRpsStatus, LPCTSTR lpszRandnum, LPCSTR lpszCert);
    virtual void DrawStatusViewString(LPCSTR lpszTest);
    virtual void FillPicture(const yl::string& strPic);
    virtual void DrawContentString(LPCTSTR lpszTest, xColor color = xColor(0x00, 0x00, 0x00));

    virtual TEST_STATE MapStateOfKey(int iKey, bool EXPKey = false);
    virtual void DrawKeyString(bool bCorrect, int iIndex);
    virtual void TestAllLightOn(int mLineColor, bool bFuncKey = true);
    virtual int  GetLedMaxStep() {
        return 3;
    }

    virtual int GetNoteTextFont(int nTextType);

};

#endif //IS_COLOR
#endif //__TEST_PHONE_T54_H__
