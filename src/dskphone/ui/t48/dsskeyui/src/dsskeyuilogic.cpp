#include "dsskeyuilogic.h"
#include "cdsskeylightlogic.h"
#include "configparser/modconfigparser.h"
#include "configiddefine.h"
#include "moddsskeyui.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "dsskey/include/moddsskey.h"
#include "globalmodel.h"
#include "uicommon/uimanager/uimanager_inc.h"
#include "keymap.h"
#include "dsskeymanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "service_config.h"
#include "moddsskeyperformance.h"

int Divide(int dividend, int divisor, int * premainder = NULL)
{
    int iRet = -1;
    // 除数为零无效
    if (0 == divisor)
    {
        DSSKEY_WARN("The divisor cann't be zero.");
        iRet = -1;
        if (NULL != premainder)
        {
            (*premainder) = -1;
        }
    }
    else
    {
        iRet = dividend / divisor;
        if (NULL != premainder)
        {
            (*premainder) = dividend % divisor;
        }
    }
    return iRet;
}

LRESULT dsskeyUI_OnConfigChange(msgObject & msg)
{
    bool bHandled = true;
    switch (msg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (ST_PAGE_TIP == msg.wParam)
            {
                DSSKEY_INFO("dsskey config change ST_PAGE_TIP");
                BOOL bPageEnable = configParser_GetConfigInt(kszEnablePageTips) != 0;
                _DssKeyUILogic.SwitchPageTip(bPageEnable);
                bHandled = true;
            }
        }
        break;
    default:
        {
            bHandled = false;
        }
        break;
    }
    return bHandled;
}

CDssKeyUILogic & CDssKeyUILogic::GetInstance()
{
    static CDssKeyUILogic sObjDsskeyLogic;
    return sObjDsskeyLogic;
}

CDssKeyUILogic::CDssKeyUILogic()
{
    m_bForbidden = FALSE;
    m_bEnablePageTips = configParser_GetConfigInt(kszEnablePageTips) != 0;
    m_iCurPageNum = 1;
    m_iPageTotal = 1;
    m_iUpdateCount = 0;

    // 默认至少有一页，GoToPage中判断会用到
    m_iMaxPage = 1;
    m_iCurrentPageIndex = -1;

}

CDssKeyUILogic::~CDssKeyUILogic()
{
}

// 初始化界面数据
void CDssKeyUILogic::Init()
{
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, \
                          CONFIG_MSG_BCAST_CHANGED, dsskeyUI_OnConfigChange);
}

// 释放界面数据
void CDssKeyUILogic::Uninit()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, \
                            CONFIG_MSG_BCAST_CHANGED, dsskeyUI_OnConfigChange);
}

// 获取每页Dsskey个数
int CDssKeyUILogic::GetNumPerPage()
{
    if (m_iLinenumPerPage < 0)
    {
        return 1;
    }
    return m_iLinenumPerPage;
}

void CDssKeyUILogic::SetMaxNumPerPage(int iMaxNumPerPage)
{
    if (iMaxNumPerPage < 0)
    {
        return;
    }
    m_iMaxNumPerPage = iMaxNumPerPage;

    // 设置一页中灯的最大个数
    g_pDssKeyLightLogic->SetMaxNumPerPage(iMaxNumPerPage);
    // 设置翻页键索引
    m_iPageIndex = m_iMaxNumPerPage - 1;
    m_iLinenumPerPage = m_iMaxNumPerPage;
    DsskeyDisplayData * pDispData = NULL;
    m_vecCurrentShow.clear();
    // m_aryPage必须先初始化再翻页
    for (int i = 0; i < m_iMaxNumPerPage; ++i)
    {
        m_vecCurrentShow.push_back(pDispData);
    }
}

void CDssKeyUILogic::SetMaxPage(int iMaxPage)
{
    if (iMaxPage < 0)
    {
        return;
    }
    m_iMaxPage = iMaxPage;
    m_vecPageButtonInfo.clear();
    DsskeyDisplayData DisplayData;
    for (int i = 0; i < iMaxPage; ++i)
    {
        m_vecPageButtonInfo.push_back(DisplayData);
    }
}

