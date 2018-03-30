#include "talkuifactory.h"
#include "dlgbasetalkui.h"

#include "qtcommon/qmisc.h"

//#include "commonfunction.h"
#ifdef _T48
#include "callmanager/modcallmanager.h"
#endif

#include "commonapi/stringhelperapi.h"

//错误的sessionID  清除数据的时候 会设置
#define WRONG_SESSIONID -2

CTalkUIFactory & CTalkUIFactory::GetInstance()
{
    static CTalkUIFactory s_cTalkUIFactory;

    return s_cTalkUIFactory;
}

CTalkUIFactory::CTalkUIFactory(void)
{
}

CTalkUIFactory::~CTalkUIFactory(void)
{
}

bool CTalkUIFactory::Init()
{
    return true;
}

void CTalkUIFactory::UnInit()
{
    ClearAllTalkUI();
}

//该函数调用 GetTalkUIByData 或 GetTalkUIByDataNoDelete 获取通话子界面
//在该函数内部判断是要调用哪个函数来实现
CDlgBaseTalkUI * CTalkUIFactory::GetTalkUIBySessionIDAndUIName(int iSessionID,
        yl::string strTalkUIName)
{
    CDlgBaseTalkUI * v_pDlgTalkUI = NULL;

    v_pDlgTalkUI = GetTalkUIByData(iSessionID, strTalkUIName);

    return v_pDlgTalkUI;

}

CDlgBaseTalkUI * CTalkUIFactory::GetTalkUIByData(int iSessionID, yl::string strTalkUIName)
{
    //如果SessionID非法 那么返回NULL
    if (false == IsSessionIDValid(iSessionID))
    {
        return NULL;
    }

    //如果UI名称为空 那么返回NULL
    if (true == strTalkUIName.empty())
    {
        return NULL;
    }

    CDlgBaseTalkUI * v_pTalkUI = NULL;
    v_pTalkUI = GetTalkUIFromCache(iSessionID, strTalkUIName);

    //从缓存中有获取到对应的实例 则直接返回 不需要删除了
    if (NULL != v_pTalkUI)
    {
        return v_pTalkUI;
    }

    //如果是获取拨号界面  由于拨号界面是单例 所以需要先把缓存中的拨号界面清除
    //先与创建之前清除
    //这边也修改 变成不是单例 目前v80 彩屏都不是单例
    //if(DLG_Dial == strTalkUIName)
    //{
    //  ClearTalkUIByTalkUIName(DLG_Dial);
    //}

    //该函数内部赋值 SessionID和TalkUIName
    v_pTalkUI = CreateTalkUI(iSessionID, strTalkUIName);

    //创建不成功 直接返回NULL
    if (NULL == v_pTalkUI)
    {
        return NULL;
    }

    //需要先删除之前需要删除的界面  然后才把当前对话框入栈
    //在插入队列之前插入与iSessionID相关的对话框实例
    //这边是与GetTalkUIByDataNoDelete函数有差异的地方
//  ClearTalkUIBySessionID(iSessionID);
    CheckTalkUIBySessionID(iSessionID);

    //这边拨号界面也需要入栈
    //把该对话框实例插入到缓存列表中
    m_listTalkUI.push_back(v_pTalkUI);

    return v_pTalkUI;
}

//该函数暂时没有使用
CDlgBaseTalkUI * CTalkUIFactory::GetTalkUIByDataNoDelete(int iSessionID, yl::string strTalkUIName)
{
    //如果SessionID非法 那么返回NULL
    if (false == IsSessionIDValid(iSessionID))
    {
        return NULL;
    }

    //如果UI名称为空 那么返回NULL
    if (true == strTalkUIName.empty())
    {
        return NULL;
    }

    CDlgBaseTalkUI * v_pTalkUI = NULL;
    v_pTalkUI = GetTalkUIFromCache(iSessionID, strTalkUIName);

    //从缓存中有获取到对应的实例 则直接返回
    if (NULL != v_pTalkUI)
    {
        return v_pTalkUI;
    }

    //如果是获取拨号界面  由于拨号界面是单例 所以需要先把缓存中的拨号界面清除
    //先与创建之前清除
    if (DLG_Dial == strTalkUIName)
    {
        ClearTalkUIByTalkUIName(DLG_Dial);
    }

    //该函数内部赋值 SessionID和TalkUIName
    v_pTalkUI = CreateTalkUI(iSessionID, strTalkUIName);

    //创建不成功 直接返回NULL
    if (NULL == v_pTalkUI)
    {
        return NULL;
    }

    //这边是与GetTalkUIByData函数有差异的地方
    //.................


    //把该对话框实例插入到缓存列表中
    m_listTalkUI.push_back(v_pTalkUI);

    return v_pTalkUI;
}

