#include "autotestmgr.h"

#if IF_XWINDOW_AUTO_TEST
#include <dlfcn.h>
#include <cstdio>
#include "service_config.h"
#include "innermsgdefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "modautotest.h"
#include "xWindowManager.h"


namespace
{
#define USER_ENV_GUI_TESTABILITY "GuiTestability"

//配置改变ID，0xff000000 + 服务端口号
static const int ST_TESTABILITY_CHANGED = 0x7f000000 + 65381;

enum
{
    //TODO #warning messageid
    AUTO_TEST_MSG_EXPORT_ALL_VIEW = INNER_MSG_AUTO_TEST_MESSAGE_BEGIN,
    AUTO_TEST_MSG_SEND_KEY,
    AUTO_TEST_MSG_SCREEN_CAPTURE,
    AUTO_TEST_MSG_INPUT_TEXT,
    AUTO_TEST_MSG_END = INNER_MSG_AUTO_TEST_MESSAGE_END,
};

typedef bool (*FUN_SET_AUTO_TEST_CALL_BACK)(const char* pszSymbol, void* pFunction);
typedef void (*FUN_SO_MAIN_TYPE)(void);
}

CAutoTestMgr::CAutoTestMgr()
    : m_pLibHandle(NULL)
{
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, &CAutoTestMgr::OnTestabilityChanged);
    etl_RegisterMsgHandle(AUTO_TEST_MSG_EXPORT_ALL_VIEW, AUTO_TEST_MSG_EXPORT_ALL_VIEW, &CAutoTestMgr::OnExportAllView);
    etl_RegisterMsgHandle(AUTO_TEST_MSG_SEND_KEY, AUTO_TEST_MSG_SEND_KEY, &CAutoTestMgr::OnSendKey);
    etl_RegisterMsgHandle(AUTO_TEST_MSG_SCREEN_CAPTURE, AUTO_TEST_MSG_SCREEN_CAPTURE, &CAutoTestMgr::OnScreenCapture);
    etl_RegisterMsgHandle(AUTO_TEST_MSG_INPUT_TEXT, AUTO_TEST_MSG_INPUT_TEXT, &CAutoTestMgr::OnInputText);
}

CAutoTestMgr::~CAutoTestMgr()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, &CAutoTestMgr::OnTestabilityChanged);
    etl_UnregisterMsgHandle(AUTO_TEST_MSG_EXPORT_ALL_VIEW, AUTO_TEST_MSG_EXPORT_ALL_VIEW, &CAutoTestMgr::OnExportAllView);
    etl_UnregisterMsgHandle(AUTO_TEST_MSG_SEND_KEY, AUTO_TEST_MSG_SEND_KEY, &CAutoTestMgr::OnSendKey);
    etl_UnregisterMsgHandle(AUTO_TEST_MSG_SCREEN_CAPTURE, AUTO_TEST_MSG_SCREEN_CAPTURE, &CAutoTestMgr::OnScreenCapture);
    etl_UnregisterMsgHandle(AUTO_TEST_MSG_INPUT_TEXT, AUTO_TEST_MSG_INPUT_TEXT, &CAutoTestMgr::OnInputText);
}

CAutoTestMgr& CAutoTestMgr::GetInstance()
{
    static CAutoTestMgr* s_pAutoTestMgr = NULL;

    if (NULL == s_pAutoTestMgr)
    {
        s_pAutoTestMgr = new CAutoTestMgr;
    }

    return *s_pAutoTestMgr;
}

void CAutoTestMgr::Init()
{
    LoadTestability();
}

LRESULT CAutoTestMgr::OnTestabilityChanged(msgObject& msg)
{
    if (ST_TESTABILITY_CHANGED == msg.wParam)
    {
        g_AutoTestMgr.LoadTestability();
        return TRUE;
    }

    return FALSE;
}

void CAutoTestMgr::LoadTestability()
{
    if (NULL != m_pLibHandle)
    {
        dlclose(m_pLibHandle);
        m_pLibHandle = NULL;
    }

    if (USER_ENV_GUI_TESTABILITY "=1" != GetCmdResult("/boot/bin/userenv -g " USER_ENV_GUI_TESTABILITY))
    {
        return;
    }

    m_pLibHandle = dlopen("libtestability.so", RTLD_LAZY);

    if (NULL == m_pLibHandle)
    {
        XWINDOW_ERR("Load Testablity libraries failed %s.", strerror(errno));

        return;
    }

    FUN_SET_AUTO_TEST_CALL_BACK pFunSetAutoTestCallBack = (FUN_SET_AUTO_TEST_CALL_BACK)dlsym(m_pLibHandle, "SetAutoTestCallback");

    if (NULL == pFunSetAutoTestCallBack)
    {
        XWINDOW_ERR("Load Testablity SetAutoTestCallback failed %s.", strerror(errno));

        return;
    }

    (*pFunSetAutoTestCallBack)("AutoTest_DumpAllView", (void*)&AutoTest_DumpAllView);
    (*pFunSetAutoTestCallBack)("AutoTest_ScreenCapture", (void*)&AutoTest_ScreenCapture);
    (*pFunSetAutoTestCallBack)("AutoTest_SendKey", (void*)&AutoTest_SendKey);
    (*pFunSetAutoTestCallBack)("AutoTest_InputText", (void*)&AutoTest_InputText);

    FUN_SO_MAIN_TYPE pFunSoMain = (FUN_SO_MAIN_TYPE)dlsym(m_pLibHandle, "somain");

    if (NULL == pFunSoMain)
    {
        XWINDOW_ERR("Load Testablity somain failed %s.", strerror(errno));

        return;
    }

    pFunSoMain();
}

