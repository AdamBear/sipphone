#ifndef __AUTO_TEST_MGR_H__
#define __AUTO_TEST_MGR_H__

#include <xwindow/xwindowdefine.h>
#include "xWindow.h"


#ifdef IF_XWINDOW_AUTO_TEST
class CAutoTestMgr
{
public:
    CAutoTestMgr();
    ~CAutoTestMgr();

    static CAutoTestMgr& GetInstance();

    static LRESULT OnTestabilityChanged(msgObject& msg);
    static LRESULT OnExportAllView(msgObject& msg);
    static LRESULT OnSendKey(msgObject& msg);
    static LRESULT OnScreenCapture(msgObject& msg);
    static LRESULT OnInputText(msgObject& msg);

public:
    void Init();

    int ExportAllView(char* pBuffer, int nSize);
    bool SendKey(int nKeyCode, bool bPress);
    bool ScreenCapture(const char* pszFile);
    bool InputText(const char* pszText);

protected:
    void LoadTestability();
    yl::string GetCmdResult(const yl::string& strCmd);

protected:
    void* m_pLibHandle;
};

#define g_AutoTestMgr CAutoTestMgr::GetInstance()
#endif

#endif
