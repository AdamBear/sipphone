#ifndef __LANGUAGE_HELPER_H__
#define __LANGUAGE_HELPER_H__

#include "languagedef.h"
#if TRANSLATE_INFO_RECORD
#include "xwindow/xwindowcommon.h"
#endif
#if TRANSLATE_INFO_RECORD || IF_CONFIG_LISTENER
#include "configparser/configlistener.h"
#else
#include <ETLLib/ETLLib.hpp>
#endif
#include "xwindow-kernel/include/xkernel.h"
#include "xwindow-kernel/include/xthreadtimer.h"


USING_XKERNEL_NAMESPACE
//#define IF_XML_TRANS_LANG
//////////////////////////////////////////////////////////////////////////
//翻译大约1200+条,取1500附近质数
#define HASH_SIZE 1459

// 翻译标记长度
#define LANG_FLAGE_SIZE         2
// 翻译标记
#define LANG_FLAG               "?`"
//翻译整个字符串的标志的长度
#define LANG_ALLFLAGE_SIZE      2
//翻译整个字符串的标志
#define LANG_FLAG_ALL           "`?"
// 不翻译标记长度
#define LANG_SKIP_FLAGE_SIZE    4
// 不翻译标记,只用于xml文件中
#define LANG_SKIP_FLAG          "?`-1"

#define LANG_STRING_LEN_FLAG    2   //用于存储语言字符串长度的位数

class xWorkStation;

////////////////////////////////////////////////////////////////////////////////////////////
class CLanguageHelper
#if TRANSLATE_INFO_RECORD
    : public xTimerHandler
#endif
{
public:
    // 根据翻译标记执行翻译,如果lpszText未发现翻译标记则返回FALSE,如果翻译标记正常则返回TRUE
    BOOL            Translate(LPCSTR lpszText, yl::string& strOut);
    // 给字符串添加翻译标记
    yl::string      AppenLangFlage(LPCSTR lpszText);

public:
    static CLanguageHelper& getInstance();

#if TRANSLATE_INFO_RECORD
#if IF_SUPPORT_TRAN_MULTI_THREAD
    static LRESULT  OnMessage(msgObject& refMessage);
    void            TriggerRecord();
#endif

    void            ReportTranslateInfo(xThreadTimer* timer, LPVOID pExtraData);
#endif

public:
    CLanguageHelper();
    virtual ~CLanguageHelper();

#if TRANSLATE_INFO_RECORD && IF_SUPPORT_TRAN_MULTI_THREAD
    void Init();
    void UnInit();
#endif

#if TRANSLATE_INFO_RECORD
protected:
    void PushTranTextRecord(LIST_STRING& listText, const yl::string& strText);
    bool IsTextExist(LIST_STRING& listText, const yl::string& strText);
#endif

protected:
#if TRANSLATE_INFO_RECORD
#if IF_SUPPORT_TRAN_MULTI_THREAD
    chThreadLocal*      m_pHostThread;
    chMutex             m_lock;
#endif
    xThreadTimer        m_timer;
    LIST_STRING         m_listUsedText;// 记录UI使用过的不带翻译标志的字符串
    LIST_STRING         m_listLostTranslateText;// 附带翻译标志但是缺少翻译的字符串
#endif
};

#define _LangHelper CLanguageHelper::getInstance()


////////////////////////////////////////////////////////////////////////////////////////////
#if IF_SUPPORT_TRAN_MULTI_THREAD
class CTextTranBindHelper;
#endif
class CLanguageHelperHandler
#if IF_CONFIG_LISTENER
    : public IConfigListener
#endif
{
public:
    CLanguageHelperHandler();
    virtual ~CLanguageHelperHandler();

    static CLanguageHelperHandler& getInstance();

public:
#if IF_CONFIG_LISTENER
    virtual bool OnConfigChanged(const CConfigBase& cfg);
#else
    static LRESULT OnConfigChange(msgObject& msg);
#endif

    void            Init();
    void            UnInit();
#if IF_SUPPORT_TRAN_TEXT_BIND
    void            RegisterTranHelper(xWorkStation* pWorkStation);
    void            UnRegisterTranHelper(xWorkStation* pWorkStation);
#endif

    void OnLangChanged();

private:
#if IF_SUPPORT_TRAN_MULTI_THREAD
    chMutex         m_lock;
    typedef YLHashMap<xWorkStation*, CTextTranBindHelper*> MAP_TEXT_TRAN_BIND_HELPER;

    MAP_TEXT_TRAN_BIND_HELPER m_mapBindHelper;
#endif
};

#define _LangHandle CLanguageHelperHandler::getInstance()

#endif // __LANGUAGE_HELPER_H__
