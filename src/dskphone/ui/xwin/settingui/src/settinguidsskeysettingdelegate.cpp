#include "settinguidsskeysettingdelegate.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/modsettingui.h"
#include "settinguimanager.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguicustomcomboboxitemdata.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "dsskeyuilogic/include/moddsskeyuilogic.h"
#include "baseui/include/baseuicommon.h"
#include "account/include/account_datadefine.h"
#include "account/include/modaccount.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "language/common/common_function.h"


namespace
{
static const mapItem s_attr_info_arr[] =
{
    { DSSKEY_ATTR_ITEM_NONE,        ""                        },
    { DSSKEY_ATTR_ITEM_LABEL,       DSSKEY_CONFIG_LABEL         },
    { DSSKEY_ATTR_ITEM_VALUE,       DSSKEY_CONFIG_VALUE         },
    { DSSKEY_ATTR_ITEM_EXT,         DSSKEY_CONFIG_EXTENSION     },
    { DSSKEY_ATTR_ITEM_LINE,        DSSKEY_CONFIG_LINE          },
    { DSSKEY_ATTR_ITEM_INDEX,       DSSKEY_CONFIG_INDEX         },
    { DSSKEY_ATTR_ITEM_LINE_VALUE,  DSSKEY_CONFIG_LINE_VALUE    },
    { DSSKEY_ATTR_ITEM_TYPE,        DSSKEY_CONFIG_TYPE          },
    { DSSKEY_ATTR_ITEM_KEYEVENT,    DSSKEY_CONFIG_KEYEVENT_TYPE },
#if IF_DSSKEY_SUPPORT_SHORT_LABEL
    { DSSKEY_ATTR_ITEM_LABEL,       DSSKEY_CONFIG_SHORT_LABEL },
#endif
};

#define ITEM_MAX_LENGTH 99
#define SET_IME_ALL "all"
#define SET_IME_PASSWORD "pwd"
}

bool CSettinguiDsskeySettingDelegate::CompareID(int& nIDL, int& nIDR)
{
    return nIDL < nIDR;
}

CSettinguiDsskeySettingDelegate::CSettinguiDsskeySettingDelegate()
{
}

CSettinguiDsskeySettingDelegate::~CSettinguiDsskeySettingDelegate()
{
}

CSettingUIDelegateBase* CSettinguiDsskeySettingDelegate::CreateDsskeySettingDelegate()
{
    CSettinguiDsskeySettingDelegate* pStatusDelegate = new CSettinguiDsskeySettingDelegate;

    return (CSettingUIDelegateBasePtr)pStatusDelegate;
}

#if IF_SUPPORT_SELECT_CONTACT_INFO
void CSettinguiDsskeySettingDelegate::UpdateContactInfo(const yl::string& strContactName, const yl::string& strContactNumber)
{
    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDDsskeySetting);
    if (NULL == pDelegate)
    {
        return;
    }

    CSettinguiDsskeySettingDelegate* pDsskeySetDelegate = static_cast<CSettinguiDsskeySettingDelegate*>(pDelegate);
    if (NULL == pDsskeySetDelegate)
    {
        return;
    }

    pDsskeySetDelegate->UpdateDsskeyData(strContactName, strContactNumber);
}

void CSettinguiDsskeySettingDelegate::UpdateDsskeyData(const yl::string& strContactName, const yl::string& strContactNumber)
{
    if (!strContactName.empty())
    {
        SetValue(DSSKEY_CONFIG_LABEL, strContactName);
    }
    if (!strContactNumber.empty())
    {
        SetValue(DSSKEY_CONFIG_VALUE, strContactNumber);
    }

    GetPageData();
}

bool CSettinguiDsskeySettingDelegate::IsShowDirectorySoftkey()
{
    yl::string strItemID = GetItemIDByDataIndex(GetFocusIndex());
    if ((DSSKEY_CONFIG_LABEL == strItemID || DSSKEY_CONFIG_VALUE == strItemID)
            && (1 != configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch)
                || DT_SPEEDDIAL != m_DssKeyData.m_eType))
    {
        yl::string strValue = GetValue(strItemID);
        if (strValue.empty())
        {
            return true;
        }
    }
    return false;
}

bool CSettinguiDsskeySettingDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_DIRECTORY == iType)
    {
        SettingUI_EnterPage(kszPageIDDirectory, (void*)&CSettinguiDsskeySettingDelegate::UpdateContactInfo);
        return true;
    }

    return CDirectoryAssociateBaseDelegate::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}
#endif

void CSettinguiDsskeySettingDelegate::InitExtraData(void* pData)
{
    if (NULL == pData)
    {
        return;
    }
    m_PageInfo = *(static_cast<DsskeyPageInfo*>(pData));
}

CSettingUIPageDataBase* CSettinguiDsskeySettingDelegate::LoadPageData(
    bool bReLoad /* = false*/)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL == m_pPageData)
    {
        return m_pPageData;
    }

    if (bReLoad)
    {
        UpdateTitle();
        GetLogicData();
    }

    LoadListItemData(bReLoad);

    SetPageValue();

    return m_pPageData;
}

bool CSettinguiDsskeySettingDelegate::SavePageData(CSettingUIPageDataBase* pPageData,
        bool bBackToPrevious /* = true*/, bool bPopHint /* = true*/)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    GetPageData();

    // 给出保存提示
    CDsskeyPair errorTips;
    if (dsskeyuilogic_IsDataValid(m_PageInfo, m_DssKeyData, errorTips))
    {
        dsskeyuilogic_SaveData(m_PageInfo, m_DssKeyData);
        ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE);
        return true;
    }
    else
    {
        CSettingUIDataValidInfo validInfo;
        yl::string strItemID = GetItemByAttrType(errorTips.iIndex);
        validInfo.SetValidIndex(GetItemDataIndexByItemId(strItemID));
        validInfo.SetValidNote(_LANG(errorTips.strValue.c_str()));
        g_SetUICallBackFun.DealWithValidData(validInfo);
        return false;
    }
}

void CSettinguiDsskeySettingDelegate::OnComboBoxValueChange(int nIndex)
{
    if (NULL != m_pPageData)
    {
        CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByDataIndex(nIndex);
        if (NULL == pItemData || pItemData->GetItemID().empty())
        {
            return;
        }
        yl::string strItemID = pItemData->GetItemID();
        if (DSSKEY_CONFIG_TYPE == strItemID || DSSKEY_CONFIG_KEYEVENT_TYPE == strItemID)
        {
            bool bKeyEvent = DSSKEY_CONFIG_KEYEVENT_TYPE == strItemID;

            int nRealType = GetRealType();

            bool bReset = nRealType == m_OldDssKeyData.m_eType;
            if (!bReset && !bKeyEvent)
            {
                // 切到KeyEvent时需要重新加载
                int iIndex = GetComboboxValue(DSSKEY_CONFIG_TYPE);
                if (GetTypeByIndex(iIndex, FALSE) == DT_KEYEVENT)
                {
                    bReset = dsskeyuilogic_CheckKeyEventSet(m_PageInfo, m_OldDssKeyData.m_eType);
                }
            }

            // 还原为刚加载的数据
            if (bReset)
            {
                m_DssKeyData = m_OldDssKeyData;
            }
            else
            {
                // 切换类型时其它数据统统清除
                m_DssKeyData.Reset();
                // 重新设置当前的type，方便后续接口判断类型
                m_DssKeyData.m_eType = (DssKey_Type)nRealType;
                // 对于需要实时更新的类型
                dsskeyuilogic_UpdateData(m_PageInfo, m_DssKeyData);
            }

            g_SetUICallBackFun.RefreshUI(false, GetPageID());
        }
        else if (DSSKEY_CONFIG_LINE == strItemID)
        {
            // 修改Line可能需要实时更新，因此先获取当前页面数据
            GetPageData();

            // 还原为刚加载的数据
            if (m_DssKeyData.m_eType == m_OldDssKeyData.m_eType
                    && m_DssKeyData.m_iLineIndex == m_OldDssKeyData.m_iLineIndex)
            {
                m_DssKeyData = m_OldDssKeyData;
            }
            else
            {
                // 切换时Label要置空，重新获取
                m_DssKeyData.m_strLabel.clear();

                // 对于需要实时更新的类型，如Line, BLF
                dsskeyuilogic_UpdateData(m_PageInfo, m_DssKeyData);
            }

            g_SetUICallBackFun.RefreshUI(false, GetPageID());
        }
    }
}

