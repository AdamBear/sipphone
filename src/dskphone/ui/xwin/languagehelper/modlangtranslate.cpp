#include "ETLLib/ETLLib.hpp"
#include "modlangtranslate.h"
#include "languagehelper.h"
#include "texttranbindhelper.h"
#include "configiddefine.h"
#include "xwindow/xWindowManager.h"

bool lang_Init()
{
#if TRANSLATE_INFO_RECORD && IF_SUPPORT_TRAN_MULTI_THREAD
    _LangHelper.Init();
#else
    _LangHelper;
#endif

    _LangHandle.Init();

    return true;
}

bool DECLARE_EXPORT lang_UnInit()
{
#if TRANSLATE_INFO_RECORD && IF_SUPPORT_TRAN_MULTI_THREAD
    _LangHelper.UnInit();
#endif

    _LangHandle.UnInit();

    return true;
}

bool lang_Register(xWorkStation* pWorkStation)
{
    if (NULL == pWorkStation)
    {
        return false;
    }

#if IF_SUPPORT_TRAN_TEXT_BIND
    _LangHandle.RegisterTranHelper(pWorkStation);
#endif

    pWorkStation->SetFunLang(&lang_AppenLangFlage);
    pWorkStation->SetFunTran(&lang_Translate);

    return true;
}

bool lang_UnRegister(xWorkStation* pWorkStation)
{
    if (NULL == pWorkStation)
    {
        return false;
    }

    pWorkStation->SetFunLang(NULL);
    pWorkStation->SetFunTran(NULL);

#if IF_SUPPORT_TRAN_TEXT_BIND
    _LangHandle.UnRegisterTranHelper(pWorkStation);
#endif

    return true;
}

yl::string lang_AppenLangFlage(const char* lpszText)
{
    return _LangHelper.AppenLangFlage(lpszText);
}

yl::string lang_AppenLangFlage(const yl::string& strText)
{
    return lang_AppenLangFlage(strText.c_str());
}

yl::string lang_Translate(const char* lpszText)
{
    yl::string strOut;

    if (!_LangHelper.Translate(lpszText, strOut))
    {
        if (NULL == lpszText)
        {
            return "";
        }

        return yl::string(lpszText);
    }

    return strOut;
}

yl::string lang_Translate(const yl::string& strText)
{
    yl::string strOut;

    if (_LangHelper.Translate(strText.c_str(), strOut))
    {
        return strOut;
    }

    return strText;
}
