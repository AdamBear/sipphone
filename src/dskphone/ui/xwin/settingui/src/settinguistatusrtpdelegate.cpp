#include "settinguistatusrtpdelegate.h"
#include "uicallbackfun.h"
#include "uimanager/moduimanager.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/vqrtcp/include/modvqrtcp.h"
#include "feature/include/featurecommon.h"
#include "feature/include/modfeature.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettinguiStatusRtpDelegate::CSettinguiStatusRtpDelegate()
{

}

CSettinguiStatusRtpDelegate::~CSettinguiStatusRtpDelegate()
{
    UIManager_LockReturnToIdle(false);
}

CSettingUIDelegateBase * CSettinguiStatusRtpDelegate::CreatStatusRtpDelegate()
{
    CSettinguiStatusRtpDelegate * pDelegate = new CSettinguiStatusRtpDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CSettinguiStatusRtpDelegate::InitExtraData(void * pData)
{
    if (talklogic_SessionExist())
    {
        UIManager_LockReturnToIdle(true);
    }

    CSettinguiScrollByPageDelegateBase::InitExtraData(pData);

    m_vecText.clear();
    GetShowText(m_vecText);
}

bool CSettinguiStatusRtpDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_SWITCH == iType)
    {
        vqrtcp_RTPStatusSwitch();
        return true;
    }

    return CSettinguiScrollByPageDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

void CSettinguiStatusRtpDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (SOFTKEY_NUM != vecSoftKey.size())
    {
        return;
    }

    if (talklogic_SessionExist())
    {
        vecSoftKey[0].m_iType = ST_EMPTY;
        vecSoftKey[0].m_strSoftkeyTitle = "";
        vecSoftKey[1].m_iType = ST_EMPTY;
        vecSoftKey[1].m_strSoftkeyTitle = "";
        if (vqrtcp_IsRTPStatusSwitch())
        {
            vecSoftKey[2].m_iType = ST_SWITCH;
            vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_SWITCH);
        }
        else
        {
            vecSoftKey[2].m_iType = ST_EMPTY;
            vecSoftKey[2].m_strSoftkeyTitle = "";
        }
        vecSoftKey[3].m_iType = ST_BACK;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_CANCEL);
    }
    else
    {
        vecSoftKey[0].m_iType = ST_BACK;
        vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_BACK);
        vecSoftKey[1].m_iType = ST_EMPTY;
        vecSoftKey[1].m_strSoftkeyTitle = "";
        vecSoftKey[2].m_iType = ST_EMPTY;
        vecSoftKey[2].m_strSoftkeyTitle = "";
        vecSoftKey[3].m_iType = ST_EMPTY;
        vecSoftKey[3].m_strSoftkeyTitle = "";
    }
}

void CSettinguiStatusRtpDelegate::GetShowText(VEC_TEXT & vecText)
{
    // 通话是否存在
    bool bSessionExist = talklogic_SessionExist();

    // 判断是否处于通话中
    SVQReportData objData;

    if (bSessionExist)
    {
        vqrtcp_RTPStatusGetCurrentReport(objData);
    }
    else
    {
        vqrtcp_RTPStatusGetSessionReport(objData);
    }

    const RTPConfigureList * pRTPlist = feature_RTPConfigGetEnableList();
    if (NULL != pRTPlist && 0 != pRTPlist->size())
    {
        RTPConfigureList sLstData = *pRTPlist;

        RTPConfigureList::iterator iter = sLstData.begin();
        for (; iter != sLstData.end(); iter++)
        {
            RTPConfigureItem item = *iter;
            yl::string strText = objData.GetAttributeValueById(item.m_strIdName);
            if (item.m_strIdName == "CurrentTime" && !bSessionExist)
            {
                // Session Report里需要显示Stop time
                strText = _LANG(TRID_STOP_TIME) + yl::string(": ") + strText;
            }
            else
            {
                strText = _LANG(item.m_strDisplayName.c_str()) + yl::string(": ") + strText;
            }

            m_vecText.push_back(strText);
        }
    }
    else
    {
        yl::string strText = "";
        m_vecText.push_back(strText);
    }
}
