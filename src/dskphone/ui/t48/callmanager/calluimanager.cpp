#include "calluimanager.h"
#include "uimanager/moduimanager.h"

#if IF_BUG_28455
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "configiddefine.h"
#include "modconfigparser.h"
#endif

void CallUIManager_NormalShowPage(const yl::string &
                                  strUIName)     //默认调用的进入子界面时回调函数   //当设置的回调函数指针为空时调用
{
    //不存在该元素相关信息 那么直接返回
    if (!_CallUIManager.IsExistItem(strUIName))
    {
        return ;
    }

    //先尝试获取 对话框子界面
    CCallManagerBaseDlg * pDlg = _CallUIManager.GetPageDlgByName(strUIName);

    //如果获取到的为空 那么直接从uimanager 获取， 并设置
    if (NULL == pDlg)
    {
        pDlg = UIManager_CreatePrivateDlg(strUIName);

        if (NULL == pDlg)
        {
            return ;
        }

        //设置窗口指针
        _CallUIManager.SetPageDlgByName(strUIName, pDlg);
    }

    //显示该界面
    _CallUIManager.ModelEnterPage(strUIName);

}

void CallUIManager_NormalHidePage(const yl::string &
                                  strUIName)         //默认调用的 退出子界面时回调函数    //当设置的回调函数指针未空时调用
{
    //不存在该元素相关信息 那么直接返回
    if (!_CallUIManager.IsExistItem(strUIName))
    {
        return ;
    }

    //先尝试获取 对话框子界面
    CCallManagerBaseDlg * pDlg = _CallUIManager.GetPageDlgByName(strUIName);

    //释放该子窗口指针
    _CallUIManager.ModelPutPage(strUIName);

    ////如果获取到的子界面为空 直接返回
    //if(NULL == pDlg)
    //{
    //  return ;
    //}

    ////释放子窗口指针
    //UIManager_PutPrivateDlg(pDlg);
}

void CallUIManager_NormalUnboundCallBack(const yl::string & strUIName,
        CCallManagerBaseDlg * pDlg)    //解除绑定时的回调函数
{
    //释放窗口
    if (NULL != pDlg)
    {
        UIManager_PutPrivateDlg(pDlg);
    }
}

CCallUIManager::CCallUIManager(void)
{
    m_pDlgCallManager = NULL;
    m_pCurrentCallMGRInfo = NULL;
}

CCallUIManager::~CCallUIManager(void)
{
    CCallMGRInfo * pCallMGRInfo = NULL;
    YLList<CCallMGRInfo *>::iterator ite = m_listCallMGRInfo.begin();

    for (; m_listCallMGRInfo.end() != ite; ite++)
    {
        pCallMGRInfo = *ite;

        if (NULL != pCallMGRInfo)
        {
            delete pCallMGRInfo;

            pCallMGRInfo = NULL;
        }
    }

    m_pCurrentCallMGRInfo = NULL;
    m_pDlgCallManager = NULL;
}

//获取CallUIManager 实例
CCallUIManager & CCallUIManager::GetInstance()
{
    static CCallUIManager s_cCallUIManager;

    return s_cCallUIManager;
}

