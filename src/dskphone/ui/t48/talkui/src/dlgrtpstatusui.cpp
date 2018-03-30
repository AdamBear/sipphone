#include "dlgrtpstatusui.h"
#include "feature/include/modfeature.h"
#include "uimanager/rectdefine.h"
#include "uikernel/languagehelper.h"
#include "baseui/framelist.h"
#include "rtpstatusitem.h"
#include "uikernel/framelistbase.h"
#include "qtcommon/qmisc.h"
#include "baseui/t4xpicpath.h"
#include "contactinfoframe.h"

#include "talkuifactory.h"
#include "talkuiframemanager.h"
#include "voice/include/modvoice.h"

namespace
{
#define TALK_RTP_MAX_PAGE_ITEM_COUNT 7
#define TALK_RTP_LIST_SAPCING 32
#define TALK_RTP_LIST_ITEM_LEFT_SPACE 0
#define TALK_RTP_LIST_DEFAULT_FONT_SIZE 16
#define TALK_RTP_LIST_ITEM_DEFAUT_HEIGHT 41
const QColor ITEM_COLOR(Qt::white);
const QColor GRID_COLOR = QColor(162, 161, 161, 255);
const QRect RTP_RECT_IN_TALK(0, 0, 480, 287);
}

// 重新加载数据
void IRTPListDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    //TALKUI_INFO("IRTPListDelegate::OnLoadData, [%d][%d]", nDataIndex, nDataCount);
    if (NULL == pFrameList || nDataIndex > m_vecText.size())
    {
        return;
    }

    //记录当前序号
    m_iCurrentIndex = nDataIndex;

    pFrameList->ClearContent();

    int iPageMaxItemCount = pFrameList->GetMaxPageItemCount();
    int iTotal = m_vecText.size();
    int iItemToMGet = nDataCount > iPageMaxItemCount ? iPageMaxItemCount : nDataCount;
    int iMaxIndex = iItemToMGet + nDataIndex;

    pFrameList->SetTotalDataCount(m_vecText.size());

    for (int i = nDataIndex; i < iMaxIndex && i < iTotal; i++)
    {
        CRTPStatusItem * pItem = new CRTPStatusItem(pFrameList);
        pItem->setObjectName(QString("CRTPStatusItem%1").arg(i));
        pItem->SetTag(m_vecText[i].first);
        pItem->SetValue(m_vecText[i].second);
        pItem->SetDataIndex(i);

        pItem->SetFontSize(TALK_RTP_LIST_DEFAULT_FONT_SIZE);
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

bool IRTPListDelegate::LoadData(SVQReportData * pSVPRData)
{
    if (NULL == pSVPRData)
    {
        return false;
    }

    //每次load之前清空
    m_vecText.clear();

    //根据配置决定显示哪些选项
    const RTPConfigureList * pRTPlist = feature_RTPConfigGetEnableList();

    if (NULL == pRTPlist)
    {
        return false;
    }

    RTPConfigureList pList = *pRTPlist;

    RTPConfigureList::const_iterator iter = pList.begin();

    TALKUI_INFO("RTP Status Load data size[%d]", pList.size());

    for (; iter != pList.end(); ++iter)
    {
        RTPConfigureItem item = *iter;
        if (!item.m_isEnable)
        {
            continue;
        }

        QString strTag = LANG_TRANSLATE(item.m_strDisplayName.c_str()) + ": ";
        yl::string strValue = pSVPRData->GetAttributeValueById(item.m_strIdName);

        m_vecText.push_back(pair<QString, QString>(strTag, toQString(strValue.c_str())));
    }


    //m_vecText.push_back(pair<QString, QString>("number", toQString("fdafda")));
    //m_vecText.push_back(pair<QString, QString>("number", toQString("fdafda")));
    //m_vecText.push_back(pair<QString, QString>("number", toQString("fdafda")));
    //m_vecText.push_back(pair<QString, QString>("number", toQString("fdafda")));
    //m_vecText.push_back(pair<QString, QString>("number", toQString("fdafda")));
    //m_vecText.push_back(pair<QString, QString>("number", toQString("fdafda")));
    //m_vecText.push_back(pair<QString, QString>("number", toQString("fdafda")));
    //m_vecText.push_back(pair<QString, QString>("number", toQString("fdafda")));
    //m_vecText.push_back(pair<QString, QString>("number", toQString("fdafda")));

    return true;
}

//重置序号
void IRTPListDelegate::ResetIndex()
{
    m_iCurrentIndex = 0;
}

//获取序号
int IRTPListDelegate::GetCurrentIndex()
{
    return m_iCurrentIndex;
}

LRESULT OnRTPMessage(msgObject & refMessage)
{
    //就算不在通话界面 也需要更新数据   比如退出rtp 显示还是要的
    //if(!UIManager_IsInTalk())
    //{
    //  return ;
    //}

    CBaseDialog * pDlgBase = _TalkUIFrameMananger.GetTalkUI();

    if (NULL != pDlgBase)
    {
        if (pDlgBase->inherits(DLG_RTPStatusUI))
        {
            CDlgRTPStatusUI * pRTPStatus = (CDlgRTPStatusUI *)pDlgBase;
            pRTPStatus->OnRTPStatusMessage(refMessage);

            return 0;
        }

    }

    return -1;
}

CDlgRTPStatusUI::CDlgRTPStatusUI()
    : CDlgBaseTalkUI(DLG_RTPStatusUI, TALK_UI_VOICE_TALK)
    , m_pListFrame(NULL)
    , m_iCurrentIndex(0)
    , m_bShowSwitch(false)
{
    InitData();
}

CDlgRTPStatusUI::~CDlgRTPStatusUI()
{
    if (NULL != m_pListFrame)
    {
        m_pListFrame->SetDelegate(NULL);
    }
}

void CDlgRTPStatusUI::InitData()
{
    m_pListFrame = new CFrameList(this);
    if (m_pListFrame != NULL)
    {
        m_pListFrame->setObjectName("ListFrame");
        this->setFocusPolicy(Qt::StrongFocus);
        this->setFocus();

        m_pListFrame->setGeometry(RTP_RECT_IN_TALK);
        m_pListFrame->SetDefaultFocus(true);
        m_pListFrame->SetMaxPageItemCount(TALK_RTP_MAX_PAGE_ITEM_COUNT);

        m_pListFrame->SetContentPos(TALK_RTP_LIST_SAPCING, TALK_RTP_LIST_SAPCING);
        m_pListFrame->SetGridLine(true, false);
        QColor gridClr = GRID_COLOR;
        m_pListFrame->SetGridLineColor(gridClr);

        m_pListFrame->SetItemClickBG("");
        m_pListFrame->SetItemSpace(TALK_RTP_LIST_ITEM_LEFT_SPACE);

        m_pListFrame->SetDelegate(&m_listDeleget);
    }

    //if (NULL != m_pConInfoFrame)
    //{
    //  m_pConInfoFrame->hide();
    //}
}

//初始化数据
void CDlgRTPStatusUI::Init()
{
    m_bShowSwitch = false;

    //m_iCurrentIndex.Reset
    //重置序号
    m_listDeleget.ResetIndex();

    etl_RegisterMsgHandle(RTCP_UDPATE_WND, RTCP_UDPATE_WND, OnRTPMessage);
    ////设置回调
    //vqrtcp_RTPStatusInitCallBack(s_FunRTPStatusUpdate);
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgRTPStatusUI::Uninit()
{
    etl_UnregisterMsgHandle(RTCP_UDPATE_WND, RTCP_UDPATE_WND, OnRTPMessage);
    ////清空回调
    //vqrtcp_RTPStatusInitCallBack(NULL);
}

//设置通话数据内容
void CDlgRTPStatusUI::SetTalkData(DataLogic2UIBase * pData)
{
    //重置序号
    m_listDeleget.ResetIndex();

    UpdateData();
}

bool CDlgRTPStatusUI::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::Show:
        {
            this->setFocus();
        }
        break;
    case QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            int nKey = pKeyEvent->key();
            switch (nKey)
            {
            case PHONE_KEY_CANCEL:
                {
                    OnUIAction(TALK_UI_ACTION_CANCEL);
                }
                break;
            case PHONE_KEY_UP:
                {
                    if (NULL == m_pListFrame || m_pListFrame->GetTotalItemCount() < 1)
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
                    if (NULL == m_pListFrame || m_pListFrame->GetTotalItemCount() < 1)
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
            case PHONE_KEY_LEFT:
            case PHONE_KEY_RIGHT:
                {
                    if (m_bShowSwitch)
                    {
                        OnUIAction(TALK_UI_ACTION_RTP_STATUS_SWITCH);
                        return true;
                    }

                    return true;
                }
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

// 通道键处理
bool CDlgRTPStatusUI::OnChannelKeyPress(int iKey)
{
    if (SESSION_TALKING == m_eSessionState)
    {
        return OnTalkingChannelKeyPress(iKey);
    }
    else if (SESSION_LOCALCONF == m_eSessionState)
    {
        return OnConferenceChannelKeyPress(iKey);
    }

    return CDlgBaseTalkUI::OnChannelKeyPress(iKey);

}

// 功能键处理
bool CDlgRTPStatusUI::OnFunckeyPress(int iKey, bool bEXP)
{
    switch (iKey)
    {
    case PHONE_KEY_CANCEL:
        {
            OnUIAction(TALK_UI_ACTION_CANCEL);
        }
        break;
    case PHONE_KEY_UP:
        {
            if (NULL == m_pListFrame || m_pListFrame->GetTotalItemCount() < 1)
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
            if (NULL == m_pListFrame || m_pListFrame->GetTotalItemCount() < 1)
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
    case PHONE_KEY_LEFT:
    case PHONE_KEY_RIGHT:
        {
            if (m_bShowSwitch)
            {
                OnUIAction(TALK_UI_ACTION_RTP_STATUS_SWITCH);
                return true;
            }

            return true;
        }
        break;
    default:
        break;
    }

    return true;
}

// 数字键处理
bool CDlgRTPStatusUI::OnDigitkeyPress(int iKey)
{
    return true;
}

//talking状态通道键处理
bool CDlgRTPStatusUI::OnTalkingChannelKeyPress(int iKey)
{
    //rtcp不处理
    ////为什么之前只判断 transfer 失败 ？？？？
    ////    if (m_bTransferFailed)
    //if (m_bTransferFailed || m_bCallParkFailed)
    //{
    //  //      ExitNotify();
    //  SendUIRequestAction(TALK_ACTION_EXIT_FAILD_INFO);
    //  // 通道按键继续往下传
    //}

    bool bHandled = false;
    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
        {
            if (voice_IsCallCenterMode())
            {
                // callCenterMode交给基类处理
                bHandled = false;
            }
            else
            {
                // 通话中摘手柄时按免提键，则打开群听模式
                if (voice_GetHandsetStatus() && voice_GetHandfreeStatus())
                {
                    //这边肯定是处于 TS_TALK 状态的  因为是处于Hold、Held状态的话 就退出了
                    //if(m_eTalkState == TS_TALK)
                    //{
                    TALKUI_INFO("CTalkingUI : Open GroupListening");
                    //if (1 == configParser_GetConfigInt(kszGroupListenInTalkingEnable))
                    if (1 == configParser_GetConfigInt(kszGroupListenInTalkingEnable))
                    {
                        voice_SwitchChannel(CHANNEL_GROUP);
                    }
                    else
                    {
                        voice_SwitchChannel(CHANNEL_HANDFREE);
                    }
                    bHandled = true;
                    //}
                }
                else
                {
                    bHandled = false;
                }
            }
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            if (!IsChannelKeyToExit(iKey))
            {
                bHandled = true;
                voice_SwitchChannel();
            }
        }
        break;
    default:
        bHandled = false;
        break;
    }
    if (!bHandled)
    {
        bHandled = CDlgBaseTalkUI::OnChannelKeyPress(iKey);
    }

    return bHandled;
}

//会议状态 通道键处理
bool CDlgRTPStatusUI::OnConferenceChannelKeyPress(int iKey)
{
    bool bHandled = FALSE;
    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
        {
            if (voice_IsCallCenterMode())
            {
                // callCenterMode交给基类处理
                bHandled = FALSE;
            }
            else
            {
                // 通话中摘手柄时按免提键，则打开群听模式
                if (voice_GetHandsetStatus() && voice_GetHandfreeStatus())
                {
                    //                  if (!m_bAllPartyOfConfHold)
                    //这边 本地肯定是不会处于Hold 状态的  因为如果处于Hold 状态 那么就退出rtp 界面了
                    //if(!m_bLocalHold)
                    //{
                    voice_SwitchChannel(CHANNEL_GROUP);
                    bHandled = TRUE;
                    //}
                }
            }
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            if (!IsChannelKeyToExit(iKey))
            {
                bHandled = TRUE;
                voice_SwitchChannel();
            }
        }
        break;
    default:
        bHandled = FALSE;
        break;
    }

    if (!bHandled)
    {
        bHandled = CDlgBaseTalkUI::OnChannelKeyPress(iKey);
    }

    return bHandled;
}

void CDlgRTPStatusUI::GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData,
                                       DataLogic2UIBase * pData/* = NULL*/)
{
    if (m_bShowSwitch)
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RTP_STATUS_SWITCH);
    }

    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CANCEL);
}

