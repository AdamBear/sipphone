#include "basesettinguidelegate.h"
#include "settinguimanager.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settingactiontypemgr.h"
#include "settinguiwizardmanager.h"
#include "settingui/include/editcheckmethodfactory.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/include/settingdefine.h"
#include "settinguilogic/src/datafactory.h"
#include "settinguilogic/src/dataloader.h"
#include "settinguilogic/src/datasave.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/settinguibaseaccountpagedata.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguilogic/src/settinguierrorinfo.h"
#include "settinguilogic/src/configfiltermanager.h"
#include "settinguilogic/include/pagedatafactory.h"
#include "settinguilogic/src/networksettingmanager.h"
#include "settinguilogic/src/settinguimenuitemdata.h"

#include "editcheckmethod.h"
#include "regexp/modregexp.h"
#include "commonapi/stringhelperapi.h"
#include "common/common_data_define.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "adapterbox/include/modadapterbox.h"
#include "settinguilogic/include/common.h"
#include "include/path_define.h"
#include "dsklog/log.h"
#include "feature/include/modfeature.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talklogic/include/modtalklogic.h"

namespace
{
// 有Title一页Item数量
const int s_nTitlePageCount = 9;
// 没有Title一页Item数量
const int s_nPageCount = 10;

// 配置数据路径
const char * const kszPageDataPath = PHONE_MENU_PATH;
// Setting配置数据路径
const char * const kszSettingPageDataPath = PHONE_MENU_PATH;

// 修改数据未保存，按Back退出界面提示弹框ID
static const int MSG_ID_DATA_WITHOUT_SAVE(1013);
// 修改数据未保存，按Back退出到Idle提示弹框ID
static const int MSG_ID_BACK_TO_IDLE_DATA_WITHOUT_SAVE(1014);
}

yl::string CSettingUIDelegateBase::m_strPageDataPath = kszSettingPageDataPath;

CSettingUIDelegateBase::CSettingUIDelegateBase()
    : m_iLineID(-1)
    , m_bShow(true)
    , m_bFirstBindShow(true)
    , m_strPageID("")
    , m_pPageData(NULL)
    , m_bBackToPreviousInterface(false)
    , m_bRefreshWndWhileActive(false)
    , m_bRefreshWhenTopDelegate(false)
    , m_nItemCountPerPage(-1)
{

}

CSettingUIDelegateBase::~CSettingUIDelegateBase()
{
    if (NULL != m_pPageData)
    {
        delete m_pPageData;
        m_pPageData = NULL;
    }
}

void CSettingUIDelegateBase::Init()
{
}

void CSettingUIDelegateBase::UnInit()
{
}

//////////////////// Setting界面属性 //////////////////////
const yl::string & CSettingUIDelegateBase::GetPageID() const
{
    return m_strPageID;
}

void CSettingUIDelegateBase::SetPageID(const yl::string & strPageID)
{
    m_strPageID = strPageID;
}

bool CSettingUIDelegateBase::IsFirstBindShow()
{
    return m_bFirstBindShow;
}

void CSettingUIDelegateBase::SetFirstBindShow(bool bFirstBindShow)
{
    m_bFirstBindShow = bFirstBindShow;
}

bool CSettingUIDelegateBase::IsRefreshWndWhileActive()
{
    return m_bRefreshWndWhileActive;
}

void CSettingUIDelegateBase::SetRefreshWndWhileActive(bool bRefreshWndWhileActive)
{
    m_bRefreshWndWhileActive = bRefreshWndWhileActive;
}

bool CSettingUIDelegateBase::IsRefreshDataWhenTopDelegate()
{
    return m_bRefreshWhenTopDelegate;
}

void CSettingUIDelegateBase::SetRefreshDataWhenTopDelegate(bool bRefresh)
{
    m_bRefreshWhenTopDelegate = bRefresh;
}

bool CSettingUIDelegateBase::CheckTalking()
{
    if (IsExistTalking())
    {
#ifndef _WIN32
#warning
#endif
//      g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_TALKING_PLEASE_WAIT)
//          , SETTING_MESSAGEBOX_Cancel, "", "", NS_SET::g_SetPopNormalTime);
        return true;
    }
    return false;
}

bool CSettingUIDelegateBase::IsExistTalking()
{
    return talklogic_SessionExist();
}

int CSettingUIDelegateBase::GetFocusIndex()
{
    if (NULL == m_pPageData)
    {
        return -1;
    }
    return m_pPageData->GetFocusIndex();
}

void CSettingUIDelegateBase::SetFocusIndex(int nFocusIndex)
{
    if (NULL != m_pPageData)
    {
        m_pPageData->SetFocusIndex(nFocusIndex);
    }
}

void CSettingUIDelegateBase::SetFocusIndexByItemID(const yl::string& strItemID)
{
    if (NULL != m_pPageData)
    {
        m_pPageData->SetFocusIndexByItemID(strItemID);
    }
}

void CSettingUIDelegateBase::SetPageDataIndex(int nPageDataIndex)
{
    if (NULL != m_pPageData)
    {
        m_pPageData->SetPageDataIndex(nPageDataIndex);
    }
}