//设置子界面相关信息
void CCallUIManager::SetPageInfo(const yl::string & strUIName,
                                 int iUIFlag,                       //窗口属性
                                 CCallMGRItemInfo callMGRItemInfo,  //Item 相关信息
                                 PPageEventCallBack pPageEventCallBack,     //其他事件的回调函数 如show  、  hide
                                 PShowPage pShowPage,           //需要显示该界面时调用的函数
                                 PHidePage pHidePage,   //隐藏该界面时调用的函数
                                 PUnboundCallBack pUnboundCallBack, //解除绑定时的回调函数
                                 int iItemIndex         //显示的顺序
                                )
{
    //如果已经存在这个元素了 那么直接返回
    if (IsExistItem(strUIName))
    {
        return ;
    }

    //没有获取到  要重新生成 插入队列
    CCallMGRInfo * pCallMGRInfo = new CCallMGRInfo();

    //获取要插入的 迭代器
    YLList<CCallMGRInfo *>::iterator ite = GetItemInsertItorByIndex(iItemIndex);
    if (m_listCallMGRInfo.end() == ite)
    {
        m_listCallMGRInfo.push_back(
            pCallMGRInfo);      //如果是没找到位置 那么直接插入到最后一个
    }
    else
    {
        m_listCallMGRInfo.insert(ite, pCallMGRInfo);    //插入到位置中
    }


    //设置相关信息
    pCallMGRInfo->strUIName = strUIName;                        //UI名称
    pCallMGRInfo->iUIFlag = pCallMGRInfo->iUIFlag | iUIFlag;    //属性要用并集

    //记录显示的顺序
    pCallMGRInfo->iItemIndex = iItemIndex;

    //Item相关信息
    pCallMGRInfo->callMGRItemInfo = callMGRItemInfo;        //Item相关信息

    pCallMGRInfo->pPageEventCallBack = pPageEventCallBack;      //保存一下普通回调函数指针

    //设置 进入界面回调函数  如果没有那么设置默认的函数
    if (NULL == pShowPage)
    {
        pCallMGRInfo->pShowPage = CallUIManager_NormalShowPage;     //使用默认的函数
    }
    else
    {
        pCallMGRInfo->pShowPage = pShowPage;        //进入该子界面回调
    }

    //设置退出界面回调函数  如果没有那么设置默认的函数
    if (NULL == pHidePage)
    {
        pCallMGRInfo->pHidePage = CallUIManager_NormalHidePage;
    }
    else
    {
        pCallMGRInfo->pHidePage = pHidePage;            //退出该子界面回调
    }

    //设置解除绑定的回调函数指针 如果没有那么设置默认的函数
    if (NULL == pUnboundCallBack)
    {
        pCallMGRInfo->pUnboundCallBack = CallUIManager_NormalUnboundCallBack;
    }
    else
    {
        pCallMGRInfo->pUnboundCallBack = pUnboundCallBack;
    }
}

//是否存在 该元素
bool CCallUIManager::IsExistItem(const yl::string & strUIName)
{
    CCallMGRInfo * pCallMGRInfo = NULL;
    YLList<CCallMGRInfo *>::iterator ite = m_listCallMGRInfo.begin();

    for (; m_listCallMGRInfo.end() != ite; ite++)
    {
        pCallMGRInfo = *ite;

        if (NULL != pCallMGRInfo)
        {
            //找到有名称相同的 返回true
            if (strUIName == pCallMGRInfo->strUIName)
            {
                return true;
            }
        }

        pCallMGRInfo = NULL;
    }

    //没有找到 返回false
    return false;
}

//根据ItemIndex 序号获取 插入的迭代器
YLList<CCallMGRInfo *>::iterator CCallUIManager::GetItemInsertItorByIndex(int iItemIndex)
{
    YLList<CCallMGRInfo *>::iterator ite = m_listCallMGRInfo.begin();
    CCallMGRInfo * pCallMGRInfo = NULL;

    for (; m_listCallMGRInfo.end() != ite; ite++)
    {
        pCallMGRInfo = *ite;

        if (iItemIndex < pCallMGRInfo->iItemIndex)
        {
            break;
        }
    }

    return ite;
}

//设置界面窗口
void CCallUIManager::SetPageDlgByName(const yl::string & strUIName, CCallManagerBaseDlg * pDlg)
{
    CCallMGRInfo * pCallMGRInfo = NULL;
    YLList<CCallMGRInfo *>::iterator ite = m_listCallMGRInfo.begin();

    for (; m_listCallMGRInfo.end() != ite; ite++)
    {
        pCallMGRInfo = *ite;

        if (NULL != pCallMGRInfo)
        {
            if (strUIName == pCallMGRInfo->strUIName)
            {
                break;
            }
        }

        pCallMGRInfo = NULL;
    }

    //没有找到 直接返回了
    if (NULL == pCallMGRInfo)
    {
        return ;
    }

    //如果两个对话框指针是相同的 那么直接返回
    if (pCallMGRInfo->pDlg == pDlg)
    {
        return ;
    }

    //需要退出界面的显示
    //不管对话框是否为空 都需要隐藏对话框显示
    //隐藏界面就可以了
//  DlgHidePage(pCallMGRInfo);
    if (NULL != m_pDlgCallManager)
    {
        m_pDlgCallManager->SetPageDlgNULL(pCallMGRInfo);
    }

    //对话框不为空 才需要解除绑定
    if (NULL != pCallMGRInfo->pDlg)
    {
        CCallManagerBaseDlg * pTempDlg = pCallMGRInfo->pDlg;

        //这边先赋值为空
        pCallMGRInfo->pDlg = NULL;

        //调用解除绑定的 回调函数
        if (NULL != pCallMGRInfo->pUnboundCallBack)
        {
            pCallMGRInfo->pUnboundCallBack(pCallMGRInfo->strUIName, pTempDlg);
        }
    }

    //先退出对话框的显示  然后才把新的对话框赋值
    pCallMGRInfo->pDlg = pDlg;

}