void CDssKeyUILogic::SetMaxNumber(int iMaxNumber)
{
    if (iMaxNumber < 0)
    {
        return;
    }
    m_iMaxNumber = iMaxNumber;
    // 设置Linekey灯的最大个数
    g_pDssKeyLightLogic->SetMaxNumber(iMaxNumber);
    m_vecDsskeyInfos.clear();
    DsskeyDisplayData DisplayData;
    for (int i = 0; i < m_iMaxNumber; ++i)
    {
        DisplayData.iIndex = i;
        m_vecDsskeyInfos.push_back(DisplayData);
    }

    // 判断显示数据指针是否已被初始化
    if (m_vecCurrentShow.size() == 0)
    {
        DSSKEYUI_WARN("m_vecCurrentShow.size() is empty.");
        return;
    }

    // 默认显示第一页
    GoToPage(0);
}

bool CDssKeyUILogic::IsDsskey(int iKey)
{
    bool ret = false;
    // 判断
    if (iKey >= PHONE_KEY_LINE1
            && iKey <= PHONE_KEY_LINE1 + m_iMaxNumber - 1)
    {
        ret = true;
    }

    if (m_iMaxNumber < 0)
    {
        ret = false;
    }
    return ret;
}

int CDssKeyUILogic::HardwareKeyToLogicIndex(int iButtonIndex)
{
    if (iButtonIndex < 0
            || iButtonIndex >= m_iMaxNumPerPage)
    {
        // 移除情况
        return -1;
    }

    if (m_iPageTotal == 1)
    {
        // 只有1页时，第一页为10个dsskey
        return iButtonIndex;
    }
    else
    {
        return m_iCurrentPageIndex * (m_iMaxNumPerPage - 1) + iButtonIndex;
    }

    return -1;
}

void CDssKeyUILogic::RefreshDssKeyUI(BOOL bForce/* = FALSE*/)
{
    if (m_bForbidden)
    {
        return;
    }

    // 刷新UI
    for (int i = 0; i < m_iMaxNumPerPage; ++i)
    {
        if (i == m_iMaxNumPerPage - 1
                && m_iPageTotal > 1)
        {
            continue;
        }

        RefreshDssKeyBtn(i, bForce);
    }
}

void CDssKeyUILogic::RefreshDssKeyBtn(int index, BOOL bForce/* = FALSE*/)
{
    if (bForce
            || m_vecCurrentShow[index] != NULL
            && m_vecCurrentShow[index]->bNeedRefresh)
    {
        // 刷新UI显示数据

        g_pDsskeyUIManager->NotifyByIndex(index, *m_vecCurrentShow[index]);
        g_pDssKeyLightLogic->UpdateLight(index,
                                         m_vecCurrentShow[index]->eLightColorType,
                                         m_vecCurrentShow[index]->eLightFlashType,
                                         m_vecCurrentShow[index]->iFlashuration);
        m_vecCurrentShow[index]->bNeedRefresh = FALSE;
    }
}

void CDssKeyUILogic::RefreshMultipleDsskeyData(int iBegin, int num, DSSKEY_MODULE_TYPE eModule)
{
    ListLogicData DsskeyDataList;
    DsskeyDisplayData DisplayData;
    dsskey_GetData(eModule, iBegin, num, DsskeyDataList);
    ListLogicData::const_iterator it = DsskeyDataList.begin();
    for (/* */; it != DsskeyDataList.end(); ++it)
    {
        //T48 更新数据不需要再判断锁定等操作
        bool ret = DP_ParseDsskeyPerformance(DisplayData, (DssKeyLogicData)(*it));
        if (ret == true)
        {
            if (DMT_LINEKEY_MODULE == eModule)
            {
                g_pDsskeyUIManager->OnNotify(DisplayData);
            }
        }
        else
        {
            DSSKEY_DEBUG("RefreshMultipleDsskeyData : DP_ParseDsskeyPerformance Failed.");
        }
    }
}

