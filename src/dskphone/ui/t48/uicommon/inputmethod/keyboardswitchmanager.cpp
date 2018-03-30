#include "keyboardswitchmanager.h"
#include "keyboardimemanager.h"
#include "keyboardlangmanager.h"
#include "widgetinfomanager.h"

CKeyboardSwitchManager::CKeyboardSwitchManager()
    : m_nCurrentImeIndexInLang(0)
    , m_nCurrentLangIndex(0)
    , m_eCurrentImeType(IME_TYPE_ERROR)
{

}

CKeyboardSwitchManager::~CKeyboardSwitchManager()
{

}

CKeyboardSwitchManager& CKeyboardSwitchManager::GetInstance()
{
    static CKeyboardSwitchManager myKeyboardSwitchManager;
    return myKeyboardSwitchManager;
}

int CKeyboardSwitchManager::SwitchToCapital(INPUT_DLGTYPE eResourceImeType)
{
    INPUTMETHOD_INFO("CKeyboardSwitchManager::SwitchToCapital, Resource Ime = %d", eResourceImeType);

    if (eResourceImeType < INPUT_DLGTYPE_CUSTOM)
    {
        return KEYBOARD_IME_ERROR_INDEX;
    }
    int nImeIndex = (int)eResourceImeType - (int)INPUT_DLGTYPE_CUSTOM;
    const SKeyboardIme* pKeyboardIme = g_KeyBoardImeManager.GetImeByIndex(nImeIndex);
    if (NULL == pKeyboardIme)
    {
        return KEYBOARD_IME_ERROR_INDEX;
    }

    IME_TYPE eImeType = pKeyboardIme->m_eImeType;
    //只有IME_TYPE_CHAR和IME_TYPE_CAPITAL可以进行大小写切换
    if (eImeType != IME_TYPE_CHAR && eImeType != IME_TYPE_CAPITAL)
    {
        return KEYBOARD_IME_ERROR_INDEX;
    }

    int nImeIndexShift;
    //如果是小写键盘则获取对应大写键盘
    if (IME_TYPE_CHAR == eImeType)
    {
        nImeIndexShift = nImeIndex + 1;
    }
    //否则获取小写键盘
    else
    {
        nImeIndexShift = nImeIndex - 1;
    }

    if (nImeIndexShift <= KEYBOARD_IME_ERROR_INDEX || nImeIndexShift >= g_KeyBoardImeManager.GetImeCount())
    {
        return KEYBOARD_IME_ERROR_INDEX;
    }

    const SKeyboardIme* pKeyboardImeShift = g_KeyBoardImeManager.GetImeByIndex(nImeIndexShift);
    if (NULL == pKeyboardImeShift)
    {
        return KEYBOARD_IME_ERROR_INDEX;
    }

    //如果名字相同，说明确实是对应的大小写键盘
    if (pKeyboardIme->m_strImeName == pKeyboardImeShift->m_strImeName)
    {
        INPUTMETHOD_INFO("CKeyboardSwitchManager::SwitchToCapital, Target Ime = %d", nImeIndexShift);
        return nImeIndexShift;
    }

    return KEYBOARD_IME_ERROR_INDEX;
}

int CKeyboardSwitchManager::SwitchToNextLang()
{
    INPUTMETHOD_INFO("CKeyboardSwitchManager::SwitchToNextLang");
    return Switch(true, KEYBOARD_LANG_ERROR_INDEX, false, KEYBOARD_IME_ERROR_INDEX, IME_TYPE_ERROR, false);
}

int CKeyboardSwitchManager::SwitchToNextIme()
{
    INPUTMETHOD_INFO("CKeyboardSwitchManager::SwitchToNextIme");
    return Switch(false, KEYBOARD_LANG_ERROR_INDEX, true, KEYBOARD_IME_ERROR_INDEX, IME_TYPE_ERROR, false);
}

int CKeyboardSwitchManager::FindNextIme()
{
    INPUTMETHOD_INFO("CKeyboardSwitchManager::FindNextIme");
    return Switch(false, KEYBOARD_LANG_ERROR_INDEX, true, KEYBOARD_IME_ERROR_INDEX, IME_TYPE_ERROR, true);
}

