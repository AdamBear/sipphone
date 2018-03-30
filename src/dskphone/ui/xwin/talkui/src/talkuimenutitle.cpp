#include "talkuimenutitle.h"
#include "xwindow/xWindowManager.h"
#include "talkuicommonfunction.h"
#include "talkuixmlitemname.h"

namespace talkui
{

CTalkUIMenuTitle::CTalkUIMenuTitle()
{
    m_pMyView = NULL;               //自身布局控件
    //显示的title
    m_pTitleContent = NULL;     //显示的通话title
    //显示的页码
    m_pPageNum = NULL;
}

CTalkUIMenuTitle::~CTalkUIMenuTitle()
{

}

void CTalkUIMenuTitle::LoadWidget(xView * pLayout)
{
    if (NULL == pLayout)
    {
        return;
    }

    m_pMyView = (xView *)pLayout->getViewById(TALKUI_MENUBAR_ID);       //自身布局

    ////显示的title
    m_pTitleContent = (xTextView *)pLayout->getViewById(
                          TALKUI_TITLEBAR_MENU_TITLECONTENT_ID);      //显示的通话title

    //显示的页码
    m_pPageNum = (xTextView *)pLayout->getViewById(TALKUI_TITLEBAR_MENU_PAGENUM_ID);
}

//初始化
void CTalkUIMenuTitle::Init()
{
    //设置title 为空
    SetTitle("");

    //隐藏页码
    HidePageNum();
}

//反初始化
void CTalkUIMenuTitle::Uninit()
{
    //初始化有处理就好了
}

void CTalkUIMenuTitle::Show()
{
    if (NULL != m_pMyView)
    {
        m_pMyView->show(xView::SHOW_CMD_BRING_TO_TOP);
    }
}

void CTalkUIMenuTitle::Hide()
{
    if (NULL != m_pMyView
            && m_pMyView->isVisible())
    {
        m_pMyView->hide(true);
    }
}

//设置显示title
void CTalkUIMenuTitle::SetTitle(const yl::string & strTitle)
{
    if (NULL != m_pTitleContent)
    {
        m_pTitleContent->SetText(strTitle);
    }
}

//设置显示页码
void CTalkUIMenuTitle::SetPageNum(int iPageIndex, int iPageCount)
{
    if (NULL != m_pPageNum)
    {
        char szPageNum[16] = { 0 };
        sprintf(szPageNum, "%d/%d", iPageIndex, iPageCount);
        yl::string strPageNum(szPageNum);

        m_pPageNum->SetText(strPageNum);

        m_pPageNum->show(xView::SHOW_CMD_BRING_TO_TOP);
    }
}

//隐藏页码 也就是不需要显示页码
void CTalkUIMenuTitle::HidePageNum()
{
    if (NULL != m_pPageNum)
    {
        m_pPageNum->hide(true);
    }
}

}       //namespace talkui


