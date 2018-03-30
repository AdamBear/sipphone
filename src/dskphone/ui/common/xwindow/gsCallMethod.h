#ifndef __GS_CALL_METHOD_H__
#define __GS_CALL_METHOD_H__

#include <xwindow/xwindowdefine.h>
#include "xwindowcommon.h"


#if IF_VIEW_USER_ACTION
//////////////////////////////////////////////////////////////////////////
// call_text_method()
// 该功能主要有以下几种用途
// 1、脚本中调用：读写配置文件，注册表、XML文件，如命令 idlebox "iniGetString(/yealink/config/autop.ini, TimePolicy, bEnableRepeat, 1)"
// 2、程序中调用：在配置中配置，如在版本号列表中做成，<Module name="dskPhone" version="0.0.0.0" action="fileFindText(/yealink/bin/dskPhone.exx, .Version:, 24)"/>
//                  程序读取到操作方式以后调用call_text_method("fileFindText(/yealink/bin/dskPhone.exx, .Version:, 24)");
// 3、网页中调用：在网页中静态写成system("xxx")，CGI在生成页面的时候就会执行system函数，如果是通过AJax发送过来的命令也会被执行
//////////////////////////////////////////////////////////////////////////
//
class XWINDOW_EXPORT jsParameter
{
public:
    jsParameter(LPCSTR& lpszText);

public:
    int GetCount() const;
    yl::string GetParam(int nIndex) const;

protected:
    LPCSTR ParseFunctionText(LPCSTR lpszParamText);

protected:
    VEC_STRING m_vecParam;
};

typedef yl::string(*FUN_ROUTINE_METHOD)(const jsParameter& objParameter);

struct XWINDOW_EXPORT ST_ROUTINE_METHOD
{
    yl::string          strKeyWord;
    LPCSTR              lpszParamList;
    FUN_ROUTINE_METHOD  pRoutineMethod;
};

BOOL XWINDOW_EXPORT AppendRoutineTable(ST_ROUTINE_METHOD* pRoutine);
yl::string XWINDOW_EXPORT call_text_method(LPCSTR lpszScriptText);
yl::string XWINDOW_EXPORT dump_all_text_method();
#endif

#endif // __gsCallMethod_h__