//获取界面窗口
CCallManagerBaseDlg * CCallUIManager::GetPageDlgByName(const yl::string & strUIName)
{
    CCallMGRInfo * pCallMGRInfo = NULL;
    YLList<CCallMGRInfo *>::iterator ite = m_listCallMGRInfo.begin();

    for (; m_listCallMGRInfo.end() != ite; ite++)
    {
        pCallMGRInfo = *ite;

        if (NULL != pCallMGRInfo)
        {
            if (strUIName == pCallMGRInfo->strUIName)
            {
                return pCallMGRInfo->pDlg;
            }
        }

        pCallMGRInfo = NULL;
    }

    return NULL;
}

//获取当前界面窗口
CCallManagerBaseDlg * CCallUIManager::GetCurrentDlg()
{
    if (NULL != m_pCurrentCallMGRInfo)
    {
        return m_pCurrentCallMGRInfo->pDlg;
    }

    return NULL;
}

//获取当前页的名称
yl::string CCallUIManager::GetCurrentPageName()
{
    if (NULL != m_pCurrentCallMGRInfo)
    {
        return m_pCurrentCallMGRInfo->strUIName;
    }

    return "";
}

//由外部模块调用的 进入某个子界面
void CCallUIManager::ModelEnterPage(const yl::string & strUIName)
{
    CCallMGRInfo * pCallMGRInfo = NULL;
    YLList<CCallMGRInfo *>::iterator ite = m_listCallMGRInfo.begin();

    for (; m_listCallMGRInfo.end() != ite; ite++)
    {
        pCallMGRInfo = *ite;

        if (NULL != pCallMGRInfo)
        {
            if (strUIName == pCallMGRInfo->strUIName)
            {
                break;
            }
        }

        pCallMGRInfo = NULL;
    }

    //没有找到对应的窗口信息  直接返回
    if (NULL == pCallMGRInfo)
    {
        return ;
    }

    //这边标注是已经 有使用了
    pCallMGRInfo->bIsUsed = true;

    //如果子对话框是为空 那么直接返回 显示不了
    if (NULL == pCallMGRInfo->pDlg)
    {
        return ;
    }

    //如果是窗口信息相同
    if (pCallMGRInfo == m_pCurrentCallMGRInfo)
    {
        //如果CallManager 对话框的指针为空 那么才需要获取
        if (NULL == m_pDlgCallManager)
        {
            CDlgCallManagerBase * pDlgCallManager = GetDlgCallManager();
            //获取到的为空 直接就返回
            if (NULL == pDlgCallManager)
            {
                return ;
            }

            //保存对话框指针
            m_pDlgCallManager = pDlgCallManager;

            //设置ItemBar相关信息
            m_pDlgCallManager->SetCallMGRListInfo(m_listCallMGRInfo);
        }

        //显示对话框
        DlgShowPage(pCallMGRInfo);

        //不是顶层窗口 才需要设置成顶层窗口   因为设置为顶层窗口 会自动刷新一次  这边避免重复刷新
        //比如打开了一个拨号界面 ， 这个时候又打开dsskey 界面， 在dsskey界面上又打开了 dir 界面 ， 那么这个时候是需要置顶的
        if (UIManager_GetTopWindow() != m_pDlgCallManager)
        {
            UIManager_SetTopWindow(m_pDlgCallManager);
        }
        else
        {
            //是顶层窗口 只要简单刷新一下窗口就可以了
            //刷新一下顶层窗口
            UIManager_UpdateWnd(m_pDlgCallManager);
        }

        return ;
    }

    //获取当前信息
    CCallMGRInfo * pLastCallMGRInfo = m_pCurrentCallMGRInfo;

    if (NULL != m_pCurrentCallMGRInfo)
    {
        //退出当前的焦点窗口
        //  ManagerHidePage(m_pCurrentCallMGRInfo);

        //隐藏当前窗口
        DlgHidePage(m_pCurrentCallMGRInfo);

        //当前焦点窗口赋值为空  这边一定要先赋值为空 避免下面的退出操作 把整个界面退出
        m_pCurrentCallMGRInfo = NULL;
    }

    //如果CallManager 对话框的指针为空 那么才需要获取
    if (NULL == m_pDlgCallManager)
    {
        CDlgCallManagerBase * pDlgCallManager = GetDlgCallManager();
        //获取到的为空 直接就返回
        if (NULL == pDlgCallManager)
        {
            return ;
        }

        //保存对话框指针
        m_pDlgCallManager = pDlgCallManager;

        //设置ItemBar相关信息
        m_pDlgCallManager->SetCallMGRListInfo(m_listCallMGRInfo);
    }

    //赋值当前的焦点窗口
    m_pCurrentCallMGRInfo = pCallMGRInfo;

    //显示当前的窗口
//  ManagerShowPage(m_pCurrentCallMGRInfo);
    //显示当前窗口
    DlgShowPage(m_pCurrentCallMGRInfo);

    //获取当前信息
    CCallMGRInfo * pCurrentCallMGRInfo = m_pCurrentCallMGRInfo;

    //调用上一个窗口隐藏 回调函数
    if (NULL != pLastCallMGRInfo)
    {
        if (NULL != pLastCallMGRInfo->pPageEventCallBack)
        {
            pLastCallMGRInfo->pPageEventCallBack(pLastCallMGRInfo->strUIName,
                                                 CallManagerEvent_Hide);       //调用隐藏的回调
        }
    }

    //调用当前窗口 显示回调函数
    if (NULL != pCurrentCallMGRInfo)
    {
        if (NULL != pCurrentCallMGRInfo->pPageEventCallBack)
        {
            pCurrentCallMGRInfo->pPageEventCallBack(pCurrentCallMGRInfo->strUIName, CallManagerEvent_Show);
        }
    }


    //不是顶层窗口 才需要设置成顶层窗口   因为设置为顶层窗口 会自动刷新一次  这边避免重复刷新
    if (UIManager_GetTopWindow() != m_pDlgCallManager)
    {
        UIManager_SetTopWindow(m_pDlgCallManager);
    }
    else
    {
        //是顶层窗口 只要简单刷新一下窗口就可以了
        //刷新一下顶层窗口
        UIManager_UpdateWnd(m_pDlgCallManager);
    }

    ////在这做退出操作
    //if(NULL != pLastCallMGRInfo)
    //{
    //  //如果属性是需要在隐藏时退出的 那么要调用退出的接口
    //  if(UIFlagHidePutPag == pLastCallMGRInfo->iUIFlag & UIFlagHidePutPag)
    //  {
    //      if(NULL != pLastCallMGRInfo->pPutPage)
    //      {
    //          pLastCallMGRInfo->pPutPage(pLastCallMGRInfo->strUIName);
    //      }

    //      //防止重复调用
    //      pLastCallMGRInfo->pDlg = NULL;
    //  }
    //}

    return ;
}

