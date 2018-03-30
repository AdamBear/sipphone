#include "dsskeydelegate.h"
#include "cdsskeybutton.h"
#include "clinekeyui.h"
#include "moddsskeyui.h"
#include "dsskey/include/moddsskey.h"
#include "dsskeyuilogic.h"

void CDsskeyFeatureAction::OnDsskeyAction(int iDsskeyID)
{
    DssKeyLogicData logicData;

    // 暂时无 为NA时单击进入设置界面 的需求
//  dsskey_GetData(iDsskeyID, logicData);
//  if (DT_NA == logicData.eDKType)
//  {
//      dsskeyUI_OnDsskeyLongPress(iDsskeyID);
//  }
//  else
    {
        dsskey_OnDsskeyClick(iDsskeyID);
    }
}

void CDsskeySettingAction::OnDsskeyAction(int iDsskeyID)
{
    DSSKEYUI_INFO("OnSettingAction : iDsskeyID is %d.", iDsskeyID);
    int iBtnIndex = dsskey_GetKeyIndex(iDsskeyID);
    // TODO ENTER SETTING
    dsskeyUI_EnterSetting(iBtnIndex);
}

void CDsskeyLongClickAction::OnDsskeyAction(int iDsskeyID)
{
    DSSKEYUI_INFO("OnLongClickAction : iDsskeyID is %d.", iDsskeyID);
    // 主要是为了解决融合锁定状态的iDsskeyID正确获取
    int iBtnIndex = dsskey_GetKeyIndex(iDsskeyID);
    int iRealDsskeyID;
    if (iBtnIndex >= _DssKeyUILogic.GetNumPerPage())
    {
        iRealDsskeyID = iDsskeyID;
    }
    else
    {
        int iRealIndex = _DssKeyUILogic.HardwareKeyToLogicIndex(iBtnIndex);
        iRealDsskeyID = dsskey_GetDsskeyID(DMT_LINEKEY_MODULE, 0, iRealIndex);
    }

    dsskeyUI_OnDsskeyLongPress(iRealDsskeyID);
}

CDsskeyDataDelegateBase::CDsskeyDataDelegateBase()
{
    g_pDsskeySubject->AddDataDelegate(this);
}

CDsskeyDataDelegateBase::~CDsskeyDataDelegateBase()
{
    g_pDsskeySubject->DeleteDataDelegate(this);
}

void CDsskeyDataDelegateBase::BindDsskeyFrame(CLinekeyUI* pLinekeyUI)
{
    if (NULL != pLinekeyUI)
    {
        m_pLinekeyUI = pLinekeyUI;
    }
}

void CDsskeyDataDelegateBase::OnDsskeyFrameShow()
{
    //g_pDsskeySubject->AddDataDelegate(this);
}

void CDsskeyDataDelegateBase::OnDsskeyFrameHide()
{
    //g_pDsskeySubject->DeleteDataDelegate(this);
}

void CDsskeyDataDelegateBase::RefreshDsskeyData(const DsskeyDisplayData& objDisplayData)
{
    if (NULL != m_pLinekeyUI)
    {
        m_pLinekeyUI->RefreshDsskeyData(objDisplayData);
    }
}

void CDsskeyDataDelegateBase::RefreshDsskeyData(int iIndex, const DsskeyDisplayData& objDisplayData)
{
    if (NULL != m_pLinekeyUI)
    {
        m_pLinekeyUI->RefreshDsskeyData(iIndex, objDisplayData);
    }
}

void CDsskeyDataDelegateBase::UpdatePageInfo(int iCurPageNum, int iCurrentPageIndex, bool bVisible/* = false*/)
{
    if (NULL != m_pLinekeyUI)
    {
        m_pLinekeyUI->SetPageInfo(iCurPageNum, iCurrentPageIndex);
        m_pLinekeyUI->SetPageBtnVisible(bVisible);
    }
}

void CDsskeyDataDelegateBase::UpdatePageIcon(int iPageNum, DSS_KEY_COLOR_TYPE eColor, DSS_KEY_FLUSH_TYPE eFlush)
{
    if (NULL != m_pLinekeyUI)
    {
        m_pLinekeyUI->FlashPageIcon(iPageNum, eColor, eFlush);
    }
}

//////////////////////////////////////////////////////////////////////////
CDsskeySubject* CDsskeySubject::m_pOneself = NULL;

CDsskeySubject* CDsskeySubject::GetInstance()
{
    if (NULL == m_pOneself)
    {
        m_pOneself = new CDsskeySubject();
    }

    return m_pOneself;
}

