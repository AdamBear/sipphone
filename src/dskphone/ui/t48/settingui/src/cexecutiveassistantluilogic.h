#ifndef _DLG_DEXCUTIVE_ASSISTANT_UILOGIC_H_
#define _DLG_DEXCUTIVE_ASSISTANT_UILOGIC_H_

#include <features-define.h>
#include <config.h>

#include <ylstring.h>
#include <vector>
#include <QtCore/QString>
#include <QtCore/QObject>
#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"
#include "messagebox/messageboxcallback.h"
#include "setting/include/common.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

// UI类型
enum ExeUI_TYPE
{
    UI_NONE = 0,
    UI_SelectLine,  //账号选择界面
    UI_Details,
    UI_ListView,
    UI_Filter,
    UI_Screen,
    UI_Alter,
    UI_Common,
};

class CDlgExeAssisUIBase;
class CExecutiveAssistantUILogic  : public QObject, public CMsgBoxCallBackAction
{
    Q_OBJECT

public:
    static CExecutiveAssistantUILogic & GetInstance() DECLARE_ATTR;

public:
    void SetLineId(int iLineId);

    void SetUserType(EEA_TYPE eType)
    {
        m_eUserType = eType;
    }

    void SetUIType(ExeUI_TYPE eType)
    {
        m_eUIType = eType;
    }

    int GetLineId()
    {
        return m_iLineId;
    }
    EEA_TYPE GetUserType()
    {
        return m_eUserType;
    }

    // 身份区分
    bool IsExecutive()
    {
        return m_eUserType & EET_Executive;
    }
    bool IsAssistant()
    {
        return m_eUserType & EET_Assistant;
    }

public:

    bool SaveToLogic(CDlgExeAssisUIBase * pPageUi);

    bool OnReceiveMsg(msgObject & objMessage);

    bool GetLogicData(CExeAssisInspector * pInfo);

    void OnUIShow(CDlgExeAssisUIBase * pPageUi);

    bool ProcessMsgBoxPassword(int nLineId);

    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    bool ResponseActionError(int iLineId, int errCode, bool bInLoading, int iIndex);

    bool OnKeyPress(int nKeyCode, bool bPress);

public Q_SLOTS:
    // 密码框提交响应函数
    void OnSubmitXSIPassword(const yl::string & strPswd);

    // 密码框取消响应函数
    void OnCancelInputXSIPassword();

    // 密码框取消响应函数
    void OnRefreshAfterSubmitXSI();
private:
    CExecutiveAssistantUILogic();
    virtual ~CExecutiveAssistantUILogic();

private:
    CDlgExeAssisUIBase * GetUI();

private:
    int              m_iLineId;
    EEA_TYPE         m_eUserType;
    ExeUI_TYPE       m_eUIType;
    bool             m_bQuitPage;
};

#define g_refExeAssisUILogic (CExecutiveAssistantUILogic::GetInstance())

#endif //#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

#endif // _DLG_DEXCUTIVE_ASSISTANT_UILOGIC_H_
