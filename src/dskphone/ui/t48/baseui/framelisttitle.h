#ifndef __FRAME_LIST_TITLE_H__
#define __FRAME_LIST_TITLE_H__

#include <QWidget>
#include "baseui/iframetitleaction.h"
#include <ylstring.h>

class CFrameList;
class QStylePainter;

// 标题栏类型
// 因为嵌入式设备不建议用dynamic_cast，所以加上类型进行标识
enum
{
    FRAME_LIST_TITLE_TYPE_NONE          = 0,
};

class CFrameListTitle : public QWidget
{
    Q_OBJECT

public:
    explicit CFrameListTitle(int nType, CFrameList * pFrameList = NULL,
                             IFrameTitleAction * pAction = NULL);
    virtual ~CFrameListTitle();

public:
    // 设置标题栏Action
    void SetAction(IFrameTitleAction * pAction);
    // 是否显示背景
    void SetShowBG(bool bShow);
    // 设置背景颜色
    void SetBgColor(const QColor & color);
    // 设置背景图
    void SetBgPic(const yl::string & strBgPic);

    int GetType() const;

    void RefreshTitle();

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

    void PaintBG(QStylePainter & stylePainter);

    // 通知处理用户操作Action
    void DoAction(int nAction);

    // 响应改变大小事件
    virtual void resizeEvent(QResizeEvent * pEvent);
    // 重新布局
    virtual void Relayout();

protected:
    int m_nType;

private:
    IFrameTitleAction * m_pAction;
    bool m_bShowBg;
    QColor m_clrBg;
    yl::string m_strBgPic;
};
typedef CFrameListTitle * CFrameListTitlePtr;

#endif // __FRAME_LIST_TITLE_H__