int CSettingUIDelegateBase::GetPageDataIndex()
{
    if (NULL != m_pPageData)
    {
        return m_pPageData->GetPageDataIndex();
    }

    return 0;
}

CSettingUIPageDataBase* CSettingUIDelegateBase::LoadPageData(bool bReLoad /*= false*/)
{
    // 仅重新显示刷新界面，不要求重新从底层获取数据
    if (!bReLoad)
    {
        if (NULL != m_pPageData && !m_pPageData->GetPageID().empty())
        {
            return m_pPageData;
        }
    }

    int nLastFocusIndex = -1;
    int nLastPageDataIndex = -1;

    // 重新设置数据时，先清空，以防内存泄漏（子Item是否需要遍历删除）
    if (NULL != m_pPageData)
    {
        if (!IsResetFocusWhileReload())
        {
            nLastFocusIndex = m_pPageData->GetFocusIndex();
            nLastPageDataIndex = m_pPageData->GetPageDataIndex();
        }

        delete m_pPageData;
        m_pPageData = NULL;
    }
    m_pPageData = g_PageDataFactory.GetPageData(m_strPageID);

    yl::string strPageDataPath = m_strPageDataPath;
    strPageDataPath += m_strPageID;
    if (!g_DataFactory->GetPageInfo(strPageDataPath, m_pPageData)
            || !g_DataLoader->LoadData(m_pPageData, m_iLineID))
    {
        return NULL;
    }

    if (-1 != nLastFocusIndex && -1 != nLastPageDataIndex)
    {
        m_pPageData->SetFocusIndex(nLastFocusIndex);
        m_pPageData->SetPageDataIndex(nLastPageDataIndex);
    }

    return m_pPageData;
}

bool CSettingUIDelegateBase::AddMenuItem(CSettingUIPageDataBase * pPageData,
        const yl::string & strID,
        const yl::string & strName, const yl::string & strAuthoridID /*= ""*/,
        bool bShowIndex /*= true*/, int nAccountCount /*= -1*/, const yl::string & strExtra /*= ""*/,
        const yl::string & strClickAction /*= ""*/)
{
    if (NULL == pPageData)
    {
        return false;
    }

    CSettingUIMenuItemData * pMenuItemData = static_cast<CSettingUIMenuItemData *>(
                CSettingUIMenuItemData::CreateMenuListDataItem());

    if (NULL == pMenuItemData)
    {
        return false;
    }

    pMenuItemData->SetItemID(strID);
    pMenuItemData->SetDisplayName(strName);
    pMenuItemData->SetShowIndex(bShowIndex);
    pMenuItemData->SetAuthorityID(strAuthoridID);

    if (!strClickAction.empty())
    {
        pMenuItemData->SetClickAction(strClickAction);
    }

    if (!strExtra.empty())
    {
        if (nAccountCount <= 1)
        {
            //½çÃæÎªPageID¶ÔÓ¦µÄ½çÃæ£¬¸½¼Ó²ÎÊýÊÇÄ¬ÈÏÕËºÅ
            pMenuItemData->SetInnerExtraDataType(WIDGET_VALUE_TYPE_INT);
            pMenuItemData->SetInnerExtraData(strExtra);
        }
        else
        {
            //½çÃæÎªÕËºÅÑ¡Ôñ½çÃæ£¬¸½¼Ó²ÎÊýÎª½çÃæ
            pMenuItemData->SetInnerExtraDataType(WIDGET_VALUE_TYPE_STRING);
            pMenuItemData->SetInnerExtraData(strExtra);
        }
    }

    if (!pPageData->AddItem(pMenuItemData))
    {
        delete pMenuItemData;

        pMenuItemData = NULL;

        return false;
    }

    return true;
}


bool CSettingUIDelegateBase::AddBaseItemData(CSettingUIPageDataBase * pPageData,
        const yl::string & strDisplayName,
        const yl::string & strValue, const yl::string & strID /*= ""*/,
        bool bShowIndex /*= true*/, bool bShowColon /*= true*/)
{
    if (NULL == pPageData)
    {
        return false;
    }

    CSettingUIItemDataBasePtr pItemData = new CSettingUIItemDataBase(SETTING_ITEM_TYPE_BASE);
    if (NULL == pItemData)
    {
        return false;
    }

    pItemData->SetDisplayName(strDisplayName);
    pItemData->SetShowIndex(bShowIndex);
    pItemData->SetShowColon(bShowColon);
    pItemData->SetValue(strValue);
    pItemData->SetItemID(strID);
    if (!m_pPageData->AddItem(pItemData))
    {
        delete pItemData;
        pItemData = NULL;
        return false;
    }
    return true;
}


