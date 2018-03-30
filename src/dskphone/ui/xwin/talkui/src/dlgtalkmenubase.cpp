#include "dlgtalkmenubase.h"

/////////////talkui////////////
#include "talkuipath.h"
#include "talkuixmlpath.h"
#include "talkuixmlitemname.h"
#include "talkuicommonfunction.h"
#include "talkuiframemanager.h"

//////////////////talkuilogic///////////////
#include "talkuilogic/talkuilogicactiondefine.h"

////////////uimanager///////////
#include "uimanager/moduimanager.h"

#include "uilogiccommon/include/uilogiclanghelper.h"

namespace talkui
{

CDlgTalkMenuBase::CDlgTalkMenuBase(EWindowType eWndType, const yl::string & strTalkUIClassName)
    : CBaseDialog(this, eWndType)
    , CTalkBaseUI(strTalkUIClassName)
    , m_pTitle(NULL)        //显示title
    , m_pPageInfo(NULL)     //显示选择页信息
    , m_pListView(NULL)
{
    loadContent(TALKUI_DLGTALKUIMENU_XML_PATH);

    //获取控件
    m_pTitle = (xTextView *)this->getViewById(TALKUI_TITLEBAR_MENU_TITLECONTENT_ID);    //title 显示
    //获取焦点行显示
    m_pPageInfo = (xTextView *)this->getViewById(TALKUI_TITLEBAR_MENU_PAGENUM_ID);

    m_pListView = (xListView *)this->getViewById(TALKUI_MENU_LIST);
}

CDlgTalkMenuBase::~CDlgTalkMenuBase()
{

}

// 刷新窗口
void CDlgTalkMenuBase::UpdateWnd(bool bUIReturn/* = false*/)
{
    //刷新softkey
    UpdateDlgSoftkey();

    CBaseDialog::UpdateWnd(bUIReturn);
}

// 焦点改变.
// | bFocused | [in] 为将要改变的焦点状态. true为获取焦点，false为失去焦点
void CDlgTalkMenuBase::OnFocusChanged(bool bFocused)
{
    CBaseDialog::OnFocusChanged(bFocused);
}

// 是否跳过Return To Idle，包括通话、Idle
bool CDlgTalkMenuBase::IsRejectReturnToIdle()
{
    return true;
}

//回到Idle前的处理函数
void CDlgTalkMenuBase::OnReturnToIdle()
{
    CBaseDialog::OnReturnToIdle();
}

// 是否显示Softkey，默认认为显示
bool CDlgTalkMenuBase::IsShowSoftkey()
{
    return true;
}

// about dsskey api
// 是否显示Dsskey，默认认为不显示 bExpend
bool CDlgTalkMenuBase::IsShowDsskey(bool & bExpend)
{
    return false;
}

#if IF_TALKUI_BG_PICTURE
////////////////背景///////////////////////////
//设置整个大背景的图片（不是界面自身的背景）
// xBrush CDlgTalkMenuBase::GetBackGroundBrush(bool& bResult)
// {
//      bResult = true;
//     xBrush brushBG;
//     brushBG.setPixmap(GetPixmapByString(PIC_DEFAULT_THEME_BG));
//     return brushBG;
// }
///////////////////////////////////////////////
#endif

// 是否允许键盘事件通过
//bool CDlgSelectLineUI::AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress)
//{

//}


//////////////////处理弹出框回调///////////////
//virtual void MessageBoxCallBack(CMessageBoxBase* pMessageBox) {}
///////////////////////////////////////////////////

// 是否为顶层窗体
bool CDlgTalkMenuBase::IsTopWnd()
{
    return true;
}

////////////////////////////////////////////////
//xView* CDlgSelectLineUI::GetFocusWidget()
//{

//}
////////////////////////////////////////////////

////是否显示title
//virtual bool IsShowTitle();
////获取显示title信息
//virtual void GetTitleInfo(CTalkTitleInfo& objTalkTitleInfo);

bool CDlgTalkMenuBase::onKeyPressedEvent(int key)
{
    if (PHONE_KEY_1 <= key && PHONE_KEY_9 >= key)
    {
        if (NULL != m_pListView)
        {
            m_pListView->FocusItemByDataIndex(key - PHONE_KEY_1);
        }

        //刷新当前显示 焦点项 信息
        UpdatePageInfo();

        //刷新softkey显示
        UpdateDlgSoftkey();

        return true;
    }

    if ((PHONE_KEY_UP == key) || (PHONE_KEY_DOWN == key))
    {
        if (NULL != m_pListView)
        {
            if (PHONE_KEY_UP == key)
            {
                m_pListView->OnNotifyScrollPrevious();
            }
            else if (PHONE_KEY_DOWN == key)
            {
                m_pListView->OnNotifyScrollNext();
            }
        }

        //刷新当前显示 焦点项 信息
        UpdatePageInfo();

        //刷新softkey显示
        UpdateDlgSoftkey();

        return true;
    }

    return false;
}

bool CDlgTalkMenuBase::OnSoftkeyAction(int iType, bool bLongPress)
{
    return OnSoftkeyPress((SoftKey_TYPE)iType, bLongPress);
}

//刷新显示当前焦点项序号信息
void CDlgTalkMenuBase::UpdatePageInfo(int nTotalCount)
{
    if (NULL == m_pListView
            || NULL == m_pPageInfo)
    {
        return;
    }

    if (m_pListView->GetTotalDataCount() <= 0)
    {
        HidePageInfo();
        return;
    }

    char szData[64] = { 0 };

    if (-1 == nTotalCount)
    {
        int nFocusIndex = m_pListView->GetFocusItemDataIndex();
        nTotalCount = m_pListView->GetTotalDataCount();
        sprintf(szData, "%d/%d", nFocusIndex + 1, nTotalCount);
        if (NULL != m_pPageInfo)
        {
            m_pPageInfo->SetText(szData);
        }
    }
    else
    {
        sprintf(szData, "%d/%d", 1, nTotalCount);
        if (NULL != m_pPageInfo)
        {
            m_pPageInfo->SetText(szData);
        }
    }

    m_pPageInfo->show(xView::SHOW_CMD_BRING_TO_TOP);
}

void CDlgTalkMenuBase::HidePageInfo()
{
    if (NULL != m_pPageInfo)
    {
        m_pPageInfo->hide(true);
    }
}

void CDlgTalkMenuBase::FocusList(bool bCanFocus)
{
    if (NULL != m_pListView)
    {
        if (bCanFocus)
        {
            m_pListView->focusable = xView::bool_true;
            m_pListView->setFocus();
        }
        else
        {
            m_pListView->focusable = xView::bool_false;
        }
    }
}

//刷新 softkey
void CDlgTalkMenuBase::UpdateDlgSoftkey()
{
    CArraySoftKeyBarData arraySoftkey;
    GetSoftkey(arraySoftkey);

    SetSoftKey(arraySoftkey);
}

}       //namespace talkui


