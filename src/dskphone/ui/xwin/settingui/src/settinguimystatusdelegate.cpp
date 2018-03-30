#include "settinguimystatusdelegate.h"

#if FEATURE_BROADTOUCH_UC
#include "settinguimanager.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "setuidatadefine.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/modsettingui.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "resource/modresource.h"
#include "uicallbackfun.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "presence/include/modpresence.h"
#include "settingui/src/settinguiradioitemdata.h"

namespace
{
enum PRESENCE_STATE_E
{
    PRESENCE_STATE_ONLINE,
    PRESENCE_STATE_AWAY,
    PRESENCE_STATE_BUSY,
    PRESENCE_STATE_OFFLINE,
};

#define MSGBOX_ID_SAVE_COMPLETED 1020
}

CSettingUIMyStatusDelegate::CSettingUIMyStatusDelegate()
    : m_bPresenceChange(false)
    , m_nCheckIndex(-1)
{

}

CSettingUIMyStatusDelegate::~CSettingUIMyStatusDelegate()
{

}

CSettingUIDelegateBase* CSettingUIMyStatusDelegate::CreateSettingUIMyStatusDelegate()
{
    CSettingUIMyStatusDelegate* pDelegate = new CSettingUIMyStatusDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase* CSettingUIMyStatusDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL == m_pPageData || !bReLoad)
    {
        return NULL;
    }

    SetCheckedByIndex(GetPresenceStatus());
    m_pPageData->SetFocusIndex(m_nCheckIndex);

    return m_pPageData;
}

bool CSettingUIMyStatusDelegate::SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    SetCheckedByIndex(m_pPageData->GetFocusIndex());

    // 告知服务器修改状态
    _PRESENCE_TYPE eType = _PRESENCE_TYPE_OFFLINE;
    switch (m_nCheckIndex)
    {
    case PRESENCE_STATE_ONLINE:
        eType = _PRESENCE_TYPE_ONLINE;
        break;
    case PRESENCE_STATE_AWAY:
        eType = _PRESENCE_TYPE_AWAY;
        break;
    case PRESENCE_STATE_BUSY:
        eType = _PRESENCE_TYPE_BUSY;
        break;
    case PRESENCE_STATE_OFFLINE:
        eType = _PRESENCE_TYPE_OFFLINE;
        break;
    default:
        eType = _PRESENCE_TYPE_OFFLINE;
        break;
    }

    m_bPresenceChange = Presence_ReqChangPresence(eType);
    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_STATUS_REQUESTING),
                                           SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_SHORT_TIME, MSGBOX_ID_SAVE_COMPLETED);
    return true;
}

bool CSettingUIMyStatusDelegate::ProcessMsgCallBack(int nResult, int nID, void* pExtraData /*= NULL*/)
{
    if (MSGBOX_ID_SAVE_COMPLETED == nID)
    {
        if (m_bPresenceChange)
        {
            g_SetUICallBackFun.FallBackToPrevousPage();
        }
        else
        {
            g_SetUICallBackFun.PopCommonMessageBox(NULL, _LANG(TRID_NOTE), _LANG(TRID_SET_FAILED),
                                                   SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_SHORT_TIME);
            //重新设置为当前状态
            SetCheckedByIndex(GetPresenceStatus());
            m_pPageData->SetFocusIndex(m_nCheckIndex);
        }
        return true;
    }

    return CSettinguiDataDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

int CSettingUIMyStatusDelegate::GetPresenceStatus()
{
    int nStatusIndex = -1;

    _PRESENCE_TYPE ePresenceType = Presence_GetLocalPresence().ePresenceType;

    switch (ePresenceType)
    {
    case _PRESENCE_TYPE_ONLINE:
        {
            nStatusIndex = PRESENCE_STATE_ONLINE;
        }
        break;
    case _PRESENCE_TYPE_AWAY:
        {
            nStatusIndex = PRESENCE_STATE_AWAY;
        }
        break;
    case _PRESENCE_TYPE_BUSY:
        {
            nStatusIndex = PRESENCE_STATE_BUSY;
        }
        break;
    default:
        {
            // 其他状态都以off表示
            nStatusIndex = PRESENCE_STATE_OFFLINE;
        }
        break;
    }

    return nStatusIndex;
}

void CSettingUIMyStatusDelegate::SetCheckedByIndex(int nDataIndex)
{
    SetItemCheckByDataIndex(m_nCheckIndex, false);
    m_nCheckIndex = nDataIndex;
    SetItemCheckByDataIndex(m_nCheckIndex, true);
}

void CSettingUIMyStatusDelegate::SetItemCheckByDataIndex(int nDataIndex, bool bCheck)
{
    CSettingUIRadioItemDataPtr pItem = CSettingUIRadioItemData::GetRadioListDataItem(GetItemDataByDataIndex(
                                           nDataIndex));
    if (NULL != pItem)
    {
        pItem->SetCheck(bCheck);
    }
}

#endif