// 刷新界面数据
bool CDssKeyUILogic::UpdateLineKeyStatus(DssKeyLogicData & objData)
{
    // CDssKeyUILogic只管linekey
    int iLKIdx = dsskey_GetKeyIndex(objData.iDssKeyID);

    if (iLKIdx >= m_iMaxNumber || iLKIdx < 0)
    {
        // 超出linekey最大个数了或ID 索引非法
        return false;
    }

    // dsskey对应界面上的button
    int iButtonIndex = 0;
    Divide(iLKIdx, m_iLinenumPerPage, &iButtonIndex);

    if (!(iButtonIndex >= 0 && iButtonIndex < m_vecCurrentShow.size()))
    {
        DSSKEYUI_WARN("m_vecCurrentShow[ iButtonIndex(%d) ] is invalid.", iButtonIndex);
        return false;
    }

    if (m_vecCurrentShow[iButtonIndex] != NULL
            && m_vecCurrentShow[iButtonIndex]->bLockKeyed
            && m_vecCurrentShow[iButtonIndex]->iDsskeyID != objData.iDssKeyID)//
    {
        // 要求更新的dsskey的位置被锁定，且不是锁定的dsskey要求更新数据，则忽略
        DSSKEYUI_INFO("m_vecCurrentShow[%d] is bLockKeyed && ID Diff: Old(%d) New(%d).", \
                      iButtonIndex, m_vecCurrentShow[iButtonIndex]->iDsskeyID, objData.iDssKeyID);
        return false;
    }

    // 获取当前更新的锁定类型
    DssKeyUI_LockType eLockType = DSSKEYUI_LOCK_TYPE_DEFAULT;
    if (DT_LINE == objData.eDKType && (DS_LINE_NONE < objData.iStatus \
                                       && objData.iStatus < DS_LINE_END && DS_LINE_DISABLE != objData.iStatus))
    {
        int iLock = atoi(objData.strValue.c_str());
        eLockType = static_cast<DssKeyUI_LockType>(iLock);
    }

    if (DSSKEYUI_LOCK_TYPE_LOCK == eLockType)
    {
        // 锁定
        LockButton(iLKIdx);
    }
    else if (m_vecDsskeyInfos[iLKIdx].bLockKeyed) // 原来已经锁住
    {
        UnLockButton(iLKIdx);
    }

    DSSKEYUI_INFO("dsskeyID[%d]  eLockType:%d, %d, %d\n", objData.iDssKeyID, \
                  objData.eLockType, objData.iStatus, m_iCurrentPageIndex);

    // 暂时去除Dsskey 的Float功能
//  if (DSSKEYUI_LOCK_TYPE_FLOAT == eLockType)
//  {
//      // FLOAT
//      if (DS_LINE_USING == objData.iStatus)
//      {
//          YLList<int>::iterator itr = objData.listAuxiliaryState.begin();
//          YLList<int>::iterator itrEnd = objData.listAuxiliaryState.end();
//          Dsskey_Status eTalkStatu = DS_NON;
//          for (; itr != itrEnd; itr = itr->next)
//          {
//              if (itr->dataEntry > DS_SESSION_FIRST
//                  && itr->dataEntry < DS_SESSION_END)
//              {
//                  eTalkStatu = (Dsskey_Status)itr->dataEntry;
//              }
//          }
//
//          // 只有来电才进行float
//          if (eTalkStatu == DS_SESSION_CALL_IN)
//          {
//              FloatByDsskey(iLKIdx);
//          }
//      }
//      else
//      {
//          // 尝试执行对应dsskey的RollBack
//          RollBackToBeforeFloatPage(iLKIdx);
//      }
//  }

    if (!(iLKIdx >= 0 && iLKIdx < m_vecDsskeyInfos.size()))
    {
        DSSKEYUI_WARN("m_vecDsskeyInfos[ iLKIdx(%d) ] is invalid.", iLKIdx);
        return false;
    }
    // 更新Dsskey显示信息
    bool bRet = DP_ParseDsskeyPerformance(m_vecDsskeyInfos[iLKIdx], objData);
    if (!bRet)
    {
        DSSKEYUI_WARN("dsskeyUI_ParseLogicData Failed!");
        return false;
    }

    m_vecDsskeyInfos[iLKIdx].bNeedRefresh = TRUE;
    m_vecDsskeyInfos[iLKIdx].iTimestamp = ++m_iUpdateCount;
    UpdatePageInfo();
    RefreshPageKey();
    RefreshDssKeyUI();
    return true;
}

