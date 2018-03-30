#include "texttranbindhelper.h"
#include "languagehelper.h"
#include "languagemessagedefine.h"
#include <etlmsghandler/modetlmsghandler.h>
#include "xwindow/xWindowManager.h"
#include "xwindow/xWindow.h"


#if IF_SUPPORT_TRAN_TEXT_BIND
CTextTranBindHelper::CTextTranBindHelper()
#if IF_SUPPORT_TRAN_MULTI_THREAD
    : m_pHostThread(NULL)
#endif
{
}

CTextTranBindHelper::~CTextTranBindHelper()
{
}

CTextTranBindHelper& CTextTranBindHelper::getInstance()
{
#if IF_MULTI_XKERNEL_ENABLE
    MULTI_KERNEL_INSTANCE(CTextTranBindHelper)
#else
    SINGLE_KERNEL_INSTANCE(CTextTranBindHelper)
#endif
}

void CTextTranBindHelper::Init()
{
#if IF_SUPPORT_TRAN_MULTI_THREAD
    m_pHostThread = g_uCSystemMessage.GetCurrentThreadObject();

    etl_RegisterMsgHandle(LANGUAGE_MESSAGE_HELPER_TEXT_BIND_RE_INIT, LANGUAGE_MESSAGE_HELPER_TEXT_BIND_RE_INIT,
                          &CTextTranBindHelper::OnMessage);
#endif
}

void CTextTranBindHelper::UnInit()
{
#if IF_SUPPORT_TRAN_MULTI_THREAD
    etl_UnregisterMsgHandle(LANGUAGE_MESSAGE_HELPER_TEXT_BIND_RE_INIT, LANGUAGE_MESSAGE_HELPER_TEXT_BIND_RE_INIT,
                            &CTextTranBindHelper::OnMessage);
#endif

    m_mapTranText.clear();
}

#if IF_SUPPORT_TRAN_MULTI_THREAD
LRESULT CTextTranBindHelper::OnMessage(msgObject& refMessage)
{
    if (LANGUAGE_MESSAGE_HELPER_TEXT_BIND_RE_INIT == refMessage.message)
    {
        _TextTranBindHelper.ReinstallTranText();

        return TRUE;
    }

    return FALSE;
}
#endif

bool CTextTranBindHelper::SetupTextTran(xTextTranslate* pTextTran, const yl::string& strText)
{
    if (NULL == pTextTran)
    {
        return false;
    }

    yl::string strTran;

    if (!strText.empty() && _LangHelper.Translate(strText.c_str(), strTran))
    {
        _TextTranBindHelper.AddTranText(pTextTran, strText);

        pTextTran->SetTranText(strTran);

        return true;
    }

    _TextTranBindHelper.DeleteTranText(pTextTran);
    return false;
}

void CTextTranBindHelper::ReleaseTextTran(xTextTranslate* pTextTran)
{
    _TextTranBindHelper.DeleteTranText(pTextTran);
}

bool CTextTranBindHelper::AddTranText(xTextTranslate* pTextTranslate, const yl::string& strText)
{
    if (NULL == pTextTranslate)
    {
        return false;
    }

    m_mapTranText[pTextTranslate] = strText;
    return true;
}

bool CTextTranBindHelper::DeleteTranText(xTextTranslate* pTextTranslate)
{
    MAP_TRAN_TEXT::iterator iter = m_mapTranText.find(pTextTranslate);

    if (iter != m_mapTranText.end())
    {
        m_mapTranText.erase(iter);

        return true;
    }

    return false;
}

void CTextTranBindHelper::ReinstallTranText()
{
    MAP_TRAN_TEXT::iterator iter = m_mapTranText.begin();

    for (; iter != m_mapTranText.end(); ++iter)
    {
        xTextTranslate* pTextTran = iter->first;

        if (NULL == pTextTran)
        {
            continue;
        }

        yl::string& strText = iter->second;
        yl::string strTran;

        if (_LangHelper.Translate(strText.c_str(), strTran))
        {
            pTextTran->SetTranText(strTran);
        }
    }
}

#endif
