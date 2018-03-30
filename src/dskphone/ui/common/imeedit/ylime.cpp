#include "ylime.h"

#include <assert.h>

#include <ylstringalgorithm.h>
#include <dsklog/log.h>
#include <keymap.h>
#include <limits.h>
#include "imecommon.h"

namespace
{
#define INVALID_IME_INDEX -1
}

/////////////////////////////////////////
CImeInputer::CImeInputer()
    : m_pTarget(NULL)
    , m_nClickCount(0)
    , m_nLastKey(PHONE_KEY_NONE)
    , m_nImeIndex(INVALID_IME_INDEX)
{

}

CImeInputer::CImeInputer(CImeTarget* pTarget)
    : m_pTarget(pTarget)
    , m_nClickCount(0)
    , m_nLastKey(PHONE_KEY_NONE)
    , m_nImeIndex(0)
{

}

CImeInputer::~CImeInputer()
{

}

IMECHAR CImeInputer::GetImeChar(int nKey)
{
    //按键改变，重置输入法
    if (nKey != m_nLastKey)
    {
        ResetCount();
        m_nLastKey = nKey;
    }

    IMECHAR cIme = 0;

    if (!GetCharFromIme(nKey, cIme, m_nClickCount))
    {
        m_nLastKey = PHONE_KEY_NONE;
    }

    return cIme;
}

void CImeInputer::ResetCount()
{
    m_strCurrentImeKey = L"";
    m_nClickCount = 0;

    m_tmIme.KillTimer();

    if (PHONE_KEY_NONE != m_nLastKey)
    {
        m_nLastKey = PHONE_KEY_NONE;

        if (NULL != m_pTarget)
        {
            m_pTarget->OnPreInputConfirm();
        }
    }
}

bool CImeInputer::IsImeValid(const yl::string& strImeName)
{
    return true;
}

bool CImeInputer::AddSupportIme(const yl::string& strImeName)
{
    if (!IsImeValid(strImeName))
    {
        return false;
    }

    m_vecSupImes.push_back(strImeName);

    if (1 == m_vecSupImes.size())
    {
        SetIme(strImeName);
    }

    return true;
}

bool CImeInputer::ClearSupportIme()
{
    if (m_vecSupImes.size() <= 0)
    {
        return true;
    }

    m_vecSupImes.clear();

    ResetCount();

    return true;
}

int CImeInputer::GetImeIndex(const yl::string& strImeName)
{
    if (strImeName.empty())
    {
        return INVALID_IME_INDEX;
    }

    VEC_SUPPORT_IME::iterator iter = m_vecSupImes.begin();
    int i = 0;

    for (; iter != m_vecSupImes.end(); ++iter, ++i)
    {
        const yl::string& strTmpIME = *iter;

        if (strTmpIME == strImeName)
        {
            return i;
        }
    }

    return INVALID_IME_INDEX;
}

yl::string CImeInputer::GetImeNameByIndex(int nIndex)
{
    int nSize = m_vecSupImes.size();

    if (nIndex >= 0 && nIndex < nSize)
    {
        return m_vecSupImes[nIndex];
    }

    return yl::string("");
}

bool CImeInputer::SetIme(const yl::string& strImeName)
{
    if (strImeName.empty())
    {
        return false;
    }

    m_nImeIndex = GetImeIndex(strImeName);

    if (INVALID_IME_INDEX == m_nImeIndex)
    {
        m_nImeIndex = 0;
    }

    m_strIme = GetImeNameByIndex(m_nImeIndex);

    if (m_strIme.empty())
    {
        m_strIme = EDIT_TEXT_VIEW_DEFAULT_IME;
    }

    OnImeChanged();

    return true;
}

void CImeInputer::OnImeChanged()
{
    ResetCount();

    if (NULL != m_pTarget)
    {
        m_pTarget->OnImeChange(GetImeName());
    }
}

bool CImeInputer::SwitchIme(const yl::string& strImeName /*= ""*/)
{
    int nSize = m_vecSupImes.size();

    if (nSize <= 0)
    {
        m_strIme = EDIT_TEXT_VIEW_DEFAULT_IME;

        OnImeChanged();

        return false;
    }

    ++m_nImeIndex;
    m_nImeIndex %= nSize;

    m_strIme = GetImeNameByIndex(m_nImeIndex);

    if (m_strIme.empty())
    {
        m_strIme = EDIT_TEXT_VIEW_DEFAULT_IME;
    }

    OnImeChanged();

    return true;
}

