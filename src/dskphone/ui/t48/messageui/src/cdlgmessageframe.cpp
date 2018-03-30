#include "cdlgmessageframe.h"
#include "cdlgmessageedit.h"
#include "cmsguilogic.h"
#include <QtGui>

#include "uimanager/uimanager_inc.h"
#include "uimanager/rectdefine.h"
#include "uimanager/uimanager_header.h"
#include "uimanager/uimanager_common.h"

#include "imagemanager/modimagemanager.h"
#include "setting/src/menu/menucontroller.h"
#include "setting/include/common.h"
#include "settingui/src/cdlgbasesubpage.h"
#include "inputmethod/inputmethod.h"
#include "qtcommon/qmisc.h"
#include "mainwnd/mainwnd.h"
#ifdef _T3X
#include "T3XStyleUI/T3XPicPath.h"
#endif
#ifdef _T4X
#include "baseui/t4xpicpath.h"
#endif
#include "commonunits/modcommonunits.h"
#include "uikernel/scrollpage.h"
#include "uikernel/languagehelper.h"
#include "messagebox/modmessagebox.h"
#include "messagebox/messageboxbase.h"

#include "setting/src/textmessagecontroller.h"
#include "translateiddefine.h"

#include "uicommon.h"
#include "baseui/t4xpicpath.h"

namespace
{
// message id
#define MSGBOX_ID_DELETE_ALL    91

//#define LOAD_ALL_DATA_FROM_LOGIC
}

///////////////////////////////////////////////////////////////////////////////
CDlgMessageFrame::CDlgMessageFrame(QWidget * parent /*= NULL*/)
    : CDlgBaseSubPage(parent)
{
    SetWindowType(eWT_Message);

    InitData();
}

CDlgMessageFrame::~CDlgMessageFrame()
{
    if (NULL != m_pGeneralFrame)
    {
        m_pGeneralFrame->SetDelegate(NULL);
    }

    if (NULL != m_pMsgListDelegate)
    {
        m_pMsgListDelegate->UnBindFramelist();
    }

    //退出时清除当前box类型信息
    g_pMsgUILogic->ExitMessageDetail();

}

void CDlgMessageFrame::InitData()
{
    this->installEventFilter(this);

    m_pMsgListDelegate = new MsgCommonDelegate();
    m_pMsgDataDelegate = new MsgDataDelegate();

    if (NULL != m_pGeneralFrame && NULL != m_pMsgListDelegate && NULL != m_pMsgDataDelegate)
    {
        m_pMsgListDelegate->setParent(this);
        //绑定列表
        m_pMsgListDelegate->BindFramelist(m_pGeneralFrame);
        m_pMsgListDelegate->InitSettingListStyle();
        m_pMsgListDelegate->SetItemParent(this);

        m_pMsgDataDelegate->setParent(this);
        m_pMsgListDelegate->BindDataDelegate(m_pMsgDataDelegate);

        connect(m_pMsgListDelegate, SIGNAL(listItemAction(int, QString)), this, SLOT(OnMyListItemAction(int,
                QString)));
    }
}

void CDlgMessageFrame::UpdateWnd()
{
    if (NULL != m_pGeneralFrame)
    {
        m_pGeneralFrame->setGeometry(0, 0, 536, 336);
    }

    if (NULL == m_pMsgListDelegate)
    {
        return;
    }

    m_pMsgListDelegate->UpdateItemStatus(true);

    m_pMsgListDelegate->UpdateList();
}

void CDlgMessageFrame::OnReShow()
{
    UpdateWnd();
}

void CDlgMessageFrame::Init()
{
    //获取对象名，作为box类型信息
    yl::string strAction = objectName().toUtf8().data();

    yl::string strLogicAction = g_pMsgUILogic->GetBoxAction();
    if ("" == strLogicAction)
    {
        //如果MsgUiLogic里没有box类型信息，则设置box类型
        g_pMsgUILogic->SetBoxAction(strAction);
    }
    else
    {
        //如果MsgUiLogic里已经存有box类型信息，说明该列表窗口不是从menu打开的，而是从dsskey或idle的弹出框回调打开的
        strAction = strLogicAction;
    }

    g_pMsgUILogic->LoadMsgList(0);

    emit RefreshSoftkey();
}

//在退出前，重置Message界面，以便下一次显示
void CDlgMessageFrame::Uninit()
{

}

void CDlgMessageFrame::SetMessageList(TextMsgSimpListData & sMsgList)
{
    if (NULL != m_pMsgListDelegate)
    {
        QWPVector   m_vecWidgetPair;
        YLList<TextMsgItemSimpData>::ListIterator iter = sMsgList.m_listMessage.begin();
        YLList<TextMsgItemSimpData>::ListIterator listEnd  = sMsgList.m_listMessage.end();
        for (; iter != listEnd; ++iter)
        {
            QWidgetPair qPair;
            qPair.eType = MESSAGE_LIST_ITEM;

            qPair.bAddIndex = false;
            qPair.first = (*iter).m_strName.c_str();
            qPair.bAddIcon = true;
            qPair.strIcon = (*iter).m_strIconPath;

            QLabel * pLalTime = new QLabel();
            pLalTime->setText(Timet2String((*iter).m_tTime).c_str());
            qPair.second = pLalTime;

            m_vecWidgetPair.push_back(qPair);
        }

        m_pMsgListDelegate->SetContent(&m_vecWidgetPair);

        UpdateWnd();
    }
}

