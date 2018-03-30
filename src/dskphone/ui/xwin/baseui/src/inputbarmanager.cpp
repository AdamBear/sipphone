#include <ylstl/ylstringalgorithm.h>
#include <ylstl/ylstring.h>

#include "inputbarmanager.h"
#include "uimanager/moduimanager.h"
#include "imeedit/ximeedit.h"
#include "xwindow/xWindowManager.h"
#include "globalinputhook/modinputhook.h"

bool InputbarKeyHook(int nKeyCode, bool bPress);

CInputBarManager::CInputBarManager()
    : m_bReayHide(true)
    , m_pHostWidget(NULL)
{
    for (int i = 0; i != BAR_MAX; ++i)
    {
        m_pInputBar[i] = new CInputBar();

        if (m_pInputBar[i] != NULL)
        {
            m_pInputBar[i]->Init();
            m_pInputBar[i]->SetBarType((INPUTBAR_TYPE)i);
            m_pInputBar[i]->hide();
        }
    }
    inputHook_RegisterKeyPadHookProc(InputbarKeyHook, HOOK_MODULE_IME);
}


CInputBarManager::~CInputBarManager()
{
    for (int i = 0; i != BAR_MAX; ++i)
    {
        if (m_pInputBar[i] != NULL)
        {
            delete m_pInputBar[i];
            m_pInputBar[i] = NULL;
        }
    }
    inputHook_UnRegisterKeyPadHookProc(HOOK_MODULE_IME);
}

CInputBarManager & CInputBarManager::GetInstance()
{
    static CInputBarManager * pInputBarManager = NULL;
    if (NULL == pInputBarManager)
    {
        pInputBarManager = new CInputBarManager();
    }
    return *pInputBarManager;
}

void CInputBarManager::ShowInputBar(INPUTBAR_TYPE eType, xView* pEdit)
{
    if (eType < BAR_CANDIDATE || eType >= BAR_MAX)
    {
        return;
    }

    if (m_pInputBar[eType] != NULL)
    {
        chPoint ptShow = GetBarPos(eType, pEdit);

        if (eType == BAR_CANDIDATE)
        {
            if (!m_bReayHide)
            {
                ptShow = m_ptLast;
            }
            else
            {
                // 需要使用新的位置
                m_bReayHide = false;
                m_ptLast = ptShow;
            }
            if (m_pInputBar[BAR_INPUT] != NULL)
            {
                m_pInputBar[BAR_INPUT]->hide();
            }
        }
        else if (eType == BAR_INPUT)
        {
            if (m_pInputBar[BAR_CANDIDATE] != NULL)
            {
                m_pInputBar[BAR_CANDIDATE]->hide();
            }
        }

        if (m_pHostWidget != pEdit)
        {
            m_pHostWidget = pEdit;
        }

        chRect rcBar = m_pInputBar[eType]->geometry();
        m_pInputBar[eType]->setGeometry(chRect(chPoint(ptShow.x, ptShow.y), rcBar.Size()));
        m_pInputBar[eType]->show();
    }
}

void CInputBarManager::HideInputBar(INPUTBAR_TYPE eType)
{
    m_pHostWidget = NULL;

    if (eType < BAR_CANDIDATE || eType >= BAR_MAX)
    {
        return;
    }

    if (m_pInputBar[eType] != NULL)
    {
        if (eType == BAR_CANDIDATE)
        {
            // hide 就表明需要重新获取bar位置
            m_bReayHide = true;
            m_pInputBar[eType]->hide();
        }
        else
        {
            m_pInputBar[eType]->hide();
        }
    }
}

bool CInputBarManager::IsVisibleBar(INPUTBAR_TYPE eType)
{
    if (eType < BAR_CANDIDATE || eType >= BAR_MAX)
    {
        return false;
    }

    if (m_pInputBar[eType] != NULL)
    {
        return m_pInputBar[eType]->isVisible();
    }

    return false;
}

