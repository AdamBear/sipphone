#ifndef __TEST_PHONE_T46_H__
#define __TEST_PHONE_T46_H__

#if IS_COLOR
#include "testphonet54.h"

///////////////////////////CTestPhoneT54///////////////////////////////////
class CTestPhoneT46 : public CTestPhoneT54
{
public:
	CTestPhoneT46();
	virtual ~CTestPhoneT46();

	virtual void FlushLightIndex();
	virtual void InitCmpString();

	virtual bool ChangeEchoState(int iKey, ECHO_MODE & eMode);
	virtual void TestAllLightOn(int mLineColor, bool bFuncKey = true);
};

#endif //IS_COLOR
#endif //__TEST_PHONE_T46_H__