// 将第iIndex个button锁定为iDsskeyIndex
bool CDssKeyUILogic::LockButton(int iDsskeyID)
{
    if (iDsskeyID < 0
            || iDsskeyID >= m_iMaxNumber)
    {
        return false;
    }

    if (!(iDsskeyID >= 0 && iDsskeyID < m_vecDsskeyInfos.size()))
    {
        DSSKEYUI_WARN("m_vecDsskeyInfos[ iDsskeyID(%d) ] is invalid.", iDsskeyID);
        return false;
    }

    if (m_vecDsskeyInfos[iDsskeyID].bLockKeyed)
    {
        // 原来已经锁住
        return false;
    }

    int iButtonIndex;
    Divide(iDsskeyID, (m_iMaxNumPerPage - 1),
           &iButtonIndex); // page键对应的dsskey不应该被锁,网页上应该属于第二页
    for (int i = m_iMaxNumPerPage - 1 + iButtonIndex; i < m_iMaxNumber; i += m_iMaxNumPerPage - 1)
    {
        // 其他对应的dsskey信息被清空掉
        m_vecDsskeyInfos[i].clear();
    }

    if (!(iButtonIndex >= 0 && iButtonIndex < m_vecCurrentShow.size()))
    {
        DSSKEYUI_WARN("m_vecCurrentShow[ iButtonIndex(%d) ] is invalid.", iButtonIndex);
        return false;
    }

    m_vecCurrentShow[iButtonIndex] = &(m_vecDsskeyInfos[iDsskeyID]);
    m_vecCurrentShow[iButtonIndex]->bLockKeyed = TRUE;
    m_vecCurrentShow[iButtonIndex]->bNeedRefresh = TRUE;

    return true;
}

// 解锁指定的Dsskey对应的Button
bool CDssKeyUILogic::UnLockButton(int iDsskeyID)
{
    if (iDsskeyID < 0
            || iDsskeyID >= m_iMaxNumPerPage - 1)
    {
        // 解锁只能通过第一页来实现
        return false;
    }

    if (NULL == m_vecCurrentShow[iDsskeyID])
    {
        return false;
    }

    if (!m_vecCurrentShow[iDsskeyID]->bLockKeyed)
    {
        // 本来就没锁住
        return false;
    }

    if (!(iDsskeyID >= 0 && iDsskeyID < m_vecCurrentShow.size()))
    {
        DSSKEYUI_WARN("m_vecCurrentShow[ iDsskeyID(%d) ] is invalid.", iDsskeyID);
        return false;
    }

    // 重新把当前显示的设置成对应位置的dsskey
    m_vecCurrentShow[iDsskeyID]->bLockKeyed = FALSE; // 先解锁
    m_vecCurrentShow[iDsskeyID] = &(m_vecDsskeyInfos[m_iCurrentPageIndex * m_iLinenumPerPage +
                                    iDsskeyID]);
    m_vecCurrentShow[iDsskeyID]->bNeedRefresh = TRUE;

    return true;
}