int CKeyboardSwitchManager::SwitchLang(int nTargetLangIndex)
{
    INPUTMETHOD_INFO("CKeyboardSwitchManager::SwitchLang, Target Lang = %d", nTargetLangIndex);
    return Switch(false, nTargetLangIndex, false, KEYBOARD_IME_ERROR_INDEX, IME_TYPE_ERROR, false);
}

int CKeyboardSwitchManager::SwitchIme(int nTargetImeIndex)
{
    INPUTMETHOD_INFO("CKeyboardSwitchManager::SwitchIme, Target Ime = %d", nTargetImeIndex);
    return Switch(false, KEYBOARD_LANG_ERROR_INDEX, false, nTargetImeIndex, IME_TYPE_ERROR, false);
}

INPUT_DLGTYPE CKeyboardSwitchManager::GetNewTypeWhenLangChange(INPUT_DLGTYPE eDlgType)
{
    INPUTMETHOD_INFO("CKeyboardSwitchManager::GetNewTypeWhenLangChange, Resource Type = %d", eDlgType);

    IME_TYPE eOriginType = GetTypeByDlgType(eDlgType);
    if (IME_TYPE_ERROR == eOriginType)
    {
        return INPUT_DLGTYPE_CUSTOM;
    }

    int nTargetImeIndex = SwitchImeType(eOriginType);
    if (KEYBOARD_IME_ERROR_INDEX != nTargetImeIndex)
    {
        INPUTMETHOD_INFO("CKeyboardSwitchManager::GetNewTypeWhenLangChange, Target Ime = %d", nTargetImeIndex);
        return (INPUT_DLGTYPE)(INPUT_DLGTYPE_CUSTOM + nTargetImeIndex);
    }

    return INPUT_DLGTYPE_CUSTOM;
}

int CKeyboardSwitchManager::SwitchImeType(IME_TYPE eImeType)
{
    INPUTMETHOD_INFO("CKeyboardSwitchManager::SwitchImeType, Ime Type = %d", eImeType);
    return Switch(false, KEYBOARD_LANG_ERROR_INDEX, false, KEYBOARD_IME_ERROR_INDEX, eImeType, false);
}

