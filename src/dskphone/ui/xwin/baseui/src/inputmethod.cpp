#include "inputmethod.h"
#include "xwindow/xwindowcommon.h"
#include "language/input/include/imelogic.h"
#include "languagehelper/languagehelper.h"

#include "keymap.h"
#include "imeedit/ximeedit.h"
#include "inputbarmanager.h"

const char * IME_ALL = (IME_2aB "," IME_123 "," IME_abc "," IME_ABC "," IME_DIY);
const char * IME_DIGIT = (IME_123 "," IME_abc "," IME_ABC "," IME_DIY "," IME_2aB);
const char * IME_LOWER = (IME_abc "," IME_ABC "," IME_DIY "," IME_2aB "," IME_123);
const char * IME_UPPER = (IME_ABC "," IME_DIY "," IME_2aB "," IME_123 "," IME_abc);
const char * IME_Number = IME_123;
const char * IME_DIAL = (IME_123_Dial "," IME_abc "," IME_ABC "," IME_2aB);
const char * IME_STAR_ONLY_DIAL = (IME_DIY "," IME_abc "," IME_ABC "," IME_2aB);
const char * IME_DIAL_2AB = (IME_2aB"," IME_123_Dial "," IME_abc "," IME_ABC);
const char * IME_IP = IME_123_IP;
const char * IME_Name = (IME_Abc "," IME_2aB "," IME_123 "," IME_abc "," IME_ABC "," IME_DIY);
const char * IME_CUSTOM = (IME_DIY "," IME_2aB "," IME_123 "," IME_abc "," IME_ABC);
// 密码界面输入法
const char * IME_PWD = (IME_abc_Pwd "," IME_ABC_Pwd "," IME_2aB_Pwd "," IME_123_Pwd);

typedef YLVector<yl::string> VEC_STRING;

////////////////////////////////////////////////////////////////////////////////////////////
class CCommonImeInputer : public CImeInputer
{
public:
    explicit CCommonImeInputer(CImeTarget * pTarget);
    virtual ~CCommonImeInputer();

public:
    virtual IMECHAR GetImeChar(int nKey);
    virtual bool SendImeKey(int nKey);
    virtual void OnImeEnd(xThreadTimer* timer, LPVOID pExtraData);
    virtual bool SwitchIme(const yl::string& strImeName = "");

    virtual void TargetFocusChange(bool bFocusIn);
    virtual bool TargetKeyPress(int key);
    virtual void TargetConfirm();

protected:
    virtual bool IsImeValid(const yl::string & strImeName);
    virtual void OnImeChanged();

    virtual bool GetCharFromIme(int nKey, IMECHAR & cIme, int & nClickCount);

    virtual bool CheckImeText(const yl::wstring & strWText, yl::wstring & strNewWText);

protected:
    bool SwitchIme(bool bNext = true);
    void OnSwitchImeEnd(xThreadTimer* timer, LPVOID pExtraData);
    void GetAllIme(VEC_STRING &vecAllIme);

protected:
    ImeKeyMap * m_pKeyMap;
};

CCommonImeInputer::CCommonImeInputer(CImeTarget * pTarget)
    : CImeInputer(pTarget)
    , m_pKeyMap(NULL)
{
}

CCommonImeInputer::~CCommonImeInputer()
{

}

IMECHAR CCommonImeInputer::GetImeChar(int nKey)
{
    //按键改变，重置输入法
    if (nKey != m_nLastKey)
    {
        // 是否是左右键
        if (g_inputbarmanager.IsVisibleBar(BAR_CANDIDATE))
        {
            if (nKey == PHONE_KEY_LEFT)
            {
                m_nClickCount--;
                if (m_nClickCount < 0)
                {
                    m_nClickCount = m_strCurrentImeKey.length() - 1;
                }
            }
            else if (nKey == PHONE_KEY_RIGHT)
            {
                m_nClickCount++;
            }
            else
            {
                ResetCount();
                m_nLastKey = nKey;
            }
        }
        else
        {
            ResetCount();
            m_nLastKey = nKey;
        }

    }
    else
    {
        // 这里处理count
        m_nClickCount++;
    }

    IMECHAR cIme = 0;

    if (!GetCharFromIme(nKey, cIme, m_nClickCount))
    {
        m_nLastKey = PHONE_KEY_NONE;
    }

    return cIme;
}