bool CSettingUIDelegateBase::AddLineEditItemData(CSettingUIPageDataBase * pPageData,
        const yl::string & strID,
        const yl::string & strName,  const yl::string & strValue /*= ""*/,
        int nMaxLength/*= -1*/, const yl::string & strIme/*= ""*/,
        const yl::string & strDefaultIme/*= ""*/, bool bPassword /*= false*/,
        const yl::string & strAuthorid /*= ""*/, bool bAllowEmpty /*= false*/,
        bool bShowIndex /*= true*/, bool bShowColon/* = true*/)
{
    if (NULL == pPageData)
    {
        return false;
    }

    CSettingUILineEditItemDataPtr pLineEditItem = new CSettingUILineEditItemData;
    if (NULL != pLineEditItem)
    {
        pLineEditItem->SetPassword(bPassword);
        pLineEditItem->SetItemID(strID);
        pLineEditItem->SetAuthorityID(strAuthorid);
        pLineEditItem->SetDisplayName(strName);
        pLineEditItem->SetIME(strIme);
        pLineEditItem->SetDefaultIME(strDefaultIme);
        pLineEditItem->SetMaxLength(nMaxLength);
        pLineEditItem->SetShowIndex(bShowIndex);
        pLineEditItem->SetAllowEmpty(bAllowEmpty);
        pLineEditItem->SetShowColon(bShowColon);

        if (!pPageData->AddItem(pLineEditItem))
        {
            delete pLineEditItem;
            pLineEditItem = NULL;
            return false;
        }
    }
    return true;
}


void CSettingUIDelegateBase::SetItemShow(CSettingUIPageDataBase * pPagedata,
        const yl::string & strItemId, bool bShow /*= false*/)
{
    if (NULL != pPagedata)
    {
        CSettingUIItemDataBasePtr pItemData = pPagedata->GetItemByItemId(strItemId);
        if (NULL != pItemData)
        {
            bool bAuthorityShow = true;
            yl::string strAuthorityID = pItemData->GetAuthorityID();
            if (feature_UserModeIsOpen() && !strAuthorityID.empty())
            {
                bAuthorityShow = feature_UserModeConfigItemShow(strAuthorityID);
            }

            pItemData->SetShow(bAuthorityShow && bShow);
        }
    }
}

bool CSettingUIDelegateBase::IsItemShow(const yl::string & strItemId)
{
    if (NULL != m_pPageData)
    {
        CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByItemId(strItemId);
        if (NULL != pItemData)
        {
            return pItemData->IsShow();
        }
    }

    return false;
}

void CSettingUIDelegateBase::SetItemEnable(CSettingUIPageDataBase * pPagedata,
        const yl::string & strItemId, bool bEnable)
{
    if (NULL != pPagedata)
    {
        CSettingUIItemDataBasePtr pItemData = pPagedata->GetItemByItemId(strItemId);
        if (NULL != pItemData)
        {
            bool bAuthorityEnable = true;
            yl::string strAuthorityID = pItemData->GetAuthorityID();
            if (feature_UserModeIsOpen() && !strAuthorityID.empty())
            {
                bAuthorityEnable = !feature_UserModeConfigItemReadOnly(strAuthorityID);
            }

            pItemData->SetEnable(pPagedata->IsEnable() && bAuthorityEnable && bEnable);
        }
    }
}

void  CSettingUIDelegateBase::HideAllItem(CSettingUIPageDataBase * pPagedata)
{
    if (NULL == pPagedata)
    {
        return;
    }

    for (int i = 0; i < pPagedata->GetTotalItemNumber(); ++i)
    {
        CSettingUIItemDataBasePtr pItemData = pPagedata->GetItemByDataIndex(i);
        if (NULL != pItemData)
        {
            pItemData->SetShow(false);
        }
    }
}

void CSettingUIDelegateBase::LoadItemData(int nDataIndex)
{
    if (NULL != m_pPageData)
    {
        CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByDataIndex(nDataIndex);
        if (NULL != pItemData)
        {
            g_DataLoader->LoadItemData(pItemData);
        }
    }
}

bool CSettingUIDelegateBase::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    if (CheckTalking() || NULL == pPageData || !pPageData->IsEnable() || !m_bShow)
    {
#ifdef IF_SETTING_WIZARD
        if (NULL != g_pSettingUIWizardManager && g_pSettingUIWizardManager->IsInWizard())
        {
            return true;
        }
#endif
        return false;
    }
    CSettingUIDataValidInfo dataValid;
    if (!PageDataValid(pPageData, dataValid))
    {
        g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
        return false;
    }

    bool bResult = g_DataSaver->SaveData(pPageData);

    if (bResult)
    {
        SaveExtraData();

        if (bPopHint)
        {
            m_bBackToPreviousInterface = bBackToPrevious;
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_SAVING_CONFIG_FILE)
                                                   , SETTING_MESSAGEBOX_NOTE, "", "", NS_SET::g_SetPopNormalTime
                                                   , MSGBOX_ID_SAVE, (void*)(&m_bBackToPreviousInterface));
        }
    }
    else
    {
        if (bPopHint)
        {
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_SAVE_FAILED)
                                                   , SETTING_MESSAGEBOX_Cancel, "", "", NS_SET::g_SetPopNormalTime);
        }
    }

    return bResult;
}

void CSettingUIDelegateBase::SaveExtraData()
{

}

bool CSettingUIDelegateBase::OnBackAction()
{
    if (IsCheckDataWhileDropOutPage())
    {
        g_SetUICallBackFun.CacheDlgData(m_strPageID);
        if (NULL != m_pPageData && IsPageValueChange(m_pPageData))
        {
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE),
                                                   _LANG(TRID_DO_YOU_WANT_TO_DISCARD_YOUR_CHANGES)
                                                   , SETTING_MESSAGEBOX_OkCancel, "", "", -1, MSG_ID_DATA_WITHOUT_SAVE);
            return false;
        }
    }

    return true;
}

