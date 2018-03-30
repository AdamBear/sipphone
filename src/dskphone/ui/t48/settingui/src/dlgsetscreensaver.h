#ifndef __DLG_SET_SCREENSAVERS_H__
#define  __DLG_SET_SCREENSAVERS_H__

#include <QDialog>
#include "cdlgbasesubpage.h"
#include "setting/include/common.h"
#include "uikernel/qcheckbox.h"


class CDlgSetScreenSaver : public CDlgBaseSubPage
{
public:
    CDlgSetScreenSaver(QWidget * pParent = NULL);
    virtual ~CDlgSetScreenSaver();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgSetScreenSaver();
    }

public:
    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData();

    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

private:
    void AdjustWaitTime(int & nTimes);

    void InitData();
    void InitListItemData();

    void FillWaitTimeCmb();

    bool IsInitSucceed() const;
    bool IsHasCumtomPicture();

    int GetWaitTime();
    int GetShowClockType();
    int GetScreensaverType();

private:
    QComboBox * m_pCmbWaitTime;
    qCheckBox * m_pChBoxShowSysClock;
    QComboBox * m_pCmbSaverType;

    bool       m_bInitSucceed;
};

#endif // #ifndef __DLG_SET_SCREENSAVERS_H__
