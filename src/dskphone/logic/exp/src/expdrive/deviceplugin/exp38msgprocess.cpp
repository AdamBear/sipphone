#include "exp38msgprocess.h"

#ifdef _EXP_BLACK_SUPPORT
CExp38MsgProcess::CExp38MsgProcess()
{

}

CExp38MsgProcess::~CExp38MsgProcess()
{

}

void CExp38MsgProcess::KeyProcess(int iKeyStaus, msgObject & msg)
{
    DsskeyID dssId = msg.wParam;

    DEVICE_TYPE_DEF expType = static_cast<DEVICE_TYPE_DEF>(msg.lParam);

//  ExpID expId = GetExpIndexFromDssKeyID(dssId);

    DEV_MESSAGE_CODE msgContent = GetMsgFromKeyStatus(iKeyStaus);

    AutoSetKillLongPressTimer(iKeyStaus);

    if (IsSameKeyUp(dssId, iKeyStaus))
    {
        PostMsgToApp(msgContent, dssId, iKeyStaus);
    }

    RegisterKeyDown(dssId, iKeyStaus);
}
#endif