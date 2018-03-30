#ifndef _DLG_DEXCUTIVE_ASSISTANT_SCREEN_H_
#define _DLG_DEXCUTIVE_ASSISTANT_SCREEN_H_

#include <features-define.h>
#include <config.h>

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include "cdlgexeassisuibase.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

class CDlgExeAssisScreen
    : public CDlgExeAssisUIBase
{
    Q_OBJECT

public:
    explicit CDlgExeAssisScreen(QWidget * parent = 0);
    virtual ~CDlgExeAssisScreen() {}

    static CBaseDialog * CreateInstance()
    {
        return new CDlgExeAssisScreen();
    }

public:

    virtual bool RefreshData();

    virtual ExeAssis_Action GetAction();

    virtual bool IsStatusChanged();

    virtual void InitData();

    virtual CExeAssisInspector * GetPageData();

private:
    QComboBox   *   m_pCobScreenSwitch;
    QComboBox   *   m_pCobAlterType;
    QComboBox   *   m_pCobMobility;
    QComboBox   *   m_pCobAnywhere;
    QComboBox   *   m_pCobSCA;

    CExeScreenInfo  m_dataInfo;
};
#endif //#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

#endif
