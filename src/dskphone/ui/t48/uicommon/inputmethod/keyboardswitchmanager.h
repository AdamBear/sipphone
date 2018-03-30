#ifndef CKEYBOARDSWITCHMANAGER_H
#define CKEYBOARDSWITCHMANAGER_H

#include "inputmanagerdefine.h"
#include "datastruct.h"

class CKeyboardSwitchManager
{
private:
    CKeyboardSwitchManager();
    ~CKeyboardSwitchManager();

public:
    static CKeyboardSwitchManager& GetInstance();

    //切换到下一组Lang，返回要显示的Ime的Index
    int SwitchToNextLang();
    //切换到下一个Ime，返回要显示出的Ime的Index
    int SwitchToNextIme();
    //获取下一个Ime，返回要显示出的Ime的Index
    int FindNextIme();

    //切换到目标Lang，返回要显示的Ime的Index
    int SwitchLang(int nTargetLangIndex);
    //切换到目标Ime，返回要显示出的Ime的Index
    int SwitchIme(int nTargetImeIndex);

    //在当前语言内获取首个某种类型Ime
    int SwitchImeType(IME_TYPE eImeType);

    //切换到大写/小写
    int SwitchToCapital(INPUT_DLGTYPE eResourceImeType);

    //当语言变化时，根据原有的INPUT_DLGTYPE获取新的相同imeType的INPUT_DLGTYPE
    INPUT_DLGTYPE GetNewTypeWhenLangChange(INPUT_DLGTYPE eDlgType);

    //根据INPUT_DLGTYPE记录当前的ImeType和ImeIndex
    void RecordCurrentIme(INPUT_DLGTYPE eDlgType);

    //获取当前使用的Lang Index
    int GetCurrentLang()
    {
        return m_nCurrentLangIndex;
    }

private:
    int Switch(bool bLangIndexPlus
               , int nTargetLangIndex
               , bool bImeIndexPlus
               , int nTargetImeIndex
               , IME_TYPE eImeType
               , bool bNotSet);

    //遍历查找nTargetLangIndex的Lang，如果存在就记录下来
    bool TraversalAndSetTargetLangIndex(int nTargetLangIndex, const vecKeyboardLang& tvecKeyboardLang);

    //根据ImeIndex遍历查找，如果存在于当前lang内就记录下来
    bool TraversalAndSetTargetImeIndex(int nTargetImeIndex, const vecKeyboardImeIndex& tvecKeyboardIme);

    //根据ImeType遍历查找，如果找到对应类型就记录下来，没有找到就置为第一个
    bool TraversalAndSetTargetImeType(IME_TYPE eTargetImeType, const vecKeyboardImeIndex& tvecKeyboardIme);

    //根据原有的DlgType，转换成IME_TYPE
    IME_TYPE GetTypeByDlgType(INPUT_DLGTYPE eDlgType);

private:
    //当前使用的ime的序号（在Lang结构内的顺序，不是Ime的序号）
    int             m_nCurrentImeIndexInLang;
    //当前使用的lang的序号（在langmanager里的顺序）
    int             m_nCurrentLangIndex;

    //当前的imeType
    IME_TYPE    m_eCurrentImeType;
};

#define g_KeyBoardSwitchManager (CKeyboardSwitchManager::GetInstance())

#endif // CKEYBOARDSWITCHMANAGER_H
