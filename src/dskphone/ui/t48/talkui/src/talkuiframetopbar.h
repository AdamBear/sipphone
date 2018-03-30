#ifndef _TALK_UI_FRAME_TOPBAR_H
#define _TALK_UI_FRAME_TOPBAR_H

#include <QFrame>
#include <ylstl/ylstring.h>

// 前向声明
class qWidgetList;

class qFrameStatusImage : public QFrame
{
    Q_OBJECT

public:
    qFrameStatusImage(QWidget * parent = 0);
    ~qFrameStatusImage();

public:
    //清除所有图标
    void ClearAllIcon();

    // 添加或者删除图标
    void AddOrDeleteIcon(const yl::string & strIconPath, bool bAdd);

private:
    //改变大小事件
    virtual void resizeEvent(QResizeEvent * pResizeEvent);


private:
    // 界面图标到通道图标的映射
    yl::string UiPictureToChannel(const yl::string & strIconPath);

	QSize GetIconSize(const yl::string & strIconPath);
private:
    // widgetlist
    qWidgetList  *  m_pWidgetList;
};


#endif
