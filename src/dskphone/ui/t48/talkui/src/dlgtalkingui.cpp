// mod-this(TalkUI)
#include "dlgtalkingui.h"
#include "talkui_define.h"
#include "talkuimanager.h"
#include "contactinfoframe.h"
#include "commonwidget.h"
#include "commonfunction.h"

#include "talkuidelegate.h"

// mod-UIKernel
#include "uikernel/quipushbutton.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "uikernel/qscrolltextlabel.h"
#include "uikernel/qtimepiece.h"

// mod-MainWnd
#include "mainwnd/mainwnd.h"

// mod-Common
#include "imagemanager/modimagemanager.h"
#include "keymap.h"
#include <configiddefine.h>
#include "picpath.h"
#include "commonunits/modcommonunits.h"

// mod-Talk
// #include "talk/talklogic_inc.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talklogic/include/callinfo.h"
// #include "talk/talklogicmanager.h"

// mod-CallCenter
#include "broadsoft/callcenter/include/modcallcenter.h"

// mod-MainWnd
#include "uimanager/rectdefine.h"


#include "baseui/modbaseui.h"

#include "talk/talklogic/include/commonfunctions.h"

#include "callmanager/modcallmanager.h"

#include "uicommon/messagebox/modmessagebox.h"

// PicPath
#ifdef _T3X
#include "T3XStyleUI/T3XPicPath.h"
#endif

#ifdef _T4X
#include "baseui/t4xpicpath.h"
#endif

#include "talk/talklogic/include/modtalklogic.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "settingui/include/modsettingui.h"
#include "setting/include/common.h"

#include "talkuicommon.h"
#include "networkconfmgritem.h"

namespace
{
//计时控件的位置
const QRect CONTACT_INFO_FRAME_GEOMETRY(6, 0, RECT_TALK_FRAME.width() - 8,
                                        RECT_TALK_FRAME.height());
const QRect TIME_PIECE_GEOMETRY(RECT_TALK_PAGE.width() - 118, 119, 110, 30);
const QRect COVER_LABEL_GEOMETRY(19, 13, 106, 106);
const QRect STATUS_ICON_LAB_GEOMETRY = COVER_LABEL_GEOMETRY;
const QRect STATUS_ICON_LAB_GEOMETRY_NO_CONTACTIMAGE(18, 78, 410, 25);
const QRect INCOMING_CALL_ANIMATION_GEOMETRY(2, RECT_TALK_PAGE.height() - 30, 26, 24);
const QRect INCOMING_CALL_TIP_GEOMETRY(INCOMING_CALL_ANIMATION_GEOMETRY.right(),
                                       RECT_TALK_PAGE.height() - 34, 125, 32);
const QRect INCOMING_CALL_NUM_GEOMETRY(INCOMING_CALL_TIP_GEOMETRY.right(),
                                       RECT_TALK_PAGE.height() - 34, RECT_TALK_PAGE.width() - INCOMING_CALL_TIP_GEOMETRY.right() - 6, 32);

#define TALK_CONF_MGR_MAX_PAGE_ITEM_COUNT 5
#define TALK_CONF_MGR_LIST_SAPCING 32
#define TALK_CONF_MGR_LIST_ITEM_LEFT_SPACE 0
#define TALK_CONF_MGR_LIST_DEFAULT_FONT_SIZE 18
const QColor ITEM_COLOR(Qt::white);
const QColor GRID_COLOR = QColor(162, 161, 161, 255);
const QRect CONF_MGR_RECT_IN_TALK(0, 0, 480, 287);
}

// 重新加载数据
void INetworkManagerListDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex,
        int nDataCount)
{
    if (NULL == pFrameList || nDataIndex > m_vecData.size())
    {
        return;
    }

    //记录当前序号
    m_iCurrentIndex = nDataIndex;

    pFrameList->ClearContent();

    int iPageMaxItemCount = pFrameList->GetMaxPageItemCount();
    int iTotal = m_vecData.size();
    int iItemToMGet = nDataCount > iPageMaxItemCount ? iPageMaxItemCount : nDataCount;
    int iMaxIndex = iItemToMGet + nDataIndex;

    pFrameList->SetTotalDataCount(m_vecData.size());

    for (int i = nDataIndex; i < iMaxIndex && i < iTotal; i++)
    {
        CNetworkMgrItem * pItem = new CNetworkMgrItem(pFrameList);
        QObject::connect(pItem, SIGNAL(RequestRemoveMember(QString, QString)), pFrameList->parent(),
                         SLOT(OnRemoveMember(QString, QString)));
        pItem->setObjectName(QString("CNetworkMgrItem%1").arg(i));
        pItem->SetName(m_vecData[i].m_strName);
        pItem->SetUserID(toQString(m_vecData[i].m_strID));
        pItem->SetAllowRemove(m_vecData[i].m_bRemove);
        pItem->SetDataIndex(i);

        pItem->SetFontSize(TALK_CONF_MGR_LIST_DEFAULT_FONT_SIZE);
        pItem->SetFontColor(ITEM_COLOR);

        if (pFrameList->GetMaxPageItemCount() > 0)
        {
            pItem->setFixedHeight(pFrameList->height() / pFrameList->GetMaxPageItemCount());
        }

        pFrameList->AddItem(pItem);
        pItem->show();
    }

    pFrameList->update();
}