//由外部模块调用的 退出某个子界面
void CCallUIManager::ModelPutPage(const yl::string & strUIName)
{
    CCallMGRInfo * pCallMGRInfo = NULL;
    YLList<CCallMGRInfo *>::iterator ite = m_listCallMGRInfo.begin();

    for (; m_listCallMGRInfo.end() != ite; ite++)
    {
        pCallMGRInfo = *ite;

        if (NULL != pCallMGRInfo)
        {
            if (strUIName == pCallMGRInfo->strUIName)
            {
                break;
            }
        }

        pCallMGRInfo = NULL;
    }

    //没有找到对应的窗口信息  直接返回
    if (NULL == pCallMGRInfo)
    {
        CheckReleaseDlgCallManager();

        return ;
    }

    //这边退出  那么设置成没有使用
    pCallMGRInfo->bIsUsed = false;

    //这边后续的退出界面如果是当前窗口的 那么是一定要把dlgcallmanager 释放的

    //如果不是焦点窗口 那么 直接进行退出流程就可以了
    if (pCallMGRInfo != m_pCurrentCallMGRInfo)
    {
        //  ManagerPutPage(pCallMGRInfo);
        //非焦点窗口  不需要隐藏窗口
        //DlgHidePage(pCallMGRInfo);

        //如果对话框不为空
        if (NULL != pCallMGRInfo->pDlg)
        {
            CCallManagerBaseDlg * pNofocusDlg = pCallMGRInfo->pDlg;

            pCallMGRInfo->pDlg = NULL;

            //需要解除绑定窗口
            if (NULL != pCallMGRInfo->pUnboundCallBack)
            {
                pCallMGRInfo->pUnboundCallBack(pCallMGRInfo->strUIName, pNofocusDlg);
            }
        }
    }
    else    //焦点窗口退出
    {
        //循环退出非焦点窗口
        for (;;)
        {
            CCallMGRInfo * pNoFocusCallMGRInfo = GetFirstNoFocusItem();

            if (NULL == pNoFocusCallMGRInfo)
            {
                break;
            }

            //如果是有使用 那么要调用退出的函数
            if (true == pNoFocusCallMGRInfo->bIsUsed)
            {
                //这边设置成 false 免得重复调用
                pNoFocusCallMGRInfo->bIsUsed = false;

                //退出窗口 直接调用退出窗口的指针
                if (NULL != pNoFocusCallMGRInfo->pHidePage)
                {
                    pNoFocusCallMGRInfo->pHidePage(pNoFocusCallMGRInfo->strUIName);
                }
            }
            else if (NULL != pNoFocusCallMGRInfo->pDlg)
            {
                CCallManagerBaseDlg * pNoFocusCallMGRInfoDlg = pNoFocusCallMGRInfo->pDlg;

                //这边把对话框设置成false 以免重复调用
                pNoFocusCallMGRInfo->pDlg = NULL;

                //需要解除绑定窗口
                if (NULL != pNoFocusCallMGRInfo->pUnboundCallBack)
                {
                    pNoFocusCallMGRInfo->pUnboundCallBack(pNoFocusCallMGRInfo->strUIName, pNoFocusCallMGRInfoDlg);
                }
            }
        }

        //最后退出焦点窗口
        //也是退出当前窗口了  因为当前窗口 已经调用了一次退出函数了  所以这边简单的退出就可以了
        if (NULL != m_pCurrentCallMGRInfo)
        {
            CCallMGRInfo * pFocusCallMGRInfo = m_pCurrentCallMGRInfo;       //保存焦点窗口信息值

            //首先需要隐藏一下当前焦点窗口
            //DlgHidePage(pFocusCallMGRInfo);
            if (NULL != m_pDlgCallManager)
            {
                m_pDlgCallManager->SetPageDlgNULL(pFocusCallMGRInfo);
            }

            //把当前焦点信息设置为空
            m_pCurrentCallMGRInfo = NULL;

            //这边 不调用 隐藏的回调了 因为是外部发起的  退出该界面显示， 所以不用， Hide 事件用于切换
            ////先要调用 焦点窗口 隐藏的回调函数
            //if(NULL != pFocusCallMGRInfo->pPageEventCallBack)
            //{
            //  pFocusCallMGRInfo->pPageEventCallBack(pFocusCallMGRInfo->strUIName, CallManagerEvent_Hide);
            //}

            //如果对话框不为空  需要设置对话框为空  并且要调用解除绑定的回调函数
            if (NULL != pFocusCallMGRInfo->pDlg)
            {
                CCallManagerBaseDlg * pFocusDlg = pFocusCallMGRInfo->pDlg;

                pFocusCallMGRInfo->pDlg = NULL;

                //需要解除绑定窗口
                if (NULL != pFocusCallMGRInfo->pUnboundCallBack)
                {
                    pFocusCallMGRInfo->pUnboundCallBack(pFocusCallMGRInfo->strUIName, pFocusDlg);
                }
            }
        }
    }

    //查看是否要退出
    CheckReleaseDlgCallManager();

}

