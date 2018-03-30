#include "e911setlocationdelegate.h"

#if IF_FEATURE_GENBAND_E911

#include "uicallbackfun.h"
#include "settingui/include/pageiddefine.h"
#include "setuidatadefine.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "messagebox/modmessagebox.h"
#include "settinguimanager.h"
#include "etlmsghandler/modetlmsghandler.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/settinguibasepagedata.h"

#include "e911address/include/e911address_def.h"
#include "e911address/include/mode911address.h"

namespace
{
#define INVALID_E911_ID             -1
#define E911_MSG_BOX_SHOW_TIME      1500
enum
{
    MSG_ID_SHOW_NOTE_BEFORE_BACK = 1300,
    MSG_ID_CANCEL_DOWNLOAD_E911_SET_LOCATION,
};
}

CE911SetLocationDelegate::CE911SetLocationDelegate()
    : m_nId(kszE911RootID)
{
    etl_RegisterMsgHandle(E911_MESSAGE_BEGIN, E911_MESSAGE_END,
                          &CE911SetLocationDelegate::OnE911LocationMsg);
}

CE911SetLocationDelegate::~CE911SetLocationDelegate()
{
    etl_UnregisterMsgHandle(E911_MESSAGE_BEGIN, E911_MESSAGE_END,
                            &CE911SetLocationDelegate::OnE911LocationMsg);
}

CSettingUIDelegateBase* CE911SetLocationDelegate::CreateE911SetLocationDelegate()
{
    CE911SetLocationDelegate* pDelegate = new CE911SetLocationDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CE911SetLocationDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    switch (iType)
    {
    case ST_NEXT:
        {
            NextPage();
            return true;
        }
        break;
    case ST_PREV_PAGE:
        {
            BackToPrePage();
            return true;
        }
        break;
    case ST_CANCEL:
    case ST_BACK:
        {
            g_SetUICallBackFun.FallBackToPrevousPage();
            return true;
        }
        break;
    default:
        break;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

void CE911SetLocationDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    if (vecSoftKey.size() < MIN_SETTING_SOFTKEY_NUM)
    {
        return;
    }

    if (kszE911RootID == m_nId)
    {
        vecSoftKey[0].m_iType = ST_BACK;
        vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_BACK);
    }
    else
    {
        vecSoftKey[0].m_iType = ST_PREV_PAGE;
        vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_PREV);
        vecSoftKey[1].m_iType = ST_CANCEL;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_CANCEL);
    }

    int nId = GetFocusItemID();
    E911AddressData* pAddressData = NULL;
    pAddressData = e911_GetAddressDetailByID(nId);
    if (NULL == pAddressData)
    {
        return;
    }

    if (pAddressData->m_vChildrenID.size() <= 0)
    {
        vecSoftKey[3].m_iType = ST_SAVE;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SAVE);
    }
    else
    {
        vecSoftKey[3].m_iType = ST_NEXT;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_NEXT);
    }
}

CSettingUIPageDataBase* CE911SetLocationDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);
    if (NULL == m_pPageData)
    {
        return NULL;

    }

    if (bReLoad)
    {
        e911_ReqAddress();
        g_SetUICallBackFun.PopCommonMessageBox(kszPageIDSetE911Location, _LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT),
                                               SETTING_MESSAGEBOX_Cancel, "", "", -1,
                                               MSG_ID_CANCEL_DOWNLOAD_E911_SET_LOCATION);
    }

    return m_pPageData;
}

bool CE911SetLocationDelegate::SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    int nLocationID = GetFocusItemID();
    yl::string strAddr;
    e911_GetFullLocationAddr(nLocationID, strAddr);
    e911_SetLocationID(nLocationID, strAddr);
    e911_ClearListData();
    return CSettingUIDelegateBase::SavePageData(pPageData, true, true);
}

bool CE911SetLocationDelegate::ProcessMsgCallBack(int nResult, int nID, void* pExtraData /*= NULL*/)
{
    switch (nID)
    {
    case MSG_ID_CANCEL_DOWNLOAD_E911_SET_LOCATION:
        {
            if (MessageBox_Cancel == nResult)
            {
                g_SetUICallBackFun.FallBackToPrevousPage();
                return true;
            }
        }
        break;
    case MSG_ID_SHOW_NOTE_BEFORE_BACK:
        {
            g_SetUICallBackFun.FallBackToPrevousPage();
            return true;
        }
    default:
        break;
    }

    return CSettingUIDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

bool CE911SetLocationDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (bPress)
    {
        switch (nKeyCode)
        {
        case PHONE_KEY_OK:
            {
                if (IsLastAddress())
                {
                    OnSoftkeyAction(ST_SAVE, false, GetFocusIndex());
                }
                else
                {
                    NextPage();
                }
                return true;
            }
            break;
        case PHONE_KEY_CANCEL:
            {
                BackToPrePage();
                return true;
            }
            break;
        default:
            break;
        }
    }

    return CSettingUIDelegateBase::OnKeyEvent(nKeyCode, bPress, nDataIndex);
}

