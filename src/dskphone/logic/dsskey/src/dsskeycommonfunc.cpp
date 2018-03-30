#include "dsskeycommonfunc.h"
#include "dsskeyaction.h"
#include <stdarg.h>
#include <interfacedefine.h>
#include <cstdlib>
#include "../include/dsskey_def.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "dsskey_common.h"
#include "account/include/account_datadefine.h"
#include "devicelib/phonedevice.h"
#include "dsskeystructure.h"

#define VP_PHONE_THREAD_NAME    "app_vpPhone"

// Max path length
static const int knMaxPathLength = 260;
// Max buffer len.
static const int knMaxBufferLen = 1024;

CDSSKeyAction * CreateDsskeyAction(DssKey_Type dsskeyType)
{
    if (dsskeyType != DT_NONE && dsskeyType != DT_LAST)
    {
        CDSSKeyAction * pAction = new CDSSKeyAction(dsskeyType);
        return pAction;
    }
    return NULL;
}

DSSKEY_MODULE_TYPE GetModuleType(int dsskeyID)
{
    return (DSSKEY_MODULE_TYPE)(dsskeyID >> 24);
}

int GetModuleIndex(int dsskeyID)
{
    return ((dsskeyID >> 6) & 0xff);
}

int GetModuleID(int dsskeyID)
{
    return (dsskeyID & (~GetDsskeyIndex(dsskeyID)));
}

int GetUniDsskeyID(int dsskeyID)
{
    //! 对于不同的exp其id应该需要一致
    DSSKEY_MODULE_TYPE moduleType = GetModuleType(dsskeyID);
    int iModuleIndex = GetModuleIndex(dsskeyID);

    if (IsEXPModule(moduleType))
    {
        moduleType = (DSSKEY_MODULE_TYPE)4;
    }

    return GetDsskeyDataID(moduleType, iModuleIndex, GetDsskeyIndex(dsskeyID));
}

int GetDsskeyDataID(DSSKEY_MODULE_TYPE moduleType, int iExpIndex, int dataIndex)
{
    return ((moduleType << 24) + ((iExpIndex & 0xff) << 6) + dataIndex);
}

int GetModuleIDByModuleIndex(DSSKEY_MODULE_TYPE moduleType, int iExpIndex)
{
    if (moduleType == DMT_NON)
    {
        return INVALID_DSSKEY_ID;
    }

    if (iExpIndex < 0)
    {
        if (IsEXPModule(moduleType))
        {
            return INVALID_DSSKEY_ID;
        }

        iExpIndex = 0;
    }

    return GetDsskeyDataID(moduleType, iExpIndex, 0);
}

int GetDsskeyIndex(int dsskeyID)
{
    return (dsskeyID & 0x003f);
}

bool IsEXPModule(DSSKEY_MODULE_TYPE eModuleType)
{
    return (eModuleType >= DMT_EXP20_MODULE
            && eModuleType <= DMT_EXP40_MODULE)
           || eModuleType == DMT_EXP50_MODULE
           || eModuleType == DMT_EXPCM69_MODULE;
}

bool IsValidStatus(Dsskey_Status status)
{
    switch (status)
    {
    case DS_NON:
    case DS_LINE_FIRST:
    case DS_LINE_END:
    case DS_BLF_FIRST: // 到DS_BLFLAST之间为blf状态
    case DS_BLF_LAST:
    case DS_SCA_CALLPARK_FIRST:
    case DS_SCA_CALLPARK_END:
    case DS_AUTO_PICKUP_FIRST:
    case DS_AUTO_PICKUP_END:
    case DS_ACD_FIRST: // 到DS_ACDLAST之间为ACD状态
    case DS_ACD_LAST:
    case DS_SESSION_FIRST:
    case DS_SESSION_END:
    case DS_FWD_FIRST:
    case DS_FWD_END:
    case DS_DND_FIRST:
    case DS_DND_END:
    case DS_URL_FIRST:
    case DS_URL_END:
    case DS_HOTELING_FIRST:
    case DS_HOTELING_END:
    case DS_RECORD_FIRST:
    case DS_RECORD_END:
    case DS_UC_FIRST:
    case DS_UC_END:
    case DS_NOTIFY_FIRST:
    case DS_NOTIFY_END:
    case DS_AUTOANSWER_END:
    case DS_AUTOANSWER_FIRST:
    case DS_BTLINE_FIRST:
        return false;
    }
    return true;
}