////内部调用的 显示
//void CCallUIManager::ManagerShowPage(CCallMGRInfo* pCallMGRInfo)
//{
//  //指针为空 返回
//  if(NULL == pCallMGRInfo)
//  {
//      return ;
//  }
//
//  //对话框指针为空 返回
//  if(NULL == pCallMGRInfo->pDlg)
//  {
//      return ;
//  }
//
//  //单纯显示子对话框
//  DlgShowPage(pCallMGRInfo);
//
//  //放外部调用
//  ////需要调用回调
//  //if(NULL != pCallMGRInfo && NULL != pCallMGRInfo->pChangeItemShow)
//  //{
//  //  pCallMGRInfo->pChangeItemShow(pCallMGRInfo->strUIName);
//  //}
//}

////内部调用的 隐藏
//void CCallUIManager::ManagerHidePage(CCallMGRInfo* pCallMGRInfo)
//{
//  //为空 那么直接返回
//  if(NULL == pCallMGRInfo)
//  {
//      return ;
//  }
//
//  //隐藏 一定得是隐藏当前的窗口
//  if(pCallMGRInfo != m_pCurrentCallMGRInfo)
//  {
//      return ;
//  }
//
//  //如果是隐藏了 当前焦点窗口   那么要调用相关的回调函数
//  if(pCallMGRInfo == m_pCurrentCallMGRInfo)
//  {
//      //把子界面隐藏了
//      DlgHidePage(pCallMGRInfo);
//
//      ////最后才看是否焦点窗口 然后赋值空
//      ////如果是当前焦点窗口 那么要把当前焦点窗口信息 赋值为空
//      //m_pCurrentCallMGRInfo = NULL;
//  }
//
//}
//
////这个是去除某个窗口
//void CCallUIManager::ManagerPutPage(CCallMGRInfo* pCallMGRInfo)
//{
//  //如果相关信息为空 那么直接返回
//  if(NULL == pCallMGRInfo)
//  {
//      return ;
//  }
//
//  //如果是当前的焦点窗口 那么还需要隐藏
//  if(pCallMGRInfo == m_pCurrentCallMGRInfo)
//  {
//      //隐藏窗口
//      DlgHidePage(pCallMGRInfo);
//
//      ////当前焦点窗口设置为空
//      m_pCurrentCallMGRInfo = NULL;
//  }
//
//  //无论是焦点还是非焦点对话框 这边都要把 对话框赋值NULL   需要把子窗口赋值为空
//  pCallMGRInfo->pDlg = NULL;
//}