void INetworkManagerListDelegate::SetData(const std::vector<NetworkConfMgrData> & vecData)
{
    m_vecData = vecData;
}

//重置序号
void INetworkManagerListDelegate::ResetIndex()
{
    m_iCurrentIndex = 0;
}

//获取序号
int INetworkManagerListDelegate::GetCurrentIndex()
{
    return m_iCurrentIndex;
}


CDlgTalkingUI::CDlgTalkingUI()
    : m_pListFrame(NULL)
    , m_bBWConfMgr(false)
{
    InitData();
}

CDlgTalkingUI::~CDlgTalkingUI(void)
{
    if (NULL != m_pListFrame)
    {
        m_pListFrame->SetDelegate(NULL);
    }
}

// 初始化
void CDlgTalkingUI::Init()
{
    CDlgTalkingBase::Init();

    //将标志复原
    m_bBWConfMgr = false;

    m_listDeleget.ResetIndex();

    ShowFailInfo(false);
    m_HoldFrame.hide();

    //重新启动计时器
    m_qTimePiece.StopTimer();
    m_qTimePiece.StartTimer();

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    if (0 != configParser_GetConfigInt(kszUSBAutoRecordEnable))
    {
        TALKUI_INFO("Auto record when talk established.");
        StartUsbRecord();
    }
#endif
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgTalkingUI::Uninit()
{
    m_bBWConfMgr = false;

    //停止计时器
    m_qTimePiece.StopTimer();

    CDlgTalkingBase::Uninit();
}

//给界面设置数据
void CDlgTalkingUI::SetTalkData(DataLogic2UIBase * pData)
{
    HideAllWidget();

    //基类设置数据
    CDlgTalkingBase::SetTalkData(pData);

    // 入口参数检查
    DataTalking2UI * pTalkingData = dynamic_cast<DataTalking2UI *>(pData);
    if (NULL == pTalkingData)
    {
        return;
    }

    //设置其他数据
    CCallInfo * pCallInfo = pTalkingData->pCallInfo;

    m_bSecurity = pTalkingData->bAllowSecurity;
    m_eTalkUIType = pTalkingData->eTalkUIType;

    if (UpdateConfManagerStyle(pTalkingData))
    {
        return;
    }

    //设置是否显示联系人头像
    bool bIsShowContactImage = IsDisplayContactPhoto();
    m_cConInfoFrame.AdjustLayout(bIsShowContactImage);
    m_cConInfoFrame.show();

    //设置一下 错误提示的位置
    QFont objFont;
    objFont.setPixelSize(18);
    objFont.setFamily("Tahoma");
    if (bIsShowContactImage)
    {
        objFont.setPixelSize(14);
        m_qFailedInfo.setFont(objFont);
        m_qFailedInfo.setGeometry(STATUS_ICON_LAB_GEOMETRY);
        m_qFailedInfo.setAlignment(Qt::AlignCenter);
    }
    else
    {
        m_qFailedInfo.setFont(objFont);
        m_qFailedInfo.setGeometry(STATUS_ICON_LAB_GEOMETRY_NO_CONTACTIMAGE);
        m_qFailedInfo.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    }

    //设置错误提示数据

    UpdateFialInfo();

    //这边先清空一次联系人信息
    m_cConInfoFrame.Clear();

    if (pCallInfo != NULL)
    {

        //更新联系人相关信息
        UpdateContactInfo(pCallInfo);

        // 如果是网络会议则需要单独刷图片（此时无法从联系人获取到头像）
        if (pTalkingData->eTalkUIType == TALKING_UI_NETWORKCONF)
        {
            //图片确认
            yl::string strNetworkConfImage = PIC_CONTACTS_CONFERENCE;
            m_cConInfoFrame.SetContactImage(strNetworkConfImage);

            SetCustomImageMark(strNetworkConfImage);
        }

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
        if (pCallInfo->m_eRecordResult == CALL_RECORD_START_FAILED)
        {
            pCallInfo->m_eRecordResult = CALL_RECORD_NONE;
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_START_RECORD_FAILED), MESSAGEBOX_NOTE, 2000);
        }
        else if (pCallInfo->m_eRecordResult == CALL_RECORD_STOP_FAILED)
        {
            pCallInfo->m_eRecordResult = CALL_RECORD_NONE;
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_STOP_RECORD_FAILED), MESSAGEBOX_NOTE, 2000);
        }
        else if (pCallInfo->m_eRecordResult == CALL_RECORD_PAUSE_FAILED)
        {
            pCallInfo->m_eRecordResult = CALL_RECORD_NONE;
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_PAUSE_RECORD_FAILED), MESSAGEBOX_NOTE, 2000);
        }
        else if (pCallInfo->m_eRecordResult == CALL_RECORD_RESUME_FAILED)
        {
            pCallInfo->m_eRecordResult = CALL_RECORD_NONE;
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_RESUME_RECORD_FAILED), MESSAGEBOX_NOTE, 2000);
        }