//该函数用于在获取通话子窗口的时候，清除同ID中不需要缓存的子窗口
void CTalkUIFactory::CheckTalkUIBySessionID(int iSessionID)
{
    //如果SessionID非法 那么返回NULL
    if (false == IsSessionIDValid(iSessionID))
    {
        return ;
    }

    YLList<CDlgBaseTalkUI *>::iterator v_itList = m_listTalkUI.begin();
    CDlgBaseTalkUI * v_pTalkUI = NULL;

    //遍历 TalkUI列表查找 DlgBaseTalkUI
    for (; m_listTalkUI.end() != v_itList;)
    {
        v_pTalkUI = *v_itList;

        //做容错判断
        if (NULL == v_pTalkUI)
        {
            v_itList = m_listTalkUI.erase(v_itList);
            continue;
        }

        if (v_pTalkUI->GetSessionId() == iSessionID)
        {
            yl::string strTalkUIName = v_pTalkUI->GetTalkUIName();

            //只有 Talking、Conf、ConfManager 等需要界面保存数据的 才需要缓存
            if ((DLG_Talking != strTalkUIName)
                    && (DLG_Conference != strTalkUIName)
                    && (DLG_ConfManagerUI != strTalkUIName)
               )
            {
                ReleaseTalkUI(v_pTalkUI);
                v_itList = m_listTalkUI.erase(v_itList);
                continue;
            }
        }

        v_itList++;

    }
}

CDlgBaseTalkUI * CTalkUIFactory::FindTalkUIBySessionId(int iSessionId, yl::string strTalkUIName)
{
    CDlgBaseTalkUI * pFindTalkUI = NULL;
    //查找TalkUI
    YLList<CDlgBaseTalkUI *>::const_iterator itFind = m_listTalkUI.begin();

    for (; itFind != m_listTalkUI.end(); ++itFind)
    {
        CDlgBaseTalkUI * pTalkUI = (CDlgBaseTalkUI *)*itFind;
        if (pTalkUI != NULL
                && pTalkUI->GetSessionId() == iSessionId)
        {
            if (pFindTalkUI == NULL)
            {
                pFindTalkUI = pTalkUI;
            }
            if (strTalkUIName == ""
                    || strTalkUIName == pTalkUI->GetTalkUIName())
            {
                //查找到对应SessionId的TalkUI指针
                return pTalkUI;
            }
        }
    }

    //返回对应指针
    return pFindTalkUI;

}


CDlgBaseTalkUI * CTalkUIFactory::GetTalkUIFromCache(int iSessionID, yl::string strTalkUIName)
{
    //如果SessionID非法 那么返回NULL
    if (false == IsSessionIDValid(iSessionID))
    {
        return NULL;
    }

    //如果UI名称为空 那么返回NULL
    if (true == strTalkUIName.empty())
    {
        return NULL;
    }

    YLList<CDlgBaseTalkUI *>::iterator v_itList = m_listTalkUI.begin();
    CDlgBaseTalkUI * v_pTalkUI = NULL;

    //遍历 TalkUI列表查找 DlgBaseTalkUI
    for (; m_listTalkUI.end() != v_itList;)
    {
        v_pTalkUI = *v_itList;

        //做容错判断
        if (NULL == v_pTalkUI)
        {
            v_itList = m_listTalkUI.erase(v_itList);
            continue;
        }

        if ((v_pTalkUI->GetSessionId() == iSessionID)
                && (v_pTalkUI->GetTalkUIName() == strTalkUIName))
        {
            return v_pTalkUI;
        }

        v_itList++;

    }

    return NULL;
}

