#ifndef _TALKUI_MENU_TITLE_H
#define _TALKUI_MENU_TITLE_H

#include "xwindow/xWindow.h"
#include "talkuilogic/talkuilogiccommon.h"

namespace talkui
{
//通话UI的menu title
class CTalkUIMenuTitle
{
public:
    CTalkUIMenuTitle();
    virtual ~CTalkUIMenuTitle();

public:
    //加载控件
    void LoadWidget(xView * pLayout);

public:
    //初始化
    void Init();

    //反初始化
    void Uninit();

public:
    void Show();
    void Hide();

public:
    //设置显示title
    void SetTitle(const yl::string & strTitle);

    //设置显示页码
    void SetPageNum(int iPageIndex, int iPageCount);
    //隐藏页码 也就是不需要显示页码
    void HidePageNum();

protected:
    xView * m_pMyView;              //自身布局控件

    //显示的title
    xTextView * m_pTitleContent;    //显示的通话title

    //显示的页码
    xTextView * m_pPageNum;

};          //class CTalkUIMenuTitle


}       //namespace talkui


#endif //_TALKUI_MENU_TITLE_H
