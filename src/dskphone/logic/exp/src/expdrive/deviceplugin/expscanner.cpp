#include "expscanner.h"

#if defined(_EXP_COLOR_SUPPORT) || defined(_EXP_BLACK_SUPPORT)
#include "drivelogic.h"
#include "rjdrive.h"
#include "usbdrive.h"
#include "expscannerdefine.h"
#include "exp38msgprocess.h"
#include "exp40msgprocess.h"
#include "exp50msgprocess.h"
#include "devicelib/phonedevice.h"

#define  NULL_EXP_MSGPROCESS NULL

#define   SAFE_POINTER(p) if ((p) == NULL)  \
{                                           \
    return FALSE;                           \
}                                           \
    (p)


LRESULT CScannner::OnMsg(msgObject & msg)
{
    if (msg.message == TM_TIMER)
    {
        return g_ExpScanner.OnMsgTimer(msg);
    }
    else
    {
        return g_ExpScanner.OnMsgFromScanThread(msg);
    }
}

void CScannner::UnInit()
{
	DestroyMsgProcess(DT_ALL);
    m_bEnableMixedInsert = false;
    m_CurrentExpType = DT_INVALID;
    m_ScanDrive = NULL;
    m_bIsInited = false;
    m_MsgProcessList.clear();
}

void CScannner::RegisterMsgHandle()
{
    RegisterPrivateMsg(EXP_IN);
    RegisterPrivateMsg(EXP_OUT);
    RegisterPrivateMsg(EXP_KEY_DOWN);
    RegisterPrivateMsg(EXP_KEY_UP);
    RegisterPrivateMsg(TM_TIMER);
    RegisterPrivateMsg(DEV_MSG_EXP_KEY_TEST);
}

CScannner::CScannner()
{
    m_bEnableMixedInsert = false;
    m_CurrentExpType = DT_INVALID;
    m_ScanDrive = NULL;
    m_bIsInited = false;
    m_MsgProcessList.clear();
}

CScannner::~CScannner()
{
	DestroyMsgProcess(DT_ALL);
	m_MsgProcessList.clear();
}


CScannner & CScannner::GetInstance()
{
    static CScannner instance;
    return instance;
}

LRESULT CScannner::OnMsgFromScanThread(msgObject & msg)
{
	ExpID expId = msg.wParam;
    DEVICE_TYPE_DEF expType = static_cast<DEVICE_TYPE_DEF>(msg.lParam);

	//ehs和exp分开处理
	if (expType == DT_EXP_EHS)
	{
		return FALSE;
	}

    EXP_INFO("EXP msg from virtual-drive is msg [%d], exptype is [%d] exp id[%d]",
             msg.message - EXP_BEG,
             expType - DT_ALL,
             msg.wParam);

	CBaseExpMsgProcess * pProcess = NULL_EXP_MSGPROCESS;
	if (msg.message == EXP_IN)
	{
		pProcess = GetOrCreateMsgProcessByExpType(expType);//插入时，有则获取，无则新建
	}
	else
	{
		pProcess = GetMsgProcessByExpType(expType);
	}

    switch (msg.message)
    {
    case EXP_IN:
        {
            if (pProcess == NULL)
            {
                return FALSE;
            }

            EXP_INFO("On EXP_IN [%d]", pProcess->GetExpCounts());
            if (pProcess->GetExpCounts() >= MAX_EXP_NUM)
            {
				EXP_INFO("Current exp count over Max, Cannot insert.");
                return FALSE;
            }

            if (!IsAllowExpInsert(expType))
            {
                return FALSE;
            }

            RegisterCurrentExpType(expType);

            InitExpByMsg(msg);

            pProcess->ExpPluginProcess(msg);

            break;
        }

    case EXP_KEY_DOWN:
        {
            EXP_INFO("On EXP_KEY_DOWN");
            PostMsgToApp(DSK_MSG_LIGHT_STATUS_CHANGE, 0, 0);
            SAFE_POINTER(pProcess)->KeyProcess(EXP_KEY_STATUS_PRESS, msg);
            break;
        }

    case EXP_KEY_UP:
        {
            EXP_INFO("On EXP_KEY_UP");
            PostMsgToApp(DSK_MSG_LIGHT_STATUS_CHANGE, 0, 0);
            SAFE_POINTER(pProcess)->KeyProcess(EXP_KEY_STATUS_RELEASE, msg);
            break;
        }

    case DEV_MSG_EXP_KEY_TEST:
        {
            EXP_INFO("On DEV_MSG_EXP_KEY_TEST");
            PostMsgToApp(DSK_MSG_LIGHT_STATUS_CHANGE, 0, 0);
            SAFE_POINTER(pProcess)->KeyProcess(EXP_KEY_STATUS_TEST, msg);
            break;
        }
        break;

    case EXP_OUT:
        {
            if (NULL == pProcess)
            {
                break;
            }

            EXP_INFO("[Exp-Info]: expIndex[%d] expType[%d] has been pull out, current count[%d]", expId, msg.lParam, pProcess->GetExpCounts());

            (pProcess)->ExpRemoveProcess(msg);

			//某类型的exp数量为0，则将该类型process释放
			if (0 == pProcess->GetExpCounts())
			{
				DestroyMsgProcess(pProcess->GetMsgProcessType());
			}
			
			// 最后一台exp拔出时，将当前类型记录为空
			if (0 == GetConnectedExpCounts(DT_ALL))
			{
				RegisterCurrentExpType(DT_INVALID);
			}
			
            break;
        }

    default:

        break;
    }
    return TRUE;
}