#endif

        //更新Hold、Held、Mute 状态
        bool bHold = pCallInfo->IsLocalHold();
        bool bHeld = pCallInfo->IsRemoteHold();
        bool bMute = talklogic_GetMuteStatus();

        UpdateHoldAndMuteInfo(bHold, bHeld, bMute);
    }


}

// NetWork、Dtmf、音量条等Tip显示状态变化时的处理
void CDlgTalkingUI::OnTipChange(TALK_TIP_TYPE eTipType)
{

}

void CDlgTalkingUI::OnRemoveMember(QString strUserID, QString strName)
{
    QString strNote = LANG_TRANSLATE(TRID_DO_YOU_WANT_TO_REMOVE) + " " + strName + " " + LANG_TRANSLATE(
                          TRID_FROM_THE_CONFERENCE);
    MessageBox_ShowCommonMsgBox(this, strNote, \
                                MESSAGEBOX_OkCancel, -1, LANG_TRANSLATE(TRID_BACK), "", MSGBOX_ID_REMOVE,
                                QVariant(strUserID));
}

// 处理弹出框回调
void CDlgTalkingUI::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iMsgID = pMessageBox->GetID();
    MESSAGEBOX_RESULT eResault = pMessageBox->GetResult();

    TALKUI_INFO("CDlgTalkingUI::MessageBoxCallBack Popbox ID %d ,Result is %d.", iMsgID, eResault);

    switch (iMsgID)
    {
    //http://bugfree.yealink.com/Bug.php?BugID=133586 talking界面的所有弹框回调都在这里处理
    case MSGBOX_ID_REMOVE:
        {
            if (MessageBox_OK == eResault)
            {
                char * chrTemp = pMessageBox->GetExtraData().toString().toLatin1().data();
                TALKUI_INFO("Remove user : [%s]", chrTemp);
                SendUIRequestAction(TALK_ACTION_REMOVE_NET_CONF_PARTY, 0, 0, chrTemp);
            }
        }
        break;
    case MSGBOX_ID_SECURITY:
        {
            if (pMessageBox->GetResult() == MessageBox_OK)
            {
                int iSessionId = talklogic_GetFocusedSessionID();
                CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(iSessionId);
                if (pCallInfo == NULL)
                {
                    return;
                }

                //设置当前账号的安全等级
                talklogic_ModifyAccountOverrideLevel(pCallInfo->GetAccountID(),
                                                     fromQString(pMessageBox->GetExtraData().toString()));
            }
        }
        break;
    default:
        break;
    }
}

//设置区域
void CDlgTalkingUI::AdjustLayout()
{
    //设置 状态大小区域
    m_HoldFrame.setGeometry(0, 0, this->width(), this->height());

}

//获取通话时间
QCustomTime CDlgTalkingUI::GetTalkTime()
{
    UINT uHour = 0, uMinute = 0, uSecond = 0;
    m_qTimePiece.GetTime(uHour, uMinute, uSecond);
    TALKUI_INFO("uHour %d, uMinute %d, uSecond %d", uHour, uMinute, uSecond);
    QCustomTime timeTalk(uHour, uMinute, uSecond);
    return timeTalk;
}

