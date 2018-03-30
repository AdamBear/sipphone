#include "languagehelper.h"
#include "configiddefine.h"
#include "texttranbindhelper.h"
#include "languagemessagedefine.h"
#include <etlmsghandler/modetlmsghandler.h>
#if IF_CONFIG_LISTENER
#include "include/configdefine.h"
#else
#include "service_config.h"
#endif
#include "commonapi/stringhelperapi.h"
#include "language/translate/include/unicode_detect.h"
#include "language/translate/include/translate.h"
#include "language/input/include/modInput.h"
#include "xwindow/xWindowManager.h"


#if IF_SUPPORT_TRAN_MULTI_THREAD
class auto_mutex
{
public:
    auto_mutex(chMutex* pMutex)
    {
        chASSERT(pMutex != NULL);
        m_pMutex = pMutex;
        m_pMutex->Lock();
    }

    ~auto_mutex()
    {
        m_pMutex->Unlock();
    }
    chMutex* m_pMutex;
};

#define AUTO_MUTEX(Mutex)   auto_mutex objAutoMutex(&(Mutex))
#endif

//////////////////////////////////////////////////////////////////////////
// class CLanguageHelper
CLanguageHelper::CLanguageHelper()
#if TRANSLATE_INFO_RECORD && IF_SUPPORT_TRAN_MULTI_THREAD
    : m_pHostThread(NULL)
#endif
{
#if TRANSLATE_INFO_RECORD && IF_SUPPORT_TRAN_MULTI_THREAD
    m_lock.OpenMutex("lang_record_lock");
#endif
}

CLanguageHelper::~CLanguageHelper()
{
#if TRANSLATE_INFO_RECORD
#if IF_SUPPORT_TRAN_MULTI_THREAD
    m_lock.CloseMutex();
#endif

    m_timer.KillTimer();

    m_listUsedText.clear();
    m_listLostTranslateText.clear();
#endif
}

CLanguageHelper& CLanguageHelper::getInstance()
{
    static CLanguageHelper* s_pLanguageHelper = NULL;

#if IF_SUPPORT_TRAN_MULTI_THREAD
    getStaticObjectCritical().Lock();
#endif

    if (NULL == s_pLanguageHelper)
    {
        s_pLanguageHelper = new CLanguageHelper;
    }

#if IF_SUPPORT_TRAN_MULTI_THREAD
    getStaticObjectCritical().Unlock();
#endif

    return *s_pLanguageHelper;
}

#if TRANSLATE_INFO_RECORD
#if IF_SUPPORT_TRAN_MULTI_THREAD
void CLanguageHelper::Init()
{
    AUTO_MUTEX(m_lock);

    m_pHostThread = g_uCSystemMessage.GetCurrentThreadObject();

    etl_RegisterMsgHandle(LANGUAGE_MESSAGE_HELPER_TRIGGER_RECORD_TRAN_INFO, LANGUAGE_MESSAGE_HELPER_TRIGGER_RECORD_TRAN_INFO,
                          &CLanguageHelper::OnMessage);
}

void CLanguageHelper::UnInit()
{
    AUTO_MUTEX(m_lock);

    m_pHostThread = NULL;

    etl_UnregisterMsgHandle(LANGUAGE_MESSAGE_HELPER_TRIGGER_RECORD_TRAN_INFO, LANGUAGE_MESSAGE_HELPER_TRIGGER_RECORD_TRAN_INFO,
                            &CLanguageHelper::OnMessage);
}
#endif

void CLanguageHelper::PushTranTextRecord(LIST_STRING& listText, const yl::string& strText)
{
#if IF_SUPPORT_TRAN_MULTI_THREAD
    AUTO_MUTEX(m_lock);
#endif

    if (IsTextExist(listText, strText))
    {
        return;
    }

    listText.push_back(strText);

#if IF_SUPPORT_TRAN_MULTI_THREAD
    if (g_uCSystemMessage.GetCurrentThreadObject() != m_pHostThread)
    {
        if (NULL != m_pHostThread)
        {
            msgPostMsgToThread(msgGetThreadByName(m_pHostThread->m_strThreadName.c_str()),
                               LANGUAGE_MESSAGE_HELPER_TRIGGER_RECORD_TRAN_INFO, 0, 0);
        }
    }
    else
    {
#endif
        m_timer.SetTimer(500, TIMER_METHOD(this, CLanguageHelper::ReportTranslateInfo));
#if IF_SUPPORT_TRAN_MULTI_THREAD
    }
#endif
}

bool CLanguageHelper::IsTextExist(LIST_STRING& listText, const yl::string& strText)
{
    LIST_STRING::iterator iter = listText.begin();

    for (; iter != listText.end(); ++iter)
    {
        if (strText == (*iter))
        {
            return true;
        }
    }

    return false;
}
#endif