void CSettinguiDsskeySettingDelegate::SetPageValue()
{
    SetComboboxValue(DSSKEY_CONFIG_TYPE, GetIndexByType(m_DssKeyData.m_eType, FALSE));
    SetComboboxValue(DSSKEY_CONFIG_KEYEVENT_TYPE, GetIndexByType(m_DssKeyData.m_eType, TRUE));

#ifdef IF_FEATURE_PSTN
    int nLineID = m_DssKeyData.m_iLineIndex;
    int nIndex = 0;
    ListAccounts listAccountId;

    if (!GetAccountList(listAccountId))
    {
        return;
    }

    for (ListAccounts::iterator iter = listAccountId.begin(); iter != listAccountId.end();
            ++iter, ++nIndex)
    {
        if (*iter == nLineID)
        {
            break;
        }
    }
    SetComboboxValue(DSSKEY_CONFIG_LINE, nIndex);
#else
    SetComboboxValue(DSSKEY_CONFIG_LINE, m_DssKeyData.m_iLineIndex);
#endif

    SetValue(DSSKEY_CONFIG_LABEL, m_DssKeyData.m_strLabel);
    SetValue(DSSKEY_CONFIG_VALUE, m_DssKeyData.m_strValue);
    SetValue(DSSKEY_CONFIG_EXTENSION, m_DssKeyData.m_strExtension);

    SetComboboxValue(DSSKEY_CONFIG_INDEX, GetIndex());
    SetComboboxValue(DSSKEY_CONFIG_LINE_VALUE, GetIndex());
#if IF_DSSKEY_SUPPORT_SHORT_LABEL
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    if (PT_T52 == ePhoneType)
    {
        SetValue(DSSKEY_CONFIG_SHORT_LABEL, m_DssKeyData.m_strShortLabel);
    }
#endif
}

void CSettinguiDsskeySettingDelegate::UpdateTitle()
{
    if (NULL != m_pPageData)
    {
        yl::string strTypeName;
        switch (m_PageInfo.m_eUIType)
        {
        case DSSKEY_UI_SOFTKEY:
            {
                strTypeName = _LANG(TRID_SOFTKEY_SETTING);
            }
            break;
        case DSSKEY_UI_LINEKEY:
            {
                strTypeName = _LANG(TRID_LINEKEY);
            }
            break;
        case DSSKEY_UI_EXPKEY:
            {
                strTypeName = _LANG(TRID_EXP_KEY);
            }
            break;
        default:
            break;
        }

        yl::string strLineID = commonAPI_FormatString("%s %d", strTypeName.c_str(),
                               m_PageInfo.m_iIndex + 1);
        m_pPageData->SetPageTitle(strLineID);
    }
}

void CSettinguiDsskeySettingDelegate::GetLogicData()
{
    dsskeyuilogic_GetData(m_PageInfo, m_DssKeyData);
    m_OldDssKeyData = m_DssKeyData;
    // 取数据
    dsskeyuilogic_GetAllDsskey(m_PageInfo, m_vecKeyList, false);
    dsskeyuilogic_GetAllDsskey(m_PageInfo, m_vecKeyEvent, true);
}