bool CSettingUIDelegateBase::OnBackToIdle()
{
    if (IsCheckDataWhileDropOutPage())
    {
        g_SetUICallBackFun.CacheDlgData(m_strPageID);
        if (NULL != m_pPageData && IsPageValueChange(m_pPageData))
        {
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE),
                                                   _LANG(TRID_DO_YOU_WANT_TO_DISCARD_YOUR_CHANGES)
                                                   , SETTING_MESSAGEBOX_OkCancel, "", "", -1, MSG_ID_BACK_TO_IDLE_DATA_WITHOUT_SAVE);
            return false;
        }
    }

    return true;
}

void CSettingUIDelegateBase::UpdatePagedata()
{
}

bool CSettingUIDelegateBase::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /*= false*/)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bUserModeOpen = feature_UserModeIsOpen();
    if (bUserModeOpen)
    {
        yl::string strPageAuthorid = pPagedata->GetPageAuthorid();
        if (!strPageAuthorid.empty())
        {
            pPagedata->SetEnable(!feature_UserModeConfigItemReadOnly(strPageAuthorid));
        }
    }

    int nTotal = pPagedata->GetTotalItemNumberInMenory();
    for (int iItem = 0; iItem < nTotal; ++iItem)
    {
        CSettingUIItemDataBasePtr pItemData = pPagedata->GetItemByDataIndexFromMenory(iItem);
        if (NULL == pItemData)
        {
            continue;
        }
        // 复位
        if (pItemData->IsResetable())
        {
            pItemData->SetShow(true);
            pItemData->SetEnable(true);
        }

        // 三级权限开启，才进行三级权限过滤
        yl::string strAuthorityID = pItemData->GetAuthorityID();
        if (bUserModeOpen && pItemData->IsShow())
        {
            if (!strAuthorityID.empty())
            {
                pItemData->SetShow(feature_UserModeConfigItemShow(strAuthorityID) && GetShowStatus());
                bool bItemEnable = !feature_UserModeConfigItemReadOnly(strAuthorityID);
                pItemData->SetEnable(pPagedata->IsEnable() && bItemEnable);
            }
            else
            {
                pItemData->SetShow(GetShowStatus());
                pItemData->SetEnable(pPagedata->IsEnable());
            }
        }

        // 配置过滤
        if (pItemData->IsShow())
        {
            // 动态过滤
            SettingDynamicConfigFilter * pDynamicFilter = g_pConfigFilterMgr->GetAllowShowDynamic(
                        strAuthorityID);
            if (NULL != pDynamicFilter)
            {
                FilterDynamicConfig(pDynamicFilter, pItemData, iItem);
            }
        } // end if 配置过滤
    }

    return true;
}

void CSettingUIDelegateBase::FilterDynamicConfig(SettingDynamicConfigFilter * pDynamicFilter,
        CSettingUIItemDataBase * pItemData, int iStartIndex)
{
    if (NULL == pDynamicFilter || NULL == pItemData || NULL == m_pPageData)
    {
        return;
    }

    bool bDeal = false;
    for (int i = iStartIndex - 1; i >= 0; --i)
    {
        CSettingUIItemDataBasePtr pItem = m_pPageData->GetItemByDataIndexFromMenory(i);
        if (NULL != pItem)
        {
            bool bShow = true;
            bool bEditable = true;
            bDeal = g_pConfigFilterMgr->DynamicFilter(pDynamicFilter, pItem->GetConfigID(),
                    pItem->GetAuthorityID()
                    , pItem->GetValue(), bShow, bEditable);
            if (bDeal)
            {
                pItemData->SetShow(bShow && pItemData->IsShow());
                pItemData->SetEnable(bEditable && pItemData->IsEnable());
            }
        }
    }
}

bool CSettingUIDelegateBase::CheckValue(SettingDynamicConfigFilter * pDynamicFilter,
                                        SettingDynamicItem * pDynamicItem
                                        , CSettingUIItemDataBase * pItemData, const yl::string & strV)
{
    LIST_STRING::iterator it = pDynamicItem->listValue.begin();
    for (; it != pDynamicItem->listValue.end(); ++it)
    {
        yl::string strFilter = *it;
        bool bR = (0 == strV.compare(strFilter.trim_both()));
        // 隐藏项(visiable时用与；unvisiable时用或)
        if ((1 == pDynamicFilter->eFilterType && !bR) || (0 == pDynamicFilter->eFilterType && bR))
        {
            pItemData->SetShow(false);
            return false;
        }
    }

    return true;
}

