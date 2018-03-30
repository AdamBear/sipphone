#include "acddispositioncodedelegate.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "settingui/include/itemnamedefine.h"

CACDDispositionCodeDelegate::CACDDispositionCodeDelegate()
    : m_nCallId(-1)
{

}

CACDDispositionCodeDelegate::~CACDDispositionCodeDelegate()
{

}

CSettingUIDelegateBase * CACDDispositionCodeDelegate::CreateACDDispositionCodeDelegate()
{
    CACDDispositionCodeDelegate * pDelegate = new CACDDispositionCodeDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CACDDispositionCodeDelegate::InitExtraData(void * pData)
{
    if (NULL == pData)
    {
        return;
    }

    m_nCallId = *(int *)pData;
}

CSettingUIPageDataBase * CACDDispositionCodeDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (!bReLoad || NULL == m_pPageData)
    {
        return m_pPageData;
    }

    //获取diposition code列表
    DispCodeList dcl;

#ifdef _WIN32
    DispCode dc;
    dc.m_strCode = "1001";
    dc.m_strCodeName = "CodeName1";
    dcl.push_back(dc);

    dc.m_strCode = "1002";
    dc.m_strCodeName = "CodeName2";
    dcl.push_back(dc);

    dc.m_strCode = "1003";
    dc.m_strCodeName = "CodeName3";
    dcl.push_back(dc);

    dc.m_strCode = "1004";
    dc.m_strCodeName = "CodeName4";
    dcl.push_back(dc);

    dc.m_strCode = "1005";
    dc.m_strCodeName = "CodeName5";
    dcl.push_back(dc);

#else
    ACD_GetDispCodeList(dcl);
#endif

    m_codeVec.clear();

    //添加数据项
    for (DispCodeList::iterator iter = dcl.begin(); iter != dcl.end(); ++iter)
    {
        PushbackItem((*iter).m_strCode, (*iter).m_strCodeName);
    }

    return m_pPageData;
}

bool CACDDispositionCodeDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return true;
    }

    int iRsnIdx = pPageData->GetFocusIndex();

    yl::string strCode = "";
    if (iRsnIdx == 0)
    {
        CSettingUIItemDataBasePtr pItemData = pPageData->GetItemByItemId(ACD_DISPOSITION_CODE_ENTERCODE);
        if (NULL != pItemData)
        {
            strCode = pItemData->GetValue();
        }
    }
    else if (iRsnIdx > 0)
    {
        // 去掉第一项的编辑框
        iRsnIdx--;
        if (iRsnIdx < m_codeVec.size())
        {
            strCode = m_codeVec[iRsnIdx];
        }
    }

    CallCenter_SendDispCode(strCode, m_nCallId);

    g_SetUICallBackFun.FallBackToPrevousPage();

    return true;
}

bool CACDDispositionCodeDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        eActionType = SET_ACTION_TYPE_SAVE;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

void CACDDispositionCodeDelegate::PushbackItem(const yl::string & strCode,
        const yl::string & strName)
{
    if (strCode.empty())
    {
        // 如果Code为空，则不添加
        return;
    }

    CSettingUIItemDataBasePtr pItemData = new CSettingUIItemDataBase(SETTING_ITEM_TYPE_BASE);
    if (NULL == pItemData)
    {
        return;
    }

    pItemData->SetDisplayName(strCode);
    pItemData->SetShowIndex(true);
    pItemData->SetValue(strName);
    if (!m_pPageData->AddItem(pItemData))
    {
        delete pItemData;
    }

    //记录Reason项，用于操作
    m_codeVec.push_back(strCode);
}

void CACDDispositionCodeDelegate::SetCallID(int iCallID)
{
    m_nCallId = iCallID;
}
