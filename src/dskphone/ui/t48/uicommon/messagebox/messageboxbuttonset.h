#ifndef MESSAGEBOXBUTTONSET_H
#define MESSAGEBOXBUTTONSET_H

#include <QtGui>
#include "uikernel/quipushbutton.h"
#include <ylstring.h>

class CMessageBoxButtonSet : public QWidget
{
    Q_OBJECT
public:
    CMessageBoxButtonSet(QWidget * parent);
    ~CMessageBoxButtonSet();
    void AddButton(qUIPushButton * pBtn);
    void SetMaxBtnCount(int iCount)
    {
        m_iMaxBtnCount = iCount;   //设置最大按钮数
    }
    int GetMaxBtnCount()
    {
        return m_iMaxBtnCount;   //获取最大按钮数
    }
    void SetBackGround(yl::string strBackground);
    void DetachAllBtn();
protected:
    bool eventFilter(QObject * pObj, QEvent * pEvent);
    void paintEvent(QPaintEvent * pEvent);
private:
    int m_iMaxBtnCount;
    int m_iCurrentBtnCount;
    yl::string m_strBackground;
    QRect m_rect;
    std::vector<qUIPushButton *> m_btnVec;
};

#endif // MESSAGEBOXBUTTONSET_H