BOOL CLanguageHelper::Translate(const char* lpszText, yl::string& strOut)
{
    if (NULL == lpszText)
    {
        strOut = "";
        return FALSE;
    }

    // 先检查是否是翻译整个标记
    if (strncmp(lpszText, LANG_FLAG_ALL, LANG_ALLFLAGE_SIZE) == 0)
    {
        //跳过标记直接翻译即可
        LPCSTR lpszTranText = lpszText + LANG_ALLFLAGE_SIZE;
        LPCSTR lpszResult = _TransLogic.Translate(lpszTranText);

        if (NULL != lpszResult)
        {
#if TRANSLATE_INFO_RECORD
            yl::string strTranText = lpszTranText;
            PushTranTextRecord(m_listUsedText, strTranText);
#endif
            strOut = lpszResult;
        }
        else
        {
#if TRANSLATE_INFO_RECORD
            yl::string strTranText = lpszTranText;
            PushTranTextRecord(m_listLostTranslateText, strTranText);
#endif
            strOut = "";
        }
        return TRUE;
    }

    strOut = lpszText;
    LPSTR lpFlagePos = strstr((LPSTR)lpszText, LANG_FLAG);

    if (NULL == lpFlagePos)
    {
#if TRANSLATE_INFO_RECORD
        PushTranTextRecord(m_listLostTranslateText, lpszText);
#endif
        return FALSE;
    }

    size_t iLength = strlen(lpszText);
    char* lpBuffer = new char[iLength + 1];
    memset(lpBuffer, 0, iLength + 1);
    yl::string strResult = "";
    strResult.append(lpszText, lpFlagePos - lpszText);

    while (lpFlagePos)
    {
        char lpSize[LANG_STRING_LEN_FLAG + 1] = "";
        strncpy(lpSize, lpFlagePos + LANG_FLAGE_SIZE, LANG_STRING_LEN_FLAG);
        int iSize = atoi(lpSize);
        int iNextContentOffset = iSize + LANG_FLAGE_SIZE + LANG_STRING_LEN_FLAG;//跳过标志,长度和对应内容即下一个文本
        if (lpFlagePos + iNextContentOffset > lpszText + iLength)
        {
            delete[] lpBuffer;
            return FALSE;
        }

        strncpy(lpBuffer, lpFlagePos + LANG_FLAGE_SIZE + LANG_STRING_LEN_FLAG, iSize);
        lpBuffer[iSize] = '\0';

        LPCSTR lpszResult = _TransLogic.Translate(lpBuffer);

        if (NULL != lpszResult)
        {
#if TRANSLATE_INFO_RECORD
            yl::string strTranText = lpBuffer;
            PushTranTextRecord(m_listUsedText, strTranText);
#endif
            strResult += lpszResult;
        }
#if TRANSLATE_INFO_RECORD
        else
        {
            yl::string strTranText = lpBuffer;
            PushTranTextRecord(m_listLostTranslateText, strTranText);
        }
#endif

        char* lpCurrent = lpFlagePos + iNextContentOffset;
        lpFlagePos = strstr(lpFlagePos + iNextContentOffset, LANG_FLAG);
        if (NULL == lpFlagePos)
        {
            strResult = strResult + lpCurrent;
        }
        else
        {
            strResult.append(lpCurrent, lpFlagePos - lpCurrent);
        }
    }

    strOut = strResult;
    delete[] lpBuffer;
    return TRUE;
}

yl::string CLanguageHelper::AppenLangFlage(LPCSTR lpszText)
{
    if (NULL == lpszText)
    {
        return "";
    }

    if (lpszText == NULL || 0 == strcmp(lpszText, ""))
    {
        //字符串为空，返回空
        return "";
    }

    if (NULL != strstr(lpszText, LANG_FLAG))
    {
        return lpszText;
    }

    return commonAPI_FormatString("%s%.2d%s", LANG_FLAG, strlen(lpszText), lpszText);
}

#if TRANSLATE_INFO_RECORD
#if IF_SUPPORT_TRAN_MULTI_THREAD
LRESULT CLanguageHelper::OnMessage(msgObject& refMessage)
{
    if (LANGUAGE_MESSAGE_HELPER_TRIGGER_RECORD_TRAN_INFO == refMessage.message)
    {
        _LangHelper.TriggerRecord();
        return TRUE;
    }

    return FALSE;
}

void CLanguageHelper::TriggerRecord()
{
    m_timer.SetTimer(500, TIMER_METHOD(this, CLanguageHelper::ReportTranslateInfo));
}
#endif

BOOL CLanguageHelper::ReportTranslateInfo(xThreadTimer* timer, LPVOID pExtraData)
{
#if IF_SUPPORT_TRAN_MULTI_THREAD
    AUTO_MUTEX(m_lock);
#endif
    chFileStdio objOutput;
#ifdef WIN32
    objOutput.Open("./textlist.txt", chFileStdio::modeCreateWrite);
#else
    objOutput.Open("/tmp/textlist.txt", chFileStdio::modeCreateWrite);
#endif

    objOutput.WriteString("----------------text list----------------\n");
    LIST_STRING::iterator iter = m_listUsedText.begin();
    for (; iter != m_listUsedText.end(); ++iter)
    {
        objOutput.WriteString((*iter).c_str());
        objOutput.WriteString("\n");
    }

    objOutput.WriteString("----------------not translate item----------------");
    iter = m_listLostTranslateText.begin();
    for (; iter != m_listLostTranslateText.end(); ++iter)
    {
        objOutput.WriteString((*iter).c_str());
        objOutput.WriteString("\n");
    }

    m_timer.KillTimer();
}
#endif