void CInputBarManager::SetBarInfo(int nRepeat, yl::wstring & strSequence)
{
    yl::wstring str = strSequence;
    yl::wstring strTmp;

    int nLen = strSequence.length();

    // 12345      123456
    //   _          _
    int nMid = (BAR_ITEM_COUNT % 2) == 0 ? (BAR_ITEM_COUNT / 2 - 1) : (BAR_ITEM_COUNT / 2);

    // 重复次数对应的Index
    int nIndex = (nRepeat - 1) % nLen;

    if (nLen <= BAR_ITEM_COUNT)
    {
        strTmp = strSequence;
    }
    else
    {
        if (nIndex <= nMid)
        {
            strTmp = str.substr(0, BAR_ITEM_COUNT);
        }
        else
        {
            // 1234|56789|
            //        -
            if (nIndex >= nLen - (BAR_ITEM_COUNT - nMid))
            {
                strTmp = str.substr(nLen - BAR_ITEM_COUNT, BAR_ITEM_COUNT);
                nIndex = nIndex - nLen + BAR_ITEM_COUNT;
            }
            // 1|23456|789
            //     _
            else
            {
                strTmp = str.substr(nIndex - nMid, BAR_ITEM_COUNT);
                nIndex = nMid;
            }
        }
    }

    if (m_pInputBar[BAR_CANDIDATE] != NULL)
    {
        m_pInputBar[BAR_CANDIDATE]->SetBarInfo(nIndex, strTmp.length(), strTmp);
    }
}

int CInputBarManager::FindImeIndex(const VEC_STRING& vecAllIme, const yl::string & strCurIme)
{
    for (int i = 0; i != vecAllIme.size(); ++i)
    {
        if (vecAllIme[i] == strCurIme)
        {
            return i;
        }
    }
    return -1;
}

VEC_STRING CInputBarManager::MidIme(const VEC_STRING& vecAllIme, int nStartPos, int nCount)
{
    VEC_STRING vecString;

    int nSize = vecAllIme.size();
    if (nSize == 0)
    {
        return vecString;
    }

    int i = nStartPos;
    int nCnt = 0;
    for (; i != nSize && nCnt != nCount; ++i, ++nCnt)
    {
        vecString.push_back(vecAllIme[i]);
    }

    return vecString;
}

xView * CInputBarManager::GetHostWidget()
{
    return m_pHostWidget;
}

void CInputBarManager::SetInputInfo(const VEC_STRING& vecAllIme, const yl::string & strCurIme)
{
    yl::string strCur = strCurIme;

    VEC_STRING listShow = vecAllIme;

    // 12345      123456
    //   _          _
    int nMid = (BAR_ITEM_COUNT % 2) == 0 ? (BAR_ITEM_COUNT / 2 - 1) : (BAR_ITEM_COUNT / 2);
    int nLen = vecAllIme.size();

    // 当前输入法的index
    int nIndex = -1;
    nIndex = FindImeIndex(vecAllIme, strCurIme);
    if (nIndex == -1)
    {
        return;
    }

    int nIndexShow = nIndex;

    if (nLen <= BAR_ITEM_COUNT)
    {
        listShow = vecAllIme;
    }
    else
    {
        if (nIndexShow <= nMid)
        {
            listShow = MidIme(vecAllIme, 0, BAR_ITEM_COUNT);
        }
        else
        {
            if (nIndexShow >= nLen - (BAR_ITEM_COUNT - nMid))
            {
                listShow = MidIme(vecAllIme, nLen - BAR_ITEM_COUNT, BAR_ITEM_COUNT);
                nIndexShow = nIndexShow - nLen + BAR_ITEM_COUNT;
            }
            else
            {
                listShow = MidIme(vecAllIme, nIndexShow - nMid, BAR_ITEM_COUNT);
                nIndexShow = nMid;
            }
        }
    }

    if (m_pInputBar[BAR_INPUT] != NULL)
    {
        m_pInputBar[BAR_INPUT]->SetInputInfo(nIndexShow, listShow);
    }
}

