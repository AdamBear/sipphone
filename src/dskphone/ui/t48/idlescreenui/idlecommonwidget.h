#ifndef _IDLE_COMMON_WIDGET_H_
#define _IDLE_COMMON_WIDGET_H_

#include <QtGui>

// NoteLabel类
class CNoteLabel : public QLabel
{
    Q_OBJECT

public:
    CNoteLabel(QWidget * parent = NULL);

    ~CNoteLabel();

    //设置小图标
    void SetLabelIcon(const QString & strIcon);

    //设置文字
    void SetLabelText(const QString & strText);

    QString GetLabelText()
    {
        return m_strText;
    }

private:
    // 控件初始化
    void Init();

    void paintEvent(QPaintEvent * paintEvent);

private:
    QString m_strIcon;

    QString m_strText;
};
#endif
