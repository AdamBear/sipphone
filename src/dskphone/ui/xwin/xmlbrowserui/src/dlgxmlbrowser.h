#ifndef _DLG_XML_BROWSER_H_
#define _DLG_XML_BROWSER_H_

#include "uimanager/basedialog.h"
#include "extended/xmlbrowser/include/logicanduicommon.h"

class xEditTextView;
class xTextView;
class xXMLBrowserUIFrame;

#if XML_BROWSER_SUPPORT_UI

class CDlgXMLBrowser : public xFrameLayout, public CBaseDialog
{
public:
    static CBaseDialog * CreateInstance();

public:
    CDlgXMLBrowser();
    virtual ~CDlgXMLBrowser();
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();
    virtual void OnFocusChanged(bool bFocused);

public:
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    virtual void loadChildrenElements(xml_node & node);
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);
    // 是否显示softkey
    virtual bool IsShowSoftkey();

    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, yl::string & strPix);
    //音量条显示改变
    void OnVolumeVisibleChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                LPVOID pData);

    void InitData();

    // 刷新界面
    void RefreshUI();

    // 更新界面数据
    void UpdateUI(const XMLLogicToUIData & objData);

    // 更新softkey数据
    void UpdateSoftkey(VecSoftkeyParam & refSoftkeyData);

    // 更新softkey数据
    void UpdateSoftkey();

    void OnXBFrameQuit();

    // 处理RegionAction
    void OnChangeSoftkey(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

    // 设置小窗口数据
    void InitDialogData(const XMLLogicToUIData & objData);
    void ExitXmlUI(int nElementID);

protected:
    virtual bool OnSoftkeyAction(int iType, bool bLongPress);

private:
    xXMLBrowserUIFrame * m_pXBUIFrame;
    bool                m_bFullScreen;
};
#endif

#endif //_DLG_XML_BROWSER_H_
