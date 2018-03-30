#ifndef _SETTINGUI_EXECUTIVE_AND_ASSISTANT_DELEGATE_H_
#define _SETTINGUI_EXECUTIVE_AND_ASSISTANT_DELEGATE_H_

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

enum PAGE_UI_TYPE
{
    PAGE_UI_TYPE_NONE = 0,
    PAGE_UI_TYPE_DETAIL,
    PAGEUI_TYPE_COMMON,
    PAGEUI_TYPE_FILTER,
    PAGEUI_TYPE_SCREEN,
    PAGEUI_TYPE_ALERT,
    PAGEUI_TYPE_LISTVIEW
};

#include "settinguixsidelegatebase.h"
#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"

class CSettinguiExecutiveAndAssistantDelegate : public CSettinguiXSIDelegatebase
{
public:
    CSettinguiExecutiveAndAssistantDelegate();
    virtual ~CSettinguiExecutiveAndAssistantDelegate();

    static CSettingUIDelegateBase * CreateExecutiveAndAssistantDelegate();

public:
    static LRESULT OnMessage(msgObject & msg);
    virtual void OnMessageReqFinish(msgObject & msg);
    virtual void OnMessageReqFail(msgObject & msg);
    void OnMessageFinishAll(msgObject & msg);

    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

public:
    virtual void InitExtraData(void * pData);
    virtual void OnEnterPage(const yl::string & strPage, int iDataIndex);

    virtual bool IsHasAccountRegister();

    //初始化界面相关数据
    void InitPageTypeData();

    void AddExecutiveListviewItem(const yl::string & strItemID, const yl::string & strDisplayName,
                                  const yl::string & strValue);
    void AddAssistantListviewItem(const yl::string & strItemID, const yl::string & strDisplayName,
                                  const yl::string & strValue);

    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);

    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
    virtual bool OnAction(int eActionType, int iDataIndex);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual void OnComboBoxValueChange(int nIndex);
    virtual void OnCancleClick();
    virtual bool IsResetFocusWhileReload();
protected:
    void AddComboBoxOption(const yl::string & strItemID);
    // UI数据类型映射到逻辑层类型
    ExeAssis_Action MapUIType2ActionType();

    // XSI结果返回之后，依据返回结果刷新页面
    bool UpdateUIData(bool bRefresh = true);

    // 合法性校验函数集合
    bool CheckValid();
    bool CheckDigit(const yl::string & strItemName, int minValue, int maxValue);
    // 校验时，需要根据pItemDependName控件指定的iDependValue值来确定
    bool CheckEmpty(const yl::string & strItemName, const yl::string & strItemDependName = "",
                    int iDependValue = 0);

    // 动态配置控件，根据已有配置，动态调整控件的隐藏和只读属性
    bool ConfigItem(bool bCheckFocus = true);
    // 依pItemDependName项的iDependValue值，决定是否隐藏或者只读（bHide）pItemName对应的控件
    bool ConfigItem(const char * pItemName, const char * pItemDependName, int iDependValue = 0,
                    bool bHide = true, bool bCheckFocus = true);

    // 呼出
    bool CallOut(int nFocusIndex = -1);
    bool OnCallInitiationPress();

    void CorrectData(CExeAlterInfo & dataInfo);

protected:
    bool IsExecutive()
    {
        return m_eUserType & EET_Executive;
    }
    bool IsAssistant()
    {
        return m_eUserType & EET_Assistant;
    }
private:
    EEA_TYPE        m_eUserType;
    PAGE_UI_TYPE    m_eUIType;
    bool            m_bQuitPage;
};

#endif //IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
#endif //_SETTINGUI_EXECUTIVE_AND_ASSISTANT_DELEGATE_H_