bool CDlgRTPStatusUI::OnUIAction(const QString & strAction)
{
    TALKUI_INFO("CDlgRTPStatusUI::OnUIAction(%s)", strAction.toUtf8().data());
    if (strAction == TALK_UI_ACTION_CANCEL)
    {
        //直接调用刷新当前窗口 的接口就可以了
        talklogic_UpdateWnd();
        //  SendUIRequestAction(TALK_ACTION_CANCEL);
    }
    else if (strAction == TALK_UI_ACTION_RTP_STATUS_SWITCH)
    {
        //切换显示
        vqrtcp_RTPStatusSwitch();
        //SendUIRequestAction(TALK_ACTION_RTP_STATUS_SWITCH);
    }
    else
    {
        chWARNING2(false, "RTPStatusUI Action string error!");
        return false;
    }

    return true;
}

void CDlgRTPStatusUI::UpdateWnd()
{
    if (NULL != m_pListFrame)
    {
        int iPageItemCount = m_pListFrame->GetMaxPageItemCount();

        if (0 == iPageItemCount)
        {
            iPageItemCount = 1;
        }

        m_pListFrame->RefreshData((m_iCurrentIndex / iPageItemCount) * iPageItemCount);
        m_pListFrame->FocusItemByIndex(m_iCurrentIndex);
    }
}