void CTalkUIFactory::ClearTalkUIBySessionID(int iSessionID)
{
    //这边非法的 也需要清除
    //如果SessionID非法 那么返回NULL
    //if(false == IsSessionIDValid(iSessionID))
    //{
    //  return ;
    //}

    YLList<CDlgBaseTalkUI *>::iterator v_itList = m_listTalkUI.begin();
    CDlgBaseTalkUI * v_pTalkUI = NULL;

    //遍历 TalkUI列表查找 DlgBaseTalkUI
    for (; m_listTalkUI.end() != v_itList;)
    {
        v_pTalkUI = *v_itList;

        //做容错判断
        if (NULL == v_pTalkUI)
        {
            v_itList = m_listTalkUI.erase(v_itList);
            continue;
        }

        if (v_pTalkUI->GetSessionId() == iSessionID)
        {
            ReleaseTalkUI(v_pTalkUI);
            v_itList = m_listTalkUI.erase(v_itList);
            continue;
        }

        v_itList++;

    }

}

//把session对应的数据设置为无效  即sessionid设置为 错误的
void CTalkUIFactory::SetSessionDataInvalid(int iSessionID)
{
    //如果SessionID非法 那么返回NULL
    if (false == IsSessionIDValid(iSessionID))
    {
        return ;
    }

    YLList<CDlgBaseTalkUI *>::iterator v_itList = m_listTalkUI.begin();
    CDlgBaseTalkUI * v_pTalkUI = NULL;

    //遍历 TalkUI列表查找 DlgBaseTalkUI
    for (; m_listTalkUI.end() != v_itList;)
    {
        v_pTalkUI = *v_itList;

        //做容错判断
        if (NULL == v_pTalkUI)
        {
            v_itList = m_listTalkUI.erase(v_itList);
            continue;
        }

        //设置 sessionID 为错误的
        if (v_pTalkUI->GetSessionId() == iSessionID)
        {
            v_pTalkUI->SetSessionId(WRONG_SESSIONID);
        }

        v_itList++;
    }
}

//清除错误SessionID 的 通话界面
void CTalkUIFactory::ClearInvalidTalkUI()
{
    ClearTalkUIBySessionID(WRONG_SESSIONID);
}

//清除所有的对话框实例
void CTalkUIFactory::ClearAllTalkUI()
{
    YLList<CDlgBaseTalkUI *>::iterator v_itList = m_listTalkUI.begin();
    CDlgBaseTalkUI * v_pTalkUI = NULL;

    //遍历 TalkUI列表查找 DlgBaseTalkUI
    for (; m_listTalkUI.end() != v_itList;)
    {
        v_pTalkUI = *v_itList;

        //做容错判断
        if (NULL == v_pTalkUI)
        {
            v_itList = m_listTalkUI.erase(v_itList);
            continue;
        }

        ReleaseTalkUI(v_pTalkUI);
        v_itList = m_listTalkUI.erase(v_itList);
    }
}

int CTalkUIFactory::GetTalkUICount()
{
    return m_listTalkUI.size();
}

CDlgBaseTalkUI * CTalkUIFactory::GetTalkUIByTalkUIName(yl::string strTalkUIName)
{
    //名称为空 直接返回NULL
    if (0 >= strTalkUIName.length())
    {
        return NULL;
    }

    YLList<CDlgBaseTalkUI *>::iterator v_itList = m_listTalkUI.begin();
    CDlgBaseTalkUI * v_pTalkUI = NULL;

    //遍历 TalkUI列表查找 DlgBaseTalkUI
    for (; m_listTalkUI.end() != v_itList;)
    {
        v_pTalkUI = *v_itList;

        //做容错判断
        if (NULL == v_pTalkUI)
        {
            v_itList = m_listTalkUI.erase(v_itList);
            continue;
        }

        //如果名称是相同的 那么就直接返回
        if (v_pTalkUI->GetTalkUIName() == strTalkUIName)
        {
            return v_pTalkUI;
        }

        v_itList++;

    }

    return NULL;
}

