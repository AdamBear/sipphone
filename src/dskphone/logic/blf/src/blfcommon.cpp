#include "blfcommon.h"
#include "blfpraseaction.h"
#include "service_sip_define.h"
#include "contacts/directory/include/moddirinterface.h"
#include "dsskey/include/moddsskey.h"
#include "account/include/modaccount.h"


int GetStatusLevels(int cStatus)
{
    ////----qt
    //confirmed、trying->early、proceeding->terminated->unavailable
    // 定义各状态优先级,数值越低,优先级越高,0为最高优先级
    switch (cStatus)
    {
    case DS_BLF_STATUS_RINGING:
        {
            return 0;
        }
        break;
    case DS_BLF_STATUS_TALKING:
        {
            return 1;
        }
        break;
    case DS_BLF_STATUS_CALLOUT:
        {
            return 2;
        }
        break;
    case DS_BLF_STATUS_PARK:
        {
            return 3;
        }
        break;
    case DS_BLF_STATUS_HOLD:
        {
            return 4;
        }
        break;
    case DS_BLF_STATUS_DND:
        {
            return 5;
        }
        break;
    case DS_BLF_STATUS_IDLE:
        {
            return 6;
        }
        break;
    case DS_BLF_STATUS_UNKOWN:
        {
            return 7;
        }
        break;
    default:
        {
            return 10;
        }
        break;
    }
}

BLFBaseData::~BLFBaseData()
{
    ClearDialogList();
}

BLFBaseData::BLFBaseData()
{
    m_bIsFullStatus = false;

    m_nVersion = -1;
}

void BLFBaseData::ClearDialogList()
{
    for (YLList<DialgInfo *>::ListIterator iter = m_ListDialgInfo.begin();
            iter != m_ListDialgInfo.end(); iter++)
    {
        DialgInfo * pData = *iter;

        if (pData != NULL)
        {
            delete pData;
        }
    }

    m_ListDialgInfo.clear();
}

bool BLFBaseData::AddDialog(DialgInfo * pDialog)
{
    bool bFind = false;
    for (YLList<DialgInfo *>::ListIterator iter = m_ListDialgInfo.begin();
            iter != m_ListDialgInfo.end(); iter++)
    {
        DialgInfo * pData = *iter;

        if (pData == NULL)
        {
            continue;
        }

        if (GetStatusLevels(pData->m_eStatus) > GetStatusLevels(pDialog->m_eStatus))
        {
            m_ListDialgInfo.insert(iter, pDialog);
            bFind = true;
            break;
        }
    }

    if (!bFind)
    {
        m_ListDialgInfo.push_back(pDialog);
    }

    return !bFind;
}

bool BLFBaseData::IsMatch(DialgInfo * pDialog)
{
    for (YLList<DialgInfo *>::ListIterator iter = m_ListDialgInfo.begin();
            iter != m_ListDialgInfo.end(); iter++)
    {
        DialgInfo * pData = *iter;

        if (pData == NULL)
        {
            continue;
        }

        if (pData->m_strDialgId == pDialog->m_strDialgId
                && pData->m_eStatus == pDialog->m_eStatus
                && pData->m_strDisplayName == pDialog->m_strDisplayName)
        {
            return true;
        }
    }

    return false;
}

DialgInfo * BLFBaseData::GetDialogInfoByIndex(int index /* = 0 */)
{
    int i = 0;

    for (YLList<DialgInfo *>::ListIterator iter = m_ListDialgInfo.begin();
            iter != m_ListDialgInfo.end(); iter++)
    {
        if (index == i)
        {
            return *iter;
        }

        i++;
    }

    return NULL;
}

Dsskey_Status BLFBaseData::GetShowStatus()
{
    if (m_ListDialgInfo.size() == 0)
    {
        return DS_BLF_STATUS_UNKOWN;
    }
    else
    {
        YLList<DialgInfo *>::ListIterator iter = m_ListDialgInfo.begin();

        DialgInfo * pData = *iter;

        if (pData != NULL)
        {
            return pData->m_eStatus;
        }
        else
        {
            return DS_BLF_STATUS_UNKOWN;
        }
    }
}

int BLFBaseData::GetMode()
{
    if (m_ListDialgInfo.size() == 0)
    {
        return 0;
    }
    else
    {
        YLList<DialgInfo *>::ListIterator iter = m_ListDialgInfo.begin();

        DialgInfo * pData = *iter;

        if (pData != NULL)
        {
            return pData->m_nMode;
        }
        else
        {
            return 0;
        }
    }
}

yl::string BLFBaseData::GetDisplayName()
{
    yl::string strName;

    Dir_GetCotactNameByNumber(m_strNumber, strName);

    if (strName.empty() && !m_strExten.empty())
    {
        Dir_GetCotactNameByNumber(m_strExten, strName);
    }

    if (strName.empty())
    {
        if (!m_strDisplayName.empty())
        {
            return m_strDisplayName;
        }
        strName = m_strExten.empty() ? m_strNumber : m_strExten;
    }

    return strName;
}

