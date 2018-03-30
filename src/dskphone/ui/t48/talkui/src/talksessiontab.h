#ifndef _TALK_SESSION_TAB_
#define _TALK_SESSION_TAB_

#include <vector>
#include "uikernel/quipushbutton.h"
#include "baseui/iscrollnotify.h"

//mod-QT
#include <QtGui>

//mod-Talk
// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talkadapter/include/modtalkadapter.h"

class qLoopPicAnimation;
class qScrollTextLabel;
class QTimePiece;
class CPageCtrl;

enum TALK_TAB_LAYOUT_STYLE
{
    TALK_TAB_LAYOUT_STYLE_NORMAL = 0,
    TALK_TAB_LAYOUT_STYLE_SHOW_SECOND_LAB,
};

//通话管理框子项，从UIPushButton中继承，其包含多个Label，并自动排列位置
class CTalkSessionTab : public qUIPushButton
{
    Q_OBJECT

public:
    //构造函数
    CTalkSessionTab(QWidget * pParent = 0);
    //析构函数
    virtual ~CTalkSessionTab();

public:
    //设置文本
    void SetTabText(const QString & strText);
    // 设置图片动画
    void SetAnimationPic(const QString & strPic,
                         const QString & strEnablePic,
                         bool bLoop = false);
    //设置显示时间
    void SetTime(UINT uHour, UINT uMinute, UINT uSecond);
    //是否显示时间
    void ActiveTimer(bool bShow);

    // 清空数据
    void ClearData();

    // 自动调整控件布局
    void AdjustLayout(bool bHasScrollBar);

    //设置所存放的数据
    inline void SetData(const QVariant & varData)
    {
        m_varData = varData;
    }

    void SetFocusFlag(bool bFocus)
    {
        m_bFocusItem = bFocus;
    }

    bool GetFocusFlag()
    {
        return m_bFocusItem;
    }

    void SetUIName(const QString & strUIName)
    {
        m_strUIName = strUIName;
    }

    QString GetUIName()
    {
        return m_strUIName;
    }

    //获取所存放的数据
    const QVariant & GetData()
    {
        return m_varData;
    }

    void SetEnableTextColor(QColor clr)
    {
        m_enableClrText = clr;
    }

    void SetDisableTextColor(QColor clr)
    {
        m_disableClrText = clr;
    }

    void SetLayoutStyle(TALK_TAB_LAYOUT_STYLE eLayout);

    void SetNextLabText(const QString& strText);

signals:
    //点击信号，带指针发送
    void clicked(CTalkSessionTab * pItem);

    //按下信号，带指针发送
    void pressed(CTalkSessionTab * pItem);

protected slots:
    //响应点击事件槽
    virtual void OnClicked();

    // 响应按下事件槽
    virtual void OnPressed();

protected:
    void paintEvent(QPaintEvent * pe);

private:
    void ReLayoutWidget();

protected:
    //计时器控件
    QTimePiece     *    m_pTimePiece;
    // 滚动文字控件
    qScrollTextLabel  * m_pLabel;
    // 又一个滚动文字控件
    qScrollTextLabel*   m_pLabelNext;
    //控件所存放的数据
    QVariant            m_varData;
    // 对应UI的名字
    QString             m_strUIName;
    QString             m_strEnableIconPic;    // Button被点击时显示的Icon图片
    QColor              m_enableClrText;
    QColor              m_disableClrText;

    // 是否为焦点Item
    bool                m_bFocusItem;
    //显示布局风格
    TALK_TAB_LAYOUT_STYLE m_eLayout;
};


//通话管理框最多显示子项数目
#define MAX_TALKMANAGER_SHOW_COUNT      5

//通话管理框，从QFrame派生，管理多个子项，通过从Logic层获取对应子项数据列表进行显示
class CTalkTabManager : public QFrame, public IScrollNotify
{
    Q_OBJECT

public:
    //构造函数
    CTalkTabManager(QWidget * pParent = 0);
    //析构函数
    virtual ~CTalkTabManager();

public:
    //刷新内容
    // iCurrentPage选中的页数,为-1时表示跳转到focusSession的那一页
    void RefreshContent(int iCurrentPage = -1);

    // 清除页面数据
    void ClearData();

    // 通知向前滚动
    virtual void OnNotifyScrollPrevious();

    // 通知向后滚动
    virtual void OnNotifyScrollNext();

public:
    //返回当前是否已经显示了 来电session
    bool GetIsShowIncomingCallSession();

    //获取当前通话窗口页数
    int GetPageSize();

protected:
    //加载数据
    void LoadData(List_AllSession_Data & listSessionData, int nBegin,
                  int nCountGet, int nFocusIndex);

    //设置连接类型项
    void SetConnectTab(CTalkSessionTab & objItem, const DataLogic2UI & objData, bool bFocus);
    //设置来电类型项
    void SetRingingTab(CTalkSessionTab & objItem, const DataLogic2UI & objData, bool bFocus);
    //设置通话类型项
    void SetTalkingTab(CTalkSessionTab & objItem, const DataLogic2UI & objData, bool bFocus);
    //设置结束类型项
    void SetFinishedTab(CTalkSessionTab & objItem, const DataLogic2UI & objData, bool bFocus);
    //设置会议类型项
    void SetConferenceTab(CTalkSessionTab & objItem, const DataLogic2UI & objData, bool bFocus);
    //设置ParkNotify类型
    void SetParkNotifyTab(CTalkSessionTab & objItem, const DataLogic2UI & objData, bool bFocus);

private:
    QString GetTabTip(const QString & strStatus, const QString & strName, const QString & strNumber);

    QString GetTabTip(const QString & strStatus, CCallInfo * pCallInfo);

    void AdjustItemLayout();

protected slots:
    //某个Item被选中
    void OnTalkManagerItemClicked(CTalkSessionTab * pItem);

    void DelayFocusConferenceUI();

signals:
    //通话选项被点击事件，带SessionId
    void talkItemClicked(int nSessionId);

    // 焦点UI位置改变
    void FocusUIGeometryChange(bool bShow, const QRect & geometry);

    //切页信号
    void SigNotifyScroll();

private:
    // 通话内容选项
    CTalkSessionTab     m_arrayTalkTab[MAX_TALKMANAGER_SHOW_COUNT];

    QFrame              m_frameTalkUIBG;

    // 起始索引
    int                 m_nCurrentPage;
    // 滚动条控件
    CPageCtrl     *     m_pScrollBar;
    // 保存页面
    std::vector<int>    m_vecPageInfo;

    //是否来电焦点有显示
    bool                m_bIsIncomingCallSessionShow;
};

#endif // _TALK_SESSION_TAB_