bool CImeInputer::SendImeKey(int nKey)
{
    IMECHAR cIme = GetImeChar(nKey);

    if (L'\0' != cIme)
    {
        if (IsOneChar())
        {
            m_nLastKey = PHONE_KEY_NONE;
            return m_pTarget->OnChar(cIme);
        }

        m_tmIme.SingleShot(1000, TIMER_METHOD(this, CImeInputer::OnImeEnd));

        return m_pTarget->OnPreChar(cIme);
    }

    return false;
}

void CImeInputer::ResetTarget(CImeTarget* pTarget)
{
    if (NULL != pTarget)
    {
        ResetCount();
        m_pTarget = pTarget;
    }
}

yl::string CImeInputer::GetImeName() const
{
    size_t sPos = m_strIme.find("_");

    if (yl::string::npos != sPos)
    {
        return m_strIme.substr(0, sPos);
    }

    return m_strIme;
}

yl::string CImeInputer::GetCurrentIme() const
{
    return m_strIme;
}

void CImeInputer::OnImeEnd(xThreadTimer* timer, LPVOID pExtraData)
{
    ResetCount();
}

////////////////////////////////////////////////
FUN_CREATE_INPUTER  CImeTarget::s_pFunCreateInputer = NULL;
FUN_RELEASE_INPUTER CImeTarget::s_pFunReleaseInputer = NULL;

CImeTarget::CImeTarget()
    : m_pInputer(NULL)
{
    if (NULL != s_pFunCreateInputer)
    {
        m_pInputer = (*s_pFunCreateInputer)(this);
    }
}

CImeTarget::~CImeTarget()
{
    if (NULL != m_pInputer && (NULL == s_pFunReleaseInputer || (!(*s_pFunReleaseInputer)(m_pInputer))))
    {
        delete m_pInputer;
    }
}

void CImeTarget::SetCreateInputerFun(FUN_CREATE_INPUTER pFunCreateInputer)
{
    s_pFunCreateInputer = pFunCreateInputer;
}

void CImeTarget::SetReleaseInputerFun(FUN_RELEASE_INPUTER pFunReleaseInputer)
{
    s_pFunReleaseInputer = pFunReleaseInputer;
}

bool CImeTarget::EndIme()
{
    if (NULL == m_pInputer)
    {
        return false;
    }

    m_pInputer->ResetCount();

    return true;
}

bool CImeTarget::SendImeKey(int iKey)
{
    if (NULL != m_pInputer)
    {
        return m_pInputer->SendImeKey(iKey);
    }

    return false;
}

bool CImeTarget::SetIme(const yl::string& strImeName)
{
    if (NULL != m_pInputer)
    {
        return m_pInputer->SetIme(strImeName);
    }

    return false;
}

bool CImeTarget::SwitchIme(const yl::string& strImeName /*= ""*/)
{
    if (NULL != m_pInputer)
    {
        return m_pInputer->SwitchIme(strImeName);
    }

    return false;
}

bool CImeTarget::AddSupportIme(const yl::string& strImeName)
{
    if (NULL != m_pInputer)
    {
        return m_pInputer->AddSupportIme(strImeName);
    }

    return false;
}

bool CImeTarget::ClearSupportIme()
{
    if (NULL != m_pInputer)
    {
        return m_pInputer->ClearSupportIme();
    }

    return false;
}


void CImeTarget::TargetFocusChange(bool bFocusIn)
{
    if (NULL != m_pInputer)
    {
        m_pInputer->TargetFocusChange(bFocusIn);
    }
}


bool CImeTarget::TargetKeyPress(int key)
{
    if (NULL != m_pInputer)
    {
        return m_pInputer->TargetKeyPress(key);
    }

    return false;
}


void CImeTarget::TargetConfirm()
{
    if (NULL != m_pInputer)
    {
        m_pInputer->TargetConfirm();
    }
}

yl::string CImeTarget::GetImeName() const
{
    yl::string strName = "";

    if (NULL != m_pInputer)
    {
        strName = m_pInputer->GetImeName();
    }

    return strName;
}

yl::string CImeTarget::GetCurrentIme() const
{
    if (NULL != m_pInputer)
    {
        return m_pInputer->GetCurrentIme();
    }

    return yl::string("");
}

bool CImeTarget::CheckImeText(const yl::wstring& strWText, yl::wstring& strNewWText)
{
    if (NULL != m_pInputer)
    {
        return m_pInputer->CheckImeText(strWText, strNewWText);
    }

    return false;
}
