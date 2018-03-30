#ifndef CDLGCUSTOMFORWARDOPTION_H
#define CDLGCUSTOMFORWARDOPTION_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>
#include "uikernel/qcomboboxlabel.h"
#include "config.h"
#include "setting/src/callforwardcontroller.h"
#include "cforworduilogic.h"

#ifdef IF_FEATURE_FORWARD_OPT
#define QFwdWidget qComboBoxLabel
#else
#define QFwdWidget QLabel
#endif

class CDlgCustomForwardOption
    : public CDlgBaseFwdPageUI
{
    Q_OBJECT

public:
    explicit CDlgCustomForwardOption(QWidget * parent = 0);
    ~CDlgCustomForwardOption();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgCustomForwardOption();
    }

public:
    //加载数据
    virtual bool LoadPageData();

    //获取数据状态是否改变
    virtual bool IsStatusChanged();

    //保存数据
    virtual bool SavePageData();

    //界面初始化
    void InitPageData();

    virtual bool OnAction(const QString & strAction);

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData);

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    bool GetPageData(ForwardData * pData, FWD_TYPE eFwdType);
protected:
    virtual void showEvent(QShowEvent * e);

private:
    bool InitFwdData(QFwdWidget * pWidget, FWD_TYPE eType);

    bool GetPageDataByType(FWD_TYPE eType);

    bool GetLogicDataByType(FWD_TYPE eType);

    bool IsAllLineMode()
    {
        return GetLineID() == GLOAD_ID;
    }

private:
    QFwdWidget * m_pLabAlwaysStatus;
    QFwdWidget * m_pLabBusyStatus;
    QFwdWidget * m_pLabNoAnswerStatus;

    ForwardData m_fwdPageData;
    ForwardData m_fwdLogicData;
};

#endif // CDLGCUSTOMFORWARDOPTION_H