//float到指定的页面
bool CDssKeyUILogic::FloatByDsskey(int iDsskeyID)
{
    // 计算Dsskey所在页
    int iPage = Divide(iDsskeyID, m_iLinenumPerPage);
    if (iPage < 0)
    {
        return false;
    }

    YLList<FLOAT_INFO>::iterator itr = m_listFloatInfo.begin();
    YLList<FLOAT_INFO>::iterator itrEnd = m_listFloatInfo.end();
    for (; itr != itrEnd; itr++)
    {
        // 先删除已经存在的
        if (iDsskeyID == (*itr).iDsskeyID)
        {
            YLList<FLOAT_INFO>::iterator itrNext = itr;
            itrNext++;
            if (itrNext != itrEnd)
            {
                // 下一个节点继承被删除的返回信息
                (*itrNext).iPage = (*itr).iPage;
            }

            m_listFloatInfo.erase(itr);
            break;
        }
    }

    FLOAT_INFO tFloatInfo;
    tFloatInfo.iDsskeyID = iDsskeyID;
    tFloatInfo.iPage = m_iCurrentPageIndex;
    m_listFloatInfo.push_back(tFloatInfo);

    GoToPage(iPage);
    return true;
}

//回退到float前的页面
bool CDssKeyUILogic::RollBackToBeforeFloatPage(int iDsskeyID)
{
    int iTargetPage = -1;
    bool bNeedTurnPage = true;

    // 多路处于float的状态下,对于不是最后一个的float只是简单删除
    YLList<FLOAT_INFO>::iterator itr = m_listFloatInfo.begin();
    YLList<FLOAT_INFO>::iterator itrEnd = m_listFloatInfo.end();
    for (; itr != itrEnd; itr++)
    {
        if (iDsskeyID == (*itr).iDsskeyID)
        {
            iTargetPage = (*itr).iPage;
            YLList<FLOAT_INFO>::iterator itrNext = itr;
            itrNext++;
            if (itrNext != itrEnd)
            {
                // 中间的float发生的RollBack
                bNeedTurnPage = false;
                // 继承被删除的返回信息
                (*itrNext).iPage = (*itr).iPage;
            }

            m_listFloatInfo.erase(itr);
            break;
        }
    }


    if (bNeedTurnPage
            && iTargetPage >= 0)
    {
        GoToPage(iTargetPage);
        return true;
    }

    return false;
}

void CDssKeyUILogic::GoToPage(int iPage)
{
    if (iPage < 0
            || iPage >= m_iPageTotal
            || m_iCurrentPageIndex == iPage)
    {
        DSSKEYUI_INFO("CDssKeyUILogic::GoToPage(%d) Failed.", iPage);
        return;
    }

    for (int i = 0; i < m_iLinenumPerPage; ++i)
    {
        if (m_vecCurrentShow[i] != NULL
                && m_vecCurrentShow[i]->bLockKeyed)
        {
            // 如果锁定了，则翻页时不用更改当前显示的dsskey信息
        }
        else
        {
            m_vecCurrentShow[i] = &(m_vecDsskeyInfos[i + iPage * m_iLinenumPerPage]);
            m_vecCurrentShow[i]->bNeedRefresh = TRUE;
        }
    }

    m_iCurrentPageIndex = iPage;
    // 更新当前要显示的页面个数
    UpdatePageNum();
    RefreshPageKey();
}

void CDssKeyUILogic::UpdatePageNum()
{
    if (m_iPageTotal <= m_iMaxPage)
    {
        m_iCurPageNum = m_iPageTotal;
    }
    else
    {
        // 需要重新计算当前显示页码个数
        int nTotalPageofPage = Divide((m_iPageTotal + m_iMaxPage - 1), m_iMaxPage);
        int nCurPageofPage = Divide((m_iCurrentPageIndex + m_iMaxPage), m_iMaxPage);
        Divide(m_iPageTotal, m_iMaxPage, &m_iCurPageNum);
        if (nTotalPageofPage == nCurPageofPage)
        {
            // 最后一页按实际个数显示页码
            Divide(m_iPageTotal, m_iMaxPage, &m_iCurPageNum);
        }
        else
        {
            m_iCurPageNum = m_iMaxPage;
        }
    }
}