bool CSettingUIDelegateBase::PageDataValid(CSettingUIPageDataBase * pPagedata,
        CSettingUIDataValidInfo & validInfo)
{
    if (NULL == pPagedata)
    {
        return false;
    }

    int nTotal = pPagedata->GetTotalItemNumber();
    for (int i = 0; i < nTotal; ++i)
    {
        CSettingUIItemDataBasePtr pItem = pPagedata->GetItemByDataIndex(i);
        if (NULL != pItem)
        {
            CSettingUILineEditItemDataPtr pInputItem = CSettingUILineEditItemData::GetLineEditDataItem(pItem);
            if (NULL != pInputItem)
            {
                if (!pInputItem->IsEnable() || !pInputItem->IsShow())
                {
                    continue;
                }
                // 有条件判断，如开关开启才判断数据合法性。必须首先判断
                if (!pInputItem->GetCheckBaseID().empty())
                {
                    yl::string strCheckID = pInputItem->GetCheckBaseID();
                    // 获取当前界面被过滤项
                    int nTotal = m_pPageData->GetTotalItemNumber();
                    bool bChecked = false;
                    for (int i = nTotal - 1; i >= 0; --i)
                    {
                        CSettingUIItemDataBasePtr pItem = GetItemDataByDataIndex(i);
                        if (NULL != pItem && 0 == strCheckID.compare(pItem->GetItemID()))
                        {
                            if (0 != pInputItem->GetCheckBaseValue().compare(pItem->GetValue()))
                            {
                                // 限制项值不是指定值时，不进行数据校验
                                bChecked = true;
                                break;
                            }
                        }
                    }
                    if (bChecked)
                    {
                        continue;
                    }
                }
                // 字符为空校验
                if (IsContentEmpty(pInputItem->GetValue()))
                {
                    if (!pInputItem->IsAllowEmpty())
                    {
                        SetValidHintData(pInputItem, validInfo, TRID_TEXT_CANNOT_BE_EMPTY, i, ITEM_INVALID_DATA_TYPE_EMPTY);
                        return false;
                    }
                    else
                    {
                        continue;
                    }
                }
                // 长度校验
                int iValueLen = pInputItem->GetValue().length();
                if (pInputItem->GetMaxLength() > 0 && iValueLen > pInputItem->GetMaxLength())
                {
                    yl::string strNote = commonAPI_FormatString("String should not more than %d"
                                         , pInputItem->GetMaxLength());
                    SetValidHintData(pInputItem, validInfo, strNote, i, ITEM_INVALID_DATA_TYPE_NONE);
                    return false;
                }
                // 值域校验
                int nMin = pInputItem->GetMinValue();
                int nMax = pInputItem->GetMaxValue();
                yl::string strValue = pItem->GetValue();
                if (-1 != nMax && -1 != nMin && !isInRange(strValue, nMin, nMax))
                {
                    yl::string strNote = TRID_VALUE_MUSB_BE_BETWEEN;
                    strNote += commonAPI_FormatString("%d-%d", nMin, nMax);
                    SetValidHintData(pInputItem, validInfo, strNote, i, ITEM_INVALID_DATA_TYPE_RANGE);
                    return false;
                }
                // 正则表达式校验
                if (!pInputItem->GetRegularExpressions().empty())
                {
                    yl::string strRegex = pInputItem->GetRegularExpressions();
                    int iResult = regExp_IsMatchedExact(strRegex.c_str(), pInputItem->GetValue().c_str());
#if IF_SETTING_FILTER_DEBUG
                    printf("----------- [CSettingUIDelegateBase::PageDataValid] regex[%s][%s], value[%s], result[%d] =========\n"
                           , strRegex.c_str(), pInputItem->GetRegularExpressions().c_str()
                           , pInputItem->GetValue().c_str(), iResult);
#endif
                    if (-1 == iResult)
                    {
                        SETTINGUI_INFO("[CSettingUIDelegateBase::PageDataValid], regexRule[%s], value[%s]"
                                       , pInputItem->GetRegularExpressions().c_str(), pInputItem->GetValue().c_str());
                        SetValidHintData(pInputItem, validInfo, TRID_VALUE_IS_INVALID, i, ITEM_INVALID_DATA_TYPE_NONE);
                        return false;
                    }
                }
                // check method校验
                yl::string strCheckMethodType = pInputItem->GetCheckMethodType();
                if (!strCheckMethodType.empty()
                        && !g_CheckMethodFactory.CheckValueValidByType(strCheckMethodType, pInputItem->GetValue()))
                {
                    SetValidHintData(pInputItem, validInfo, TRID_VALUE_IS_INVALID, i, ITEM_INVALID_DATA_TYPE_EMPTY);
                    return false;
                }
            }
        }
    }

    for (int i = 0; i < nTotal; ++i)
    {
        CSettingUILineEditItemDataPtr pInputItem = CSettingUILineEditItemData::GetLineEditDataItem(
                    pPagedata->GetItemByDataIndex(i));
        if (NULL != pInputItem && -1 != pInputItem->GetMinValue() && -1 != pInputItem->GetMaxValue())
        {
            yl::string  strOldValue = pInputItem->GetValue();
            yl::string strRealValue = commonAPI_FormatString("%d", atoi(strOldValue.c_str()));
            pInputItem->SetValue(strRealValue);
        }
    }

    return true;
}

