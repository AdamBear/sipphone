#ifndef _DLG_DEXCUTIVE_ASSISTANT_LISTVIEW_H_
#define _DLG_DEXCUTIVE_ASSISTANT_LISTVIEW_H_

#include <features-define.h>
#include <config.h>

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include "cdlgexeassisuibase.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

class CDlgExeAssisListView
    : public CDlgExeAssisUIBase
{
    Q_OBJECT

public:
    explicit CDlgExeAssisListView(QWidget * parent = 0);
    virtual ~CDlgExeAssisListView();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgExeAssisListView();
    }

public:

    virtual bool RefreshData();

    virtual ExeAssis_Action GetAction();

    virtual bool IsStatusChanged();

    virtual void InitData();

    virtual CExeAssisInspector * GetPageData();

    virtual bool OnAction(const QString & strAction);

    virtual bool OnKeyPress(int nKeyCode, bool bPress);

    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

private:
    QString GetFocusItemName();

    bool CallOut();

private:
    CExeCommon   *  m_pExeDataInfo;
    CAssCommon   *  m_pAssDataInfo;
};
#endif //#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

#endif
