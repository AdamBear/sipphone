#include "talkbaseui.h"

namespace talkui
{


//设置父窗口
//void SetViewParent(xView* pSon, xViewGroup* pNewParent)
//{
//  //指针判空
//  if(NULL == pSon)
//  {
//      return ;
//  }
//
//  //获取当前的父窗口
//  xViewGroup* pOldParent = pSon->parent();
//
//  //和父窗口相同 那么就不做处理了
//  if(pOldParent == pNewParent)
//  {
//      return ;
//  }
//
//  printf("------------------- old pop \n");
//  //之前的父窗口不为空 那么需要把该窗口退出
//  if(NULL != pOldParent)
//  {
//      pOldParent->popupChildView(pSon);
//  }
//
//  printf("------------------- new add %d \n", pNewParent);
//
//  //加入现在的父窗口
//  if(NULL != pNewParent)
//  {
//      pNewParent->addChildView(pSon);
//  }
//
//  printf("------------------- new add end \n");
//
//}


CTalkBaseUI::CTalkBaseUI(const yl::string & strTalkUIClassName)
    : m_strTalkUIClassName(strTalkUIClassName)
{

}

CTalkBaseUI::~CTalkBaseUI()
{

}

////是否显示title
//bool CDlgTalkUIFrame::IsShowTitle()
//{
//  if(NULL != m_pTalkUI)
//  {
//      return m_pTalkUI->IsShowTitle();
//  }
//
//  return false;
//}
//
////获取显示title信息
//void CDlgTalkUIFrame::GetTitleInfo(CTalkTitleInfo& objTalkTitleInfo)
//{
//  if(NULL != m_pTalkUI)
//  {
//      m_pTalkUI->GetTitleInfo(objTalkTitleInfo);
//  }
//}


////设置父窗口
//void CTalkBaseUI::SetParent(xViewGroup* pBaseUI)
//{
//  SetViewParent(this, pBaseUI);
//}
//
////获取父窗口指针
//xViewGroup* CTalkBaseUI::GetParent()
//{
//  return parent();
//}
//
////提到最顶层
//void CTalkBaseUI::Raise()
//{
//  xViewGroup* pParent = GetParent();
//
//  if(NULL != pParent)
//  {
//      pParent->moveChildToTop(this);
//  }
//}
//
////放到最低层
//void CTalkBaseUI::Lower()
//{
//  xViewGroup* pParent = GetParent();
//
//  if(NULL != pParent)
//  {
//      pParent->moveChildToBottom(this);
//  }
//
//}
//
////设置区域
//void CTalkBaseUI::SetRect(const chRect& rectDlg)
//{
//  setGeometry(rectDlg);
//}
//
////删除
//void CTalkBaseUI::DeleteLater()
//{
//  //用close就可以了  在xwindow中close的函数 会调用（类似）deletelater的功能
//  close();
//}


yl::string CTalkBaseUI::GetTalkUIClassName()        //获取通话子界面类名称
{
    return m_strTalkUIClassName;
}

}       //namespace talkui
