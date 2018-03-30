#include "dlgkbtrigger.h"

#ifdef _CP920
#include "globalinputhook/modinputhook.h"
#include "baseui/include/modbaseui.h"


namespace
{
const char * DLG_KB_TRIGGER_LAYOUT_FILE = "testmode/dlgkbtrigger.xml";
const char * DLG_KB_TRIGGER_NAME_TEXT_KEY = "txtKey";
}

CDlgKeyTrigger::CDlgKeyTrigger()
    : CBaseDialog(this, eWT_TestMode)
    , m_pTextView(NULL)
{

}

CDlgKeyTrigger::~CDlgKeyTrigger()
{

}

CBaseDialog * CDlgKeyTrigger::CreateInstance()
{
    CDlgKeyTrigger * pKeyPadUI = new CDlgKeyTrigger();

    if (NULL != pKeyPadUI)
    {
        pKeyPadUI->loadContent(DLG_KB_TRIGGER_LAYOUT_FILE);
    }

    return pKeyPadUI;
}

void CDlgKeyTrigger::Init()
{

}

void CDlgKeyTrigger::Uninit()
{

}

void CDlgKeyTrigger::loadChildrenElements(xml_node & node)
{
    xLinearLayout::loadChildrenElements(node);

    m_pTextView = static_cast<xTextView *>(getViewById(DLG_KB_TRIGGER_NAME_TEXT_KEY));
}

bool CDlgKeyTrigger::IsRejectReturnToIdle()
{
    return true;
}

bool CDlgKeyTrigger::IsShowSoftkey()
{
    return false;
}

bool CDlgKeyTrigger::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (NULL == m_pTextView || !bPress)
    {
        return true;
    }

    int nRawKey = 0;

#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
    nRawKey = inputHook_GetCurrentRawKey();
#endif

    char szBuffer[32];

    _snprintf(szBuffer, sizeof(szBuffer), "%d ", nRawKey);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    yl::string strText = m_pTextView->GetText() + szBuffer;

    m_pTextView->SetText(strText);

    // 超出显示范围，则最多删除最前面的两次输入
    for (int i = 0; i < 2; ++i)
    {
        const chSize & sizePaint = m_pTextView->CachePaintTextSize();
        chRect rcText = m_pTextView->GetTextRect();

        if (rcText.Height() >= sizePaint.cy)
        {
            break;
        }

        yl::string::size_type nPos = strText.find(" ");

        if (yl::string::npos != nPos)
        {
            strText = strText.substr(nPos + 1);

            m_pTextView->SetText(strText);
        }
    }

    return true;
}
#endif //IF_KEYBOARD_AUTO_TEST