void CDsskeySubject::ReleaseInstance()
{
    if (NULL != m_pOneself)
    {
        delete m_pOneself;
        m_pOneself = NULL;
    }
}

void CDsskeySubject::OnNotify(const DsskeyDisplayData& objDisplayData)
{
    YLList<CDsskeyDataDelegateBase*>::iterator it = m_listDelegate.begin();
    for (/* */; it != m_listDelegate.end(); ++it)
    {
        if (NULL != (*it))
        {
            (*it)->RefreshDsskeyData(objDisplayData);
            DSSKEYUI_DEBUG("CDsskeySubject->OnNotify : iDsskeyID is %d.", objDisplayData.iDsskeyID);
        }
    }
}

void CDsskeySubject::NotifyByIndex(int iIndex, const DsskeyDisplayData& objDisplayData)
{
    YLList<CDsskeyDataDelegateBase*>::iterator it = m_listDelegate.begin();
    for (/* */; it != m_listDelegate.end(); ++it)
    {
        if (NULL != (*it))
        {
            (*it)->RefreshDsskeyData(iIndex, objDisplayData);
            DSSKEYUI_DEBUG("CDsskeySubject->OnNotify : index is %d.", iIndex);
        }
    }
}

void CDsskeySubject::OnNotifyPageInfo(int iCurPageNum, int iCurrentPageIndex, bool bVisible/* = false*/)
{
    YLList<CDsskeyDataDelegateBase*>::iterator it = m_listDelegate.begin();
    for (/* */; it != m_listDelegate.end(); ++it)
    {
        if (NULL != (*it))
        {
            (*it)->UpdatePageInfo(iCurPageNum, iCurrentPageIndex, bVisible);
            DSSKEYUI_DEBUG("OnNotifyPageInfo : iCurPageNum(%d) iCurrentPageIndex(%d) bVisible(%d).", \
                           iCurPageNum, iCurrentPageIndex, bVisible);
        }
    }
}

void CDsskeySubject::OnNotifyPageIcon(int iPageNum, DSS_KEY_COLOR_TYPE eColor, DSS_KEY_FLUSH_TYPE eFlush/* = REFRESH_TYPE_NO_FLUSH*/)
{
    YLList<CDsskeyDataDelegateBase*>::iterator it = m_listDelegate.begin();
    for (/* */; it != m_listDelegate.end(); ++it)
    {
        if (NULL != (*it))
        {
            (*it)->UpdatePageIcon(iPageNum, eColor, eFlush);
            DSSKEYUI_DEBUG("OnNotifyPageIcon : iPageNum(%d) eColor(%d).", \
                           iPageNum, eColor);
        }
    }
}

void CDsskeySubject::AddDataDelegate(CDsskeyDataDelegateBase* pDataDelegate)
{
    // 判空
    if (NULL == pDataDelegate)
    {
        return;
    }
    YLList<CDsskeyDataDelegateBase*>::iterator it = m_listDelegate.begin();
    for (/* */; it != m_listDelegate.end(); ++it)
    {
        if (pDataDelegate == (*it))
        {
            break;
        }
    }
    // 如果代理已经存在,则不再添加
    if (it != m_listDelegate.end())
    {
        return;
    }

    m_listDelegate.push_back(pDataDelegate);
}

void CDsskeySubject::DeleteDataDelegate(CDsskeyDataDelegateBase* pDataDelegate)
{
    // 判空
    if (NULL == pDataDelegate)
    {
        return;
    }
    YLList<CDsskeyDataDelegateBase*>::iterator it = m_listDelegate.begin();
    for (/* */; it != m_listDelegate.end(); ++it)
    {
        if (pDataDelegate == (*it))
        {
            break;
        }
    }
    // 如果代理存在,则删除
    if (it != m_listDelegate.end())
    {
        m_listDelegate.erase(it);
    }
}


void CDsskeyDataDelegateSetting::RefreshDsskeyData(const DssKeyLogicData& objLogicData)
{
    // to do : parse logic data to display data
//  DsskeyDisplayData objDisplayData;
//  bool ret = dsskeyUI_ParseLogicData(objDisplayData, objLogicData);
//  if (ret == true)
//  {
//      RefreshDsskeyData(objDisplayData);
//  }
//  else
//  {
//      DSSKEY_DEBUG("CDsskeyDataDelegateBase : dsskeyUI_ParseLogicData Failed.");
//  }
}


void CDsskeyDataDelegateSetting::OnDsskeyFrameShow()
{
    g_pDsskeySubject->AddDataDelegate(this);
}

void CDsskeyDataDelegateSetting::OnDsskeyFrameHide()
{
    g_pDsskeySubject->DeleteDataDelegate(this);
}
