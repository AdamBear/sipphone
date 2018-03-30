#ifndef _DLG_MANUAL_SETTING_H_
#define _DLG_MANUAL_SETTING_H_

#include "dlgsettingui.h"

class xEditTextView;
class xTextView;

class CDlgManualSetting : public CDlgSettingUI
{
public:
    static CBaseDialog * CreateInstance();

public:
    CDlgManualSetting();
    virtual ~CDlgManualSetting();
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

public:
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    virtual void loadChildrenElements(xml_node & node);
    virtual void RefreshUI(bool bReLoadData = false);
    virtual bool LoadPageData(bool bReLoadData = false);
    virtual bool onKeyPressedEvent(int key);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress);
    virtual bool IsRejectReturnToIdle();

    void SetupEditTextChangeSlotByID(const yl::string strName);
    void RemoveEditTextChangeSlotByID(const yl::string strName);
protected:
    yl::string IntTimeToStr(int nTime);
    void OnDirectionKeyPress(int nKey);
    void OnSavePress();

    void SetEditValueById(const yl::string & strID, int nValue);
    yl::string GetEditValueById(const yl::string & strID);

    void SetCommonSoftkey();
#if IF_SUPPORT_LEFT_RIGHT_SOFTKEY
    void SetLeftRightSoftKey();
    void OnSoftKeyPress(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
#endif
    void RectifyFocusEditValue(bool bEditFinish = false);
    void RectifyDayEditValue();
    void slotOnTextChange(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

protected:
    xEditTextView * m_pFocusView;
    bool m_bEditAble;
};

#endif //_DLG_MANUAL_SETTING_H_