void BLFBaseData::UbindDsskeyByLine(int nLine)
{
    if (m_nBindDsskeyId.size() <= 1)
    {
        m_nBindDsskeyId.clear();
        return;
    }

    for (int i = m_nBindDsskeyId.size() - 1; i >= 0 ; i--)
    {
        if (dsskey_GetDsskeyLine(m_nBindDsskeyId[i]) == nLine)
        {
            m_nBindDsskeyId.removeAt(i);
        }
    }
}

void BLFBaseData::UbindDsskeyByDsskeyId(int nId)
{
    if (m_nBindDsskeyId.size() <= 1)
    {
        m_nBindDsskeyId.clear();
        return;
    }

    for (int i = m_nBindDsskeyId.size() - 1; i >= 0 ; i--)
    {
        if (m_nBindDsskeyId[i] == nId)
        {
            m_nBindDsskeyId.removeAt(i);
        }
    }
}

void BLFBaseData::CheckBindDsskey(int iDsskeyId, int iAccountId, const yl::string & strNumber)
{
    for (int i = m_nBindDsskeyId.size() - 1; i >= 0 ; i--)
    {
        DssKeyLogicData objData;
        dsskey_GetData(m_nBindDsskeyId[i], objData);
        // 原来该dsskey的类型已经被修改成别的类型了
        if (objData.eDKType != DT_BLFLIST)
        {
            DSSKEY_INFO("BLFLIST Remove Bind Dsskey [%d]", m_nBindDsskeyId[i]);
            m_nBindDsskeyId.removeAt(i);
        }
        else
        {
            // 该dsskey监控的号码已经改变
            if (strNumber != objData.strValue)
            {
                DSSKEY_INFO("BLFLIST Remove Bind Dsskey [%d]", m_nBindDsskeyId[i]);
                m_nBindDsskeyId.removeAt(i);
            }
            // 该dsskey已监控
            else if (iDsskeyId == m_nBindDsskeyId[i])
            {
                m_nBindDsskeyId.removeAt(i);
            }
#if !IF_BUG_32812
            // 一个账号只能监控相同的号码一个
            else if (objData.iLineIndex == iAccountId)
            {
                DSSKEY_INFO("BLFLIST Remove Bind Dsskey [%d]", m_nBindDsskeyId[i]);
                m_nBindDsskeyId.removeAt(i);
            }
#endif
        }
    }
}

CBLFPraseAction * CreateBLFPraseAction(char * strStr, bool IsPath)
{
    CBLFPraseAction * pAction = new CBLFPraseAction(strStr, IsPath);

    if (pAction != NULL)
    {
        pAction->SetRead(true);
        pAction->SetSync(false);

        return pAction;
    }

    return NULL;
}

yl::string GetBLFkeyByDsskeyId(int nId)
{
    yl::string strValue = dsskey_GetDsskeyValue(nId);

    //http://bugfree.yealink.com/Bug.php?BugID=100417
    //非bsft 时，value为空直接使用 call park code
    if (configParser_GetConfigInt(kszBroadsoftActive) == 0
            && strValue.empty())
    {
        strValue = configParser_GetConfigString(kszCallParkCode);
    }

    if (strValue.substr(0, 4) == "sip:")
    {
        strValue = strValue.substr(4, strValue.size() - 4);
    }

    int index = strValue.find(":");

    if (index != yl::string::npos)
    {
        strValue = strValue.substr(0, index);
    }

    if (strValue.find("@") != yl::string::npos)
    {
        return strValue;
    }
    else
    {
        strValue.append("@");

        strValue.append(acc_GetServerName(dsskey_GetDsskeyLine(nId)));
    }

    return strValue;
}

bool DialgInfo::IsMatch(DialgInfo * pDialog)
{
    return m_strDialgId.compare(pDialog->m_strDialgId) == 0;
}

Dsskey_Status MapSipBLFStatusToDsskeyStatus(int iStatus)
{
    Dsskey_Status dsskeyStatus = DS_NON;
    switch (iStatus)
    {
    case SIP_BLF_STATUS_IDLE:
        dsskeyStatus = DS_BLF_STATUS_IDLE;
        break;
    case SIP_BLF_STATUS_RINGING:
        dsskeyStatus = DS_BLF_STATUS_RINGING;
        break;
    case SIP_BLF_STATUS_CALLOUT:
        dsskeyStatus = DS_BLF_STATUS_CALLOUT;
        break;
    case SIP_BLF_STATUS_TALKING:
        dsskeyStatus = DS_BLF_STATUS_TALKING;
        break;
    case SIP_BLF_STATUS_PARKED:
        dsskeyStatus = DS_BLF_STATUS_PARK;
        break;
    case SIP_BLF_STATUS_HOLD:
        dsskeyStatus = DS_BLF_STATUS_HOLD;
        break;
    case SIP_BLF_STATUS_DND:
        dsskeyStatus = DS_BLF_STATUS_DND;
        break;
    case SIP_BLF_STATUS_UNKOWN:
        dsskeyStatus = DS_BLF_STATUS_UNKOWN;
        break;
    case SIP_BLF_STATUS_GENBEN_UNKOWN:
        dsskeyStatus = DS_BLF_STATUS_UNKOWN;
        break;
    default:
        break;
    }

    return dsskeyStatus;
}