//构造函数调用初始化数据
void CDlgTalkingUI::InitData()
{
    //!!!! 字体的设置是否机型统一
    QFont objFont;
    objFont.setPixelSize(18);
    objFont.setFamily("Tahoma");

    // 初始化计时控件
    //t48 的计时控件都放在TalManager中进行了
    m_qTimePiece.setObjectName("TimePiece");
    m_qTimePiece.setParent(this);
    m_qTimePiece.hide();

    //头像遮罩
    m_qCoverLabel.setObjectName("CoverLabel");
    m_qCoverLabel.setParent(this);

    m_qCoverLabel.setScaledContents(true);
    m_qCoverLabel.setGeometry(COVER_LABEL_GEOMETRY);
    //覆盖一张半透明的黑色图片
    m_qCoverLabel.setPixmap(THEME_GET_BMP(PIC_GENERAL_ICON_COVER));
    m_qCoverLabel.hide();


    //失败提示控件
    m_qFailedInfo.setObjectName("FailedInfo");
    m_qFailedInfo.setParent(this);
    //!!!!!,字体大小尽量统一
    objFont.setPixelSize(14);
    m_qFailedInfo.setGeometry(STATUS_ICON_LAB_GEOMETRY);
    m_qFailedInfo.setAlignment(Qt::AlignCenter);
    m_qFailedInfo.setFont(objFont);
    m_qFailedInfo.hide();

    //HOld遮罩
    m_HoldFrame.setObjectName("HoldFrame");
    m_HoldFrame.setParent(this);

    NetworkConfManagerInitData();
}

// 显示Park错误的信息
void CDlgTalkingUI::ShowFailInfo(bool bShow, const QString & strInfo)
{
    if (bShow)
    {
        m_qFailedInfo.SetTipsText(strInfo);
        m_qFailedInfo.show();
        m_qCoverLabel.show();//覆盖一张半透明的黑色图片
    }
    else
    {
        m_qFailedInfo.SetTipsText("");
        m_qFailedInfo.hide();
        m_qCoverLabel.hide();
    }
}

// 刷新联系人信息
void CDlgTalkingUI::UpdateContactInfo(CCallInfo * pCallInfo)
{
    // 入口参数判断
    if (NULL == pCallInfo)
    {
        return;
    }

    //获取联系人信息显示策略
    SHOW_CONTACTDETAIL_TYPE nDisplayMethod = (SHOW_CONTACTDETAIL_TYPE)
            _TalkUIDelegate.GetDisplayMethod();

    yl::string strName = GetNameFromCallInfo(pCallInfo);
    yl::string strNumber = GetNumberFromCallInfo(pCallInfo);
    yl::string strFullContactInfo = GetFullContactInfoFromCallInfo(pCallInfo);

    // 显示安全级别
    if (m_bSecurity)
    {
        ChangeNameAndNumberBySecurityLevel(strName, strNumber);
    }

    //根据这些信息获取 需要显示的信息
    ChangeNameAndNumberByInfoMethod(strName, strNumber, strFullContactInfo, nDisplayMethod);

    //重置一下联系人信息
    m_cConInfoFrame.Clear();

    //设置联系人相关信息
    m_cConInfoFrame.SetContactName(strName);
    m_cConInfoFrame.SetContactNumber(strNumber);

    yl::string strContactImage = GetContactImageFromCallInfo(pCallInfo);

    m_cConInfoFrame.SetContactImage(strContactImage);

    SetCustomImageMark(strContactImage);
}

//刷新错误提示信息
void CDlgTalkingUI::UpdateFialInfo()
{
    //callparkt失败提示
    if (m_bCallParkFailed || m_bCallPushFailed)
    {
        // 将字符串:后面的断行
        QString strTemp = toQString(m_strParkFailedInfo);
        int iPos = strTemp.indexOf(":");
        if (iPos != -1 && iPos != strTemp.length()
                && IsDisplayContactPhoto())
        {
            strTemp.insert(iPos + 1, "\n");
        }

        ShowFailInfo(true, strTemp);
    }
    else if (m_bTransferFailed) //transfer失败提示
    {
        ShowFailInfo(true, LANG_TRANSLATE(TRID_TRANSFER_FAILED));
    }
    else    //没有错误信息
    {
        // MainWnd提供C接口
        ShowFailInfo(false);
    }
}

