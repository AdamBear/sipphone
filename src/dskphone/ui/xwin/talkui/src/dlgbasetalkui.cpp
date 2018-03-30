//////////////talkui//////////////////
#include "dlgbasetalkui.h"
#include "talkuicommonfunction.h"
#include "talkuixmlitemname.h"
#include "talkuipath.h"
#include "talkuiframemanager.h"

#include "modtalkui.h"
#include "messagebox/selectmessagebox.h"
#include "talk/talklogic/include/modtalklogic.h"

//////////////talkuilogic//////////////////
#include "talkuilogic/talkuilogicsoftkey.h"
#include "talkuilogic/talkuilogicdelegate.h"

#include "baseui/include/t2xpicpath.h"

namespace talkui
{

#define SECURITY_MSGBOX_EXIT_TIME   60000
#define MSGBOX_ID_SECURITY              100

/*//////////////////// CDlgBaseTalkUI //////////////////////*/
CDlgBaseTalkUI::CDlgBaseTalkUI(const yl::string & strTalkUIClassName, EWindowType eType)
    : CTalkBaseUI(strTalkUIClassName)
    , m_eWndType(eType)         //窗体类型
    , m_eTitleType(TALKUI_TITLE_TYPE_NORMAL)
    , m_bLayoutLoaded(false)
{

}

CDlgBaseTalkUI::~CDlgBaseTalkUI()
{

}

void CDlgBaseTalkUI::JudgeLoadLayout()
{
    if (!m_bLayoutLoaded)
    {
        m_bLayoutLoaded = true;
        LoadLayout();
    }
}

//获取session状态
SESSION_STATE CDlgBaseTalkUI::GetSessionState()
{
    CBaseTalkUIProcessor * pTalkUIProcessor = GetTalkUIProcessor();

    if (NULL != pTalkUIProcessor)
    {
        return pTalkUIProcessor->GetSessionState();
    }

    return SESSION_UNKNOWN;
}

bool CDlgBaseTalkUI::GetIsFullScreen()
{
    return false;
}

//硬按键预处理
bool CDlgBaseTalkUI::OnKeyPreProcess(int iKey)
{
    CBaseTalkUIProcessor * pTalkUIProcessor = GetTalkUIProcessor();

    if (NULL != pTalkUIProcessor)
    {
        pTalkUIProcessor->DealKeyPreProcess(iKey);
    }

    return false;
}

//硬按键按下处理
bool CDlgBaseTalkUI::OnKeyPress(int iKey)
{
    if (talkuiIsChannelKey(iKey))
    {
        return OnChannelKeyPress(iKey);
    }
    else if (talkuiIsDigitKey(iKey))
    {
        return OnDigitkeyPress(iKey);
    }

    return OnFunckeyPress(iKey);
}

// 通道键处理
bool CDlgBaseTalkUI::OnChannelKeyPress(int iKey)
{
    CBaseTalkUIProcessor * pTalkUIProcessor = GetTalkUIProcessor();

    if (NULL != pTalkUIProcessor)
    {
        return pTalkUIProcessor->DealChannelKeyPress(iKey);
    }

    return false;
}

// 功能键处理
bool CDlgBaseTalkUI::OnFunckeyPress(int iKey)
{
    CBaseTalkUIProcessor * pTalkUIProcessor = GetTalkUIProcessor();

    if (NULL != pTalkUIProcessor)
    {
        return pTalkUIProcessor->DealFunckeyPress(iKey);
    }

    return false;
}

// 数字键处理
bool CDlgBaseTalkUI::OnDigitkeyPress(int iKey)
{
    CBaseTalkUIProcessor * pTalkUIProcessor = GetTalkUIProcessor();

    if (NULL != pTalkUIProcessor)
    {
        return pTalkUIProcessor->DealDigitkeyPress(iKey);
    }

    return false;
}

//按键弹起处理
bool CDlgBaseTalkUI::OnKeyRelease(int iKey)
{
    CBaseTalkUIProcessor * pTalkUIProcessor = GetTalkUIProcessor();

    if (NULL != pTalkUIProcessor)
    {
        return pTalkUIProcessor->DealKeyRelease(iKey);
    }

    return false;
}

//音量条改变
void CDlgBaseTalkUI::OnTalkVolumnValueChange(int iValue)
{
    CBaseTalkUIProcessor * pTalkUIProcessor = GetTalkUIProcessor();

    if (NULL != pTalkUIProcessor)
    {
        pTalkUIProcessor->OnTalkVolumnValueChange();
    }
}

//音量条显示状态改变
void CDlgBaseTalkUI::OnTalkVolumnVisibleChange(bool bVisible)
{
    CBaseTalkUIProcessor * pTalkUIProcessor = GetTalkUIProcessor();

    if (NULL != pTalkUIProcessor)
    {
        pTalkUIProcessor->OnTalkVolumnVisibleChange(bVisible);
    }
}

//刷新通话时间显示
void CDlgBaseTalkUI::UpdateTalkUITime(uint iHour, uint iMin, uint iSecond)
{

}

//获取通话图标
void CDlgBaseTalkUI::GetIconList(YLList<TALKUI_ICON>& listIcons)
{
    listIcons.clear();

    //获取绑定的processor
    CBaseTalkUIProcessor * pTalkUIProcessor = GetTalkUIProcessor();
    if (NULL != pTalkUIProcessor)
    {
        pTalkUIProcessor->GetIconList(listIcons);
    }
}

//是否显示DTMF条
bool CDlgBaseTalkUI::IsShowDtmfLine() const
{
    return false;
}

// 是否显示切换话路的滚动条
bool CDlgBaseTalkUI::IsShowSessionScrollBar()
{
    return true;
}

// 能否切换Session
bool CDlgBaseTalkUI::IsChangeSession(int iKey)
{
    return true;
}

//继承talkuibaseui ///////////////////////////////////
// 初始化
void CDlgBaseTalkUI::Init()
{

}

// 释放数据，将窗.口清空回复到初始化状态
void CDlgBaseTalkUI::Uninit()
{

}

// 刷新窗口
void CDlgBaseTalkUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    CBaseTalkUIProcessor * pTalkUIProcessor = GetTalkUIProcessor();