LRESULT CScannner::OnMsgTimer(msgObject & msg)
{
    //  转发到各个机型的msgProcess
    for (MsgProcessIter it = m_MsgProcessList.begin(); it != m_MsgProcessList.end(); it++)
    {
        CBaseExpMsgProcess * pProcess = *it;
        if (NULL != pProcess)
        {
            pProcess->OnTimer(msg);
        }
    }
    return 1;
}

void CScannner::Init(EXP_DRIVE_TYPE eDriveType)
{
    if (eDriveType == EXP_DRIVE_TYPE_INVALID)
    {
        return;
    }

    if (m_bIsInited)
    {
        return;
    }

    m_ScanDrive = ::GetDriveByPhoneType(eDriveType);

    if (m_ScanDrive == NULL)
    {
        EXP_ERR("[EXP-Info]get exp drive failed");
        return;
    }

    RegisterMsgHandle();

    m_bIsInited = true;
}


DEVICE_TYPE_DEF CScannner::GetExpTypeByExpIndex(ExpID expId)
{
#ifdef _EXP_COLOR_SUPPORT
    //彩屏扩展台与黑白屏扩展台驱动实现不一致，区分处理
    if (NULL != m_ScanDrive && EXP_DRIVE_TYPE_USB == m_ScanDrive->GetDriverType())
    {
        // 彩屏扩展台识别存在一个过程，识别成功的状态才是连接上
        if (expId >= 0 && g_USBDrive.GetExpStatus(expId) == EXPUSB_MODE_APP && expId < MAX_EXP_NUM)
        {
            return GetCurrentExpType();
        }
    }
#endif

#ifdef _EXP_BLACK_SUPPORT
	if (NULL != m_ScanDrive && EXP_DRIVE_TYPE_RJ == m_ScanDrive->GetDriverType())
    {
		CRJDrive * pDrive = dynamic_cast<CRJDrive *>(m_ScanDrive);
		if (pDrive  == NULL)
		{
			return DT_INVALID;
		}
		//RJ驱动层有记录设备列表
		return pDrive->GetExpTypeByExpIndex(expId);
    }
#endif

    return DT_INVALID;
}

DEVICE_TYPE_DEF CScannner::GetCurrentExpType()
{
    return m_CurrentExpType;
}

int CScannner::GetCurrentPageByExpIndex(ExpID expId)
{
    CBaseExpMsgProcess * pProcess = GetMsgProcessByExpType(GetCurrentExpType());

    if (NULL == pProcess)
    {
        return 0;
    }

    return pProcess->GetCurrentExpPage(expId);
}

bool CScannner::SetCurrentPageByIndex(int iExpID, int iPage)
{
    CBaseExpMsgProcess * pProcess = GetMsgProcessByExpType(GetCurrentExpType());

    if (NULL == pProcess)
    {
        return false;
    }

    pProcess->SetCurrentExpPage(iExpID, iPage);
    return true;
}

