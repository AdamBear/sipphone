#ifndef __INPUT_METHOD_H__
#define __INPUT_METHOD_H__

#include "imeedit/ylime.h"
#include "language/common/lang_common.h"


extern const char * IME_ALL;
extern const char * IME_DIGIT;
extern const char * IME_LOWER;
extern const char * IME_UPPER;
extern const char * IME_Number;
extern const char * IME_DIAL;
extern const char * IME_STAR_ONLY_DIAL;
extern const char * IME_DIAL_2AB;
extern const char * IME_IP;
extern const char * IME_Name;
extern const char * IME_CUSTOM;
// 密码界面输入法
extern const char * IME_PWD;

CImeInputer * IME_CreateInputer(CImeTarget * pTarget);
bool IME_ReleaseInputer(CImeInputer * pInputer);

bool IME_InstallIME(CImeTarget * pTarget, LPCSTR lpszIMEName);
void IME_UninstallIME(CImeTarget * pTarget);

//重新加载 输入法  提供给autop 上传输入法配置 输入法文件   以及 设置界面修改语言时使用
void IME_Reload();

#endif
