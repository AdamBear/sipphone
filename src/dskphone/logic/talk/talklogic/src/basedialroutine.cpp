#include "talklogic_inc.h"

CBaseDialRoutine::CBaseDialRoutine()
{
}


CBaseDialRoutine::~CBaseDialRoutine()
{
}

void CBaseDialRoutine::AfterRoutineCreate()
{
    m_pCallInfo->m_iDigitKey = PHONE_KEY_NONE;
}

// 处理ui发送过来的Action事件
bool CBaseDialRoutine::OnActionEvent(DataUI2Logic* pActionData)
{
    if (NULL == pActionData)
    {
        return false;
    }

    bool bHandled = false;
    switch (pActionData->eAction)
    {
    case TALK_ACTION_REDIAL:
        {
            bHandled = Redial();
        }
        break;
    case TALK_ACTION_SEND:
        {
            DataDial2Logic* pDialData = (DataDial2Logic*)pActionData->pData;
            if (pDialData == NULL)
            {
                return false;
            }

            return CallOut(pDialData->strNumber, pDialData->strName, pActionData->wActionParam);
        }
        break;
    default:
        bHandled = CBaseRoutine::OnActionEvent(pActionData);
        break;
    }

    return bHandled;
}

bool CBaseDialRoutine::Redial()
{
    yl::string strNumber = configParser_GetConfigString(kszRedialNumber);
    if (!strNumber.empty())
    {
        const yl::string& strDialed = m_pCallInfo->GetDialText();
        // 如果已输入号码且号码和RD号码不同，则用RD号码替换当前输入号码，不呼出
        if (!strDialed.empty() && strDialed != strNumber)
        {
            m_pCallInfo->SetDialedNumber(strNumber);
        }
        else
        {
            // 设置redial的名称
            yl::string strName = configParser_GetConfigString(kszRedialName);
            CallOut(strNumber, strName);
        }

        return true;
    }

    return false;
}

