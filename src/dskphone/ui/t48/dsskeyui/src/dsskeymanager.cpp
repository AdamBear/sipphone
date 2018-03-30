#include "dsskeymanager.h"
#include "dsskeybtnbase.h"
#include "dsskeyframe.h"
#include <algorithm>
#include "moddsskeyui.h"
#include "dsskey/include/moddsskey.h"
#include "dsskeyui/include/dsskeyui_common.h"
#include "dsskeyuilogic.h"
#include "adapterbox/include/modadapterbox.h"

void CDsskeyFeatureAction::OnDsskeyAction(int iDsskeyID)
{
    DssKeyLogicData logicData;

    dsskey_GetData(iDsskeyID, logicData);
    if (DT_NA == logicData.eDKType)
    {
#if IF_FEATURE_FAVORITE_DIR
        if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) != 1)
#endif
        {
            if (AdapterBox_TalkUIIsInTalk())
            {
                //通话中不能进入配置界面
            }
            else
            {
                dsskeyUI_OnDsskeyLongPress(iDsskeyID);
            }
        }
    }
    else
    {
        dsskey_OnDsskeyClick(iDsskeyID);
    }
}

void CDsskeyLongClickAction::OnDsskeyAction(int iDsskeyID)
{
    DSSKEYUI_DEBUG("OnLongClickAction : iDsskeyID is %d.", iDsskeyID);
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

// CDsskeyDataDelegateBase::CDsskeyDataDelegateBase()
// {
//  g_pDsskeySubject->AddDataDelegate(this);
// }
//
// CDsskeyDataDelegateBase::~CDsskeyDataDelegateBase()
// {
//  g_pDsskeySubject->DeleteDataDelegate(this);
// }
//
// void CDsskeyDataDelegateBase::BindDsskeyFrame(qDsskeyFrame* pDsskeyFrame)
// {
//  if (NULL != pDsskeyFrame)
//  {
//      m_pDsskeyFrame = pDsskeyFrame;
//  }
// }
//
// void CDsskeyDataDelegateBase::OnDsskeyFrameShow()
// {
//  //g_pDsskeySubject->AddDataDelegate(this);
// }
//
// void CDsskeyDataDelegateBase::OnDsskeyFrameHide()
// {
//  //g_pDsskeySubject->DeleteDataDelegate(this);
// }
//
// void CDsskeyDataDelegateBase::RefreshDsskeyData(const DsskeyDisplayData& objDisplayData)
// {
//  if (NULL != m_pDsskeyFrame)
//  {
//      m_pDsskeyFrame->RefreshDsskeyData(objDisplayData);
//  }
// }
//
// void CDsskeyDataDelegateBase::RefreshDsskeyData(int iIndex, const DsskeyDisplayData& objDisplayData)
// {
//  if (NULL != m_pDsskeyFrame)
//  {
//      m_pDsskeyFrame->RefreshDsskeyData(iIndex, objDisplayData);
//  }
// }

//////////////////////////////////////////////////////////////////////////
CDsskeyFrameManager * CDsskeyFrameManager::m_pOneself = NULL;

CDsskeyFrameManager * CDsskeyFrameManager::GetInstance()
{
    if (NULL == m_pOneself)
    {
        m_pOneself = new CDsskeyFrameManager();
    }

    return m_pOneself;
}

void CDsskeyFrameManager::ReleaseInstance()
{
    if (NULL != m_pOneself)
    {
        delete m_pOneself;
        m_pOneself = NULL;
    }
}

void CDsskeyFrameManager::OnNotify(const DsskeyDisplayData & objDisplayData)
{
    list_pDsskeyFrame::iterator it = m_listpDsskeyFrame.begin();
    for (/* */; it != m_listpDsskeyFrame.end(); ++it)
    {
        if (NULL != (*it))
        {
            (*it)->RefreshDsskeyData(objDisplayData);
            DSSKEYUI_DEBUG("Dsskey FrameManager Notify : index is %d.", objDisplayData.iIndex);
        }
    }
}

void CDsskeyFrameManager::RefreshDsskeyGroundPic(const DsskeyDisplayData & objDisplayData)
{
    list_pDsskeyFrame::iterator it = m_listpDsskeyFrame.begin();
    for (/* */; it != m_listpDsskeyFrame.end(); ++it)
    {
        if (NULL != (*it))
        {
            (*it)->RefreshDsskeyGroundPic(objDisplayData);
            DSSKEYUI_DEBUG("Dsskey FrameManager RefreshDsskeyGroundPic : index is %d.", objDisplayData.iIndex);
        }
    }
}

void CDsskeyFrameManager::NotifyByIndex(int iIndex, const DsskeyDisplayData & objDisplayData)
{
    list_pDsskeyFrame::iterator it = m_listpDsskeyFrame.begin();
    for (/* */; it != m_listpDsskeyFrame.end(); ++it)
    {
        if (NULL != (*it))
        {
            (*it)->RefreshDsskeyData(iIndex, objDisplayData);
            DSSKEYUI_DEBUG("Dsskey FrameManager Notify by index[%d].", iIndex);
        }
    }
}

bool CDsskeyFrameManager::AddDsskeyFrame(qDsskeyFrame * pDsskeyFrame)
{
    // 判空
    if (NULL == pDsskeyFrame)
    {
        return false;
    }
    list_pDsskeyFrame::iterator it = m_listpDsskeyFrame.begin();
    for (/* */; it != m_listpDsskeyFrame.end(); ++it)
    {
        if (pDsskeyFrame == (*it))
        {
            break;
        }
    }
    // 如果代理已经存在,则不再添加
    if (it != m_listpDsskeyFrame.end())
    {
        return false;
    }

    m_listpDsskeyFrame.push_back(pDsskeyFrame);

    return true;
}

bool CDsskeyFrameManager::DeleteDsskeyFrame(qDsskeyFrame * pDsskeyFrame)
{
    // 判空
    if (NULL == pDsskeyFrame)
    {
        return false;
    }
    list_pDsskeyFrame::iterator it = m_listpDsskeyFrame.begin();
    for (/* */; it != m_listpDsskeyFrame.end(); ++it)
    {
        if (pDsskeyFrame == (*it))
        {
            break;
        }
    }
    // 如果代理存在,则删除
    if (it != m_listpDsskeyFrame.end())
    {
        m_listpDsskeyFrame.erase(it);
    }
    return true;
}

bool CDsskeyFrameManager::SetDisplayDelegate(int iBtnIndex, DsskeyBtnDisplayDelegete * pDelegate)
{
    bool bRet = false;
    list_pDsskeyFrame::iterator it = m_listpDsskeyFrame.begin();
    for (/* */; it != m_listpDsskeyFrame.end(); ++it)
    {
        if (NULL != (*it))
        {
            bRet = (*it)->SetDisplayDelegate(iBtnIndex, pDelegate);
            DSSKEYUI_DEBUG("DsskeyFrame Manager Set Display Delegate : index is %d.", iBtnIndex);
        }
    }
    return bRet;
}

void CDsskeyFrameManager::DebindDisplayDelegate(int iBtnIndex)
{
    list_pDsskeyFrame::iterator it = m_listpDsskeyFrame.begin();
    for (/* */; it != m_listpDsskeyFrame.end(); ++it)
    {
        if (NULL != (*it))
        {
            (*it)->DebindDisplayDelegate(iBtnIndex);
            DSSKEYUI_DEBUG("DsskeyFrame Manager Debind Delegate : index is %d.", iBtnIndex);
        }
    }
}

// 刷新所有当前在显示的Dsskey Frame
void CDsskeyFrameManager::RefreshAllDisplay()
{
    list_pDsskeyFrame::iterator it = m_listpDsskeyFrame.begin();
    for (/* */; it != m_listpDsskeyFrame.end(); ++it)
    {
        if (NULL != (*it))
        {
            if ((*it)->isVisible())
            {
                (*it)->update();
            }
        }
    }
}

// void CDsskeyDataDelegateSetting::RefreshDsskeyData(const DssKeyLogicData& objLogicData)
// {
//  // to do : parse logic data to display data
// //   DsskeyDisplayData objDisplayData;
// //   bool ret = dsskeyUI_ParseLogicData(objDisplayData, objLogicData);
// //   if (ret == true)
// //   {
// //       RefreshDsskeyData(objDisplayData);
// //   }
// //   else
// //   {
// //       DSSKEY_DEBUG("CDsskeyDataDelegateBase : dsskeyUI_ParseLogicData Failed.");
// //   }
// }
//
//
// void CDsskeyDataDelegateSetting::OnDsskeyFrameShow()
// {
//  g_pDsskeySubject->AddDataDelegate(this);
// }
//
// void CDsskeyDataDelegateSetting::OnDsskeyFrameHide()
// {
//  g_pDsskeySubject->DeleteDataDelegate(this);
// }
