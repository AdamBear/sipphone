#ifndef __X_PAGE_CTRL_H__
#define __X_PAGE_CTRL_H__

#include <xwindow/xlistviewdefine.h>
#include "iscrollnotify.h"
#include "xWindow.h"
#include "ylstring.h"

#if IF_XLISTVIEW_SCROLL_BY_PAGE_CTRL
class xPageCtrl : public xView
{
public:
    explicit xPageCtrl(IScrollNotify* pScrollNotify);
    ~xPageCtrl();

    // 设置翻页通知
    void SetScrollCtrlNotify(IScrollNotify* pScrollNotify);

    // 设置控件位置
    void SetRect(const chRect& rect);

    // 设置页数信息
    void SetScrollInfo(int nPageRange, int nRange, int nPos);

    // 设置页码
    void SetScrollPos(int nPos);

    // 获取页码
    int GetPageNo();

    // 获取总页数
    int GetPageCount();

protected:
    // 重排窗口
    void Relayout();

private:
    IScrollNotify* m_pScrollNotify;

    int m_nPageNo;
    int m_nPageCount;

    chRect m_rcText;
    yl::string m_strPageInfo;
};
#endif

#endif // __PAGE_CTRL_H__