// 刷新Hold、Held、Mute的信息
void CDlgTalkingUI:: UpdateHoldAndMuteInfo(bool bHold, bool bHeld, bool bMute)
{
    //Hold 状态
    if (true == bHold)
    {
        //显示Hold图标
        m_HoldFrame.SetTipInfo(LANG_TRANSLATE(TRID_HOLD), PIC_ICON_HOLD_MASK);
        m_HoldFrame.show();

        return ;
    }
    else if (true == bHeld) //Held 状态
    {
        //显示Held图标
        m_HoldFrame.SetTipInfo(LANG_TRANSLATE(TRID_HELD), PIC_ICON_HOLD_MASK);
        m_HoldFrame.show();

        return ;
    }
    else if (true == bMute) //Mute 状态
    {
        m_HoldFrame.SetTipInfo(LANG_TRANSLATE(TRID_MUTE), PIC_ICON_MUTE_MASK);
        m_HoldFrame.show();

        return ;
    }

    //3个状态都没有 那么隐藏状态
    m_HoldFrame.hide();
}

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
bool CDlgTalkingUI::OnUIAction(const QString & strAction)
{
    bool bHandle = true;
    if (strAction == TALK_UI_ACTION_RECORD)
    {
        int iSessionId = talklogic_GetFocusedSessionID();
        CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(iSessionId);
        if (pCallInfo == NULL)
        {
            return false;
        }
        GetSoftkeyData(m_SoftkeyData, NULL);
        //刷新softkey
        emit RequestRefreshSoftkey();
    }
    else if (strAction == TALK_UI_ACTION_BACK)
    {
        SendUIRequestAction(TALK_ACTION_RECORD_BACK);
    }
    else
    {
        int iSessionId = talklogic_GetFocusedSessionID();
        CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(iSessionId);
        if (pCallInfo == NULL)
        {
            return false;
        }
        if (strAction == TALK_UI_ACTION_STARTRECORD)
        {
            if (pCallInfo->m_eRecordResult != CALL_RECORD_ACTION_PROCESSING)
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_ACTION_PROCESSING;
                SendUIRequestAction(TALK_ACTION_RECORD_START);
            }
            else
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_DEAL_LAST_ACTION), MESSAGEBOX_NOTE, 2000);
            }
        }
        else if (strAction == TALK_UI_ACTION_STOPRECORD)
        {
            if (pCallInfo->m_eRecordResult != CALL_RECORD_ACTION_PROCESSING)
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_ACTION_PROCESSING;
                SendUIRequestAction(TALK_ACTION_RECORD_STOP);
            }
            else
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_DEAL_LAST_ACTION), MESSAGEBOX_NOTE, 2000);
            }
        }
        else if (strAction == TALK_UI_ACTION_PAUSERECORD)
        {
            if (pCallInfo->m_eRecordResult != CALL_RECORD_ACTION_PROCESSING)
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_ACTION_PROCESSING;
                SendUIRequestAction(TALK_ACTION_RECORD_PAUSE);
            }
            else
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_DEAL_LAST_ACTION), MESSAGEBOX_NOTE, 2000);
            }
        }
        else if (strAction == TALK_UI_ACTION_RESUMERECORD)
        {
            if (pCallInfo->m_eRecordResult != CALL_RECORD_ACTION_PROCESSING)
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_ACTION_PROCESSING;
                SendUIRequestAction(TALK_ACTION_RECORD_RESUME);
            }
            else
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_DEAL_LAST_ACTION), MESSAGEBOX_NOTE, 2000);
            }
        }
        else
        {
            bHandle = false;
        }
    }

    if (bHandle)
    {
        return true;
    }

    return CDlgTalkingBase::OnUIAction(strAction);
}
#endif

bool CDlgTalkingUI::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            int nKey = pKeyEvent->key();
            switch (nKey)
            {
            case PHONE_KEY_UP:
                {
                    if (NULL == m_pListFrame || m_pListFrame->GetTotalItemCount() < 1 \
                            || !m_bBWConfMgr)
                    {
                        break;
                    }

                    if (m_pListFrame->GetTotalDataCount() > m_pListFrame->GetMaxPageItemCount())
                    {
                        m_pListFrame->OnNotifyScrollPrevious();
                        return true;
                    }
                }
                break;
            case PHONE_KEY_DOWN:
                {
                    if (NULL == m_pListFrame || m_pListFrame->GetTotalItemCount() < 1 \
                            || !m_bBWConfMgr)
                    {
                        break;
                    }

                    if (m_pListFrame->GetTotalDataCount() > m_pListFrame->GetMaxPageItemCount())
                    {
                        m_pListFrame->OnNotifyScrollNext();
                        return true;
                    }
                }
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

    return false;
}

