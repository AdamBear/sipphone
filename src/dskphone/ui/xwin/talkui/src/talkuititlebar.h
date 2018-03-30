#ifndef _TALKUI_TITLE_H
#define _TALKUI_TITLE_H

#include "xwindow/xWindow.h"

#include "talkuilogic/talkuilogiccommon.h"
#include "baseui/include/timerpieceview.h"

namespace talkui
{
//titlebar 与其他控件不大一样  titlebar 的loadwidget 是在 init函数执行之前
//通话UI的title
class CTalkUITitleBar
{
public:
    CTalkUITitleBar();
    virtual ~CTalkUITitleBar();

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
    //设置通道图标
    void SetChannelIcon(const yl::string & strChannelIcon, bool bShow = true);

    //设置显示title
    void SetTitle(const yl::string & strTitle);
    void SetTitleScroll(bool bScroll);

    //设置显示页码
    void SetPageNum(int iPageIndex, int iPageCount);
    //隐藏页码 也就是不需要显示页码
    void HidePageNum();

    //清除所有图标  这边不包含通道图标
    void ClearAllIcons();

    //添加图标  这边不包含通道图标
    void AddIcon(const yl::string & strIcon);

    //删除图标  不包含通道图标
    void DeleteIcon(const yl::string & strIcon);

#if IF_TALKUI_USB_ENABLE
    void StartRecord();
    void PauseRecord();
    void StopRecord();
    void SyncRecordTime();
#endif

protected:
    xView * m_pMyView;              //自身布局控件
    //通道图标
    xImageView * m_pChannelIcon;    //通道图标
    //显示的title
    xTextView * m_pTitleContent;    //显示的通话title
    //图标数组控件
    talkui_vector<xImageView *> m_vectorIconView;
    //图标数组 图标路径
    talkui_list<yl::string> m_listIconString;
    //显示的页码
    xTextView * m_pPageNum;

    int m_iIconCount;       //图标个数

#if IF_TALKUI_USB_ENABLE
    //录制控件整体
    xView * m_pRecordWidget;
    //录制图标
    xImageView * m_pRecordIcon;
    //录制时间
    xTimerPieceView* m_pRecordTime;
#endif

};          //class CTalkUITitleBar


}       //namespace talkui


#endif