void CTalkUIFactory::ClearTalkUIByTalkUIName(yl::string strTalkUIName)
{
    //如果 strTalkUIName 为空 那么直接返回
    if (0 >= strTalkUIName.length())
    {
        return ;
    }

    YLList<CDlgBaseTalkUI *>::iterator v_itList = m_listTalkUI.begin();
    CDlgBaseTalkUI * v_pTalkUI = NULL;

    //遍历 TalkUI列表查找 DlgBaseTalkUI
    for (; m_listTalkUI.end() != v_itList;)
    {
        v_pTalkUI = *v_itList;

        //做容错判断
        if (NULL == v_pTalkUI)
        {
            v_itList = m_listTalkUI.erase(v_itList);
            continue;
        }

        //如果名称是相同的 那么就清除掉
        if (v_pTalkUI->GetTalkUIName() == strTalkUIName)
        {
            ReleaseTalkUI(v_pTalkUI);
            v_itList = m_listTalkUI.erase(v_itList);
            continue;
        }

        v_itList++;

    }
}

//创建CDlgBaseTalkUI实例
CDlgBaseTalkUI * CTalkUIFactory::CreateTalkUI(int iSessionID, yl::string strTalkUIName)
{
    CDlgBaseTalkUI * v_pTalkUI = NULL;

    if (false == IsSessionIDValid(iSessionID))
    {
        return NULL;
    }

    if (0 >= strTalkUIName.length())
    {
        return NULL;
    }

    ////t48 生成
    //#ifdef _T48
    //  //如果 是拨号界面  就要有特殊方式来获取
    //  if(DLG_Dial == strTalkUIName)
    //  {
    //      v_pTalkUI = (CDlgBaseTalkUI*)CallManager_GetPage(CALLMGR_DIALING);
    //  }
    //  else
    //  {
    //      yl::string strFormatUIName = commonAPI_FormatString("%s_%d", strTalkUIName.c_str(), iSessionID);

    //      CBaseDialog* v_pBaseDlg = UIManager_CreatePrivateDlg(strTalkUIName, strFormatUIName);

    //      //判断获取的指针是否为空
    //      if(NULL == v_pBaseDlg)
    //      {
    //          return NULL;
    //      }

    //      if(false == v_pBaseDlg->inherits(DLG_BaseTalkUI))
    //      {
    //          UIManager_PutPrivateDlg(v_pBaseDlg);
    //          v_pBaseDlg = NULL;
    //      }
    //      else
    //      {
    //          v_pTalkUI = (CDlgBaseTalkUI*)v_pBaseDlg;
    //      }

    //
    //  }

    //t48 v80 统一改成都从uimanager 获取   拨号界面的显示支持多个
    yl::string strFormatUIName = commonAPI_FormatString("%s_%d", strTalkUIName.c_str(), iSessionID);
    CBaseDialog * v_pBaseDlg = UIManager_CreatePrivateDlg(strTalkUIName, strFormatUIName);

    if (NULL == v_pBaseDlg)
    {
        return NULL;
    }

    //判断该获得的窗口指针是否为TalkUI对象指针
    if (v_pBaseDlg->inherits(DLG_BaseTalkUI))
    {
        //TalkUI对象指针
        //转化指针
        v_pTalkUI = (CDlgBaseTalkUI *)v_pBaseDlg;

    }
    else
    {
        //非TalkUI对象指针
        //将生成的对象指针释放返回
        UIManager_PutPrivateDlg(v_pBaseDlg);
        v_pBaseDlg = NULL;
    }

    if (NULL != v_pTalkUI)
    {
        //这边要赋值 sessionID 和 TalkUIName
        v_pTalkUI->SetSessionId(iSessionID);
        //这边暂时不设置UIName了 因为在生成的时候 就会有设置
        //  v_pTalkUI->SetTalkUIName(strTalkUIName);
    }

    return v_pTalkUI;
}

//释放CDlgBaseTalkUI实例
void CTalkUIFactory::ReleaseTalkUI(CDlgBaseTalkUI * pTalkUI)
{
    if (NULL == pTalkUI)
    {
        return ;
    }

    UIManager_PutPrivateDlg(pTalkUI);
}

bool CTalkUIFactory::IsSessionIDValid(int iSessionID)
{
    if (iSessionID < 0)
    {
        return false;
    }

    return true;
}

