#ifndef __MOCK_MISC_H__
#define  __MOCK_MISC_H__

#include <gmock/gmock.h>
#include <ylstl/ylstring.h>
#include "account/include/account_datadefine.h"
#include "devicelib/phonedevice.h"
class MockMisc
{
public:

    MOCK_METHOD2(pw_data_encrypt, int(const unsigned char * src, unsigned char * dest));
    MOCK_METHOD2(pw_data_decrypt, int(const unsigned char * src, unsigned char * dest));
    MOCK_METHOD1(talklogic_SessionExist, bool(bool bIngoreAutoRedialSession));
    MOCK_METHOD0(IsNetworkReady, BOOL());
    MOCK_METHOD1(modVoip_QueryLineState, LINE_STATE(int i));

    MOCK_METHOD0(devicelib_GetPhoneType, PHONE_TYPE_DEF());

    MOCK_METHOD1(netGetMACText, LPCSTR(ACHAR cSplit));
    MOCK_METHOD1(pathFileExist, BOOL(LPCSTR lpszFileName));

    MOCK_METHOD1(AdapterBox_ResetFactory, void(int iResetType));

    MOCK_METHOD2(idl_SetStatus, bool(int i, const yl::string));
    MOCK_METHOD1(idl_DropStatus, bool(int t));
};


extern MockMisc * g_pMockMisc;



#endif