void CSettingUIDelegateBase::SetValidHintData(CSettingUILineEditItemData * pInputItem,
        CSettingUIDataValidInfo & validInfo
        , const yl::string & strDefaultHint, int nValidIndex, ITEM_INVALID_DATA_TYPE eType)
{
    if (NULL != pInputItem)
    {
        validInfo.SetValidIndex(nValidIndex);
        CSettingUIErrorInfoPtr pErrorInfo = pInputItem->GetItemErrorInfo();
        if (NULL != pErrorInfo)
        {
            yl::string strValidNote = "";
            if (ITEM_INVALID_DATA_TYPE_EMPTY == eType)
            {
                strValidNote = _LANG(pErrorInfo->GetEmptyError());
            }
            else if (ITEM_INVALID_DATA_TYPE_RANGE == eType)
            {
                strValidNote = _LANG(pErrorInfo->GetRangeError());
            }
            else if (ITEM_INVALID_DATA_TYPE_FORMAT == eType)
            {
                strValidNote = _LANG(pErrorInfo->GetFormateError());
            }

            validInfo.SetValidNote(strValidNote);
        }
        if (validInfo.GetValidNote().empty())
        {
            validInfo.SetValidNote(_LANG(strDefaultHint));
        }
    }
}

bool CSettingUIDelegateBase::IsPageValueChange(CSettingUIPageDataBase * pPagedata)
{
    int nTotItems = pPagedata->GetTotalItemNumber();
    for (int i = 0, j = 0; i < nTotItems; ++i)
    {
        CSettingUIItemDataBasePtr pItemData = pPagedata->GetItemByDataIndex(i);
        CSettingUIItemDataBase configItemData(SETTING_ITEM_TYPE_BASE);
        if (NULL != pItemData && pItemData->IsEnable())
        {
            configItemData.SetValue("");
            configItemData.SetValueType(pItemData->GetValueType());
            configItemData.SetConfigID(pItemData->GetConfigID());
            configItemData.SetItemID(pItemData->GetItemID());
            if (g_DataLoader->LoadItemData(&configItemData, m_iLineID)
                    && 0 != pItemData->GetValue().compare(configItemData.GetValue()))
            {
#if IF_SETTING_CONFIG_LOAD_SAVE_DEBUG
                SETTINGUI_INFO("[CSettingUIDelegateBase::IsPageValueChange] configID[%s] memValue[%s], cfgValue[%s], lineID[%d]"
                               , pItemData->GetConfigID().c_str()
                               , pItemData->GetValue().c_str(), configItemData.GetValue().c_str(), m_iLineID);
#endif
                return true;
            }
        }
    }

    return false;
}


void CSettingUIDelegateBase::SetPageDataPath(const yl::string & strPath)
{
    m_strPageDataPath = strPath;
}

const yl::string & CSettingUIDelegateBase::GetPageDataPath()
{
    return m_strPageDataPath;
}

bool CSettingUIDelegateBase::IsPageEditAble()
{
    if (NULL != m_pPageData)
    {
        return m_pPageData->IsEnable();
    }

    return true;
}

void CSettingUIDelegateBase::OnComboBoxValueChange(int nIndex)
{
    g_SetUICallBackFun.RefreshUI(false, m_strPageID);
}


void CSettingUIDelegateBase::OnEditValueChange(int nIndex)
{

}

CSettingUIItemDataBase * CSettingUIDelegateBase::GetItemDataByDataIndex(int nDataIndex)
{
    if (NULL == m_pPageData)
    {
        return NULL;
    }

    return m_pPageData->GetItemByDataIndex(nDataIndex);
}

CSettingUIItemDataBase * CSettingUIDelegateBase::GetItemByItemId(const yl::string & strItemId)
{
    if (NULL == m_pPageData)
    {
        return NULL;
    }

    return m_pPageData->GetItemByItemId(strItemId);
}

int CSettingUIDelegateBase::GetItemDataIndexByItemId(const yl::string & strItemId)
{
    if (NULL == m_pPageData)
    {
        return -1;
    }

    return m_pPageData->GetItemDataIndexByItemId(strItemId);
}

void CSettingUIDelegateBase::SetItemDataValueByDataIndex(int nDataIndex,
        const yl::string & strValue)
{
    CSettingUIItemDataBase* pBaseItemData = GetItemDataByDataIndex(GetFocusIndex());
    if (NULL == pBaseItemData || !pBaseItemData->IsNeedSaveData())
    {
        return;
    }

    if (NULL != m_pPageData)
    {
        m_pPageData->SetItemDataValueByDataIndex(nDataIndex, strValue);
    }
}

void CSettingUIDelegateBase::SetItemDataValueToConfigByDataIndex(int nDataIndex,
        const yl::string & strValue)
{
    if (NULL != m_pPageData)
    {
        // 同步更新保存到内存中
        SetItemDataValueByDataIndex(nDataIndex, strValue);
        bool bChange = false;
        g_DataSaver->SaveItemData(m_pPageData, m_pPageData->GetItemByDataIndex(nDataIndex), bChange);
    }
}

void * CSettingUIDelegateBase::GetItemExtraDataByDataIndex(int nDataIndex)
{
    CSettingUIItemDataBasePtr pItemData = GetItemDataByDataIndex(nDataIndex);
    if (NULL != pItemData)
    {
        return pItemData->GetExtraData();
    }

    return NULL;
}

int CSettingUIDelegateBase::GetTotalDataCount()
{
    if (NULL == m_pPageData)
    {
        return 0;
    }

    return m_pPageData->GetTotalItemNumber();
}