//根据名称信息显示界面
void CCallUIManager::DlgShowPage(CCallMGRInfo * pCallMGRInfo)
{
    //需要有子界面对话框才显示
    if (NULL != pCallMGRInfo && NULL != pCallMGRInfo->pDlg)
    {
        //调用对话框进入当前界面函数
        if (NULL != m_pDlgCallManager)
        {
            m_pDlgCallManager->EnterPage(pCallMGRInfo);
        }
    }
}

//退出某个界面
void CCallUIManager::DlgHidePage(CCallMGRInfo * pCallMGRInfo)
{
    //需要调用对话框退出当前界面函数
    if (NULL != m_pDlgCallManager)
    {
        m_pDlgCallManager->PutPage(pCallMGRInfo);
    }
}

//获取第一个非焦点窗口  子对话框指针不能为空
CCallMGRInfo * CCallUIManager::GetFirstNoFocusItem()
{
    CCallMGRInfo * pCallMGRInfo = NULL;
    YLList<CCallMGRInfo *>::iterator ite = m_listCallMGRInfo.begin();

    for (; m_listCallMGRInfo.end() != ite; ite++)
    {
        pCallMGRInfo = *ite;

        if (NULL != pCallMGRInfo)
        {
            if (pCallMGRInfo != m_pCurrentCallMGRInfo)
            {
                if (true == pCallMGRInfo->bIsUsed)      //已经有使用的要调用退出流程
                {
                    return pCallMGRInfo;
                }
                else if (NULL !=
                         pCallMGRInfo->pDlg)    //有对话框的 要把对话框退出  调用对话框解绑回调函数
                {
                    return pCallMGRInfo;
                }

            }
        }

        pCallMGRInfo = NULL;
    }

    return NULL;
}

//获取CallManager 对话框指针
CDlgCallManagerBase * CCallUIManager::GetDlgCallManager()
{
    CDlgCallManagerBase * pDlg = (CDlgCallManagerBase *)UIManager_GetPrivateDlg(DLG_CALLMANAGER);

    if (NULL == pDlg)
    {
        pDlg = (CDlgCallManagerBase *)UIManager_CreatePrivateDlg(DLG_CALLMANAGER);
    }

    return pDlg;
}

//释放CallManager 对话框指针
void CCallUIManager::ReleaseDlgCallManager(CDlgCallManagerBase * pDlgCallManager)
{
    if (NULL != pDlgCallManager)
    {
        UIManager_PutPrivateDlg(pDlgCallManager);
    }
}