// 计算当前每页包含的linekey数量,页数
void CDssKeyUILogic::UpdatePageInfo()
{
    int iMaxValid = -1;
    BOOL bHaveLockKey = FALSE;

    for (int i =  0; i < m_iMaxNumber; ++i)
    {
        if (m_vecDsskeyInfos[i].bLockKeyed)
        {
            bHaveLockKey = TRUE;
            break;
        }
#ifdef _T4X
        if (m_vecDsskeyInfos[i].eDssKeytype != DT_NA)
        {
            if (DT_LINE == m_vecDsskeyInfos[i].eDssKeytype && \
                    (m_vecDsskeyInfos[i].iStatus == DS_LINE_DISABLE || \
                     m_vecDsskeyInfos[i].iStatus <= DS_LINE_NONE || \
                     m_vecDsskeyInfos[i].iStatus >= DS_LINE_END))
            {
                continue;
            }
            iMaxValid = i;
        }
#else
        if (m_vecDsskeyInfos[i].eDssKeytype != DT_NA)
        {
            iMaxValid = i;
        }
#endif
    }

    int iOldNum = m_iPageTotal;
    if (bHaveLockKey)
    {
        // 如果有按键被锁住，则会变成3页
        m_iLinenumPerPage = m_iMaxNumPerPage - 1;
        m_iPageTotal = Divide(m_iMaxNumber, m_iLinenumPerPage);
    }
    else if (iMaxValid + 1 > m_iMaxNumPerPage)
    {
        m_iLinenumPerPage = m_iMaxNumPerPage - 1;
        // 大于每页包含的linkey数
        m_iPageTotal = Divide(iMaxValid, m_iLinenumPerPage) + 1;
    }
    else
    {
        // 当小于或者等于一页的最大可显示Dsskey个数时,只有一页
        m_iPageTotal = 1;
        m_iLinenumPerPage = m_iMaxNumPerPage;
    }

    if (m_iCurrentPageIndex + 1 >= m_iPageTotal)
    {
        GoToPage(m_iPageTotal - 1);
    }

    UpdatePageNum();

    if (iOldNum != m_iPageTotal
            && m_vecCurrentShow[m_iMaxNumPerPage - 1] != NULL)
    {
        // 页数变变化,刷新翻页键
        m_vecCurrentShow[m_iMaxNumPerPage - 1]->bNeedRefresh = TRUE;
    }
}

// 更新page状态
void CDssKeyUILogic::UpdatePageStatus(DsskeyDisplayData & objDisplayData)
{
    int iPage = Divide(objDisplayData.iDsskeyID, m_iLinenumPerPage);
    if (iPage == m_iCurrentPageIndex)
    {
        // 当前页忽略
        return;
    }

    if (iPage < 0
            || iPage >= m_iPageTotal)
    {
        return;
    }

    if (objDisplayData.iLightPriority <= m_vecPageButtonInfo[iPage].iLightPriority)
    {
        MapToPageStatus(m_vecPageButtonInfo[iPage], objDisplayData);
    }
}

// 将dsskey状态映射成page状态
void CDssKeyUILogic::MapToPageStatus(DsskeyDisplayData & outPageData,
                                     const DsskeyDisplayData & objDisplayData)
{
    outPageData = objDisplayData;
}

int MapFlashTime(DSS_KEY_FLASH_TYPE eFlashType)
{
    switch (eFlashType)
    {
    case REFRESH_TYPE_NO_FLASH:
        return 0;
    case REFRESH_TYPE_SLOW_FLASH:
        return 1000;
    case REFRESH_TYPE_FLASH:
        return 500;
    case REFRESH_TYPE_FAST_FLASH:
        return 300;
    case REFRESH_TYPE_FASTER_FLASH:
        return 200;
    }

    return 0;
}