void CSettinguiDsskeySettingDelegate::LoadListItemData(bool bReLoad)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    DSSKEY_READONLY_MASK eReadOnlyMask = dsskeyuilogic_IsDsskeyReadOnly(m_PageInfo, m_DssKeyData);
    bool bReadOnly = READONLY_NONE != eReadOnlyMask;

    if (bReLoad)
    {
        LoadDsskeyTypeItem(bReadOnly);
    }
    else
    {
        m_pPageData->DelFromItem(DSSKEY_CONFIG_TYPE);
    }

    //加载KeyEventItem
    if (dsskeyuilogic_CheckKeyEventSet(m_PageInfo, m_DssKeyData.m_eType))
    {
        LoadKeyEventTypeItem(bReadOnly);
    }

    //line
    LoadLineItemData(bReadOnly);
    //Index
    LoadIndexItemData(bReadOnly, DSSKEY_CONFIG_INDEX);
    //label
    bool bLabelReadOnly = eReadOnlyMask > READONLY_EXCEPT_LABEL;
    LoadLineEditItemData(bLabelReadOnly, DSSKEY_CONFIG_LABEL);
    //value
    LoadLineEditItemData(bReadOnly, DSSKEY_CONFIG_VALUE);
    //extension
    LoadLineEditItemData(bReadOnly, DSSKEY_CONFIG_EXTENSION);
    //line-value
    LoadIndexItemData(bReadOnly, DSSKEY_CONFIG_LINE_VALUE);

#if IF_DSSKEY_SUPPORT_SHORT_LABEL
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    if (PT_T52 == ePhoneType)
    {
        // shortlabel
        LoadSimpleLineEditItem(bReadOnly, DSSKEY_CONFIG_SHORT_LABEL);
    }
#endif
}

void CSettinguiDsskeySettingDelegate::LoadDsskeyTypeItem(bool bReadOnly)
{
    LoadCandidateComboboxItem(DSSKEY_CONFIG_TYPE, _LANG(TRID_TYPE), false, bReadOnly);
}

void CSettinguiDsskeySettingDelegate::LoadKeyEventTypeItem(bool bReadOnly)
{
    LoadCandidateComboboxItem(DSSKEY_CONFIG_KEYEVENT_TYPE, _LANG(TRID_KEY_TYPE), true, bReadOnly);
}

void CSettinguiDsskeySettingDelegate::LoadLineItemData(bool bReadOnly)
{
    if (NULL == m_pPageData)
    {
        return ;
    }

    DSSKEY_ATTR_ITEM eAttr = (DSSKEY_ATTR_ITEM)GetAttrTypeByName(DSSKEY_CONFIG_LINE);
    CDsskeyItemAttr* pAttrItem = dsskeyuilogic_GetAttrByType(m_DssKeyData.m_eType, eAttr);
    if (pAttrItem == NULL || !pAttrItem->IsVisible())
    {
        return;
    }

    if (!bReadOnly)
    {
        bReadOnly = pAttrItem->IsReadOnly();
    }

    yl::string strDisPlayName = pAttrItem->GetName();

    CSettingUIComboBoxItemDataPtr pItemData = new CSettingUIComboBoxItemData;
    if (NULL != pItemData)
    {
        pItemData->SetItemID(DSSKEY_CONFIG_LINE);
        pItemData->SetDisplayName(_LANG(strDisPlayName));
        pItemData->SetShowIndex(true);
        pItemData->SetEnable(!bReadOnly);
#ifdef IF_FEATURE_PSTN
        ListAccounts listAccountId;

        if (!GetAccountList(listAccountId))
        {
            return;
        }

        ListAccounts::iterator iter = listAccountId.begin();
        for (int i = 0; iter != listAccountId.end(); ++iter, ++i)
        {
            int& nID = *iter;
            yl::string strValue = commonAPI_FormatString("%d", i);
            if (nID >= PSTN_LINE_BEGIN_ID && nID <= PSTN_LINE_END_ID)
            {
                yl::string strName = acc_GetLabel(nID);
                AddComboboxoption(pItemData, strName, strValue);
            }
            else
            {
                yl::string strName = commonAPI_FormatString("%s %d", _LANG(TRID_LINE).c_str(), nID + 1);
                AddComboboxoption(pItemData, strName, strValue);
            }
        }
#else
        int iAccountNum = acc_AccountNum(ACCOUNT_TYPE_SIP);
        for (int i = 0; i < iAccountNum; ++i)
        {
            yl::string strValue = commonAPI_FormatString("%d", i);
            yl::string strName = commonAPI_FormatString("%s %d", _LANG(TRID_LINE).c_str(), i + 1);
            AddComboboxoption(pItemData, strName, strValue);
        }
#endif

        if (!m_pPageData->AddItem(pItemData))
        {
            delete pItemData;
        }
    }
}