//检测 是否退出 dlgcallmanager ， 如果需要退出则退出
void CCallUIManager::CheckReleaseDlgCallManager()
{
    //这边把 dlgcallmanager 踢出uimanager 队列
    //判断如果全部为空就踢出
    if (IsAllItemPageNull())
    {
        //必须当前的 焦点信息 也为空
        if (NULL == m_pCurrentCallMGRInfo)
        {
            //把DlgCallManager 窗口指针从UIManager队列中释放
            if (NULL != m_pDlgCallManager)
            {
                CDlgCallManagerBase * pDlgCallManager = m_pDlgCallManager;
                //先把保存的指针置空
                m_pDlgCallManager = NULL;

                //如果是UIManager 调用 returntoidle 退出的 UImanager会自动把该窗口退出   那么 就不需要这边调用了
                if (!pDlgCallManager->IsReturnToIdle())
                {
                    //释放窗口指针
                    ReleaseDlgCallManager(pDlgCallManager);
                }
            }
        }
    }
}

//回到Idle前的处理函数
void CCallUIManager::OnReturnToIdle()
{
    //循环退出非焦点窗口
    for (;;)
    {
        CCallMGRInfo * pNoFocusCallMGRInfo = GetFirstNoFocusItem();

        if (NULL == pNoFocusCallMGRInfo)
        {
            break;
        }

        //如果是有使用 那么要调用退出的函数
        if (true == pNoFocusCallMGRInfo->bIsUsed)
        {
            //这边设置成 false 免得重复调用
            pNoFocusCallMGRInfo->bIsUsed = false;

            //退出窗口 直接调用退出窗口的指针
            if (NULL != pNoFocusCallMGRInfo->pHidePage)
            {
                pNoFocusCallMGRInfo->pHidePage(pNoFocusCallMGRInfo->strUIName);
            }
        }
        else if (NULL != pNoFocusCallMGRInfo->pDlg)
        {
            CCallManagerBaseDlg * pNoFocusCallMGRInfoDlg = pNoFocusCallMGRInfo->pDlg;

            //这边把对话框设置成false 以免重复调用
            pNoFocusCallMGRInfo->pDlg = NULL;

            //需要解除绑定窗口
            if (NULL != pNoFocusCallMGRInfo->pUnboundCallBack)
            {
                pNoFocusCallMGRInfo->pUnboundCallBack(pNoFocusCallMGRInfo->strUIName, pNoFocusCallMGRInfoDlg);
            }
        }
    }

    //最后退出焦点窗口
    if (NULL != m_pCurrentCallMGRInfo)
    {
        //这边就不做容错判断了  因为是焦点窗口的信息  那么肯定是没有退出的   如果不是这样的结果 就是流程异常
        //这边设置成 false 免得重复调用
        m_pCurrentCallMGRInfo->bIsUsed = false;

        //退出窗口 直接调用退出窗口的指针
        if (NULL != m_pCurrentCallMGRInfo->pHidePage)
        {
            m_pCurrentCallMGRInfo->pHidePage(m_pCurrentCallMGRInfo->strUIName);
        }
        //在put 的时候 会设置为空了  这边就不设置了
        //这边一定不能设置为空  否则出错
        //  m_pCurrentCallMGRInfo = NULL;
    }

    if (NULL != m_pCurrentCallMGRInfo)
    {
        CCallMGRInfo * pFocusCallMGRInfo = m_pCurrentCallMGRInfo;
        CCallManagerBaseDlg * pFocusDlg = pFocusCallMGRInfo->pDlg;

        //这边焦点窗口信息设置为空
        m_pCurrentCallMGRInfo = NULL;

        //调用隐藏回调函数
        if (NULL != pFocusCallMGRInfo->pPageEventCallBack)
        {
            pFocusCallMGRInfo->pPageEventCallBack(pFocusCallMGRInfo->strUIName, CallManagerEvent_Hide);
        }

        //这边窗口不为空 那么要调用窗口解绑回调函数
        if (NULL != pFocusCallMGRInfo->pDlg)
        {
            pFocusCallMGRInfo->pDlg = NULL;

            //需要解除绑定窗口
            if (NULL != pFocusCallMGRInfo->pUnboundCallBack)
            {
                pFocusCallMGRInfo->pUnboundCallBack(pFocusCallMGRInfo->strUIName, pFocusDlg);
            }
        }
    }

    //这边确认一下是否退出
    CheckReleaseDlgCallManager();

    //暂时先不容了  留着容易发现问题
    ////这边是容错  终究肯定是要设置成NULL的
    //if(NULL != m_pDlgCallManager)
    //{
    //  m_pDlgCallManager = NULL;
    //}
}