int CKeyboardSwitchManager::Switch(bool bLangIndexPlus, int nTargetLangIndex, bool bImeIndexPlus, int nTargetImeIndex, IME_TYPE eImeType, bool bNotSet)
{
    INPUTMETHOD_INFO("CKeyboardSwitchManager::Switch, %d, %d, %d, %d, %d", bLangIndexPlus, nTargetLangIndex, bImeIndexPlus, nTargetImeIndex, eImeType);

    //获取语言列表
    const vecKeyboardLang& tvecKeyboardLang = g_KeyBoardLangManager.GetKeyboardLang();
    int nLangSize = tvecKeyboardLang.size();

    //要求语言数大于0
    if (nLangSize <= 0)
    {
        return KEYBOARD_IME_ERROR_INDEX;
    }

    //如果要切换下一个语言，语言数又小于等于一个，则不需要切
    if (eImeType == IME_TYPE_ERROR
            && nLangSize <= 1
            && bLangIndexPlus)
    {
        return KEYBOARD_IME_ERROR_INDEX;
    }

    //优先判断是否切换到目标语言
    if (KEYBOARD_LANG_ERROR_INDEX != nTargetLangIndex)
    {
        //如果存在对应语言
        if (TraversalAndSetTargetLangIndex(nTargetLangIndex, tvecKeyboardLang))
        {
            //eImeType == IME_TYPE_ERROR说明不是从SwitchImeType进入，所以需要重置
            if (eImeType == IME_TYPE_ERROR)
            {
                g_WidgetInfoManager.ResetAllDlgTypeByLanguage();
            }
            else
            {
                //
            }
        }
        else
        {
            //找不到则不切换
            return KEYBOARD_IME_ERROR_INDEX;
        }
    }
    else
    {
        //如果是要切换下一个，则当前序号加一
        if (bLangIndexPlus)
        {
            //获取下一个Lang
            ++m_nCurrentLangIndex;
            //如果超过边界就置为0
            if (m_nCurrentLangIndex < 0 || m_nCurrentLangIndex >= nLangSize)
            {
                m_nCurrentLangIndex = 0;
            }
            INPUTMETHOD_INFO("CKeyboardSwitchManager::Switch, Next Lang = %d", m_nCurrentLangIndex);

            //eImeType == IME_TYPE_ERROR说明不是从SwitchImeType进入，所以需要重置
            if (eImeType == IME_TYPE_ERROR)
            {
                g_WidgetInfoManager.ResetAllDlgTypeByLanguage();
            }
        }
    }

    //获取当前语言的Ime列表
    if (m_nCurrentLangIndex < 0 || m_nCurrentLangIndex >= nLangSize)
    {
        return KEYBOARD_IME_ERROR_INDEX;
    }
    const SKeyBoardLang& tKeyboardLang = tvecKeyboardLang[m_nCurrentLangIndex];
    const vecKeyboardImeIndex& tvecKeyboardIme = tKeyboardLang.m_vecImeIndex;

    //如果Ime列表没有Ime可选则直接返回
    int nImeSize = tvecKeyboardIme.size();
    if (nImeSize <= 0)
    {
        return KEYBOARD_IME_ERROR_INDEX;
    }

    int nNextIme = m_nCurrentImeIndexInLang;

    //优先判断是否获取某种类型Ime
    if (eImeType != IME_TYPE_ERROR)
    {
        TraversalAndSetTargetImeType(eImeType, tvecKeyboardIme);
    }
    //再判断是否切换到目标Ime
    else if (nTargetImeIndex >= 0)
    {
        if (TraversalAndSetTargetImeIndex(nTargetImeIndex, tvecKeyboardIme))
        {
            //
        }
        else
        {
            return KEYBOARD_IME_ERROR_INDEX;
        }
    }
    //否则属于要切换到下一个Ime的情况
    //有lang变化导致的ime切换、切换到下一个Ime两种情况
    else
    {
        //如果是切换语言导致，则应获取第一个Type相同的Ime
        if (bLangIndexPlus || KEYBOARD_LANG_ERROR_INDEX != nTargetLangIndex)
        {
            TraversalAndSetTargetImeType(m_eCurrentImeType, tvecKeyboardIme);
        }
        //否则获取下一个Ime
        else if (bImeIndexPlus)
        {
            INPUTMETHOD_INFO("CKeyboardSwitchManager::Switch, Current Ime In Lang = %d, Ime Size = %d", m_nCurrentImeIndexInLang, nImeSize);

            //如果最多只有一个，则不需要切
            if (nImeSize <= 1)
            {
                return KEYBOARD_IME_ERROR_INDEX;
            }
            //如果已经是最后一个
            //检测第一个和当前是否是同一个键盘的大小写键盘
            //（即该语言中一共只有这一个键盘的大小写键盘）
            //如果是则不应切换
            else if (m_nCurrentImeIndexInLang == nImeSize - 1)
            {
                const SKeyboardIme* pKeyboardIme = g_KeyBoardImeManager.GetImeByIndex(tvecKeyboardIme[m_nCurrentImeIndexInLang]);
                const SKeyboardIme* pKeyboardImeFirst = g_KeyBoardImeManager.GetImeByIndex(tvecKeyboardIme[0]);

                //如果名字相同说明是同一个键盘的大小写键盘，不需要切
                if (NULL != pKeyboardIme && NULL != pKeyboardImeFirst && pKeyboardIme->m_strImeName == pKeyboardImeFirst->m_strImeName)
                {
                    return KEYBOARD_IME_ERROR_INDEX;
                }
            }
            //如果当前不是最后一个则获取下一个
            else if (m_nCurrentImeIndexInLang < nImeSize - 1)
            {
                const SKeyboardIme* pKeyboardIme = g_KeyBoardImeManager.GetImeByIndex(tvecKeyboardIme[m_nCurrentImeIndexInLang]);
                const SKeyboardIme* pKeyboardImeNext = g_KeyBoardImeManager.GetImeByIndex(tvecKeyboardIme[m_nCurrentImeIndexInLang + 1]);

                //如果名字相同说明下一个是当前的同一个键盘的大小写键盘，因此要多加1
                if (NULL != pKeyboardIme && NULL != pKeyboardImeNext && pKeyboardIme->m_strImeName == pKeyboardImeNext->m_strImeName)
                {
                    ++nNextIme;
                }
            }

            ++nNextIme;
        }
    }

    //超过边界则置0
    if (nNextIme < 0 || nNextIme >= nImeSize)
    {
        nNextIme = 0;
    }

    if (m_nCurrentImeIndexInLang < 0 || m_nCurrentImeIndexInLang >= nImeSize)
    {
        m_nCurrentImeIndexInLang = 0;
    }

    INPUTMETHOD_INFO("CKeyboardSwitchManager::Switch, Current Lang = %d, Current Ime In Lang = %d", m_nCurrentLangIndex, nNextIme);

    if (bNotSet)
    {
        return tvecKeyboardIme[nNextIme];
    }
    else
    {
        if (bImeIndexPlus)
        {
            m_nCurrentImeIndexInLang = nNextIme;
        }

        return tvecKeyboardIme[m_nCurrentImeIndexInLang];
    }
}

