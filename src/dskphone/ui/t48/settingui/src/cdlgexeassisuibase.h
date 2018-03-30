#ifndef _DLG_DEXCUTIVE_ASSISTANT_BASE_H_
#define _DLG_DEXCUTIVE_ASSISTANT_BASE_H_

#include <features-define.h>
#include <config.h>

#include "setting/include/common.h"
#include "settingui/src/cdlgbasesubpage.h"
#include "uikernel/qcheckbox.h"
#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"
#include "cexecutiveassistantluilogic.h"
#include "setting/src/commondefine.h"
#include "actiondefinecommon.h"
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include "uikernel/qlineeditutility.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

#define NOT_EMPTY(pData) (pData != 0)


class CDlgExeAssisUIBase
    : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgExeAssisUIBase(QWidget * parent = 0);
    virtual ~CDlgExeAssisUIBase();

public:

    virtual ExeAssis_Action GetAction()
    {
        return EAA_None;
    }

    virtual bool RefreshData()
    {
        return true;
    };

    virtual CExeAssisInspector * GetPageData()
    {
        return NULL;
    }

    virtual bool IsStatusChanged()
    {
        return false;
    }

    virtual bool SavePageData();

    virtual bool LoadPageData()
    {
        return true;
    }

    // 设置子页面不可编辑时的SoftKey
    virtual bool GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData);

    virtual bool OnKeyPress(int nKeyCode, bool bPress)
    {
        return false;
    }

    virtual void BeforeSetSubPage(void * pData);

    virtual void AfterSetSubPage(void * pData);

public:
    bool IsInLoading()
    {
        return m_bIsInLoad;
    }
    void SetLoading(bool bLoading)
    {
        m_bIsInLoad = bLoading;
    }

    void QuitPage()
    {
        emit BtnBackClick();
    }

    static int GetLineId()
    {
        return g_refExeAssisUILogic.GetLineId();
    }
    // 身份区分
    static bool IsExecutive()
    {
        return g_refExeAssisUILogic.IsExecutive();
    }
    static bool IsAssistant()
    {
        return g_refExeAssisUILogic.IsAssistant();
    }

public Q_SLOTS:
    void OnShow();

protected:
    virtual void showEvent(QShowEvent * e);

public:
    ExeUI_TYPE     m_eUIType;

private:

    //表示是加载还是保存
    bool           m_bIsInLoad;
};
#endif //#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

#endif // _DLG_DEXCUTIVE_ASSISTANT_BASE_H_