bool CCommonImeInputer::SendImeKey(int nKey)
{
    IMECHAR cIme = GetImeChar(nKey);

    if (L'\0' != cIme)
    {
        if (IsOneChar())
        {
            m_nLastKey = PHONE_KEY_NONE;
            g_inputbarmanager.HideInputBar(BAR_INPUT);
            return m_pTarget->OnChar(cIme);
        }

        m_tmIme.SingleShot(1000, TIMER_METHOD(this, CImeInputer::OnImeEnd));

        bool ret = m_pTarget->OnPreChar(cIme);

        if (ret)
        {
#if IF_XWINDOW_RTTI_CHECK
            xEditTextView *editView = dynamic_cast<xEditTextView *>(m_pTarget);
#else
            xEditTextView *editView = static_cast<xEditTextView *>(m_pTarget);
#endif
            g_inputbarmanager.SetBarInfo(m_nClickCount + 1, m_strCurrentImeKey);
            g_inputbarmanager.ShowInputBar(BAR_CANDIDATE, editView);
        }

        return ret;
    }

    return false;
}

void CCommonImeInputer::OnImeEnd(xThreadTimer* timer, LPVOID pExtraData)
{
    CImeInputer::OnImeEnd(timer, pExtraData);
    g_inputbarmanager.HideInputBar(BAR_CANDIDATE);
}

void CCommonImeInputer::TargetFocusChange(bool bFocusIn)
{
    if (!bFocusIn)
    {
        g_inputbarmanager.HideInputBar(BAR_INPUT);
        g_inputbarmanager.HideInputBar(BAR_CANDIDATE);
    }
}

bool CCommonImeInputer::TargetKeyPress(int key)
{
    switch (key)
    {
    case PHONE_KEY_LEFT:
        {
            if (g_inputbarmanager.IsVisibleBar(BAR_INPUT))
            {
                SwitchIme(false);
                return true;
            }
        }
        break;
    case PHONE_KEY_RIGHT:
        {
            if (g_inputbarmanager.IsVisibleBar(BAR_INPUT))
            {
                SwitchIme(true);
                return true;
            }
        }
        break;
    default:
        {
            g_inputbarmanager.HideInputBar(BAR_INPUT);
            return false;
        }
        break;
    }

    return false;
}

void CCommonImeInputer::TargetConfirm()
{
    g_inputbarmanager.HideInputBar(BAR_CANDIDATE);
}

bool CCommonImeInputer::IsImeValid(const yl::string & strImeName)
{
    return _ImeLogic.IsVaild(strImeName.c_str());
}

void CCommonImeInputer::OnImeChanged()
{
    CImeInputer::OnImeChanged();

    m_pKeyMap = _ImeLogic.GetKeyMap(m_strIme.c_str());
}

bool CCommonImeInputer::GetCharFromIme(int nKey, IMECHAR & cIme, int & nClickCount)
{
    if (NULL == m_pKeyMap)
    {
        return 0;
    }

    if (GetCurrentImeKey().empty())
    {
        int nIndex = CImeLogic::Key2Index(nKey);

        if ((nKey == PHONE_KEY_LEFT || nKey == PHONE_KEY_RIGHT)
                && g_inputbarmanager.IsVisibleBar(BAR_CANDIDATE))
        {
            nIndex = CImeLogic::Key2Index(m_nLastKey);
        }

        if (nIndex < 0 || nIndex >= KEY_NUM)
        {
            return 0;
        }

        IMESTRING & strImeChar = m_pKeyMap->keyMap[nIndex];

        SetCurrentImeKey(strImeChar);
    }

    const IMESTRING & strImeChar = GetCurrentImeKey();
    int nSize = strImeChar.size();

    if (nSize <= 0)
    {
        return 0;
    }

    //循环返回字符
    if (nClickCount >= nSize)
    {
        nClickCount %= nSize;
    }

    cIme = strImeChar.at(nClickCount);

    // 为了方向键选择，在GetImeChar处理count
    //++nClickCount;

    return true;
}