Dsskey_Status getStatusBegin(Dsskey_Status status)
{
    if (IsValidStatus(status))
    {
        if (status > DS_LINE_FIRST && status < DS_LINE_END)
        {
            return DS_LINE_FIRST;
        }

        if (status > DS_BLF_FIRST && status < DS_BLF_LAST)
        {
            return DS_BLF_FIRST;
        }

        if (status > DS_SCA_CALLPARK_FIRST && status < DS_SCA_CALLPARK_END)
        {
            return DS_SCA_CALLPARK_FIRST;
        }

        if (status > DS_AUTO_PICKUP_FIRST && status < DS_AUTO_PICKUP_END)
        {
            return DS_AUTO_PICKUP_FIRST;
        }

        if (status > DS_ACD_FIRST && status < DS_ACD_LAST)
        {
            return DS_ACD_FIRST;
        }

        if (status > DS_SESSION_FIRST && status < DS_SESSION_END)
        {
            return DS_SESSION_FIRST;
        }

        if (status > DS_FWD_FIRST && status < DS_FWD_END)
        {
            return DS_FWD_FIRST;
        }

        if (status > DS_DND_FIRST && status < DS_DND_END)
        {
            return DS_DND_FIRST;
        }

        if (status > DS_URL_FIRST && status < DS_URL_END)
        {
            return DS_URL_FIRST;
        }

        if (status > DS_HOTELING_FIRST && status < DS_HOTELING_END)
        {
            return  DS_HOTELING_FIRST;
        }

        if (status > DS_RECORD_FIRST && status < DS_RECORD_END)
        {
            return DS_RECORD_FIRST;
        }

        if (status > DS_UC_FIRST && status < DS_UC_END)
        {
            return DS_UC_FIRST;
        }
        if (status > DS_NOTIFY_FIRST && status < DS_NOTIFY_END)
        {
            return DS_NOTIFY_FIRST;
        }

        if (status > DS_AUTOANSWER_FIRST && status < DS_AUTOANSWER_END)
        {
            return DS_AUTOANSWER_FIRST;
        }

        if (status > DS_BTLINE_FIRST && status < DS_BTLINE_END)
        {
            return DS_BTLINE_FIRST;
        }

        if (status > DS_XMLBROSER_FIRST && status < DS_XMLBROSER_END)
        {
            return DS_XMLBROSER_FIRST;
        }
    }
    return DS_NON;
}

Dsskey_Status MapLineState(int iState, int lineID)
{
    Dsskey_Status eLineStatus = DS_LINE_DISABLE;
    switch (iState)
    {
    case LS_REGISTERING:
    case LS_UNREGISTERING:
        {
            eLineStatus = DS_LINE_REGISTERING;
        }
        break;
    case LS_DISABLE_REG:
    case LS_REGISTERED:
    case LS_SYSTEM_DEFAULT_ACCOUNT:
        {
#ifdef IF_FEATURE_FORWARD_OPT
            FWD_TYPE eFWDType = FWD_TYPE_ALWAYS;
#else
            FWD_TYPE eFWDType = FWD_TYPE_ALL;
#endif
            //只有注册上了才显示DND和FWD图标
            if (fwd_GetMode() == FWDDND_MODE_ACCOUNT
                    && fwd_GetInfo(lineID, eFWDType))
            {
                eLineStatus = DS_LINE_FWD;
            }
            else if (dnd_GetMode() == FWDDND_MODE_ACCOUNT
                     && dnd_IsEnable(lineID))
            {
                eLineStatus = DS_LINE_DND;
            }
            else
            {
                eLineStatus = DS_LINE_REGISTERED;
            }

        }
        break;
    case LS_INEXISTENCE:
    case LS_UNKNOW:
        {
            eLineStatus = DS_LINE_NONE;
        }
        break;
    case LS_DISABLE:
        {
            eLineStatus = DS_LINE_DISABLE;
        }
        break;
    case LS_USING:
        {
            eLineStatus = DS_LINE_USING;
        }
        break;
    case LS_REGISTER_FAIL:
    case LS_BE_UNREGISTERED:
    default:
        {
            eLineStatus = DS_LINE_UNREGISTERED;
        }
        break;
    }

    return eLineStatus;
}

bool DsskeySendKeyMsg(int nKeyCode, bool bKeyDown /*= true*/)
{
    if (bKeyDown)
    {
        msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DEV_MSG_FUNKEY_EVENT, 0, nKeyCode);
    }
    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DEV_MSG_KEY_RELEASED, 0, nKeyCode);
    return false;
}

