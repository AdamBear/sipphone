#ifndef _DLG_DEXCUTIVE_ASSISTANT_DETAILS_H_
#define _DLG_DEXCUTIVE_ASSISTANT_DETAILS_H_

#include <features-define.h>
#include <config.h>

#include <QtGui/QLabel>
#include "cdlgexeassisuibase.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

class CDlgExeAssisDetails
    : public CDlgExeAssisUIBase
{
    Q_OBJECT

public:
    explicit CDlgExeAssisDetails(QWidget * parent = 0);
    virtual ~CDlgExeAssisDetails() {}

    static CBaseDialog * CreateInstance()
    {
        return new CDlgExeAssisDetails();
    }

public:

    virtual void InitData() {}

    virtual bool OnAction(const QString & strAction);

    virtual bool LoadPageData();

    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

private:
    // 添加项
    void AddItem(const QString & strDisplayText, QWidget * pWidget, const QString & strAction);

    yl::string GetClickActionByItemAction(const QString & strItemAction);
};
#endif //#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

#endif
