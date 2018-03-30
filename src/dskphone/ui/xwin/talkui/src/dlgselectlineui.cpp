#include "dlgselectlineui.h"

/////////////talkui////////////
#include "talkuicommonfunction.h"
#include "talkuiframemanager.h"
#include "dlgbasetalkui.h"

//////////////////talkuilogic///////////////
#include "talkuilogic/talkuilogicactiondefine.h"
#include "talkuilogic/talkuilogicdelegate.h"

////////////uimanager///////////
#include "uimanager/moduimanager.h"

// #include "uilogiccommon/include/uilogiclanghelper.h"


namespace talkui
{
// 进入选择界面
bool talkui_OpenSelectPage(int nCallerID, LineSelectResult * pSelectResult, vecLineSelectData & vecLineData,
                           yl::string strTitle, bool bShowIndex, int nSelectID)
{
    //选择账号的窗体应为顶层窗口
    CDlgSelectLineUI * pDialog = static_cast<CDlgSelectLineUI *>(UIManager_CreatePrivateDlg(
                                     DLG_SelectLineUI));
    if (pDialog != NULL)
    {
        pDialog->SetSessionData(nCallerID, pSelectResult, vecLineData, strTitle, bShowIndex, nSelectID);
        UIManager_SetTopWindow(pDialog);
        return true;
    }
    return false;
}

// 退出选择界面
void talkui_ExitSelectPage()
{
    UIManager_PutPrivateDlg(DLG_SelectLineUI);
}

//取得实例
CBaseDialog * CDlgSelectLineUI::CreateInstance()
{
    return new CDlgSelectLineUI();
}

CDlgSelectLineUI::CDlgSelectLineUI()
    : CDlgTalkMenuBase(eWT_Unknown, DLG_SelectLineUI)
{
    m_nCallerID = -1;
    m_bShowIndex = false;
    m_nSelectID = 0;
    m_pSelectResult = NULL;
}

CDlgSelectLineUI::~CDlgSelectLineUI()
{

}

// 初始化
void CDlgSelectLineUI::Init()
{
    if (NULL != m_pPageInfo)
    {
        m_pPageInfo->hide(true);
    }

    m_pSelectResult = NULL;
    m_vecLineData.clear();
    m_strTitle = "";
    m_bShowIndex = false;
    m_nSelectID = -1;

    if (NULL != m_pListView)
    {
        m_pListView->SetAdapter(&m_objLineSelectAdapter);
        m_pListView->ClearContent();
    }

    m_objLineSelectAdapter.Init();
}

// 释放数据，将窗.口清空回复到初始化状态
void CDlgSelectLineUI::Uninit()
{
    m_pSelectResult = NULL;
    m_vecLineData.clear();
    m_strTitle = "";
    m_bShowIndex = false;
    m_nSelectID = -1;

    if (NULL != m_pListView)
    {
        m_pListView->ClearContent();
        m_pListView->SetAdapter(NULL);
    }

    m_objLineSelectAdapter.Uninit();
}

// 设置SessionId
void CDlgSelectLineUI::SetSessionData(int nCallerID, LineSelectResult * pSelectResult,
                                      vecLineSelectData & vecLineData, yl::string strTitle, bool bShowIndex, int nSelectID)
{
    //保存数据
    m_nCallerID = nCallerID;
    m_pSelectResult = pSelectResult;
    m_vecLineData = vecLineData;
    m_strTitle = strTitle;
    m_bShowIndex = bShowIndex;
    m_nSelectID = nSelectID;

    //设置显示title
    if (NULL != m_pTitle)
    {
        m_pTitle->SetText(m_strTitle);
    }

    //这个用于给 frame 设置数据
    talkui_vector<yl::string> vectorAccountShowText;

    int iIndex = 0;
    vecLineSelectData::iterator iter = vecLineData.begin();
    for (; iter != vecLineData.end(); ++iter)
    {
        LINE_SELECT_DATA objData = *iter;

        yl::string strName = "";
        if (m_bShowIndex)
        {
            iIndex++;
            char szIndex[8] = { 0 };
            sprintf(szIndex, "%d.", iIndex);
            yl::string strIndex = yl::string(szIndex);

            strName = strIndex + objData.m_strName;
        }
        else
        {
            strName = objData.m_strName;
        }

        vectorAccountShowText.push_back(strName);
    }

    //给控件设置数据
    m_objLineSelectAdapter.SetLineInfo(vectorAccountShowText);

    UpdatePageInfo(vectorAccountShowText.size());
}

//获取Softkey数据
void CDlgSelectLineUI::GetSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
    CSoftKeyBarData softkeyBarData0;