void CSettinguiDsskeySettingDelegate::LoadIndexItemData(bool bReadOnly,
        const yl::string& strID)
{
    if (NULL == m_pPageData)
    {
        return ;
    }

    DSSKEY_ATTR_ITEM eAttr = (DSSKEY_ATTR_ITEM)GetAttrTypeByName(strID);
    CDsskeyItemAttr* pAttrItem = dsskeyuilogic_GetAttrByType(m_DssKeyData.m_eType, eAttr);
    if (pAttrItem == NULL || !pAttrItem->IsVisible())
    {
        return;
    }
    if (!bReadOnly)
    {
        bReadOnly = pAttrItem->IsReadOnly();
    }

    yl::string strDisPlayName = pAttrItem->GetName();

    CSettingUIComboBoxItemData* pItemData = new CSettingUIComboBoxItemData;
    if (NULL != pItemData)
    {
        pItemData->SetItemID(strID);
        pItemData->SetDisplayName(_LANG(strDisPlayName));
        pItemData->SetShowIndex(true);
        pItemData->SetEnable(!bReadOnly);

        VecCandidateValue::iterator it = m_DssKeyData.m_vecData.begin();
        for (int i = 0; it != m_DssKeyData.m_vecData.end(); ++it, ++i)
        {
            yl::string strValue = commonAPI_FormatString("%d", i);
            yl::string strName = commonAPI_FormatString("%s", _LANG(it->strValue.c_str()).c_str());
            AddComboboxoption(pItemData, strName, strValue);
        }

        if (!m_pPageData->AddItem(pItemData))
        {
            delete pItemData;
        }
    }
}

void CSettinguiDsskeySettingDelegate::LoadLineEditItemData(bool bReadOnly,
        const yl::string& strID)
{
    if (NULL == m_pPageData)
    {
        return ;
    }

    DSSKEY_ATTR_ITEM eAttr = (DSSKEY_ATTR_ITEM)GetAttrTypeByName(strID);
    CDsskeyItemAttr* pAttrItem = dsskeyuilogic_GetAttrByType(m_DssKeyData.m_eType, eAttr);
    if (pAttrItem == NULL || !pAttrItem->IsVisible())
    {
        return;
    }
    if (!bReadOnly)
    {
        bReadOnly = pAttrItem->IsReadOnly();
    }

    yl::string strDisPlayName = _LANG(pAttrItem->GetName());

    if (bReadOnly)
    {
        LoadBaseItemData(strID, strDisPlayName);
        return;
    }

    CSettingUILineEditItemData* pItemData = new CSettingUILineEditItemData;
    if (NULL != pItemData)
    {
        yl::string strIme = pAttrItem->GetIme();
        bool bPassword = pAttrItem->GetPasswdFlag();
        pItemData->SetPassword(bPassword);
        pItemData->SetShowIndex(true);
        pItemData->SetItemID(strID);
        pItemData->SetDisplayName(strDisPlayName);
        pItemData->SetMaxLength(ITEM_MAX_LENGTH);
        yl::string strImeType = bPassword ? SET_IME_PASSWORD : SET_IME_ALL;
        pItemData->SetIME(strImeType);
        pItemData->SetDefaultIMEByLogicData(strIme, bPassword);
        if (!m_pPageData->AddItem(pItemData))
        {
            delete pItemData;
        }
    }
}

#if IF_DSSKEY_SUPPORT_SHORT_LABEL
void CSettinguiDsskeySettingDelegate::LoadSimpleLineEditItem(bool bReadOnly, const yl::string& strID)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    // ShortLabel 使用Label的属性
    CDsskeyItemAttr* pAttrItem = dsskeyuilogic_GetAttrByType(m_DssKeyData.m_eType, DSSKEY_ATTR_ITEM_LABEL);
    if (pAttrItem == NULL || !pAttrItem->IsVisible())
    {
        return;
    }

    yl::string strIme = pAttrItem->GetIme();
    AddLineEditItemData(m_pPageData, strID, _LANG(TRID_SHORT_LABEL), "", ITEM_MAX_LENGTH, SET_IME_ALL,
                        strIme);
}
#endif

