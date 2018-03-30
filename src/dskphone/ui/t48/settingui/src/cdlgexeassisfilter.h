#ifndef _DLG_DEXCUTIVE_ASSISTANT_FILTER_H_
#define _DLG_DEXCUTIVE_ASSISTANT_FILTER_H_

#include <features-define.h>
#include <config.h>

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include "cdlgexeassisuibase.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

class CDlgExeAssisFilter
    : public CDlgExeAssisUIBase
{
    Q_OBJECT

public:
    explicit CDlgExeAssisFilter(QWidget * parent = 0);
    virtual ~CDlgExeAssisFilter() {}

    static CBaseDialog * CreateInstance()
    {
        return new CDlgExeAssisFilter();
    }

public:

    virtual bool RefreshData();

    virtual ExeAssis_Action GetAction();

    virtual bool IsStatusChanged();

    virtual void InitData();

    virtual CExeAssisInspector * GetPageData();

public slots:
    void OnComboxValueChanged(int iIndex);

private:
    QComboBox   *   m_pCobFilterSwitch;
    QComboBox   *   m_pCobFilterMode;
    QComboBox   *   m_pCobFilterType;

    CExeFilterInfo  m_dataInfo;
};
#endif // #ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

#endif