    if (NULL != pTalkUIProcessor)
    {
        pTalkUIProcessor->DealUpdateWnd();
    }
}

bool CDlgBaseTalkUI::onFocusMove(xView * pNewFocus, xView * pOldFocus)
{
    return false;
}

void CDlgBaseTalkUI::OnFocusChanged(bool bFocused)
{
    CBaseTalkUIProcessor * pTalkUIProcessor = GetTalkUIProcessor();

    if (NULL != pTalkUIProcessor)
    {
        pTalkUIProcessor->OnFocusChanged(bFocused);
    }
}

// 是否跳过Return To Idle，包括通话、Idle
bool CDlgBaseTalkUI::IsRejectReturnToIdle()
{
    return true;
}

//回到Idle前的处理函数
void CDlgBaseTalkUI::OnReturnToIdle()
{
    //暂时不做什么
}

//获取窗体类型
EWindowType CDlgBaseTalkUI::GetWindowType() const
{
    return m_eWndType;
}

TALKUI_TITLE_TYPE CDlgBaseTalkUI::GetTitleType() const
{
    return m_eTitleType;
}

// 获取当前页面音量条的数据
// 返回值：是否显示音量条,默认不显示音量条
// iVolumeType | [out] 音量条的类型
// iMin | [out] 音量条的最小值
// iMax | [out] 音量条的最大值
// strBGPath | [out] 音量条的背景图片
bool CDlgBaseTalkUI::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                     yl::string & strBGPath)
{
    iMin = 1;
    iMax = 15;

    return true;
}

//获取Softkey数据
void CDlgBaseTalkUI::GetSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
    CBaseTalkUIProcessor * pProcessor = GetTalkUIProcessor();

    if (NULL == pProcessor)
    {
        return ;
    }

    //先清除数据
    objSoftkeyData.clear();

    YLVector<SoftKey_TYPE> vectorSoftkey;
    int iSoftkeyCount = 0;
    //从processor中获取 softkey 数据
    pProcessor->GetTalkSoftkeyData(vectorSoftkey);

    iSoftkeyCount = vectorSoftkey.size();
    for (int i = 0; i < iSoftkeyCount; i++)
    {
        CSoftKeyBarData softkeyBarData;

        GetSoftkeyBarDataFromSoftKeyType(softkeyBarData, vectorSoftkey[i]);

        objSoftkeyData.push_back(softkeyBarData);
    }
}

// 是否显示Softkey，默认认为显示
bool CDlgBaseTalkUI::IsShowSoftkey()
{
    return true;
}

#if IF_SUPPORT_SPECIAL_SOFTKEY_BG
//softkey背景
yl::string CDlgBaseTalkUI::GetSoftkeyBG()
{
    return GetIsFullScreen() ? PIC_THEME1_SOFTKEY_NORMAL_BG : PIC_THEME1_SOFTKEY_TRANSPARENT_BG;
}
#endif

// about dsskey api
// 是否显示Dsskey，默认认为不显示 bExpend
bool CDlgBaseTalkUI::IsShowDsskey(bool & bExpend)
{
    return !GetIsFullScreen();
}

#if IF_TALKUI_BG_PICTURE
//////////////////背景///////////////////////////
////设置整个大背景的图片（不是界面自身的背景）
xBrush CDlgBaseTalkUI::GetBackGroundBrush(bool& bResult)
{
#if IF_TALKUI_BG_IDLE_STYLE
    bResult = false;
    return xBrush();
#else
    bResult = true;
    xBrush brushBG;
    brushBG.setPixmap(GetPixmapByString(PIC_DEFAULT_THEME_BG));
    return brushBG;
#endif
}

