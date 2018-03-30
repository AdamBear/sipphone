#include "rjdrive.h"
#if defined(_EHS_SUPPORT) || defined(_EXP_BLACK_SUPPORT)
#include "exp_drv.h"
#include "expscannerdefine.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"
#include "taskaction/modtaskaction.h"
#include "commonunits/commonunits_def.h"

#define EXP_EVN_INVALID -1

CRJDrive & CRJDrive::GetInstance()
{
    static CRJDrive instance;
    return instance;
}

CRJDrive::CRJDrive() :
    m_ExpHandle(EXP_DRIVER_UN_INITED)
{
    memset(&m_ExpEvent, 0, sizeof(m_ExpEvent));
    m_eDriveType = EXP_DRIVE_TYPE_RJ;
    m_vecDeviceList.clear();
}

CRJDrive::CRJDrive(const CRJDrive &)
{

}

CRJDrive::~CRJDrive()
{
}

static IMPLEMENT_THREAD_METHOD(RJScanThread)
{
    g_RJDrive.RjScanFunc();
    return NULL;
}

void CRJDrive::InitDrive()
{
    if (IsInited())
    {
        return;
    }
    m_ExpHandle = open(EXP_DEV_NAME, O_RDWR);

    //设备列表初始化
    m_vecDeviceList.clear();
    for (int i = 0; i < MAX_EXP_NUM + 1; i++)
    {
        m_vecDeviceList.push_back(DT_INVALID);
    }

    // 黑白屏exp 如果先插上再上电启动话机，不会上报插入消息
    // 先直接初始化已经连接上的exp
    InitConnectedExp();
}

void CRJDrive::InitExpByIndex(ExpID expId)
{
    exp_helper_init(m_ExpHandle, expId);
}

DEVICE_TYPE_DEF CRJDrive::GetExpTypeByIndex(ExpID expId)
{
    ExpType expType = -1;

    exp_helper_get_hoptype(m_ExpHandle, expId, &expType);

    expType = FixExpTypeByHW(expType, expId);

    return ExpType2DevType(expType);
}


int CRJDrive::GetConnectedExpCounts()
{
    int iExpCounts = -1;
    if (!IsInited())
    {
        return iExpCounts;
    }
    exp_helper_get_hopcount(m_ExpHandle, &iExpCounts);
    return iExpCounts;
}

DEVICE_TYPE_DEF CRJDrive::GetEventDevType(int iEventType, int iExpIndex)
{
    // exp拔出时，驱动是无从获知拔出的类型的，是由上层记录的
    if (iEventType == EXP_EVN_OFFLINE)
    {
        return GetExpTypeByExpIndex(iExpIndex);
    }

    return GetExpTypeByIndex(iExpIndex);
}

int CRJDrive::GetEventType()
{
    if (!IsInited())
    {
        return EXP_EVN_INVALID;
    }
    if (exp_helper_event(m_ExpHandle, &m_ExpEvent) == 0)
    {
        return (m_ExpEvent.code);
    }
    return EXP_EVN_INVALID;
}


ExpID CRJDrive::GetEventExpID()
{
    if (!IsInited())
    {
        return EXP_INVALID_EXP_ID;
    }
    return m_ExpEvent.hop;
}


KeyID CRJDrive::GetEventKeyID()
{
    if (!IsInited())
    {
        return EXP_INVALID_KEY_ID;
    }

    return (m_ExpEvent.row << 8) + m_ExpEvent.col;
}