void CKeyboardSwitchManager::RecordCurrentIme(INPUT_DLGTYPE eDlgType)
{
    INPUTMETHOD_INFO("CKeyboardSwitchManager::RecordCurrentIme, Dlg = %d", eDlgType);

    //记录类型
    IME_TYPE eOriginType = GetTypeByDlgType(eDlgType);
    if (IME_TYPE_ERROR == eOriginType)
    {
        return;
    }
    m_eCurrentImeType = eOriginType;

    //不是自定义类型就不能计算index
    if (eDlgType < INPUT_DLGTYPE_CUSTOM)
    {
        return;
    }

    //记录Index
    int nTargetImeIndex = (int)eDlgType - (int)INPUT_DLGTYPE_CUSTOM;
    //获取语言列表
    const vecKeyboardLang& tvecKeyboardLang = g_KeyBoardLangManager.GetKeyboardLang();
    int nLangSize = tvecKeyboardLang.size();

    if (nLangSize <= 0
            || m_nCurrentLangIndex < 0
            || m_nCurrentLangIndex >= nLangSize)
    {
        return;
    }
    //获取当前语言的Ime列表
    const SKeyBoardLang& tKeyboardLang = tvecKeyboardLang[m_nCurrentLangIndex];
    const vecKeyboardImeIndex& tvecKeyboardIme = tKeyboardLang.m_vecImeIndex;

    TraversalAndSetTargetImeIndex(nTargetImeIndex, tvecKeyboardIme);
}

IME_TYPE CKeyboardSwitchManager::GetTypeByDlgType(INPUT_DLGTYPE eDlgType)
{
    INPUTMETHOD_INFO("CKeyboardSwitchManager::GetTypeByDlgType, Dlg = %d", eDlgType);

    IME_TYPE eOriginType = IME_TYPE_ERROR;
    if (eDlgType >= INPUT_DLGTYPE_CUSTOM)
    {
        const SKeyboardIme* pKeyboardIme = g_KeyBoardImeManager.GetImeByIndex((int)eDlgType - (int)INPUT_DLGTYPE_CUSTOM);
        if (NULL == pKeyboardIme)
        {
            return IME_TYPE_ERROR;
        }
        eOriginType = pKeyboardIme->m_eImeType;
    }
    else if (INPUT_DLGTYPE_NUM == eDlgType)
    {
        eOriginType = IME_TYPE_SYMBOL;
    }
    else if (INPUT_DLGTYPE_EN == eDlgType)
    {
        eOriginType = IME_TYPE_CHAR;
    }
    else if (INPUT_DLGTYPE_ENCAPITAL == eDlgType)
    {
        eOriginType = IME_TYPE_CAPITAL;
    }

    return eOriginType;
}