bool CCommonImeInputer::CheckImeText(const yl::wstring & strWText, yl::wstring & strNewWText)
{
    if (IME_Abc != GetImeName())
    {
        return false;
    }

    strNewWText = strWText;
    strNewWText.to_lower();

    int nSize = strNewWText.size();

    for (int i = 0; i < nSize; ++i)
    {
        if (0 == i || strNewWText[i - 1] == L' ')
        {
            wchar_t wcTmp = strNewWText[i];

            strNewWText[i] = towupper(wcTmp);
        }
    }

    return true;
}

bool CCommonImeInputer::SwitchIme(const yl::string& strImeName /*= ""*/)
{
    if (strImeName.empty())
    {
        SwitchIme(true);
    }
    else
    {
        CImeInputer::SwitchIme(strImeName);
    }

    return true;
}

bool CCommonImeInputer::SwitchIme(bool bNext /*= true*/)
{
    int nSize = m_vecSupImes.size();

    if (nSize <= 0)
    {
        m_strIme = EDIT_TEXT_VIEW_DEFAULT_IME;

        OnImeChanged();

        return false;
    }

    if (bNext)
    {
        ++m_nImeIndex;
    }
    else
    {
        --m_nImeIndex;
        if (m_nImeIndex < 0)
        {
            m_nImeIndex = nSize - 1;
        }
    }
    m_nImeIndex %= nSize;

    m_strIme = GetImeNameByIndex(m_nImeIndex);

    if (m_strIme.empty())
    {
        m_strIme = EDIT_TEXT_VIEW_DEFAULT_IME;
    }

    VEC_STRING vecAllIme;
    GetAllIme(vecAllIme);

    OnImeChanged();
    if (vecAllIme.size() > 1)
    {
        g_inputbarmanager.SetInputInfo(vecAllIme, GetImeName());
        g_inputbarmanager.ShowInputBar(BAR_INPUT, NULL);
        m_tmIme.SingleShot(1000, TIMER_METHOD(this, CCommonImeInputer::OnSwitchImeEnd));
    }
    return true;
}

void CCommonImeInputer::OnSwitchImeEnd(xThreadTimer* timer, LPVOID pExtraData)
{
    g_inputbarmanager.HideInputBar(BAR_INPUT);
}

void CCommonImeInputer::GetAllIme(VEC_STRING &vecAllIme)
{
    yl::string strIme;
    VEC_SUPPORT_IME::iterator it = m_vecSupImes.begin();
    for (; it != m_vecSupImes.end(); ++it)
    {
        strIme = (*it);
        size_t sPos = strIme.find("_");

        if (yl::string::npos != sPos)
        {
            strIme = strIme.substr(0, sPos);
        }
        vecAllIme.push_back(strIme);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
CImeInputer * IME_CreateInputer(CImeTarget * pTarget)
{
    if (NULL == pTarget)
    {
        return NULL;
    }

    CCommonImeInputer * pInput = new CCommonImeInputer(pTarget);

    return pInput;
}

bool IME_ReleaseInputer(CImeInputer * pInputer)
{
    if (NULL == pInputer)
    {
        return false;
    }

    delete pInputer;

    return true;
}

bool IME_InstallIME(CImeTarget * pTarget, LPCSTR lpszIMEName)
{
    if (NULL == pTarget || NULL == lpszIMEName)
    {
        return false;
    }

    pTarget->ClearSupportIme();

    // 防止修改源指针
    LPCSTR lpImeSet = lpszIMEName;
    // 添加输入法
    yl::string strIMEName;

    //没有函数对应
    while (-1 != NS_xWindow::PeekPhraseByCharTokens(lpImeSet, strIMEName, ","))
    {
        strIMEName = strIMEName.trim_both();

        LPCSTR lpTargetImeName = strIMEName.c_str();

#if IF_BUG_26987
        // 关闭Abc输入法
        if (strcmp(lpTargetImeName, IME_Abc) == 0)
        {
            continue;
        }
#endif

        if (0 == strcmp(lpTargetImeName, IME_DIY))
        {
            lpTargetImeName = _ImeLogic.Ime2Name(IT_STD_Custom, 0);
        }

        pTarget->AddSupportIme(lpTargetImeName);
    }

    return true;
}

void IME_UninstallIME(CImeTarget * pTarget)
{
    if (NULL == pTarget)
    {
        return;
    }

    pTarget->ClearSupportIme();
}

void IME_Reload()
{
    _LangHandle.OnLangChanged();
}