void CSettinguiDsskeySettingDelegate::LoadCandidateComboboxItem(const yl::string& strItemID,
        const yl::string& strDisplayName, bool bKeyEvent, bool bReadOnly)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    CSettingUIComboBoxItemData* pItemData = new CSettingUIComboBoxItemData;
    if (NULL != pItemData)
    {
        pItemData->SetItemID(strItemID);
        pItemData->SetDisplayName(strDisplayName);
        pItemData->SetShowIndex(true);
        pItemData->SetEnable(!bReadOnly);

        VecCandidateValue& vecDsskeyPairs = bKeyEvent ? m_vecKeyEvent : m_vecKeyList;
        VecCandidateValue::iterator it = vecDsskeyPairs.begin();
        for (int i = 0; it != vecDsskeyPairs.end(); ++it, ++i)
        {
            yl::string strValue = commonAPI_FormatString("%d", i);
            yl::string strName = _LANG(it->strValue.c_str());
            AddComboboxoption(pItemData, strName, strValue);
        }

        if (!m_pPageData->AddItem(pItemData))
        {
            delete pItemData;
        }
    }
}

void CSettinguiDsskeySettingDelegate::LoadBaseItemData(const yl::string& strItemID,
        const yl::string& strDisplayName, const yl::string& strValue /*= ""*/)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    AddBaseItemData(m_pPageData, strDisplayName, strValue, strItemID);
}

#ifdef IF_FEATURE_PSTN
bool CSettinguiDsskeySettingDelegate::GetAccountList(ListAccounts& listAccountId)
{
    //ListAccounts listAccountId;
    listAccountId = acc_GetAccountList(ACCOUNT_TYPE_SIP | ACCOUNT_TYPE_PSTN);
    int nLineId = m_DssKeyData.m_iLineIndex;

    if (listAccountId.size() <= 0)
    {
        return false;
    }

    if (yl::find(listAccountId.begin(), listAccountId.end(), nLineId) == listAccountId.end())
    {
        listAccountId.push_back(nLineId);
    }

    Sort(listAccountId, CSettinguiDsskeySettingDelegate::CompareID);
    return true;
}
#endif

void CSettinguiDsskeySettingDelegate::AddComboboxoption(CSettingUIComboBoxItemData*
        pItemData, const yl::string& strDisplayName, const yl::string& strValue)
{
    if (NULL == pItemData)
    {
        return;
    }

    CSettingUIComboBoxOption* pOption = new CSettingUIComboBoxOption;
    if (NULL != pOption)
    {
        pOption->m_strDisplayName = strDisplayName;
        pOption->m_strItemVale = strValue;
        if (!pItemData->AddOptionItem(pOption))
        {
            delete pOption;
        }
    }
}

int CSettinguiDsskeySettingDelegate::GetAttrTypeByName(const yl::string& strID)
{
    return Str2KeyEx(strID, s_attr_info_arr);
}

LPCSTR CSettinguiDsskeySettingDelegate::GetItemByAttrType(int iIndex)
{
    return Key2StrEx(iIndex, s_attr_info_arr);
}

