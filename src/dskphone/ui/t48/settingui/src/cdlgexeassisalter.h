#ifndef _DLG_DEXCUTIVE_ASSISTANT_ALTER_H_
#define _DLG_DEXCUTIVE_ASSISTANT_ALTER_H_

#include <features-define.h>
#include <config.h>

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include "cdlgexeassisuibase.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

class CDlgExeAssisAlter
    : public CDlgExeAssisUIBase
{
    Q_OBJECT

public:
    explicit CDlgExeAssisAlter(QWidget * parent = 0);
    virtual ~CDlgExeAssisAlter() {}

    static CBaseDialog * CreateInstance()
    {
        return new CDlgExeAssisAlter();
    }

    virtual void Init()
    {
        m_bFirstEnter = true;
    }

public:

    virtual bool RefreshData();

    virtual ExeAssis_Action GetAction();

    virtual bool IsStatusChanged();

    virtual void InitData();

    virtual CExeAssisInspector * GetPageData();

    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

private:
    void AddOptions(bool bAddRolloverFwd);

public slots:
    void OnComboxCallIdNameValueChanged(int iIndex);
    void OnComboxCallIdNumberValueChanged(int iIndex);
    void OnComboxRolloverTypeValueChanged(int iIndex);
    void OnComboxAlterModeValueChanged(int iIndex);

private:
    QComboBox   *   m_pCobAlterMode;
    QComboBox   *   m_pCobAdvToNextTimer;
    QComboBox   *   m_pCobCallIdName;
    QLineEdit   *   m_pEditCallIdName;
    QComboBox   *   m_pCobCallIdNumber;
    QLineEdit   *   m_pEditCallIdNumber;
    QComboBox   *   m_pCobRecallTimer;
    QLineEdit   *   m_pEditRolloverTimer;
    QComboBox   *   m_pCobRolloverAction;
    QLineEdit   *   m_pEditRolloverFwd;

    CExeAlterInfo   m_dataInfo;

    bool                    m_bFirstEnter;
};
#endif //#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

#endif
