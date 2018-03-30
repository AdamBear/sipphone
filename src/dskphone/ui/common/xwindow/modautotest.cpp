#include "modautotest.h"
#include "xwindow/autotestmgr.h"


#if IF_XWINDOW_AUTO_TEST
#ifdef __cplusplus
extern "C"
{
#endif
void AutoTest_Init()
{
    g_AutoTestMgr.Init();
}

int AUTO_TEST_EXPORT AutoTest_DumpAllView(char* pBuffer, int nSize)
{
    return g_AutoTestMgr.ExportAllView(pBuffer, nSize);
}

bool AUTO_TEST_EXPORT AutoTest_ScreenCapture(const char* pszFile)
{
    return g_AutoTestMgr.ScreenCapture(pszFile);
}

bool AUTO_TEST_EXPORT AutoTest_SendKey(int nKeyCode, bool bPress)
{
    return g_AutoTestMgr.SendKey(nKeyCode, bPress);
}

// 设置当前焦点项的文本内容
bool AUTO_TEST_EXPORT AutoTest_InputText(const char* pszText)
{
    return g_AutoTestMgr.InputText(pszText);
}

bool AUTO_TEST_EXPORT AutoTest_DrawView(const char* pszViewId, const char* pszFile)
{
    return false;
}
#ifdef __cplusplus
}
#endif

#endif