int CSettingUIDelegateBase::GetPageDataType()
{
    if (NULL == m_pPageData)
    {
        return -1;
    }

    return m_pPageData->GetPageDataType();
}

const yl::string & CSettingUIDelegateBase::GetTitleName()
{
    if (NULL == m_pPageData)
    {
        return NS_SET::kszEmptyString;
    }

    return m_pPageData->GetPageTitle();
}

const yl::string & CSettingUIDelegateBase::GetEmptyHint()
{
    if (NULL == m_pPageData)
    {
        return NS_SET::kszEmptyString;
    }

    return m_pPageData->GetEmptyHint();
}

const yl::string & CSettingUIDelegateBase::GetItemActionByDataIndex(int nDataIndex)
{
    CSettingUIItemDataBasePtr pItemData = GetItemDataByDataIndex(nDataIndex);

    if (NULL == pItemData)
    {
        return NS_SET::kszEmptyString;
    }

    return pItemData->GetItemAction();
}

const yl::string & CSettingUIDelegateBase::GetItemIDByDataIndex(int nDataIndex)
{
    CSettingUIItemDataBasePtr pItemData = GetItemDataByDataIndex(nDataIndex);
    if (NULL == pItemData)
    {
        return NS_SET::kszEmptyString;
    }

    return pItemData->GetItemID();
}

const yl::string & CSettingUIDelegateBase::GetItemFocusActionByDataIndex(int nDataIndex)
{
    CSettingUIItemDataBasePtr pItemData = GetItemDataByDataIndex(nDataIndex);
    if (NULL == pItemData)
    {
        return NS_SET::kszEmptyString;
    }

    return pItemData->GetItemFocusAction();
}

void CSettingUIDelegateBase::UpdateShowIndex()
{
    if (NULL != m_pPageData)
    {
        m_pPageData->UpdateShowIndex();
    }
}

bool CSettingUIDelegateBase::IsSetActiveWndWhileCreate()
{
    return false;
}

void CSettingUIDelegateBase::OnEnterPage(const yl::string & strPage, int iDataIndex)
{
    PageOpenData opPageData(strPage);
    opPageData.pExtraData = GetItemExtraDataByDataIndex(iDataIndex);
    g_SetUICallBackFun.EnterPage(opPageData);
}

//////////////////// 公共界面属性 //////////////////////
bool CSettingUIDelegateBase::IsRejectReturnToIdle()
{
#ifdef IF_SETTING_WIZARD
    if (NULL != g_pSettingUIWizardManager && g_pSettingUIWizardManager->IsInWizard())
    {
        return true;
    }
#endif

    if (NULL != m_pPageData)
    {
        return !m_pPageData->IsReturnToIdle();
    }

    return false;
}

void CSettingUIDelegateBase::OnReturnToIdle()
{
}

//////////////////// 事件、Softkey、按键相关 //////////////////////

bool CSettingUIDelegateBase::OnPageAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType && NULL != m_pPageData)
    {
        const yl::string & strAction = m_pPageData->GetAction();
        if (!strAction.empty())
        {
            return ProcessAction(strAction, iDataIndex);
        }
    }

    return false;
}

bool CSettingUIDelegateBase::OnAction(int eActionType, int iDataIndex)
{
    if (NULL == m_pPageData)
    {
        return false;
    }
#ifdef IF_SETTING_WIZARD
    if (NULL != g_pSettingUIWizardManager && g_pSettingUIWizardManager->IsInWizard())
    {
        if (SET_ACTION_TYPE_PREVIOUS == eActionType)
        {
            g_pSettingUIWizardManager->BackToPreviousInterface();
            return true;
        }
        else if (SET_ACTION_TYPE_NEXT == eActionType)
        {
            if (SavePageData(m_pPageData, false, false))
            {
                g_pSettingUIWizardManager->EnterNextWizardPage(GetPageID());
            }
            return true;
        }
        else if (SET_ACTION_TYPE_SKIP == eActionType)
        {
            g_pSettingUIWizardManager->QuitWizard();
            return true;
        }
        else if (SET_ACTION_TYPE_FINISH == eActionType)
        {
            if (SavePageData(m_pPageData, false, false))
            {
                g_pSettingUIWizardManager->FinishWizard();
            }
            return true;
        }
    }
#endif
    yl::string strAction("");
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        strAction = GetItemActionByDataIndex(iDataIndex);
    }
    else if (SET_ACTION_TYPE_FOCUS == eActionType)
    {
        strAction = GetItemFocusActionByDataIndex(iDataIndex);
    }
    else if (SET_ACTION_TYPE_BACK == eActionType)
    {
        strAction = NS_SET::kszActionBack;
    }
    else if (SET_ACTION_TYPE_SAVE == eActionType)
    {
        strAction = NS_SET::kszActionSave;
    }

    return ProcessAction(strAction, iDataIndex);
}