CLanguageHelperHandler::CLanguageHelperHandler()
{
#if IF_SUPPORT_TRAN_MULTI_THREAD
    m_lock.OpenMutex("lang_lock");
#endif
}

CLanguageHelperHandler::~CLanguageHelperHandler()
{
#if IF_SUPPORT_TRAN_MULTI_THREAD
    m_lock.CloseMutex();

    m_mapBindHelper.clear();
#endif
}

CLanguageHelperHandler& CLanguageHelperHandler::getInstance()
{
    static CLanguageHelperHandler* s_pLangHandler = NULL;

#if IF_SUPPORT_TRAN_MULTI_THREAD
    getStaticObjectCritical().Lock();
#endif

    if (NULL == s_pLangHandler)
    {
        s_pLangHandler = new CLanguageHelperHandler;
    }

#if IF_SUPPORT_TRAN_MULTI_THREAD
    getStaticObjectCritical().Unlock();
#endif

    return *s_pLangHandler;
}

void CLanguageHelperHandler::Init()
{
#if IF_CONFIG_LISTENER
    kszCurrentLang.AddListener(this);
#else
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, &CLanguageHelperHandler::OnConfigChange);
#endif
}

void CLanguageHelperHandler::UnInit()
{
#if IF_CONFIG_LISTENER
    kszCurrentLang.RemoveListener(this);
#else
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, &CLanguageHelperHandler::OnConfigChange);
#endif
}

#if IF_SUPPORT_TRAN_TEXT_BIND
void CLanguageHelperHandler::RegisterTranHelper(xWorkStation* pWorkStation)
{
    if (NULL == pWorkStation)
    {
        return;
    }

#if IF_SUPPORT_TRAN_MULTI_THREAD
    AUTO_MUTEX(m_lock);
#endif

    _TextTranBindHelper.Init();

    pWorkStation->SetFunSetupTextTran(&CTextTranBindHelper::SetupTextTran);
    pWorkStation->SetFunReleaseTextTran(&CTextTranBindHelper::ReleaseTextTran);

#if IF_SUPPORT_TRAN_MULTI_THREAD
    m_mapBindHelper[pWorkStation] = &_TextTranBindHelper;
#endif
}

void CLanguageHelperHandler::UnRegisterTranHelper(xWorkStation* pWorkStation)
{
    if (NULL == pWorkStation)
    {
        return;
    }

#if IF_SUPPORT_TRAN_MULTI_THREAD
    AUTO_MUTEX(m_lock);
#endif

    pWorkStation->SetFunSetupTextTran(NULL);
    pWorkStation->SetFunReleaseTextTran(NULL);

    _TextTranBindHelper.UnInit();

#if IF_SUPPORT_TRAN_MULTI_THREAD
    MAP_TEXT_TRAN_BIND_HELPER::iterator iter = m_mapBindHelper.find(pWorkStation);

    if (iter != m_mapBindHelper.end())
    {
        m_mapBindHelper.erase(iter);
    }
#endif
}
#endif

#if IF_CONFIG_LISTENER
bool CLanguageHelperHandler::OnConfigChanged(const CConfigBase& cfg)
{
    OnLangChanged();

    return TRUE;
}
#else
LRESULT CLanguageHelperHandler::OnConfigChange(msgObject& msg)
{
    if (ST_LANG != msg.wParam)
    {
        return FALSE;
    }

    _LangHandle.OnLangChanged();

    return TRUE;
}
#endif

void CLanguageHelperHandler::OnLangChanged()
{
    Lang_ReLoadLang();

#if IF_SUPPORT_TRAN_TEXT_BIND
#if IF_SUPPORT_TRAN_MULTI_THREAD
    // 发送消息给各个线程
    AUTO_MUTEX(m_lock);

    MAP_TEXT_TRAN_BIND_HELPER::iterator iter = m_mapBindHelper.begin();

    for (; iter != m_mapBindHelper.end(); ++iter)
    {
        CTextTranBindHelper* pBindHelper = iter->second;

        if (NULL == pBindHelper)
        {
            continue;
        }

        chThreadLocal* pHostThread = pBindHelper->m_pHostThread;

        if (NULL == pHostThread)
        {
            continue;
        }

        msgPostMsgToThread(msgGetThreadByName(pHostThread->m_strThreadName.c_str()),
                           LANGUAGE_MESSAGE_HELPER_TEXT_BIND_RE_INIT, 0, 0);
    }
#else
    _TextTranBindHelper.ReinstallTranText();
#endif
#endif
}