int CScannner::GetConnectedExpCounts(DEVICE_TYPE_DEF eType /*= DT_ALL*/)
{
    if (eType == DT_ALL)
    {
        return GetConnectedExpCounts(DT_EXP_38)
               + GetConnectedExpCounts(DT_EXP_40)
               + GetConnectedExpCounts(DT_EXP_20)
               + GetConnectedExpCounts(DT_EXP_39)
               + GetConnectedExpCounts(DT_EXP_CM68)
               + GetConnectedExpCounts(DT_EXP_CM69)
			   + GetConnectedExpCounts(DT_EXP_50);
    }

    if (eType != GetCurrentExpType())
    {
        return 0;
    }

    CBaseExpMsgProcess * pProcess = GetMsgProcessByExpType(eType);

    if (NULL == pProcess)
    {
        return 0;
    }

    return pProcess->GetExpCounts();
}

bool CScannner::IsTestMode()
{
    CBaseExpMsgProcess * pProcess = GetMsgProcessByExpType(GetCurrentExpType());

    if (NULL == pProcess)
    {
        return false;
    }

    return pProcess->IsTestMode();
}

void CScannner::SetTestMode(bool bEnable)
{
    CBaseExpMsgProcess * pProcess = GetMsgProcessByExpType(GetCurrentExpType());

    if (NULL == pProcess)
    {
        return ;
    }

    pProcess->SetTestMode(bEnable);
}

CBaseExpMsgProcess * CScannner::GetMsgProcessByExpType(DEVICE_TYPE_DEF expType)
{
	if (expType == DT_INVALID || expType == DT_ALL)
	{
		return NULL_EXP_MSGPROCESS;
	}

	switch (expType)
	{
	case DT_EXP_38:
	case DT_EXP_50:
		break;
	default:
		expType = DT_EXP_40;
		break;
	}

	for (MsgProcessIter it = m_MsgProcessList.begin(); it != m_MsgProcessList.end(); it++)
	{
		if ((*it)->GetMsgProcessType() == expType)
		{
			return (*it);
		}
	}

	return NULL_EXP_MSGPROCESS;
}

CBaseExpMsgProcess * CScannner::GetOrCreateMsgProcessByExpType(DEVICE_TYPE_DEF expType)
{
	if (expType == DT_INVALID || expType == DT_ALL)
	{
		return NULL_EXP_MSGPROCESS;
	}

	CBaseExpMsgProcess * pProcess = GetMsgProcessByExpType(expType);

	if (pProcess != NULL_EXP_MSGPROCESS)
	{
		return pProcess;
	}

	switch (expType)
	{
#ifdef _EXP_BLACK_SUPPORT
	case DT_EXP_38:
		{
			CBaseExpMsgProcess * pExp38Process = new CExp38MsgProcess();
			m_MsgProcessList.push_back(pExp38Process);
			return pExp38Process;
		}
		break;
#endif
#ifdef _EXP_COLOR_SUPPORT
	case DT_EXP_50:
		{
			CBaseExpMsgProcess * pProcess = new CExp50MsgProcess();
			m_MsgProcessList.push_back(pProcess);
			return pProcess;
		}
		break;
#endif
	default:
#ifdef _EXP_BLACK_SUPPORT
		{
			CBaseExpMsgProcess * pExp40Process = new CExp40MsgProcess();
			m_MsgProcessList.push_back(pExp40Process);
			return pExp40Process;
		}
#endif
		break;
	}

	return NULL_EXP_MSGPROCESS;
}

void CScannner::DestroyMsgProcess(DEVICE_TYPE_DEF expType)
{
	MsgProcessIter iter = m_MsgProcessList.begin();
	while (iter != m_MsgProcessList.end())
	{
		if ((*iter)->GetMsgProcessType() == expType || expType == DT_ALL )
		{
			delete (*iter);
			(*iter) = NULL;
			iter = m_MsgProcessList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CScannner::InitExpByMsg(const msgObject & msg)
{
    ExpID expId = msg.wParam;

    if (m_ScanDrive == NULL)
    {
        EXP_ERR("[Exp-Info] ExpDrive has not been inited , can not init exp[%d]", expId);
        return;
    }

    m_ScanDrive->InitExpByIndex(expId);
}

bool CScannner::IsAllowExpInsert(DEVICE_TYPE_DEF eType)
{
	if (IsAllowMixedInsert())
	{
		return true;
	}

	if (m_CurrentExpType == DT_INVALID)
	{
		return true;
	}
	return m_CurrentExpType == eType;
}

ExpHandle CScannner::GetExpHandle()
{
    if (m_ScanDrive != NULL)
    {
        return m_ScanDrive->GetExpHandle();
    }

    return -1;
}

#endif