void CDssKeyUILogic::RefreshPageKey()
{
    // 更新页数到UI上

    // T48没有多页Dsskey翻页键,无需更新
//  g_pDsskeySubject->OnNotifyPageInfo(m_iCurPageNum, m_iCurrentPageIndex, IsPageButtonEnable());
//
//  DSSKEYUI_INFO("m_iPageTotal(%d), m_iCurPageNum(%d), m_iCurrentPageIndex(%d).", m_iPageTotal, m_iCurPageNum, m_iCurrentPageIndex);
//  if (m_vecCurrentShow[m_iMaxNumPerPage - 1]->bNeedRefresh == TRUE)
//  {
//      // 有变化，先关灯，后根据当前Dsskey状态更新灯状态
//      // 设置Page键对应的灯状态
//      g_pDssKeyLightLogic->UpdateLight(m_iPageIndex, LED_COLOR_OFF, REFRESH_TYPE_NO_FLASH);
//  }
//  // PageButton可用时
//  if (IsPageButtonEnable()
//      && m_bEnablePageTips)
//  {
//      int iCurrentPage = 0;
//      int iHighestOfAllKey = -1;
//      for (int iPageIndex = 0; iPageIndex < m_iPageTotal; ++iPageIndex)
//      {
//          if (iPageIndex == m_iCurrentPageIndex)
//          {
//              // 当前页不处理
//              continue;
//          }
//
//          int iHighest = -1;
//          for (int iKeyIndex = 0; iKeyIndex < m_iLinenumPerPage; ++iKeyIndex)
//          {
//              int iCurrentKeyIndex = iKeyIndex + iPageIndex*m_iLinenumPerPage;
//              if (&m_vecDsskeyInfos[iCurrentKeyIndex] == m_vecCurrentShow[iKeyIndex])
//              {
//                  // 如果dsskey因为锁等定的原因正处于当前显示状态，也不计算在内
//                  continue;
//              }
//
//              if (m_vecDsskeyInfos[iCurrentKeyIndex].ePageColorType != LED_COLOR_OFF)
//              {
//                  if (iHighest == -1
//                      || m_vecDsskeyInfos[iCurrentKeyIndex].iLightPriority < m_vecDsskeyInfos[iHighest].iLightPriority
//                      || (m_vecDsskeyInfos[iCurrentKeyIndex].iLightPriority == m_vecDsskeyInfos[iHighest].iLightPriority
//                      && m_vecDsskeyInfos[iCurrentKeyIndex].iTimestamp > m_vecDsskeyInfos[iHighest].iTimestamp))// 相同优先级后来优先
//                  {
//                      // 找出每一页的最高优先级的dsskey index
//                      iHighest = iCurrentKeyIndex;
//                  }
//              }
//          }
//
//          if (iHighest >= 0)
//          {
//
//              m_vecPageButtonInfo[iPageIndex] = m_vecDsskeyInfos[iHighest];
//          }
//          else
//          {
//              m_vecPageButtonInfo[iPageIndex].clear();
//          }
//      }
//
//          int iHighest = -1;
//      // 清空当前图标状态
//      g_pDsskeySubject->OnNotifyPageIcon(-1, LED_COLOR_OFF);
//      for (int i = 0; i < m_iPageTotal; ++i)
//      {
//          if (i != m_iCurrentPageIndex)
//          {
//              if (1 == m_vecPageButtonInfo[i].iPageIconState)
//              {
//                  g_pDsskeySubject->OnNotifyPageIcon(i, LED_COLOR_OFF);
//              }
//              else
//              {
//                  g_pDsskeySubject->OnNotifyPageIcon(i + 1, m_vecPageButtonInfo[i].ePageColorType, \
//                                  m_vecPageButtonInfo[i].ePageLightFlashType);
//              }
//
//              if (iHighest == -1
//                  || m_vecPageButtonInfo[i].iLightPriority < m_vecPageButtonInfo[iHighest].iLightPriority
//                  || (m_vecPageButtonInfo[i].iLightPriority == m_vecPageButtonInfo[iHighest].iLightPriority
//                  && m_vecPageButtonInfo[i].iTimestamp > m_vecPageButtonInfo[iHighest].iTimestamp))// 优先级相同后来的优先
//              {
//                  iHighest = i;
//              }
//          }
//      }
//
//      if (iHighest >= 0)
//      {
//          g_pDssKeyLightLogic->UpdateLight(m_iPageIndex, \
//              m_vecPageButtonInfo[iHighest].ePageColorType, m_vecPageButtonInfo[iHighest].ePageLightFlashType);
//      }
//      else
//      {
//          // 设置Page键对应的灯状态
//          g_pDssKeyLightLogic->UpdateLight(m_iPageIndex, LED_COLOR_OFF, REFRESH_TYPE_NO_FLASH);
//      }
//
//      //dsskeyUI_RefreshpageButton();
//  }
}

