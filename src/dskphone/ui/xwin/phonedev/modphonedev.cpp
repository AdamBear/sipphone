#include "modphonedev.h"
#include "phonedev.h"
#include "include/path_define.h"
#ifdef _CP920
#include "cp920keypad.h"
#endif

CPhoneDev* GetPhoneDev(const yl::string& strConfigFile)
{
    static CPhoneDev* s_pDev = NULL;

    if (NULL == s_pDev)
    {
        s_pDev = new CPhoneDev;

        s_pDev->readKeyMap(strConfigFile);

        chASSERT(NULL != s_pDev);
    }

    return s_pDev;
}

#ifdef _CP920
bool PhoneDev_IsKeyMute2(int nRawKey)
{
    return CP920_KP_KEY_MUTE2 == nRawKey;
}

bool PhoneDev_IsKeyMute3(int nRawKey)
{
    return CP920_KP_KEY_MUTE3 == nRawKey;
}
#endif
