#ifndef __TEST_PHONE_T52_H__
#define __TEST_PHONE_T52_H__

#if IS_COLOR
#include "testphonet54.h"

///////////////////////////CTestPhoneT54///////////////////////////////////
class CTestPhoneT52 : public CTestPhoneT54
{
public:
    CTestPhoneT52();
    virtual ~CTestPhoneT52();

    virtual void FlushLightIndex();
    virtual void InitCmpString();
    virtual void FillPicture(const yl::string& strPic);
    virtual int GetNoteTextFont(int nTextType);
    virtual void TestAllLightOn(int mLineColor, bool bFuncKey = true);

};

#endif //IS_COLOR
#endif //__TEST_PHONE_T52_H__