BOOL CDssKeyUILogic::OnPageButtonPress()
{
    if (IsPageButtonEnable())
    {
        // 点的是翻页键
        int iCurrentPage = m_iCurrentPageIndex;
        iCurrentPage++;
        if (iCurrentPage == m_iPageTotal)
        {
            iCurrentPage = 0;
        }

        GoToPage(iCurrentPage);
        RefreshDssKeyUI();
        return TRUE;
    }

    return FALSE;
}

int CDssKeyUILogic::GetDsskeyID(int nBtnIndex)
{
    if (m_iPageTotal == 1
            || nBtnIndex < 0
            || nBtnIndex >= m_iMaxNumPerPage)
    {
        return nBtnIndex;
    }
    else
    {
        if (m_vecCurrentShow[nBtnIndex] != NULL)
        {
            return m_vecCurrentShow[nBtnIndex]->iDsskeyID;
        }
    }

    return -1;
}

void CDssKeyUILogic::SwitchPageTip(BOOL bEnable)
{
    m_bEnablePageTips = bEnable;

    if (m_bEnablePageTips)
    {
        RefreshPageKey();
    }
    else
    {
        // 设置Page键对应的灯状态
        g_pDssKeyLightLogic->UpdateLight(m_iPageIndex, LED_COLOR_OFF, REFRESH_TYPE_NO_FLASH);
        // 清空当前图标状态
        // T48没有多页Dsskey翻页键,无需更新
        //g_pDsskeySubject->OnNotifyPageIcon(-1, LED_COLOR_OFF);
    }
}

void CDssKeyUILogic::Forbid(BOOL bForbidden)
{
    m_bForbidden = bForbidden;

    if (m_bForbidden)
    {
        SwitchPageTip(FALSE);
    }
    else
    {
        BOOL bPageEnable = configParser_GetConfigInt(kszEnablePageTips) != 0;
        SwitchPageTip(bPageEnable);
        RefreshDssKeyUI(TRUE);
    }
}


void CDssKeyUILogic::UpdateDsskeyLight(int iIndex, DSS_KEY_COLOR_TYPE   eColorType,
                                       DSS_KEY_FLASH_TYPE eFlashType)
{
    if (iIndex < 0
            || iIndex >= m_iMaxNumber)
    {
        return;
    }

    m_vecDsskeyInfos[iIndex].eLightColorType = eColorType;
    m_vecDsskeyInfos[iIndex].eLightFlashType = eFlashType;
    if (LED_COLOR_OFF == eColorType)
    {
        m_vecDsskeyInfos[iIndex].iFlashuration = 0;
    }
    // T46亮灯逻辑需重新整理
    if (m_iCurPageNum > 1)
    {
        if (iIndex >= m_iCurrentPageIndex * m_iLinenumPerPage
                && iIndex < (m_iCurrentPageIndex + 1) * m_iLinenumPerPage)
        {
            //存在翻页键的时候，点第6或第11个linekey要点下一页的第一个linekey
            {
                // 多页的点灯索引值需重新计算
                iIndex = iIndex - m_iCurrentPageIndex * m_iLinenumPerPage;
            }
            g_pDssKeyLightLogic->TurnOffLight(iIndex);
            g_pDssKeyLightLogic->UpdateLight(iIndex, eColorType, eFlashType);
        }
    }
    else
    {
        g_pDssKeyLightLogic->TurnOffLight(iIndex);
        g_pDssKeyLightLogic->UpdateLight(iIndex, eColorType, eFlashType);
    }
}
