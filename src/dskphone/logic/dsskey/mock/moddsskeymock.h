#ifndef __MOD_DSSKEY_MOCK_H__
#define __MOD_DSSKEY_MOCK_H__

#include <gmock/gmock.h>


// moddsskey.h接口mock
class DSSKeyMock
{
public:
    MOCK_METHOD2(EHSCtrl, void(int, int));
    MOCK_METHOD2(IsDsskey, bool(int iKeyCode, bool bExp));
};

// 全局mock实例指针
extern DSSKeyMock * g_pDSSKeyMock;

#endif