bool CKeyboardSwitchManager::TraversalAndSetTargetLangIndex(int nTargetLangIndex, const vecKeyboardLang& tvecKeyboardLang)
{
    INPUTMETHOD_INFO("CKeyboardSwitchManager::TraversalAndSetTargetLangIndex, Target Lang = %d", nTargetLangIndex);

    int nLangSize = tvecKeyboardLang.size();
    if (nLangSize <= 0 || nTargetLangIndex < 0 || nTargetLangIndex >= nLangSize)
    {
        return false;
    }

    //如果找到的目标Lang和原来的Lang相同，则不切换
    if (nTargetLangIndex == m_nCurrentLangIndex)
    {
        return false;
    }

    m_nCurrentLangIndex = nTargetLangIndex;
    //如果超过边界就置为0
    if (m_nCurrentLangIndex >= nLangSize || m_nCurrentLangIndex < 0)
    {
        m_nCurrentLangIndex = 0;
        return false;
    }
    else
    {
        INPUTMETHOD_INFO("CKeyboardSwitchManager::TraversalAndSetTargetLangIndex, Current Lang = %d", m_nCurrentLangIndex);
        return true;
    }
}

bool CKeyboardSwitchManager::TraversalAndSetTargetImeIndex(int nTargetImeIndex, const vecKeyboardImeIndex& tvecKeyboardIme)
{
    INPUTMETHOD_INFO("CKeyboardSwitchManager::TraversalAndSetTargetImeIndex, Target Ime = %d", nTargetImeIndex);

    if (tvecKeyboardIme.size() <= 0)
    {
        return false;
    }

    //记录Ime在Lang组中的Index（不是Ime的Index）
    int nImeIndex = 0;

    bool bFindResult = false;

    vecKeyboardImeIndex::const_iterator iterIme = tvecKeyboardIme.begin();
    for (; iterIme != tvecKeyboardIme.end(); ++iterIme, ++nImeIndex)
    {
        if (nTargetImeIndex == (*iterIme))
        {
            bFindResult = true;
            break;
        }
    }

    //找不到
    if (!bFindResult)
    {
        return false;
    }

    //如果找到的目标Ime和原来的Ime相同，返回false
    if (nImeIndex == m_nCurrentImeIndexInLang)
    {
        return false;
    }

    m_nCurrentImeIndexInLang = nImeIndex;
    if (m_nCurrentImeIndexInLang < 0 || m_nCurrentImeIndexInLang >= tvecKeyboardIme.size())
    {
        m_nCurrentImeIndexInLang = 0;
    }

    INPUTMETHOD_INFO("CKeyboardSwitchManager::TraversalAndSetTargetImeIndex, Current Ime In Lang = %d", m_nCurrentImeIndexInLang);
    return true;
}

bool CKeyboardSwitchManager::TraversalAndSetTargetImeType(IME_TYPE eTargetImeType, const vecKeyboardImeIndex& tvecKeyboardIme)
{
    INPUTMETHOD_INFO("CKeyboardSwitchManager::TraversalAndSetTargetImeType, Target Type = %d", eTargetImeType);

    int nImeIndex = 0;
    bool bFindResult = false;
    //遍历Ime
    vecKeyboardImeIndex::const_iterator iterIme = tvecKeyboardIme.begin();
    for (; iterIme != tvecKeyboardIme.end(); ++iterIme, ++nImeIndex)
    {
        const SKeyboardIme* pKeyboardIme = g_KeyBoardImeManager.GetImeByIndex((*iterIme));
        if (NULL == pKeyboardIme)
        {
            continue;
        }
        //找到相同类型的第一个，直接退出
        if (eTargetImeType == pKeyboardIme->m_eImeType)
        {
            bFindResult = true;
            break;
        }
    }

    //找不到就设置成该语言的第一个键盘
    if (!bFindResult)
    {
        m_nCurrentImeIndexInLang = 0;
    }
    else
    {
        //如果找到的目标Ime和原来的Ime相同，返回false
        if (nImeIndex == m_nCurrentImeIndexInLang)
        {
            return false;
        }

        m_nCurrentImeIndexInLang = nImeIndex;
        if (m_nCurrentImeIndexInLang < 0 || m_nCurrentImeIndexInLang >= tvecKeyboardIme.size())
        {
            m_nCurrentImeIndexInLang = 0;
        }
    }

    INPUTMETHOD_INFO("CKeyboardSwitchManager::TraversalAndSetTargetImeType, Current Ime In Lang = %d", m_nCurrentImeIndexInLang);
    return true;
}

