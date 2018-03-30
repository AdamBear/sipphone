
#ifndef __BTN_BAR_WRAPPER_H__
#define __BTN_BAR_WRAPPER_H__

#include "buttonwrapper.h"

class CBtnBarWrapper
#ifdef IF_ENABLE_TESTABILITY
    : public QObject
#endif
{
#ifdef IF_ENABLE_TESTABILITY
    Q_OBJECT
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    QString GetTestInfo();

#endif
public:
    explicit CBtnBarWrapper(QWidget * pWidget, bool bAutoDelBtn);
    virtual ~CBtnBarWrapper(void);

    void SetBtnCount(int nCount);
    bool SetBtn(int nIndex, CButtonWrapper * pBtn);

    int GetBtnCount() const;
    CButtonWrapper * GetBtn(int nIndex) const;
    CButtonWrapper * GetDownBtn() const;
    int GetDownBtnIndex() const;

    CButtonWrapper * ProcessMousePress(QMouseEvent * pEvent);
    CButtonWrapper * ProcessMouseRelease(QMouseEvent * pEvent);
    CButtonWrapper * ProcessKeyPress(QKeyEvent * pEvent);
    CButtonWrapper * ProcessKeyRelease(QKeyEvent * pEvent);

protected:
    void ResetBtnArray();

    void ResetAllBtnDown(CButtonWrapper * pExcludeBtn);

private:
    QWidget * m_pWidget;
    CButtonWrapper ** m_pArrBtn;
    int m_nBtnCount;
    bool m_bAutoDelBtn;
};

#endif
