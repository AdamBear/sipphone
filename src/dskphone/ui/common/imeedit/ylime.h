#ifndef __IME_YL_IME_H__
#define __IME_YL_IME_H__

#include <ETLLib/ETLLib.hpp>
#include <ylstring.h>
#include <ylhashmap.h>
#include <ylvector.h>
#include <utility.hpp>
#include <imeedit/imeeditdefine.h>
#include "imecommon.h"
#include "xwindow-kernel/include/xkernel.h"
#include "xwindow-kernel/include/xthreadtimer.h"


USING_XKERNEL_NAMESPACE

typedef yl::wstring             IMESTRING;
typedef IMESTRING::value_type   IMECHAR;

class CImeInputer;
class CImeTarget;

// CImeInputer
class CImeInputer : private yl::NonCopyable, public xTimerHandler
{
public:
    CImeInputer();
    explicit CImeInputer(CImeTarget* pTarget);
    virtual ~CImeInputer();

public:
    virtual bool SendImeKey(int nKey);
    bool SetIme(const yl::string& strImeName);
    virtual bool SwitchIme(const yl::string& strImeName = "");
    bool AddSupportIme(const yl::string& strImeName);
    bool ClearSupportIme();

    virtual void TargetFocusChange(bool bFocusIn) {}
    virtual bool TargetKeyPress(int key)
    {
        return false;
    }
    virtual void TargetConfirm() {}

    yl::string GetImeName() const;
    yl::string GetCurrentIme() const;

    virtual IMECHAR GetImeChar(int nKey);

    void ResetCount();
    void ResetTarget(CImeTarget* pTarget);

    virtual void OnImeEnd(xThreadTimer* timer, LPVOID pExtraData);

    virtual bool CheckImeText(const yl::wstring& strWText, yl::wstring& strNewWText)
    {
        return false;
    }

protected:
    virtual bool IsImeValid(const yl::string& strImeName);
    virtual void OnImeChanged();

    virtual bool GetCharFromIme(int nKey, IMECHAR& cIme, int& nClickCount)
    {
        return false;
    }

    int GetImeIndex(const yl::string& strImeName);
    yl::string GetImeNameByIndex(int nIndex);

    bool IsOneChar() const
    {
        return (1 == m_strCurrentImeKey.size());
    }
    void SetCurrentImeKey(const IMESTRING& strImeKey)
    {
        m_strCurrentImeKey = strImeKey;
    }
    const IMESTRING& GetCurrentImeKey() const
    {
        return m_strCurrentImeKey;
    }

protected:
    typedef YLVector<yl::string> VEC_SUPPORT_IME;
    VEC_SUPPORT_IME         m_vecSupImes;
    yl::string              m_strIme;
    int                     m_nLastKey;

protected:
    IMESTRING               m_strCurrentImeKey;
    int                     m_nImeIndex;
    int                     m_nClickCount;

    xThreadTimer            m_tmIme;         //ime定时器
    CImeTarget*             m_pTarget;
};

//IMETarget
class CImeTarget
{
public:
    CImeTarget();
    virtual ~CImeTarget();

    static void SetCreateInputerFun(FUN_CREATE_INPUTER pFunCreateInputer);
    static void SetReleaseInputerFun(FUN_RELEASE_INPUTER pFunReleaseInputer);

public:
    //Target->Inputer
    bool EndIme();
    bool SendImeKey(int iKey);
    bool SetIme(const yl::string& strImeName);
    bool SwitchIme(const yl::string& strImeName = "");
    bool AddSupportIme(const yl::string& strImeName);
    bool ClearSupportIme();

    void TargetFocusChange(bool bFocusIn);
    bool TargetKeyPress(int key);
    void TargetConfirm();

    //Input->Target
    virtual bool OnChar(IMECHAR ch) = 0;
    virtual bool OnPreChar(IMECHAR ch) = 0;

    virtual bool OnPreInputConfirm() = 0;
    virtual bool OnImeChange(const yl::string& strImeName) = 0;

    yl::string GetImeName() const;
    yl::string GetCurrentIme() const;

    bool CheckImeText(const yl::wstring& strWText, yl::wstring& strNewWText);

protected:
    CImeInputer*    m_pInputer;
    static FUN_CREATE_INPUTER   s_pFunCreateInputer;
    static FUN_RELEASE_INPUTER  s_pFunReleaseInputer;
};

#endif
