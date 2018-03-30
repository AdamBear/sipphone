#include "mtswacdselectdispcodedelegate.h"

#if IF_FEATURE_METASWITCH_ACD

#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/modsettingui.h"
#include "settinguimanager.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"

#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/modmessagebox.h"

#include "account/include/modaccount.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"

namespace
{
#define ACD_SELECTCODE_NOTE_MSGBOX_SHOW_TIME     1500
}

CMtswAcdSelectDispcodeDelegate::CMtswAcdSelectDispcodeDelegate()
{
}

CMtswAcdSelectDispcodeDelegate::~CMtswAcdSelectDispcodeDelegate()
{
}

CSettingUIDelegateBase* CMtswAcdSelectDispcodeDelegate::CreatMtswAcdSelectDispcodeDelegate()
{
    CMtswAcdSelectDispcodeDelegate* pDelegate = new CMtswAcdSelectDispcodeDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CMtswAcdSelectDispcodeDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    if (vecSoftKey.size() < MIN_SETTING_SOFTKEY_NUM)
    {
        return;
    }

    vecSoftKey[3].m_iType = ST_OK;
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_OK);
}

bool CMtswAcdSelectDispcodeDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_MTSW_ACD_MEMBER == iType)
    {
        OnOKAction();
        return true;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

void CMtswAcdSelectDispcodeDelegate::InitExtraData(void* pData)
{
}

bool CMtswAcdSelectDispcodeDelegate::SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    return false;
}

bool CMtswAcdSelectDispcodeDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (bPress && PHONE_KEY_OK == nKeyCode)
    {
        OnOKAction();
        return true;
    }

    return CSettingUIDelegateBase::OnKeyEvent(nKeyCode, bPress, nDataIndex);
}

void CMtswAcdSelectDispcodeDelegate::GetSelectDispcode()
{
    if (NULL == m_pPageData)
    {
        return;
    }

    const SMLGHQueuesData* pData = MTSWACD_GetCurQueuesData();
    if (NULL == pData)
    {
        return;
    }

    CSettingUIComboBoxItemData* pComboItemData = LoadComboboxItem(MESW_DISPCODE_QUEUE,
            _LANG(TRID_MTSW_DISPCODE_QUEUE), true);
    if (NULL != pComboItemData)
    {
        // 设置下拉框数据
        int iMLHGIndex = 0;
        yl::string strLastAnseredMLHG = MTSWACD_GetLastAnseredMHLG();

        VEC_SMLGG_DATA vecMLGHData;
        // 获取MLHG数据
        MTSWACD_GetMLHGList(vecMLGHData);

        for (int i = 0; i < vecMLGHData.size(); ++i)
        {
            if (strLastAnseredMLHG == vecMLGHData[i].strName)
            {
                iMLHGIndex = i;
            }
            AddComboboxoption(pComboItemData, vecMLGHData[i].strName, vecMLGHData[i].strName);
        }
        SetComboboxValue(MESW_DISPCODE_QUEUE, iMLHGIndex);
    }

    LoadComboboxItem(MESW_DISPCODE_CODE, _LANG(TRID_MTSW_DISPCODE_QUEUE),
                     true);
}

CSettingUIPageDataBase* CMtswAcdSelectDispcodeDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIPageDataBase* pPageData = CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData)
    {
        return NULL;
    }

    if (bReLoad)
    {
        pPageData->SetPageTitle(_LANG(TRID_MTSW_ACD_LEAVE_WRAPUP));
        GetSelectDispcode();
    }

    return pPageData;
}

void CMtswAcdSelectDispcodeDelegate::OnComboBoxValueChange(int nIndex)
{
    if (NULL == m_pPageData || MESW_DISPCODE_QUEUE != GetItemIDByDataIndex(nIndex))
    {
        return;
    }

    CSettingUIItemDataBase* pItemData = GetItemDataByDataIndex(GetItemDataIndexByItemId(MESW_DISPCODE_CODE));
    CSettingUIComboBoxItemData* pComboItemData = CSettingUIComboBoxItemData::GetComboBoxDataItem(pItemData);
    if (NULL == pComboItemData)
    {
        return;
    }

    yl::string strMlhgName;
    strMlhgName = GetValue(MESW_DISPCODE_QUEUE);

    VEC_DISPCODE    vecDispcode;
    // 获取code列表
    MTSWACD_GetDispCodeList(vecDispcode, strMlhgName);
    if (0 == vecDispcode.size())
    {
        AddComboboxoption(pComboItemData, _LANG(TRID_SOFTKEY_MTSW_ACD_NODISPCODE), "");
    }
    else
    {
        if (MTSWACD_IsDispCodeRequired(strMlhgName))
        {
            AddComboboxoption(pComboItemData, _LANG(TRID_SOFTKEY_MTSW_ACD_SELDISPCODE), "");
        }
        else
        {
            AddComboboxoption(pComboItemData, _LANG(TRID_SOFTKEY_MTSW_ACD_SELDISPCODE_OPTIONAL), "");
        }

        for (int j = 0; j <= vecDispcode.size(); ++j)
        {
            AddComboboxoption(pComboItemData, vecDispcode[j], vecDispcode[j]);
        }
    }
}

void CMtswAcdSelectDispcodeDelegate::OnOKAction()
{
    yl::string strMlhgName, strDispCode;

    strMlhgName = GetValue(MESW_DISPCODE_QUEUE);
    strDispCode = GetValue(MESW_DISPCODE_CODE);


    if (MTSWACD_IsDispCodeRequired(strMlhgName)
            && strDispCode.empty())
    {
        g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWAcdDetail, _LANG(TRID_NOTE), _LANG(TRID_MTSW_ACD_SELECT_DISPCODE),
                                               SETTING_MESSAGEBOX_NOTE, "", "", ACD_SELECTCODE_NOTE_MSGBOX_SHOW_TIME);
        return;
    }

    MTSWACD_SetDispCode(strDispCode, strMlhgName);
    g_SetUICallBackFun.FallBackToPrevousPage();
}

CSettingUIComboBoxItemData* CMtswAcdSelectDispcodeDelegate::LoadComboboxItem(const yl::string& strItemID,
        const yl::string& strDisplayName, bool bReadOnly)
{
    if (NULL == m_pPageData)
    {
        return NULL;
    }

    CSettingUIComboBoxItemData* pItemData = new CSettingUIComboBoxItemData;
    if (NULL != pItemData)
    {
        pItemData->SetItemID(strItemID);
        pItemData->SetDisplayName(strDisplayName);
        pItemData->SetShowIndex(true);
        pItemData->SetEnable(!bReadOnly);
    }
    return pItemData;
}

void CMtswAcdSelectDispcodeDelegate::AddComboboxoption(CSettingUIComboBoxItemData*
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

#endif