bool CSettingUIDelegateBase::ProcessAction(const yl::string strAction, int iDataIndex)
{
    if (strAction.empty())
    {
        return false;
    }

    int iFind = strAction.find(NS_SET::kszActionOpenPage, 0);
    if (-1 != iFind)
    {
        // openpage(xxx)  -->  xxx
        yl::string strPage = strAction.substr(iFind + 9, strAction.length() - 10);

        OnEnterPage(strPage, iDataIndex);
    }
    else if (0 == strAction.compare(NS_SET::kszActionBack))
    {
        if (OnBackAction())
        {
            g_SetUICallBackFun.FallBackToPrevousPage();
        }
    }
    else if (0 == strAction.compare(NS_SET::kszActionSave))
    {
        return SavePageData(m_pPageData);
    }
    else if (0 == strAction.compare(NS_SET::kszActionItemSave))
    {
        if (CheckTalking())
        {
            return true;
        }
        CSettingUIItemDataBasePtr pBaseItemData = m_pPageData->GetItemByDataIndex(iDataIndex);
        if (NULL != pBaseItemData)
        {
            SetItemDataValueToConfigByDataIndex(iDataIndex, pBaseItemData->GetValue());
        }
    }
    else
    {
        return false;
    }

    return true;
}

void CSettingUIDelegateBase::GetPageXmlSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (NULL == m_pPageData || SOFTKEY_NUM != vecSoftKey.size())
    {
        return;
    }

    CSettingUISoftkeyData * pSoftkeyData = m_pPageData->GetListSoftkeyData();
    if (NULL != pSoftkeyData)
    {
        for (int i = 0; i < SOFTKEY_NUM; ++i)
        {
            CSettingUISoftkeyItemData * pSoftkeyItemData = pSoftkeyData->GetSoftkeyItem(i);
            if (NULL != pSoftkeyItemData)
            {
                vecSoftKey[i].m_iType = TransStringToSoftkeyType(pSoftkeyItemData->GetAction());
                vecSoftKey[i].m_strSoftkeyTitle = pSoftkeyItemData->GetDisplayName();
            }
        }
    }
}

void CSettingUIDelegateBase::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
}

bool CSettingUIDelegateBase::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    int nSetAnctionType = g_pSettingActionTypeMgr->TransSoftkeyType2SettingActionType(iType);

    return OnAction(nSetAnctionType, nDataIndex);
}

bool CSettingUIDelegateBase::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    return false;
}

bool CSettingUIDelegateBase::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (PHONE_KEY_OK == nKeyCode && OnPageAction(SET_ACTION_TYPE_CLICK, nDataIndex))
    {
        return true;
    }

    return false;
}

void CSettingUIDelegateBase::InitExtraData(void * pData)
{
    if (NULL == pData)
    {
        return;
    }
}

bool CSettingUIDelegateBase::ProcessMsgCallBack(int nResult, int nID, void * pExtraData /*= NULL*/)
{
    if (MSG_ID_VALID_DATA == nID)
    {
        g_SetUICallBackFun.RefreshUI(false, m_strPageID);
        return true;
    }
    else if (MSG_ID_DATA_WITHOUT_SAVE == nID)
    {
        if (SETTING_MessageBox_OK == nResult)
        {
            g_SetUICallBackFun.FallBackToPrevousPage(m_strPageID);
        }
        return true;
    }
    else if (MSG_ID_BACK_TO_IDLE_DATA_WITHOUT_SAVE == nID)
    {
        if (SETTING_MessageBox_OK == nResult)
        {
            g_SetUICallBackFun.BackToIdle();
        }
        return true;
    }
    else if (m_bBackToPreviousInterface)
    {
        m_bBackToPreviousInterface = false;
        bool bRefreshPreviousView = false;
        if (NULL != m_pPageData && m_pPageData->IsRefreshPrevousView())
        {
            bRefreshPreviousView = true;
        }

        g_SetUICallBackFun.FallBackToPrevousPage(m_strPageID);

        if (bRefreshPreviousView)
        {
            g_SetUICallBackFun.RefreshUI(true);
        }

        return true;
    }

    return false;
}

int CSettingUIDelegateBase::TransStringToSoftkeyType(const yl::string & strSoftkeyAction)
{
    if (0 == strSoftkeyAction.compare(NS_SET::kszActionEnter))
    {
        return ST_ENTER;
    }
    else if (0 == strSoftkeyAction.compare(NS_SET::kszActionBack))
    {
        return ST_BACK;
    }
    else if (0 == strSoftkeyAction.compare(NS_SET::kszActionStart))
    {
        return ST_START;
    }
    if (0 == strSoftkeyAction.compare(NS_SET::kszActionSkip))
    {
        return ST_SKIP;
    }
    else if (0 == strSoftkeyAction.compare(NS_SET::kszActionNext))
    {
        return ST_NEXT;
    }
#ifdef IF_SETTING_WIZARD
    else if (0 == strSoftkeyAction.compare(NS_SET::kszActionPrevious))
    {
        return ST_PREVIOUS;
    }
    else if (0 == strSoftkeyAction.compare(NS_SET::kszActionFinish))
    {
        return ST_FINISH;
    }
#endif

    return ST_SAVE;
}

int CSettingUIDelegateBase::GetPageItemCount()
{
    return m_nItemCountPerPage;
}

void CSettingUIDelegateBase::SetPageItemCount(int nPageItem)
{
    m_nItemCountPerPage = nPageItem;
}