void CSettinguiDsskeySettingDelegate::GetPageData()
{
    if (NULL == m_pPageData)
    {
        return;
    }

    m_DssKeyData.Reset();
    m_DssKeyData.m_eType = (DssKey_Type)GetRealType();

    for (int i = 0; i < m_pPageData->GetTotalItemNumber(); i++)
    {
        CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByDataIndex(i);
        if (NULL == pItemData)
        {
            continue;
        }

        yl::string strID = pItemData->GetItemID();

        if (DSSKEY_CONFIG_LINE == strID)
        {
#ifdef IF_FEATURE_PSTN
            ListAccounts listAccountId;

            if (!GetAccountList(listAccountId))
            {
                return;
            }

            int iIndex = GetComboboxValue(strID);
            int i = 0;
            for (ListAccounts::iterator iter = listAccountId.begin(); iter != listAccountId.end();
                    ++iter, ++i)
            {
                if (i == iIndex)
                {
                    m_DssKeyData.m_iLineIndex = *iter;
                    break;
                }
            }
#else
            m_DssKeyData.m_iLineIndex = GetComboboxValue(strID);
#endif
        }
        else if (DSSKEY_CONFIG_LABEL == strID)
        {
            m_DssKeyData.m_strLabel = GetValue(strID);
        }
        else if (DSSKEY_CONFIG_VALUE == strID)
        {
            m_DssKeyData.m_strValue = GetValue(strID);
        }
        else if (DSSKEY_CONFIG_INDEX == strID || DSSKEY_CONFIG_LINE_VALUE == strID)
        {
            m_DssKeyData.m_iIndex = 0;
            int iIndex = GetComboboxValue(strID);
            // 转化为逻辑层真实的序号
            VecCandidateValue& vecValue = m_DssKeyData.m_vecData;
            if (iIndex >= 0 && iIndex < vecValue.size())
            {
                m_DssKeyData.m_iIndex = vecValue[iIndex].iIndex;
            }
        }
        else if (DSSKEY_CONFIG_EXTENSION == strID)
        {
            m_DssKeyData.m_strExtension = GetValue(strID);
        }
    }

#if IF_DSSKEY_SUPPORT_SHORT_LABEL
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    if (PT_T52 == ePhoneType)
    {
        m_DssKeyData.m_strShortLabel = GetValue(DSSKEY_CONFIG_SHORT_LABEL);
    }
#endif
}

int CSettinguiDsskeySettingDelegate::GetRealType()
{
    if (NULL == m_pPageData)
    {
        return DS_NON;
    }

    int nValue = GetComboboxValue(DSSKEY_CONFIG_TYPE);
    int nRealType = GetTypeByIndex(nValue, false);
    if (DT_KEYEVENT == nRealType)
    {
        CSettingUIItemDataBasePtr pItem = m_pPageData->GetItemByItemId(DSSKEY_CONFIG_KEYEVENT_TYPE);
        if (NULL != pItem && pItem->IsShow())
        {
            nRealType = GetTypeByIndex(atoi(pItem->GetValue().c_str()), true);
        }
        else
        {
            // 特殊情况，KeyEvent错误的兼容方式
            if (m_vecKeyEvent.size() <= 0)
            {
                nRealType = GetTypeByIndex(++nValue, false);
            }
            else
            {
                nRealType = GetTypeByIndex(0, true);
            }
        }
    }

    return nRealType;
}

int CSettinguiDsskeySettingDelegate::GetIndexByType(int nType, bool bKeyEvent)
{
    if (!bKeyEvent)
    {
        if (dsskeyuilogic_CheckKeyEventSet(m_PageInfo, (DssKey_Type)nType))
        {
            nType = DT_KEYEVENT;
        }
    }

    VecCandidateValue& vecDsskeyPairs = bKeyEvent ? m_vecKeyEvent : m_vecKeyList;
    int iIndex = 0;
    for (VecCandidateValue::iterator it = vecDsskeyPairs.begin();
            it != vecDsskeyPairs.end(); ++it, ++iIndex)
    {
        if (it->iIndex == nType)
        {
            return iIndex;
        }
    }

    return 0;
}

int CSettinguiDsskeySettingDelegate::GetTypeByIndex(int iIndex, bool bKeyEvent)
{
    if (iIndex < 0)
    {
        iIndex = 0;
    }

    VecCandidateValue& vecDsskeyPairs = bKeyEvent ? m_vecKeyEvent : m_vecKeyList;
    if (vecDsskeyPairs.size() <= 0)
    {
        return DT_NA;
    }

    return vecDsskeyPairs[iIndex % vecDsskeyPairs.size()].iIndex;
}

int CSettinguiDsskeySettingDelegate::GetIndex()
{
    VecCandidateValue::iterator it = m_DssKeyData.m_vecData.begin();
    for (int i = 0; it != m_DssKeyData.m_vecData.end(); ++it, ++i)
    {
        if (it->iIndex == m_DssKeyData.m_iIndex)
        {
            return i;
        }
    }

    return 0;
}