//网络会议管理模式初始化
void CDlgTalkingUI::NetworkConfManagerInitData()
{
    m_pListFrame = new CFrameList(this);
    if (m_pListFrame != NULL)
    {
        m_pListFrame->setObjectName("ListFrame");
        this->setFocusPolicy(Qt::StrongFocus);
        this->setFocus();

        m_pListFrame->setGeometry(CONF_MGR_RECT_IN_TALK);
        m_pListFrame->SetDefaultFocus(true);
        m_pListFrame->SetMaxPageItemCount(TALK_CONF_MGR_MAX_PAGE_ITEM_COUNT);

        m_pListFrame->SetContentPos(TALK_CONF_MGR_LIST_SAPCING, TALK_CONF_MGR_LIST_SAPCING);
        m_pListFrame->SetGridLine(false, false);
        /*QColor gridClr = GRID_COLOR;
        m_pListFrame->SetGridLineColor(gridClr);*/

        m_pListFrame->SetItemClickBG("");
        m_pListFrame->SetItemSpace(TALK_CONF_MGR_LIST_ITEM_LEFT_SPACE);

        m_pListFrame->SetDelegate(&m_listDeleget);
    }
}

//刷新成网络会议管理模式
bool CDlgTalkingUI::UpdateConfManagerStyle(DataTalking2UI * pTalkingData)
{
    if (NULL == pTalkingData || \
            NULL == pTalkingData->pCallInfo)
    {
        return false;
    }

    bool bResult = (pTalkingData->listNetUserInfo.size() > 0);

    if (!bResult)
    {
        TALKUI_INFO("Talking UI is not network conference manager mode");
        return bResult;
    }

    std::vector<NetworkConfMgrData> vecData;

    YLList<DataNetConfUserInfo>::iterator itInfo = pTalkingData->listNetUserInfo.begin();
    for (; itInfo != pTalkingData->listNetUserInfo.end(); ++itInfo)
    {
        NetworkConfMgrData objData;
        objData.m_strName = toQString((*itInfo).strUserName);
        objData.m_strID = (*itInfo).strUserID;

        bool bRemove = false;

        if (TALKING_UI_NETWORKCONF == pTalkingData->eTalkUIType && \
                !((*itInfo).bConfCreator))
        {
            bRemove = true;
        }

        objData.m_bRemove = bRemove;

        vecData.push_back(objData);
    }

    m_listDeleget.SetData(vecData);

    if (NULL != m_pListFrame)
    {
        m_pListFrame->RefreshData(0);
        m_pListFrame->show();
    }

    CCallInfo * pCallInfo = pTalkingData->pCallInfo;

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    if (pCallInfo->m_eRecordResult == CALL_RECORD_START_FAILED)
    {
        pCallInfo->m_eRecordResult = CALL_RECORD_NONE;
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_START_RECORD_FAILED), MESSAGEBOX_NOTE, 2000);
    }
    else if (pCallInfo->m_eRecordResult == CALL_RECORD_STOP_FAILED)
    {
        pCallInfo->m_eRecordResult = CALL_RECORD_NONE;
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_STOP_RECORD_FAILED), MESSAGEBOX_NOTE, 2000);
    }
    else if (pCallInfo->m_eRecordResult == CALL_RECORD_PAUSE_FAILED)
    {
        pCallInfo->m_eRecordResult = CALL_RECORD_NONE;
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_PAUSE_RECORD_FAILED), MESSAGEBOX_NOTE, 2000);
    }
    else if (pCallInfo->m_eRecordResult == CALL_RECORD_RESUME_FAILED)
    {
        pCallInfo->m_eRecordResult = CALL_RECORD_NONE;
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_RESUME_RECORD_FAILED), MESSAGEBOX_NOTE, 2000);
    }
#endif

    //更新Hold、Held、Mute 状态
    bool bHold = pCallInfo->IsLocalHold();
    bool bHeld = pCallInfo->IsRemoteHold();
    bool bMute = talklogic_GetMuteStatus();

    UpdateHoldAndMuteInfo(bHold, bHeld, bMute);

    return true;
}

//隐藏所有控件  除了网络会议管理模式下的控件
void CDlgTalkingUI::HideAllWidget()
{
    m_cConInfoFrame.hide();
    m_qTimePiece.hide();
    m_qCoverLabel.hide();

    m_qFailedInfo.hide();
    m_HoldFrame.hide();

    if (NULL != m_pListFrame)
    {
        m_pListFrame->hide();
    }
}