DsskeyID CRJDrive::KeyCode2DsskeyID(KeyID keyId, DEVICE_TYPE_DEF eType, ExpID expId)
{
    int iKeyAfterMap = EXP_KEY_NONE;
    // 为测试方便，暂时不判断EXP
    if (DT_EXP_38 == eType)
    {
        switch (keyId)
        {
        case EXP38_KEY_1:
            iKeyAfterMap = 0;
            break;
        case EXP38_KEY_2:
            iKeyAfterMap = 1;
            break;
        case EXP38_KEY_3:
            iKeyAfterMap = 2;
            break;
        case EXP38_KEY_4:
            iKeyAfterMap = 3;
            break;
        case EXP38_KEY_5:
            iKeyAfterMap = 4;
            break;
        case EXP38_KEY_6:
            iKeyAfterMap = 5;
            break;
        case EXP38_KEY_7:
            iKeyAfterMap = 6;
            break;
        case EXP38_KEY_8:
            iKeyAfterMap = 7;
            break;
        case EXP38_KEY_9:
            iKeyAfterMap = 8;
            break;
        case EXP38_KEY_10:
            iKeyAfterMap = 9;
            break;
        case EXP38_KEY_11:
            iKeyAfterMap = 10;
            break;
        case EXP38_KEY_12:
            iKeyAfterMap = 11;
            break;
        case EXP38_KEY_13:
            iKeyAfterMap = 12;
            break;
        case EXP38_KEY_14:
            iKeyAfterMap = 13;
            break;
        case EXP38_KEY_15:
            iKeyAfterMap = 14;
            break;
        case EXP38_KEY_16:
            iKeyAfterMap = 15;
            break;
        case EXP38_KEY_17:
            iKeyAfterMap = 16;
            break;
        case EXP38_KEY_18:
            iKeyAfterMap = 17;
            break;
        case EXP38_KEY_19:
            iKeyAfterMap = 18;
            break;
        case EXP38_KEY_20:
            iKeyAfterMap = 19;
            break;
        case EXP38_KEY_21:
            iKeyAfterMap = 20;
            break;
        case EXP38_KEY_22:
            iKeyAfterMap = 21;
            break;
        case EXP38_KEY_23:
            iKeyAfterMap = 22;
            break;
        case EXP38_KEY_24:
            iKeyAfterMap = 23;
            break;
        case EXP38_KEY_25:
            iKeyAfterMap = 24;
            break;
        case EXP38_KEY_26:
            iKeyAfterMap = 25;
            break;
        case EXP38_KEY_27:
            iKeyAfterMap = 26;
            break;
        case EXP38_KEY_28:
            iKeyAfterMap = 27;
            break;
        case EXP38_KEY_29:
            iKeyAfterMap = 28;
            break;
        case EXP38_KEY_30:
            iKeyAfterMap = 29;
            break;
        case EXP38_KEY_31:
            iKeyAfterMap = 30;
            break;
        case EXP38_KEY_32:
            iKeyAfterMap = 31;
            break;
        case EXP38_KEY_33:
            iKeyAfterMap = 32;
            break;
        case EXP38_KEY_34:
            iKeyAfterMap = 33;
            break;
        case EXP38_KEY_35:
            iKeyAfterMap = 34;
            break;
        case EXP38_KEY_36:
            iKeyAfterMap = 35;
            break;
        case EXP38_KEY_37:
            iKeyAfterMap = 36;
            break;
        case EXP38_KEY_38:
            iKeyAfterMap = 37;
            break;
        default:
            break;
        }
    }
    else if (DT_EXP_20 == eType
             || DT_EXP_39 == eType
             || DT_EXP_40 == eType
             || DT_EXP_CM69 == eType)
    {
        switch (keyId)
        {
        case EXP_INSERT:
            iKeyAfterMap = EXP_INSERT;
            break;
        case EXP_REMOVE:
            iKeyAfterMap = EXP_REMOVE;
            break;
        case EXP_KEY_RELEASE:
            iKeyAfterMap = EXP_KEY_RELEASE;
            break;
        case EXP40_KEY_1:
            iKeyAfterMap = 0;
            break;
        case EXP40_KEY_2:
            iKeyAfterMap = 1;
            break;
        case EXP40_KEY_3:
            iKeyAfterMap = 2;
            break;
        case EXP40_KEY_4:
            iKeyAfterMap = 3;
            break;
        case EXP40_KEY_5:
            iKeyAfterMap = 4;
            break;
        case EXP40_KEY_6:
            iKeyAfterMap = 5;
            break;
        case EXP40_KEY_7:
            iKeyAfterMap = 6;
            break;
        case EXP40_KEY_8:
            iKeyAfterMap = 7;
            break;
        case EXP40_KEY_9:
            iKeyAfterMap = 8;
            break;
        case EXP40_KEY_10:
            iKeyAfterMap = 9;
            break;
        case EXP40_KEY_11:
            iKeyAfterMap = 10;
            break;
        case EXP40_KEY_12:
            iKeyAfterMap = 11;
            break;
        case EXP40_KEY_13:
            iKeyAfterMap = 12;
            break;
        case EXP40_KEY_14:
            iKeyAfterMap = 13;
            break;
        case EXP40_KEY_15:
            iKeyAfterMap = 14;
            break;
        case EXP40_KEY_16:
            iKeyAfterMap = 15;
            break;
        case EXP40_KEY_17:
            iKeyAfterMap = 16;
            break;
        case EXP40_KEY_18:
            iKeyAfterMap = 17;
            break;
        case EXP40_KEY_19:
            iKeyAfterMap = 18;
            break;
        case EXP40_KEY_20:
            iKeyAfterMap = 19;
            break;
        case EXP40_KEY_PAGE1:
            iKeyAfterMap = 20;
            break;
        case EXP40_KEY_PAGE2:
            iKeyAfterMap = 21;
            break;
        default:
            break;
        }
    }

    return ((eType << 24) + (expId << 6) + iKeyAfterMap);
}


void CRJDrive::StartKeyScan()
{
    return;
}

void CRJDrive::StartInsertScan()
{
    TaskAction_CreateSpecialThreadByMethod(RJScanThread, TA_THREAD_RJEXP_INSERT_SCAN);
}