chPoint CInputBarManager::GetBarPos(INPUTBAR_TYPE eType, xView * pEdit)
{
    if (eType < BAR_CANDIDATE || eType >= BAR_MAX)
    {
        return chPoint();
    }
    chRect rcPhone = g_WorkStation.getScreenRect();

    if (NULL == pEdit || eType == BAR_INPUT)
    {
        chPoint ptShow;
        if (m_pInputBar[BAR_INPUT] != NULL)
        {
            ptShow = chPoint(rcPhone.Width() / 4,
                             rcPhone.Height()
                             - m_pInputBar[BAR_INPUT]->GetSoftbarHeight()
                             - m_pInputBar[BAR_INPUT]->height() - 1);
            if (ptShow.x + m_pInputBar[BAR_INPUT]->width() > rcPhone.Width())
            {
                ptShow.x = rcPhone.Width() - m_pInputBar[BAR_INPUT]->width();
            }
        }
        return ptShow;
    }


    int nBarWidth = 0;
    int nBarHeight = 0;
    if (m_pInputBar[BAR_CANDIDATE] != NULL)
    {
        nBarWidth = m_pInputBar[BAR_CANDIDATE]->width();
        nBarHeight = m_pInputBar[BAR_CANDIDATE]->height();
    }

    if (pEdit != NULL)
    {
        xEditTextView* pLineEdit = static_cast<xEditTextView*>(pEdit);

        int nCursorPos = pLineEdit->GetCursorPos();
        int nTextLenght = pLineEdit->GetText().length();

        chRect rcLineEdit = pLineEdit->geometry();
        chPoint ptShow;
        chPoint ptReal;

        chRect rcCursor = pLineEdit->GetCursorRectToGlobal();
        ptShow = rcCursor.BottomRight();

        // 全局坐标
        ptShow.x = ptShow.x + rcLineEdit.left;
        ptShow.y = ptShow.y + rcLineEdit.top;
        ptReal = ptShow;
        // 预览框超过lineedit右边，使右边对齐光标
        if (ptShow.x + nBarWidth > rcLineEdit.BottomRight().x)
        {
            ptShow.x = (ptShow.x - nBarWidth);

            // T52 if x < 0 || x > phone_width
            if (ptShow.x < 0)
            {
                ptShow = ptReal;
                ptShow.x = (0);
            }
            if (ptShow.x + nBarWidth > rcPhone.Width())
            {
                ptShow.x = (rcPhone.Width() - nBarWidth);
            }
        }
        // 覆盖了softkey，移动到lineedit上边
        if (ptShow.y + nBarHeight > rcPhone.Height() - m_pInputBar[BAR_INPUT]->GetSoftbarHeight())
        {
            ptShow.y = (rcLineEdit.TopLeft().y - nBarHeight);
        }

        return ptShow;
    }

    return chPoint();
}

bool InputbarKeyHook(int nPhoneKeyCode, bool bPress)
{
    if (bPress && nPhoneKeyCode == PHONE_KEY_OK)
    {
        if (g_inputbarmanager.IsVisibleBar(BAR_CANDIDATE))
        {
            xView* pView = g_inputbarmanager.GetHostWidget();
            if (NULL != pView)
            {
#if IF_XWINDOW_RTTI_CHECK
                xEditTextView * pEdit = dynamic_cast<xEditTextView*>(pView);
#else
                xEditTextView * pEdit = static_cast<xEditTextView*>(pView);
#endif
                if (pEdit != NULL)
                {
                    pEdit->OnPreInputConfirm();
                }
                g_inputbarmanager.HideInputBar(BAR_CANDIDATE);
                return true;
            }
        }
        else if (g_inputbarmanager.IsVisibleBar(BAR_INPUT))
        {
            g_inputbarmanager.HideInputBar(BAR_INPUT);
            return true;
        }
    }
    return false;
}