LRESULT CE911SetLocationDelegate::OnE911LocationMsg(msgObject& msg)
{
    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetTopDelegate();
    if (NULL == pDelegate || kszPageIDSetE911Location != pDelegate->GetPageID()
            || E911_MESSAGE_DOWNLOAD_RESULT == msg.wParam)
    {
        return FALSE;
    }

    CE911SetLocationDelegate* pE911SetLocDelegate = static_cast<CE911SetLocationDelegate*>(pDelegate);
    if (NULL == pE911SetLocDelegate)
    {
        return FALSE;
    }

    MessageBox_RemoveTopMessageBox(MessageBox_Destory);
    if (1 == msg.wParam)
    {
        pE911SetLocDelegate->LoadAddressList(kszE911RootID);
    }
    else
    {
        // 如果下载失败
        g_SetUICallBackFun.PopCommonMessageBox(kszPageIDSetE911Location, _LANG(TRID_NOTE), _LANG(TRID_LOAD_FAILED),
                                               SETTING_MESSAGEBOX_NOTE, "", "", E911_MSG_BOX_SHOW_TIME,
                                               MSG_ID_SHOW_NOTE_BEFORE_BACK);
    }

    return TRUE;
}

int CE911SetLocationDelegate::GetFocusItemID()
{
    int nFocusIndex = GetFocusIndex();
    CSettingUIItemDataBasePtr pItemData = GetItemDataByDataIndex(nFocusIndex);
    if (NULL == pItemData)
    {
        return INVALID_E911_ID;
    }

    yl::string strItemId = pItemData->GetItemID();
    int nId = atoi(strItemId.c_str());
    return nId;
}

void CE911SetLocationDelegate::LoadAddressList(int nId)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    m_nId = nId;
    m_pPageData->ClearItemData();
    E911AddressListData stAddressListData;
    stAddressListData.m_nIndexFrom = 0;
    E911AddressData* pAddressData = NULL;
    e911_GetAddressListData(&stAddressListData, nId);

    // item
    for (int i = 0; i < stAddressListData.m_vlistID.size(); ++i)
    {
        int nId = stAddressListData.m_vlistID[i];
        pAddressData = e911_GetAddressDetailByID(nId);
        if (NULL == pAddressData)
        {
            continue;
        }

        CSettingUIItemDataBasePtr  pItemData = new CSettingUIItemDataBase;
        if (NULL == pItemData)
        {
            break;
        }

        pItemData->SetDisplayName(pAddressData->m_strDisplayName);
        pItemData->SetShowIndex(false);
        pItemData->SetShowColon(false);
        yl::string strID = commonAPI_FormatString("%d", nId);
        pItemData->SetItemID(strID);
        if (!m_pPageData->AddItem(pItemData))
        {
            delete pItemData;
        }
    }

    g_SetUICallBackFun.RefreshUI(false);
}

void  CE911SetLocationDelegate::NextPage()
{
    //ѣզݹ֣ЮքхϢìѼ(Ǥظޚ֣idۍЖ՚քݹ֣index
    int nFocusIndex = GetFocusIndex();
    int nID = GetFocusItemID();
    if (INVALID_E911_ID == nID)
    {
        return;
    }

    //ʨ׃ҪׁȡքIDΪձǰݹ֣ЮքID
    m_vecFocusIndex.push_back(nFocusIndex);
    SetFocusIndex(0);
    LoadAddressList(nID);
}

void CE911SetLocationDelegate::BackToPrePage()
{
    //ܱȡӦكДʾքާĦidۍݹ֣index
    int nParentId = e911_GetParentIDByID(m_nId);
    if (INVALID_E911_ID == nParentId)
    {
        g_SetUICallBackFun.FallBackToPrevousPage();
        return;
    }

    int nFocusIndex;
    if (GetPreFocusIndex(nFocusIndex))
    {
        SetFocusIndex(nFocusIndex);
    }
    LoadAddressList(nParentId);
}

bool CE911SetLocationDelegate::GetPreFocusIndex(int& nFocusIndex)
{
    int nSize = m_vecFocusIndex.size();
    if (nSize <= 0)
    {
        return false;
    }

    nFocusIndex = m_vecFocusIndex[nSize - 1];
    m_vecFocusIndex.removeAt(nSize - 1);
    return true;
}

bool CE911SetLocationDelegate::IsLastAddress()
{
    int nId = GetFocusItemID();
    E911AddressData* pAddressData = NULL;
    pAddressData = e911_GetAddressDetailByID(nId);
    if (NULL == pAddressData)
    {
        return true;
    }

    if (pAddressData->m_vChildrenID.size() <= 0)
    {
        return true;
    }

    return false;
}

#endif