LRESULT CRJDrive::RjScanFunc()
{
    while (1)
    {
#ifdef WIN32
        Sleep(3600 * 1000);
#endif
        int evnType = GetEventType();

        if (evnType == EXP_EVN_INVALID)
        {
            continue;
        }

        ExpID expId = GetEventExpID();
        DEVICE_TYPE_DEF expType = GetEventDevType(evnType, expId);
        DsskeyID dssId = KeyCode2DsskeyID(GetEventKeyID(), expType, expId);
        EXP_SCANNER_MESSAGE msg = GetMsgByEventType(evnType);

        EXP_INFO("RjDrive: event type from drive is [%d] exp id [%d] dsskeyid[%d] exptype [%d] msg[%d]"
                 , evnType - EXP_EVN_OFFLINE
                 , expId
                 , dssId
                 , expType
                 , msg - EXP_BEG);

        if (msg <= EXP_BEG || msg >= EXP_END)
        {
            continue;
        }

        //当为扫描结果为按键操作时，expId值为dsskey的Id
        if (msg == EXP_OUT || msg == EXP_IN)
        {
            if (msg == EXP_IN)
            {
                RecordExpTypeWithExpId(expId, expType);
            }
            else
            {
                RecordExpTypeWithExpId(expId, DT_INVALID);
            }

            PostMsgToApp(msg, expId, expType);
        }
        else
        {
            PostMsgToApp(msg, dssId, expType);
        }
    }
    return 1;
}

void CRJDrive::EhsCtrl(ExpID expId, int iCrtlCode)
{
    exp_helper_set_ehs_hook(m_ExpHandle, expId, iCrtlCode);
}

void CRJDrive::InitConnectedExp()
{
    int iExpCounts = GetConnectedExpCounts();

    for (ExpID expId = 0; expId < iExpCounts; expId++)
    {
        DEVICE_TYPE_DEF expType = GetExpTypeByIndex(expId);

        RecordExpTypeWithExpId(expId, expType);
        PostMsgToApp(EXP_IN, expId, expType);
    }

}

ExpType CRJDrive::FixExpTypeByHW(ExpType expType, ExpID expId)
{
    if (expType == EXP_TYPE_39)
    {
        char szHwVersion[128] = { 0 };

        exp_helper_get_hwversion(m_ExpHandle, expId, 128, szHwVersion);

        char szVersioin1[128] = { 0 };
        char szVersioin2[128] = { 0 };
        char szVersioin3[128] = { 0 };

        if (3 == sscanf(szHwVersion, "%[^.].%[^.].%s", szVersioin1, szVersioin2, szVersioin3)
                && strcmp(szVersioin2, EXP20_HWV) == 0)
        {
            expType = EXP_TYPE_20;
        }
    }

    return expType;
}

EXP_SCANNER_MESSAGE CRJDrive::GetMsgByEventType(int iEventCode)
{
    EXP_SCANNER_MESSAGE msg = EXP_BEG;
    switch (iEventCode)
    {
    case EXP_EVN_OFFLINE:
        msg = EXP_OUT;
        break;
    case EXP_EVN_READY:
        msg = EXP_IN;
        break;
    case EXP_EVN_KEYDOWN:
        msg = EXP_KEY_DOWN;
        break;
    case EXP_EVN_KEYUP:
        msg = EXP_KEY_UP;
        break;
    case EXP_EVN_ONLINE:
        msg = EXP_END;
        break;
    case EXP_EVN_WIRELESS:
        PostMsgToApp(DEV_MSG_EHS_KEY, 0, 0);
        msg = EXP_END;
        break;
    default:
        msg = EXP_END;
        break;
    }
    return msg;
}

DEVICE_TYPE_DEF CRJDrive::ExpType2DevType(ExpType expType)
{
    switch (expType)
    {
    case EXP_TYPE_38:
        return DT_EXP_38;
        break;
    case EXP_TYPE_39:
        return DT_EXP_39;
        break;
    case EXP_TYPE_40:
        return DT_EXP_40;
        break;
    case EXP_TYPE_EHS:
        return DT_EXP_EHS;
        break;
    case EXP_TYPE_CM68:
        return DT_EXP_CM68;
        break;
    case EXP_TYPE_CM69:
        return DT_EXP_CM69;
        break;
    case EXP_TYPE_20:
        return DT_EXP_20;
        break;
    default:
        return DT_INVALID;
    }
    return DT_INVALID;
}

DEVICE_TYPE_DEF CRJDrive::GetExpTypeByExpIndex(ExpID expId)
{
    if (expId > MAX_EXP_NUM)
    {
        return DT_INVALID;
    }

    return m_vecDeviceList[expId];
}

bool CRJDrive::RecordExpTypeWithExpId(ExpID expId, DEVICE_TYPE_DEF eType)
{
    if (expId >= 0 && expId < MAX_EXP_NUM
            && eType != DT_INVALID)
    {
        m_vecDeviceList[expId] = eType;
        return true;
    }

    return false;
}

ExpHandle CRJDrive::GetExpHandle()
{
    return m_ExpHandle;
}

#endif
