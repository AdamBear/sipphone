#ifndef __INPUT_BAR_MANAGER_H__
#define __INPUT_BAR_MANAGER_H__

#include <ylstring.h>

#include "inputbar.h"
#include "inputbarcommon.h"
typedef YLVector<yl::string> VEC_STRING;
class CInputBarManager
{
public:
    CInputBarManager();
    ~CInputBarManager();

    static CInputBarManager & GetInstance();

public:
    void ShowInputBar(INPUTBAR_TYPE eType, xView* pEdit = NULL);
    void HideInputBar(INPUTBAR_TYPE eType);
    bool IsVisibleBar(INPUTBAR_TYPE eType);

    void SetBarInfo(int nRepeat, yl::wstring & strSequence);
    void SetInputInfo(const VEC_STRING& vecAllIme, const yl::string & strCurIme);

    chPoint GetBarPos(INPUTBAR_TYPE eType, xView * pEdit);

    int FindImeIndex(const VEC_STRING& vecAllIme, const yl::string & strCurIme);
    // 截取vec 从nStartPos开始的count个元素
    VEC_STRING MidIme(const VEC_STRING& vecAllIme, int nStartPos = 0, int nCount = -1);

    xView * GetHostWidget();
private:
    CInputBar* m_pInputBar[BAR_MAX];

    chPoint m_ptLast;
    bool m_bReayHide;
    xView * m_pHostWidget;
};

#define g_inputbarmanager CInputBarManager::GetInstance()
#endif