void CDlgMessageFrame::ClearListView()
{
    if (NULL != m_pMsgListDelegate)
    {
        m_pMsgListDelegate->ClearContent(true);
    }
}

bool CDlgMessageFrame::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE_ALL);
    objWndData[0].m_strSoftkeyAction = DM_SOFTKEY_CLICK_CLEAR;
    objWndData[0].m_strIconPath = PIC_SOFTKEY_REMOVE;

    objWndData[1].m_strSoftkeyTitle = "";
    objWndData[1].m_strSoftkeyAction = "";

    objWndData[2].m_strSoftkeyTitle = "";
    objWndData[2].m_strSoftkeyAction = "";

    objWndData[3].m_strSoftkeyTitle = "";
    objWndData[3].m_strSoftkeyAction = "";

    correctSofkeyBarData(objWndData);

    return true;
}

//根据界面内容修正Softkey的显示内容
void CDlgMessageFrame::correctSofkeyBarData(CArraySoftKeyBarData & softkeyData)
{
    //如果当前box内邮件数量为空则清空softkey
    if (m_pMsgListDelegate != NULL && m_pMsgListDelegate->IsEmpty())
    {
        CArraySoftKeyBarData::iterator iter = softkeyData.begin();

        for (; iter != softkeyData.end(); ++iter)
        {
            iter->m_strSoftkeyAction.clear();
            iter->m_strSoftkeyTitle.clear();
        }
    }
}

//具体处理响应事件
bool CDlgMessageFrame::OnAction(const QString & strAction)
{
    if (strAction == DM_SOFTKEY_CLICK_CLEAR)
    {
        //清除当前box内所有邮件
        OnMessageDeleteAll();
        return true;
    }
    else
    {
        return false;
    }
}

//当item被点击时的响应
void CDlgMessageFrame::OnMyListItemAction(int eActionType, QString strItemAction)
{
    if (eActionType == LIST_ITEM_ACTION_SELECT)
    {
        if (NULL != m_pMsgListDelegate && this->isVisible())
        {
            m_iCurrentMsgid = m_pMsgListDelegate->GetFocusItemDataIndex();
            g_pMsgUILogic->SetCurrentMsgId(m_iCurrentMsgid);
            OpenViewPage();
        }
    }
}

//根据box类型打开查看或编辑窗口
void CDlgMessageFrame::OpenViewPage()
{
    yl::string strMsgBoxAction = g_pMsgUILogic->GetBoxAction();
    //如果是收件箱、发件箱、已发送，则打开查看窗口
    if (kszCAInBox == strMsgBoxAction
            || kszCASentBox == strMsgBoxAction
            || kszCAOutBox == strMsgBoxAction)
    {
        SubMenuData subData;
        subData.strMenuAction = "OpenSubPage(text_message_view_message)";
        emit openpage(subData);
    }
    //如果是草稿箱则打开编辑窗口
    else if (kszCADraftBox == strMsgBoxAction)
    {
        SubMenuData subData;
        subData.strMenuAction = "OpenSubPage(text_message_edit_message)";
        emit openpage(subData);
    }
    else
    {
        //
    }
}

//删除所对应的列表所有Message
void CDlgMessageFrame::OnMessageDeleteAll()
{
    if (NULL != m_pMsgListDelegate && !m_pMsgListDelegate->IsEmpty())
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_IF_CLEAR_THIS_GROUP_MESSAGE),
                                    MESSAGEBOX_OkCancel, -1,  "", "",
                                    MSGBOX_ID_DELETE_ALL);
    }
}

//切页
void CDlgMessageFrame::OnCurrentPageChanged(int page)
{
    g_pMsgUILogic->LoadMsgList(page);
}

//处理切页事件
//其他事件交给子页面
bool CDlgMessageFrame::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (NULL == pObj
            || NULL == pEvent)
    {
        return true;
    }

    switch (pEvent->type())
    {
    case QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;

            if (NULL != m_pMsgListDelegate && m_pMsgListDelegate->ProcessKeyPress(pKeyEvent->key()))
            {
                return true;
            }
        }
        break;
    default:
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObj, pEvent);
}

void CDlgMessageFrame::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nResult = pMessageBox->GetResult();
    switch (pMessageBox->GetID())
    {
    case MSGBOX_ID_DELETE_ALL:
        {
            if (MessageBox_OK == nResult)
            {
                g_pMsgUILogic->ClearCurrentMsgBoxMessages();

                ClearListView();
                UpdateWnd();

                //如果这里不刷softkey的话，清空当前box内的消息后delete all的softkey就不会消失掉
                emit RefreshSoftkey();
            }
        }
        break;

    default:
        break;
    }
}

void CDlgMessageFrame::SetDelStatus(bool bDel /*= false*/)
{
#ifdef LOAD_ALL_DATA_FROM_LOGIC
    if (NULL != m_pMsgListDelegate)
    {
        m_pMsgListDelegate->SetDeleteItem(bDel);
    }
#endif
}