int GetAbsoluteIndexByID(int nDsskeyID)
{
    int nAbsoluteIndex = 0;
    DSSKEY_MODULE_TYPE eModuleType = GetModuleType(nDsskeyID);
    int nIndex = GetDsskeyIndex(nDsskeyID);

    // 旧T2X算法
    if (eModuleType == DMT_MEMKEY_MODULE)
    {
        nAbsoluteIndex = nIndex;
    }
    else if (eModuleType == DMT_LINEKEY_MODULE)
    {
        nAbsoluteIndex = nIndex + 0/*FIRST_LINEKEY_INDEX*/;
    }
    else if (IsEXPModule(eModuleType))
    {
        int nExpIndex = GetModuleIndex(nDsskeyID);
        // 扩展台
        nAbsoluteIndex = nIndex + FIRST_EXPKEY_INDEX + nExpIndex * 40;
    }

    return nAbsoluteIndex;
}

// 判断该机型是否支持该扩展台
bool IsSupportModuleType(DSSKEY_MODULE_TYPE eType)
{
    if (DMT_MEMKEY_MODULE == eType)
    {
        return false;
    }

    bool bSupport = true;
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();

    switch (ePhoneType)
    {
    case PT_T69:
    case PT_T69S:
        {
            if (DMT_EXP20_MODULE == eType
                    || DMT_EXP38_MODULE == eType
                    || DMT_EXP39_MODULE == eType
                    || DMT_EXP40_MODULE == eType
                    || DMT_EHS == eType)
            {
                DSSKEY_INFO("T%d Phone not supported Exp%d !", ePhoneType, eType);
                bSupport = false;
            }
        }
        break;
    case PT_T27:
    case PT_T27S:
    case PT_T29:
        {
            if (DMT_EXP40_MODULE == eType
                    || DMT_EXPCM69_MODULE == eType)
            {
                DSSKEY_WARN("T%d Phone not supported Exp%d !", ePhoneType, eType);
                bSupport = false;
            }
        }
        break;
    case PT_T46:
    case PT_T46S:
    case PT_T48:
    case PT_T48S:
        {
            if (DMT_EXP39_MODULE == eType
                    || DMT_EXPCM69_MODULE == eType
                    || DMT_EXP38_MODULE == eType
                    || DMT_EXP20_MODULE == eType)
            {
                bSupport = false;
            }
        }
        break;
    case PT_T54S:
    case PT_T52:
        {
            if (DMT_EXP20_MODULE == eType
                    || DMT_EXP38_MODULE == eType
                    || DMT_EXP39_MODULE == eType
                    || DMT_EXP40_MODULE == eType
                    || DMT_EHS == eType
                    || DMT_EXPCM69_MODULE == eType)
            {
                bSupport = false;
            }
        }
        break;

    case PT_T66:
        {
            if (DMT_EXP40_MODULE == eType
                    || DMT_EXP20_MODULE == eType)
            {
                bSupport = false;
            }
        }
        break;
    case PT_T41:
    case PT_T42:
    case PT_T43S:
    case PT_T43P:
        {
            if (DMT_EXP39_MODULE == eType
                    || DMT_EXP40_MODULE == eType
                    || DMT_EXPCM69_MODULE == eType
                    || DMT_EXP20_MODULE == eType)
            {
                bSupport = false;
            }
        }
        break;
    case PT_T49:
        if (DMT_EXP39_MODULE == eType
                || DMT_EXP40_MODULE == eType
                || DMT_EXPCM69_MODULE == eType
                || DMT_EXP38_MODULE == eType)
        {
            bSupport = false;
        }
        break;
    default:
        break;
    }

    return bSupport;
}

// 获取Dsskye的数量
int GetLinekeyNumber()
{
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();

    switch (ePhoneType)
    {
    case PT_T21:
        {
            return 2;
        }
        break;
    case PT_T61:
        {
            return 6;
        }
        break;
    case PT_T23:
    case PT_T40:
    case PT_T40G:
        {
            return 3;
        }
        break;
    case PT_T27:
    case PT_T27S:
    case PT_T66:
    case PT_T52:
        {
            return 21;
        }
        break;
    case PT_T41:
    case PT_T42:
    case PT_T43S:
    case PT_T43P:
        {
            return 15;
        }
        break;
    case PT_T29:
    case PT_T46:
    case PT_T46S:
    case PT_T54S:
        {
            return 27;
        }
        break;
    case PT_T69:
    case PT_T69S:
        {
            return 15;
        }
        break;
    case PT_T48:
    case PT_T48S:
    case PT_T49:
        {
            return 29;
        }
        break;
    default:
        break;
    }

    return 0;
}