yl::string CDlgBaseTalkUI::getTalkUIBackGround()
{
    return PIC_TALK_TALKFRAME_BACKGROUND;
}
#endif

/////////////////////////////////////////////////

// 是否允许键盘事件通过
bool CDlgBaseTalkUI::AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress)
{
    CBaseTalkUIProcessor * pProcessor = GetTalkUIProcessor();
    if (NULL != pProcessor)
    {
        return pProcessor->AllowKeyPadEventOnKeyLock(nKeyCode, bPress);
    }

    return false;
}

// 是否为顶层窗体
bool CDlgBaseTalkUI::IsTopWnd()
{
    return false;
}


//////////////////////////////////////////////////
xView * CDlgBaseTalkUI::GetFocusWidget()
{
    return NULL;
}
//////////////////////////////////////////////////


//处理softkey
bool CDlgBaseTalkUI::OnSoftkeyPress(SoftKey_TYPE eSoftType, bool bLongPress)
{
    if (bLongPress)
    {
        return false;
    }

    CBaseTalkUIProcessor * pTalkUIProcessor = GetTalkUIProcessor();

    if (NULL != pTalkUIProcessor)
    {
        return pTalkUIProcessor->DealSoftkeyPress(eSoftType);
    }

    return false;
}

//继承talkuibaseui 结束 ///////////////////////////////////


//获取sessionID
int CDlgBaseTalkUI::GetSessionID()
{
    CBaseTalkUIProcessor * pTalkUIProcessor = GetTalkUIProcessor();

    if (NULL != pTalkUIProcessor)
    {
        return pTalkUIProcessor->GetSessionID();
    }

    return -1;
}

void CDlgBaseTalkUI::ShowSecurityMessageBox(int nAccount)
{
    m_lstAllowLevels.clear();
    talklogic_GetAccountAllowSecurityLevels(m_lstAllowLevels, nAccount);

    TALK_INFO("Security Level counts %d", m_lstAllowLevels.size());

    //没有数据则不弹框
    if (m_lstAllowLevels.size() == 0)
    {
        return;
    }

    MSGBOX_INFO stMsgInfo;

    stMsgInfo.pAction = this;
    stMsgInfo.eType = (MESSAGEBOX_TYPE)MESSAGEBOX_SELECTBOX;
    stMsgInfo.strTitle = _UILOGIC_LANG_TRAN(TRID_SECURITY_CLASSIFICATION);

    stMsgInfo.nMsTime = SECURITY_MSGBOX_EXIT_TIME;
    stMsgInfo.iID = MSGBOX_ID_SECURITY;

    SELECTMSGBOXINFO objSelectBoxInfo;

    yl::string strLevel = talklogic_GetAccountOverrideLevel(nAccount);

    YLList<yl::string>::ListIterator iter = m_lstAllowLevels.begin();
    for (int nIndex = 0; iter != m_lstAllowLevels.end(); ++iter, ++nIndex)
    {
        ST_OPTION_ITEM_DATA objSelectBoxItem;

        objSelectBoxItem.m_nAction = nIndex;

        //获取action
        yl::string strDisp = (*iter);

        //获取title
        strDisp = _UILOGIC_LANG_TRAN(strDisp.c_str());
        if (strLevel == (*iter))
        {
            strDisp += _UILOGIC_LANG_TRAN(TRID_ASSIGNED);
            objSelectBoxInfo.m_SelectIndex = nIndex;
        }
        objSelectBoxItem.m_strTitle = strDisp;

        //加入队列
        objSelectBoxInfo.m_vecSelectMsgBoxItem.push_back(objSelectBoxItem);
    }

    objSelectBoxInfo.m_bShowCheckIcon = true;
    stMsgInfo.extraData = &objSelectBoxInfo;

    MessageBox_ShowCommonMsgBox(stMsgInfo);
}

void CDlgBaseTalkUI::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox
            || MessageBox_OK != pMessageBox->GetResult())
    {
        return;
    }

    if (MSGBOX_ID_SECURITY == pMessageBox->GetID())
    {
        int nAction = *((int *)pMessageBox->GetExtraData());
        int nSize = m_lstAllowLevels.size();

        if (nAction >= 0
                && nAction < nSize)
        {
            bool bFind = false;
            yl::string strResult = "";

            YLList<yl::string>::ListIterator iter = m_lstAllowLevels.begin();
            for (int nIndex = 0; iter != m_lstAllowLevels.end(); ++iter, ++nIndex)
            {
                if (nIndex == nAction)
                {
                    strResult = (*iter);
                    bFind = true;
                    break;
                }
            }

            if (bFind)
            {
                ProcessSecurityCallBack(strResult);
            }
        }
    }
    else
    {
        //other type
    }
}

}       //namespace talkui