    GetSoftkeyBarDataFromSoftKeyType(softkeyBarData0, ST_CANCEL);
    objSoftkeyData.push_back(softkeyBarData0);


    CSoftKeyBarData softkeyBarData1;
    objSoftkeyData.push_back(softkeyBarData1);

    CSoftKeyBarData softkeyBarData2;
    objSoftkeyData.push_back(softkeyBarData2);


    CSoftKeyBarData softkeyBarData3;

    GetSoftkeyBarDataFromSoftKeyType(softkeyBarData3, ST_SELECT);
    objSoftkeyData.push_back(softkeyBarData3);
}

bool CDlgSelectLineUI::onKeyPressedEvent(int key)
{
    //账号选择界面通常是从拨号界面弹出，因此需要发送事件通知逻辑层，重置自动呼出或界面退出的时间
    _TalkUILogicDelegate.SendUIRequestAction(m_nCallerID, TALK_ACTION_KEYPRESS, 0);

    //如果是通道按键事件  那么交给当前的通话子窗口处理
    if (talkuiIsChannelKey(key))
    {
        CDlgBaseTalkUI * pTalkUI = _TalkUIFrameManager.GetTalkUI();
        if (pTalkUI == NULL)
        {
            return false;
        }

        if (pTalkUI->OnChannelKeyPress(key))
        {
            //退出 该界面
            talkui_ExitSelectPage();

            return true;
        }

        return false;
    }
    //是在 1- 9 的范围内  说明是选择账号
    if (PHONE_KEY_1 <= key && PHONE_KEY_9 >= key)
    {
        //所选择的序号
        int iSelectItemDataIndex = key - PHONE_KEY_1;

        //选择账号
        SelectAccountByIndex(iSelectItemDataIndex);

        return true;
    }
    else if (PHONE_KEY_OK == key)
    {
        OnSoftkeyAction(ST_SELECT, false);
        return true;
    }
    else if (PHONE_KEY_CANCEL == key)
    {
        OnSoftkeyAction(ST_CANCEL, false);
        return true;
    }

    return CDlgTalkMenuBase::onKeyPressedEvent(key);
}

//处理softkey 继承自 talkbaseui
bool CDlgSelectLineUI::OnSoftkeyPress(SoftKey_TYPE eSoftType, bool bLongPress)
{
    if (bLongPress)
    {
        return false;
    }

    TALKUI_INFO("CDlgSelectLineUI-OnSoftkeyAction(%d)", eSoftType);

    switch (eSoftType)
    {
    case ST_SELECT:
        {
            if (NULL != m_pListView)
            {
                //所选择的序号
                //这个是获取 实际的data 序号   而GetFocusItemIndex 是获取的是当页的排序序号
                int iSelectItemDataIndex = m_pListView->GetFocusItemDataIndex();

                //选择账号
                SelectAccountByIndex(iSelectItemDataIndex);
            }
            return true;
        }
        break;
    case ST_CANCEL:
        {
            //Cancel退出事件
            talkui_ExitSelectPage();

            return true;
        }
        break;
    default:
        {
            return false;
        }
        break;
    }

    return true;
}

//根据序号选择账号
void CDlgSelectLineUI::SelectAccountByIndex(int iSelectIndex)
{
    //获取ID
    if (iSelectIndex >= 0 && iSelectIndex < m_vecLineData.size())
    {
        TALKUI_INFO("CDlgSelectPageUI::SelectAccountByIndex strNumber(%d)", iSelectIndex);
        if (NULL != m_pSelectResult)
        {
            m_pSelectResult->OnLineSelectResult(LINE_SELECT_RESULT_SELECT, iSelectIndex, m_nSelectID);
        }

        talkui_ExitSelectPage();
    }
}

}       //namespace talkui


