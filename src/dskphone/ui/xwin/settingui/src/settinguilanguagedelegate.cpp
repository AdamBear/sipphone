#include "settinguilanguagedelegate.h"
#include "uicallbackfun.h"
#include "settinguiradioitemdata.h"
#include "pageiddefine.h"
#include "settinguiwizardmanager.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/setuidatadefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/languagecontroller.h"
#include "common/common_data_define.h"
#include "languagehelper/modlangtranslate.h"
#include "translateiddefine.h"

namespace
{
#define MSGBOX_TINE_GUIDE_LANGUAGE_CHANGE 500
#define MSGBOX_ID_GUIDE_LANGUAGE_CHANGE 1008
}

CSettingUILanguageDelegate::CSettingUILanguageDelegate()
    : m_iConfigSelectIndex(-1)
#ifdef IF_SETTING_WIZARD
    , m_bInGuide(false)
#endif
{
}

CSettingUILanguageDelegate::~CSettingUILanguageDelegate()
{
}

CSettingUIDelegateBase * CSettingUILanguageDelegate::CreateLanguageDelegate()
{
    CSettingUILanguageDelegate * pDelegate = new CSettingUILanguageDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettingUILanguageDelegate::ProcessMsgCallBack(int nResult, int nID,
        void * pExtraData /*= NULL*/)
{
#ifdef IF_SETTING_WIZARD
    if (NULL != g_pSettingUIWizardManager && g_pSettingUIWizardManager->IsInWizard())
    {
        if (MSGBOX_ID_GUIDE_LANGUAGE_CHANGE == nID && SETTING_MessageBox_TimeOut == nResult)
        {
            g_pSettingUIWizardManager->EnterNextWizardPage(GetPageID());
            return true;
        }
    }
#endif

    return CSettingUIDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

CSettingUIPageDataBase * CSettingUILanguageDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (!bReLoad || NULL == m_pPageData)
    {
        return m_pPageData;
    }

    // 获取语言列表
    if (NULL != g_pLanguageController && g_pLanguageController->GetLanguageList(m_vecLang))
    {
        int nSize = m_vecLang.size();
        for (int i = 0; i < nSize; ++i)
        {
            CSettingUIRadioItemDataPtr pItemData = new CSettingUIRadioItemData;
            if (NULL != pItemData)
            {
                // item id
                pItemData->SetDisplayName(_LANG(m_vecLang[i]));
                pItemData->SetCheck(false);
                pItemData->SetShowIndex(true);

                if (!m_pPageData->AddItem(pItemData))
                {
                    delete pItemData;
                }
            }
        }
    }

    // 初始化选中语言
    m_iConfigSelectIndex = SelectedLanguageIndex(m_vecLang,
                           g_pLanguageController->GetCurrentLanguage());

    if (NULL != m_pPageData)
    {
        // 获取配置，设置选中项，让其显示已选标记，默认选第1项
        CSettingUIRadioItemDataPtr pChooseItem = CSettingUIRadioItemData::GetRadioListDataItem(
                    m_pPageData->GetItemByDataIndex(m_iConfigSelectIndex));
        if (NULL == pChooseItem)
        {
            m_iConfigSelectIndex = 0;
            pChooseItem = CSettingUIRadioItemData::GetRadioListDataItem(m_pPageData->GetItemByDataIndex(
                              m_iConfigSelectIndex));
        }

        if (NULL != pChooseItem)
        {
            pChooseItem->SetCheck(true);
            // 设置默认选中项
            m_pPageData->SetFocusIndex(m_iConfigSelectIndex);
        }
    }

#ifdef IF_SETTING_WIZARD
    m_bInGuide = kszPageIDGuideLanguaue == GetPageID();
#endif

    return m_pPageData;
}

bool CSettingUILanguageDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    if (CheckTalking() || NULL == pPageData || NULL == g_pLanguageController || !pPageData->IsEnable())
    {
        return true;
    }

    bool bChange = IsPageValueChange(pPageData);
    int nLanguageIndex = GetChoseIndex();
    if (nLanguageIndex >= 0 && nLanguageIndex < m_vecLang.size() && bChange)
    {
        g_pLanguageController->SetCurrentLanguage(m_vecLang[nLanguageIndex]);
    }

#ifdef IF_SETTING_WIZARD
    if (m_bInGuide && bChange)
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_CHANGE_LANGUAGE)
                                               , SETTING_MESSAGEBOX_NOTE, "", "", MSGBOX_TINE_GUIDE_LANGUAGE_CHANGE,
                                               MSGBOX_ID_GUIDE_LANGUAGE_CHANGE);
        return false;
    }
#endif

    if (bPopHint)
    {
        yl::string strNote = _LANG(TRID_SAVING_CONFIG_FILE);
        if (bChange)
        {
            strNote = _LANG(TRID_INITIALIZING_LANGUAGE_PLEASE_WAIT);
        }

        m_bBackToPreviousInterface = bBackToPrevious;
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), strNote
                                               , SETTING_MESSAGEBOX_NOTE, "", "", NS_SET::g_SetPopNormalTime);
    }

    return true;
}

bool CSettingUILanguageDelegate::OnPageAction(int eActionType, int iDataIndex)
{
#ifdef IF_SETTING_WIZARD
    if (SET_ACTION_TYPE_CLICK == eActionType && m_bInGuide)
    {
        if (m_iConfigSelectIndex != iDataIndex && NULL != m_pPageData)
        {
            //开机引导下更改选中状态
            CSettingUIItemDataBasePtr pBaseItemData = m_pPageData->GetItemByDataIndex(m_iConfigSelectIndex);
            CSettingUIRadioItemDataPtr pRadioItemData = CSettingUIRadioItemData::GetRadioListDataItem(
                        pBaseItemData);
            if (NULL != pRadioItemData)
            {
                pRadioItemData->SetCheck(false);
            }

            // 设置新的选中项
            pBaseItemData = m_pPageData->GetItemByDataIndex(iDataIndex);
            pRadioItemData = CSettingUIRadioItemData::GetRadioListDataItem(pBaseItemData);
            if (NULL != pRadioItemData)
            {
                pRadioItemData->SetCheck(true);
            }

            m_iConfigSelectIndex = iDataIndex;

            g_SetUICallBackFun.RefreshUI(false, GetPageID());
        }

        return true;
    }
#endif

    return CSettingUIDelegateBase::OnPageAction(eActionType, iDataIndex);
}

bool CSettingUILanguageDelegate::IsPageValueChange(CSettingUIPageDataBase * pPagedata)
{
    if (NULL == pPagedata || NULL == g_pLanguageController)
    {
        return false;
    }

    return (GetChoseIndex() != SelectedLanguageIndex(m_vecLang,
            g_pLanguageController->GetCurrentLanguage()));
}

int CSettingUILanguageDelegate::GetChoseIndex()
{
#ifdef IF_SETTING_WIZARD
    if (m_bInGuide)
    {
        return m_iConfigSelectIndex;
    }
#endif

    if (NULL != m_pPageData)
    {
        return m_pPageData->GetFocusIndex();
    }

    return -1;
}

int CSettingUILanguageDelegate::SelectedLanguageIndex(const VectorLangInfo & vecLang,
        const yl::string & strSelectedLanguage)
{
    if (strSelectedLanguage.empty())
    {
        return -1;
    }

    for (int i = 0; i < vecLang.size(); ++i)
    {
        if (vecLang[i] == strSelectedLanguage)
        {
            return i;
        }
    }

    return -1;
}