yl::string CAutoTestMgr::GetCmdResult(const yl::string& strCmd)
{
    FILE* pFile = popen(strCmd.c_str(), "r");
    yl::string strOutPut;

    if (NULL != pFile)
    {
        char szBuffer[256] = { 0 };
        int nReadSize = fread(szBuffer, 1, 256, pFile);

        while (nReadSize > 0)
        {
            strOutPut.append(szBuffer, nReadSize);

            nReadSize = fread(szBuffer, 1, 256, pFile);
        }

        pclose(pFile);
    }

    yl::string::size_type lastChar = strOutPut.find_last_not_of("\r\n");

    if (lastChar != yl::string::npos)
    {
        return strOutPut.substr(0, lastChar + 1);
    }

    return strOutPut;
}

LRESULT CAutoTestMgr::OnExportAllView(msgObject& msg)
{
    xml_document* pDoc = (xml_document*)msg.wParam;

    if (NULL == pDoc)
    {
        msg.ReplyMessage(FALSE);

        return TRUE;
    }

    if (g_WorkStation.DumpAllVisibleWindow(*pDoc))
    {
        msg.ReplyMessage(TRUE);
    }
    else
    {
        msg.ReplyMessage(FALSE);
    }

    return TRUE;
}

LRESULT CAutoTestMgr::OnSendKey(msgObject& msg)
{
    int nKeyCode = msg.wParam;
    bool bPress = (1 == msg.lParam);

    g_WorkStation.dispatchKeypadEvent(nKeyCode, bPress);

    msg.ReplyMessage(TRUE);

    return TRUE;
}

LRESULT CAutoTestMgr::OnScreenCapture(msgObject& msg)
{
    xPixmap* pPixmap = (xPixmap*)msg.wParam;

    if (NULL == pPixmap)
    {
        msg.ReplyMessage(FALSE);

        return TRUE;
    }

    if (g_WorkStation.ScreenCapture(*pPixmap))
    {
        msg.ReplyMessage(TRUE);
    }
    else
    {
        msg.ReplyMessage(FALSE);
    }

    return TRUE;
}

LRESULT CAutoTestMgr::OnInputText(msgObject& msg)
{
    yl::string* pText = (yl::string*)msg.wParam;

    if (NULL == pText)
    {
        msg.ReplyMessage(FALSE);

        return TRUE;
    }

    if (g_WorkStation.InputText(*pText))
    {
        msg.ReplyMessage(TRUE);
    }
    else
    {
        msg.ReplyMessage(FALSE);
    }

    return TRUE;
}

int CAutoTestMgr::ExportAllView(char* pBuffer, int nSize)
{
    if (NULL == pBuffer || nSize <= 0)
    {
        return DUMP_VIEW_RESULT_PARAM_ERROR;
    }

    xml_writer_buffer objWriter(pBuffer, nSize);
    xml_document doc;

    if (!etl_NotifyApp(true, AUTO_TEST_MSG_EXPORT_ALL_VIEW, (WPARAM)&doc, 0))
    {
        return DUMP_VIEW_RESULT_EXE_ERROR;
    }

    if (doc.empty())
    {
        return DUMP_VIEW_RESULT_EMPTY;
    }

    doc.save(objWriter);

    size_t xmlLen = objWriter.xmllen();
    size_t offset = objWriter.offset();

    if (xmlLen > offset)
    {
        return DUMP_VIEW_RESULT_LACK_BUFFER;
    }

    return (int)xmlLen;
}

bool CAutoTestMgr::SendKey(int nKeyCode, bool bPress)
{
    if (etl_NotifyApp(true, AUTO_TEST_MSG_SEND_KEY, (WPARAM)nKeyCode, (bPress ? 1 : 0)))
    {
        return true;
    }

    return false;
}

bool CAutoTestMgr::ScreenCapture(const char* pszFile)
{
    if (NULL == pszFile)
    {
        return false;
    }

    yl::string strFile = pszFile;

    if (strFile.empty())
    {
        return false;
    }

    chSize szScreen;

    // 目前xFrameBufferScreen中是常量，不会有跨线程问题，直接读取
    szScreen.cx = g_WorkStation.width();
    szScreen.cy = g_WorkStation.height();

    xPixmap pmScreen;

    if (!pmScreen.Create(szScreen.cx, szScreen.cy))
    {
        return false;
    }

    if (!etl_NotifyApp(true, AUTO_TEST_MSG_SCREEN_CAPTURE, (WPARAM)&pmScreen, 0))
    {
        return false;
    }

    return pmScreen.save(strFile, szScreen);
}

bool CAutoTestMgr::InputText(const char* pszText)
{
    if (NULL == pszText)
    {
        return false;
    }

    yl::string strText = pszText;

    if (strText.empty())
    {
        return false;
    }


    if (etl_NotifyApp(true, AUTO_TEST_MSG_INPUT_TEXT, (WPARAM)&strText, 0))
    {
        return true;
    }

    return false;
}
#endif