void CDlgRTPStatusUI::AdjustLayout()
{
}

void CDlgRTPStatusUI::resizeEvent(QResizeEvent * pEvent)
{
    if (NULL != m_pListFrame)
    {
        m_pListFrame->setGeometry(0, 0, pEvent->size().width(), pEvent->size().height());
    }

    AdjustList();
}

bool CDlgRTPStatusUI::IsShowTip()
{
    return true;
}

//处理RTPStatus消息
void CDlgRTPStatusUI::OnRTPStatusMessage(msgObject & refMessage)
{
    //更新数据
    if (0 == refMessage.wParam)
    {
        UpdateData();
    }
    else if (1 == refMessage.wParam) //退出显示
    {
        OnUIAction(TALK_UI_ACTION_CANCEL);
    }

}

//刷新数据
void CDlgRTPStatusUI::UpdateData()
{
    //记住刷新前的位置，刷新后回到该位置
    int iSltIndex = m_listDeleget.GetCurrentIndex();
    m_iCurrentIndex = iSltIndex == INVALID_ITEM_INDEX ? 0 : iSltIndex;

    //获取是否显示switch
    m_bShowSwitch = vqrtcp_IsRTPStatusSwitch();

    if (NULL != m_pListFrame)
    {
        m_pListFrame->ClearContent();
    }

    IFrameListDelegate * pListDlgt = m_pListFrame->GetFrameDelegate();

    if (NULL == pListDlgt)
    {
        return ;
    }

    SVQReportData sData;
    bool bResult = vqrtcp_RTPStatusGetCurrentReport(sData);

    if (false == bResult)
    {
        return ;
    }

    IRTPListDelegate * pDlgt = static_cast<IRTPListDelegate *>(pListDlgt);

    if (NULL != pDlgt && pDlgt->LoadData(&sData))
    {
        UpdateWnd();
    }

}

void CDlgRTPStatusUI::AdjustList()
{
    if (NULL != m_pListFrame)
    {
        m_iCurrentIndex = m_listDeleget.GetCurrentIndex();

        m_pListFrame->SetMaxPageItemCount(m_pListFrame->height() / TALK_RTP_LIST_ITEM_DEFAUT_HEIGHT);

        if (m_pListFrame->GetTotalDataCount() > 0)
        {
            UpdateWnd();
        }
    }
}


////rtp 信息改变时 回调函数
//void s_FunRTPStatusUpdate(ERTPStatusToUI eRTPStatus)
//{
//  CDlgBaseTalkUI* pTalkUI = _TalkUIFactory.GetTalkUIByTalkUIName(DLG_RTPStatusUI);
//
//  if(NULL != pTalkUI)
//  {
//      if(pTalkUI->inherits(DLG_RTPStatusUI))
//      {
//          CDlgRTPStatusUI* pRTPStatusUI = (CDlgRTPStatusUI*)pTalkUI;
//
//          pRTPStatusUI->UpdateData();
//      }
//  }
//}
