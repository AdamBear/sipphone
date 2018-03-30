#ifndef _DLG_DEXCUTIVE_ASSISTANT_COMMON_H_
#define _DLG_DEXCUTIVE_ASSISTANT_COMMON_H_

#include <features-define.h>
#include <config.h>

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include "cdlgexeassisuibase.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

class CDlgExeAssisCommon
    : public CDlgExeAssisUIBase
{
    Q_OBJECT

public:
    explicit CDlgExeAssisCommon(QWidget * parent = 0);
    virtual ~CDlgExeAssisCommon();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgExeAssisCommon();
    }

public:

    virtual CExeAssisInspector * GetPageData();

    virtual bool RefreshData();

    virtual ExeAssis_Action GetAction();

    virtual bool IsStatusChanged();

    virtual void InitData();

    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

public slots:
    void OnComboxValueChanged(int iIndex);

private:
    QComboBox   *   m_pComboBox;
    QLineEdit   *   m_pLineEdit;

    CExeCommon   *  m_pExeDataInfo;
    CAssCommon   *  m_pAssDataInfo;
};
#endif //#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

#endif
