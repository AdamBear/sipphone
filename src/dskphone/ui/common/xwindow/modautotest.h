#ifndef __MOD_AUTO_TEST_H__
#define __MOD_AUTO_TEST_H__

#include <xwindow/xwindowdefine.h>

#if IF_XWINDOW_AUTO_TEST
#ifdef __cplusplus
extern "C"
{
#endif
enum
{
    DUMP_VIEW_RESULT_EMPTY          = 0,
    DUMP_VIEW_RESULT_PARAM_ERROR    = -1,
    DUMP_VIEW_RESULT_LACK_BUFFER    = -2,
    DUMP_VIEW_RESULT_EXE_ERROR      = -3
};

//打包时会被strip，改用回调处理
//#define AUTO_TEST_EXPORT __attribute__ ((visibility("default"),externally_visible,used))
#define AUTO_TEST_EXPORT

void AutoTest_Init();

// 导出控件树，0表示内容为空，-1表示输入参数错误，-2表示内存不足
int AUTO_TEST_EXPORT AutoTest_DumpAllView(char* pBuffer, int nSize);
// 屏幕截图
bool AUTO_TEST_EXPORT AutoTest_ScreenCapture(const char* pszFile);
// 模拟输入
bool AUTO_TEST_EXPORT AutoTest_SendKey(int nKeyCode, bool bPress);
// 设置当前焦点项的文本内容
bool AUTO_TEST_EXPORT AutoTest_InputText(const char* pszText);
// 截取某个控件
bool AUTO_TEST_EXPORT AutoTest_DrawView(const char* pszViewId, const char* pszFile);
#ifdef __cplusplus
}
#endif
#endif

#endif