void CCallUIManager::OnCallManagerBarClick(const yl::string & strUIName)
{
#if IF_BUG_28455
    if (strUIName == "directory")
    {
        yl::string strXmlUrl = configParser_GetConfigString(kszXmlPhonebookUrl);
        if (!strXmlUrl.empty())
        {
            xmlbrowser_Enter(strXmlUrl.c_str());
            return;
        }
    }
#endif
    //如果点击的就是当前的窗口 那么就不需要做什么操作
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (strUIName == m_pCurrentCallMGRInfo->strUIName)
        {
            return ;
        }
    }

    //这边只要进入对应的对话框就可以了
    CCallMGRInfo * pCallMGRInfo = NULL;
    YLList<CCallMGRInfo *>::iterator ite = m_listCallMGRInfo.begin();

    for (; m_listCallMGRInfo.end() != ite; ite++)
    {
        pCallMGRInfo = *ite;

        if (NULL != pCallMGRInfo)
        {
            if (strUIName == pCallMGRInfo->strUIName)
            {
                break;
            }
        }

        pCallMGRInfo = NULL;
    }

    //没有找到对应的信息  直接返回
    if (NULL == pCallMGRInfo)
    {
        return ;
    }

    //如果是第一次进入该item  那么需要调用默认的进入Item 的接口
    if (false == pCallMGRInfo->bIsUsed)
    {
        if (NULL != pCallMGRInfo->pShowPage)
        {
            pCallMGRInfo->pShowPage(pCallMGRInfo->strUIName);
        }
    }
    else if (NULL !=
             pCallMGRInfo->pDlg) //不是第一次进入该Item 而且对话框不为空 那么才需要显示  如果对话框为空也不显示了
    {
        //直接调用切换的函数就可以了
        ModelEnterPage(strUIName);
    }

}

//退出某个窗口显示
void CCallUIManager::OnCallManagerReleasePage(const yl::string & strUIName)
{
    //这边只要进入对应的对话框就可以了
    CCallMGRInfo * pCallMGRInfo = NULL;
    YLList<CCallMGRInfo *>::iterator ite = m_listCallMGRInfo.begin();

    for (; m_listCallMGRInfo.end() != ite; ite++)
    {
        pCallMGRInfo = *ite;

        if (NULL != pCallMGRInfo)
        {
            if (strUIName == pCallMGRInfo->strUIName)
            {
                break;
            }
        }

        pCallMGRInfo = NULL;
    }

    //没有找到对应的信息  直接返回
    if (NULL == pCallMGRInfo)
    {
        return ;
    }

    //这边有使用才需要退出
    if (true == pCallMGRInfo->bIsUsed)
    {
        //调用接口退出
        if (NULL != pCallMGRInfo->pHidePage)
        {
            pCallMGRInfo->pHidePage(strUIName);
        }
    }
}

//是否当前焦点窗口
bool CCallUIManager::IsFocusPage(const yl::string & strUIName)
{
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (m_pCurrentCallMGRInfo->strUIName == strUIName)
        {
            return true;
        }
    }

    return false;
}

//是否所有的子界面都是为空   这个用于在当前界面的对话框为空的时候判断是否要退出 callmanager 界面
bool CCallUIManager::IsAllItemPageNull()
{
    CCallMGRInfo * pCallMGRInfo = NULL;
    YLList<CCallMGRInfo *>::iterator ite = m_listCallMGRInfo.begin();

    for (; m_listCallMGRInfo.end() != ite; ite++)
    {
        pCallMGRInfo = *ite;

        if (NULL != pCallMGRInfo)
        {
            //有一个还在使用 就不能退出
            if (true == pCallMGRInfo->bIsUsed)
            {
                return false;
            }
            else if (NULL !=
                     pCallMGRInfo->pDlg)    //只要有一个对话框的指针不为空 那么就返回false
            {
                return false;
            }
        }

        pCallMGRInfo = NULL;
    }

    //没有任何对话框那么返回true
    return true;
}


