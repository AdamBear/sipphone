#ifndef __TEXT_TRAN_BIND_HELPER_H__
#define __TEXT_TRAN_BIND_HELPER_H__

#include <ETLLib/ETLLib.hpp>
#include "languagedef.h"

#if IF_SUPPORT_TRAN_TEXT_BIND
#include <ylhashmap.h>
#include <ylstring.h>
#include "xwindow/xwindowcommon.h"


typedef YLHashMap<xTextTranslate*, yl::string> MAP_TRAN_TEXT;

class xWorkStation;
class CLanguageHelperHandler;

class CTextTranBindHelper
{
    friend class CLanguageHelperHandler;

public:
    CTextTranBindHelper();
    virtual ~CTextTranBindHelper();

public:
    static CTextTranBindHelper& getInstance();

    static bool SetupTextTran(xTextTranslate* pTextTran, const yl::string& strText);
    static void ReleaseTextTran(xTextTranslate* pTextTran);

#if IF_SUPPORT_TRAN_MULTI_THREAD
    static LRESULT OnMessage(msgObject& refMessage);
#endif

public:
    void Init();
    void UnInit();

    void ReinstallTranText();

protected:
    bool AddTranText(xTextTranslate* pTextTranslate, const yl::string& strText);
    bool DeleteTranText(xTextTranslate* pTextTranslate);

protected:
#if IF_SUPPORT_TRAN_MULTI_THREAD
    chThreadLocal* m_pHostThread;
#endif
    MAP_TRAN_TEXT m_mapTranText;
};

#define _TextTranBindHelper CTextTranBindHelper::getInstance()
#endif

#endif
