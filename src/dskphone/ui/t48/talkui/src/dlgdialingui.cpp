// mod-Qt
#include <QTextEdit>

// mod-this(TalkUI)
#include "dlgdialingui.h"
#include "modtalkui.h"

// mod-InputMethod
#include "inputmethod/inputmethod.h"

//mod-UIKernel
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "uikernel/qpswlineedit.h"

//mod-MainWnd
#include "mainwnd/mainwnd.h"
#include "searchwidget.h"
#include "uimanager/rectdefine.h"

// #include "keyguard/modkeyguard.h"
#include "settingui/include/modsettingui.h"

#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/qlineeditutility.h"
// #include "dsskeyui/moddsskeyui.h"
#include "dsskeyui/include/moddsskeyui.h"
#include "contactinfoframe.h"
#include "account/include/accountmessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "account/include/modaccount.h"
#include "commonfunction.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "keyguard/include/modkeyguard.h"
#include "uicommon/messagebox/modmessagebox.h"

#include "talk/talklogic/include/commonfunctions.h"
#include "feature/include/modfeature.h"

#include "talkui/src/talkuimanager.h"
#include "talkuidelegate.h"
#include "talkuiframemanager.h"

#include "contacts/directory/include/directorymessagedefine.h"
#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"

//长按的时间
static const int LONG_PRESS_TIME = 1000;

CDlgDialingUI::CDlgDialingUI()
    : m_pPswLineEdit(NULL)
    , m_pSearchTable(NULL)
    , m_pBtnDelete(NULL)
    , m_pBtnDPickup(NULL)
    , m_pBtnGPickup(NULL)
    , m_pBtnRetrieve(NULL)
    , m_pBtnInitiation(NULL)
{
    InitData();
}

CDlgDialingUI::~CDlgDialingUI(void)
{
    m_timerLongPress.stop();
}

// 初始化
void CDlgDialingUI::Init()
{
    //基类初始化
    CDlgDialingBase::Init();

    if (NULL != m_pPswLineEdit)
    {
        //防止嘟嘟声不正确的一响就消失
        m_pPswLineEdit->blockSignals(true);
        m_pPswLineEdit->clear();
        m_pPswLineEdit->blockSignals(false);

        //这边不需要做了  因为在UnInit()函数中已经有做该操作了
        // 设置当前输入法
        //qSetCurrentIME(m_pPswLineEdit, IME_123_Dial);


        // 设置密码拨号的相关参数
        if (configParser_GetConfigInt(kszEnablePSWDial) == 1)
        {
            m_pPswLineEdit->SetPrefixText(configParser_GetConfigString(kszPSWDialPrefix).c_str());
            m_pPswLineEdit->SetHideTextLength(configParser_GetConfigInt(kszPSWDialLength));
        }
        //非密码拨号的时候 不需要设置了 因为在UnInit()函数中有做了操作

        m_pPswLineEdit->setFocus();
#ifdef IF_FEATURE_LONG_PRESS_DIGIT
        connect(&m_timerLongPress, SIGNAL(timeout()), this, SLOT(OnExtendedDigitKeyPress()));
#endif
    }


    ClearSearchTable();

    m_strSaveDisplayName = "";      //暂时保存的联系人的名称
    m_strSaveNumber = "";           //暂时保存的联系人号码

}

// 释放数据，将窗口清空回复到初始化状态
void CDlgDialingUI::Uninit()
{
    ClearSearchTable();

    if (NULL != m_pPswLineEdit)
    {
        m_pPswLineEdit->blockSignals(true);
        m_pPswLineEdit->setText("");
        m_pPswLineEdit->blockSignals(false);
        m_pPswLineEdit->SetPrefixText("");
        m_pPswLineEdit->SetHideTextLength(0);
        m_pPswLineEdit->setFocus();

        // 设置当前输入法
        qSetCurrentIME(m_pPswLineEdit, IME_123_Dial);
        ResetIMEText();

        qSetIsShowWnd(m_pPswLineEdit, false);
    }

    //拨号界面 没有没有这些图标
    // 删除加密图标
    //  AddOrDeleteIcon(PIC_ICON_ENCRYPT, false);

    //基类反初始化
    CDlgDialingBase::Uninit();
}

//更新窗口
void CDlgDialingUI::UpdateWnd()
{
    //基类执行UpdateWnd 操作
    CDlgDialingBase::UpdateWnd();

    if (NULL != m_pPswLineEdit)
    {
        // http://10.3.5.199/Bug.php?BugID=63426
        //添加判断 如果是联系人选中了 那么不设置焦点
        if (NULL == m_pSearchTable || !m_pSearchTable->IsSelectItem())
        {
            //没有焦点的时候才要设置焦点  因为这边可能是重复刷新  那么光标的位置是不能变的
            if (!m_pPswLineEdit->hasFocus())
            {
                //给编辑框设置焦点
                m_pPswLineEdit->setFocus();

                // 设置光标到末尾
                m_pPswLineEdit->setCursorPosition(m_pPswLineEdit->text().length());
            }
        }
    }
}

// 焦点切换
void CDlgDialingUI::OnFocusChanged(bool bFocus)
{
    //基类需要给逻辑层发送 focuschange 事件
    CDlgDialingBase::OnFocusChanged(bFocus);

    if (!bFocus)
    {
        if (m_pPswLineEdit != NULL
                && qGetIsShowWnd())
        {
            qSetIsShowWnd(m_pPswLineEdit, false);
        }
    }
}



//事件过滤器
bool CDlgDialingUI::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL || pEvent == NULL)
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::FocusIn:
        {
            if (pObject == m_pPswLineEdit)
            {
                if (m_pSearchTable != NULL)
                {
                    //当搜索编辑获得焦点时，要去除search table的选中行焦点。
                    m_pSearchTable->SetItemFocusByIndex(INVALID_ITEM_INDEX);
                }
            }
        }
        break;
    case QEvent::MouseButtonPress:
        {
            if (FullKeyboardEnabled())
            {
                // 拨号界面可以弹出全键盘，当信息列表获取焦点的时候自动隐藏全键盘
                if (pObject->inherits("CSearchResultFrame"))
                {
                    if (qGetIsShowWnd())
                    {
                        qSetIsShowWnd(m_pPswLineEdit, false);
                    }
                }

                if (pObject == m_pPswLineEdit)
                {
                    if (!qGetIsShowWnd())
                    {
                        qSetIsShowWnd(m_pPswLineEdit, true);
                    }
                }
            }
        }
        break;
    case QEvent::MouseButtonRelease:
        {
            if (m_timerLongPress.isActive())
            {
                m_timerLongPress.stop();
            }

            return false;
        }
        break;
    case QEvent::KeyRelease:
        {
            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            int nKey = pKeyEvent->key();
            if (IsDigitKey(nKey))
            {
                if (m_timerLongPress.isActive())
                {
                    m_timerLongPress.stop();
                }

                return false;
            }
        }
        break;
    default:
        {
            //none
        }
        break;
    }

    return CDlgDialingBase::eventFilter(pObject, pEvent);
}

//给界面设置数据
void CDlgDialingUI::SetTalkData(DataLogic2UIBase * pData)
{
    //基类设置数据
    CDlgDialingBase::SetTalkData(pData);

    DataDial * pDialData = dynamic_cast<DataDial *>(pData);
    if (pDialData != NULL && NULL != m_pPswLineEdit)
    {
        //只要是transfer 类型那么就要设置穿梭数据  在设置穿梭数据中有判断 穿梭数据相同不会重复设置
        // 若从Transfer类型转换到其他类型需要清空穿梭数据
        if (DIAL_UI_TRANSFER == m_eDialUIType
#if IF_FEATURE_DIAL_ON_HOLD
                || DIAL_UI_ON_HOLD == m_eDialUIType
#endif
                || DIAL_UI_CONFERENCE == m_eDialUIType
           )
        {
            //不是transfer 要清空穿梭数据
            if (NULL != m_pSearchTable)
            {
                m_pSearchTable->ClearCustomList();
            }
        }

        //在base 中设置
        ////直接赋值 拨号界面类型
        //m_eDialUIType = pDialData->eDialUIType;

        m_dialPad.SetDialBtnAction(TALK_UI_ACTION_SEND);
        m_dialPad.SetDialBtnIcon(PIC_WIDGET_SEND);
        m_dialPad.SetDialBtnText(LANG_TRANSLATE(TRID_SEND));
        m_dialPad.SetTransferState(false);

        if (SESSION_BTDIALING == GetSessionState())
        {
            //forward to界面
            m_dialPad.SetDialBtnText(LANG_TRANSLATE(TRID_CALLFROM_MOBILE));
            m_dialPad.SetDialBtnIcon(PIC_WIDGET_BLUETOOTH_DIAL);

            if (m_eDialUIType ==
                    DIAL_UI_BLUETOOTH_DIALING)        //蓝牙拨号状态  http://10.2.1.199/Bug.php?BugID=69647
            {
                m_strTitleInfo = TRID_MOBILE_ACCOUNT;
            }
        }
        else
        {
            switch (m_eDialUIType)
            {
            case DIAL_UI_DIALING:
            case DIAL_UI_PREDIAL:
                {
                    //http://10.2.1.199/Bug.php?BugID=89076 DIAL_ON_HOLD 拨号界面显示 账号信息
                    //拨号预拨号界面显示当前所用账号的信息
                    if (pDialData->pCallInfo != NULL)
                    {
                        //优先获取hoteling 账号
                        yl::string strTemp = _TalkUIDelegate.GetHotelingUserName(pDialData->pCallInfo->GetAccountID());

                        //如果获取到的 hoteling 账号还是为空
                        if (strTemp.empty())
                        {
                            int iDisplayMethodOnDialing = _TalkUIDelegate.GetDisplayMethodOnDialing();

                            //优先显示Label
                            if (1 == iDisplayMethodOnDialing)
                            {
                                strTemp = _TalkUIDelegate.GetLabelFromLogic(pDialData->pCallInfo->GetAccountID());

                                if (strTemp.empty())
                                {
                                    strTemp = GetDisplayName(pDialData->pCallInfo->GetAccountID());
                                }

                            }
                            else if (2 == iDisplayMethodOnDialing)
                            {
                                strTemp = _TalkUIDelegate.GetDisplayNameFromLogic(pDialData->pCallInfo->GetAccountID());

                                if (strTemp.empty())
                                {
                                    strTemp = GetUserName(pDialData->pCallInfo->GetAccountID());
                                }
                            }
                            else
                            {
                                strTemp = _TalkUIDelegate.GetUserNameFromLogic(pDialData->pCallInfo->GetAccountID());
                            }

                            //如果获取的UserName 是空的 那么就不应该显示  有的时候 账号Label、DisplayName有填  但是UserName 没写 这个时候按dsskey可能还是可以进入拨号界面的
                            //这个时候title 就不应该显示有内容
                            if (_TalkUIDelegate.GetUserNameFromLogic(pDialData->pCallInfo->GetAccountID()).empty())
                            {
                                strTemp = "";
                            }



                        }

                        //V81Beta账号名称全局显示统一需求
                        if (strTemp.empty())
                        {
                            strTemp = acc_GetAccountShowText(pDialData->pCallInfo->GetAccountID());
                        }

                        m_strTitleInfo = strTemp;
                    }
                }
                break;
            case DIAL_UI_RINGFORWARD:
                {
                    //forward to界面
                    m_dialPad.SetDialBtnText(LANG_TRANSLATE(TRID_FORWARD));
                    m_dialPad.SetDialBtnIcon(PIC_WIDGET_FORWARD);
                }
                break;
#if IF_FEATURE_DIAL_ON_HOLD
            case DIAL_UI_ON_HOLD:
                {
                    //设置穿梭数据
                    SetShuttleList(pDialData->listTranCallInfo);

                    // http://bugfree.yealink.com/Bug.php?BugID=87055
                    // 如果是允许transfer 才要改变
                    if (m_bAllowTran)
                    {
                        m_dialPad.SetDialBtnAction(TALK_UI_ACTION_TRANSFER);
                        m_dialPad.SetDialBtnText(LANG_TRANSLATE(TRID_BLIND_TRANSFER));
                        m_dialPad.SetDialBtnIcon(PIC_WIDGET_BLIND_TRANSFER);
                        m_dialPad.SetTransSendBtnAction(TALK_UI_ACTION_SEND);
                        m_dialPad.SetTransSendBtnText(LANG_TRANSLATE(TRID_SEND));
                        m_dialPad.SetTransSendBtnIcon(PIC_WIDGET_TRANSFER_SMALL);
                        m_dialPad.SetTransferState(true);
                    }

                    //http://10.2.1.199/Bug.php?BugID=89076 DIAL_ON_HOLD 拨号界面显示 账号信息
                    //拨号预拨号界面显示当前所用账号的信息
                    if (pDialData->pCallInfo != NULL)
                    {
                        //优先获取hoteling 账号
                        yl::string strTemp = _TalkUIDelegate.GetHotelingUserName(pDialData->pCallInfo->GetAccountID());

                        //如果获取到的 hoteling 账号还是为空
                        if (strTemp.empty())
                        {
                            int iDisplayMethodOnDialing = _TalkUIDelegate.GetDisplayMethodOnDialing();

                            //优先显示Label
                            if (1 == iDisplayMethodOnDialing)
                            {
                                strTemp = _TalkUIDelegate.GetLabelFromLogic(pDialData->pCallInfo->GetAccountID());

                                if (strTemp.empty())
                                {
                                    strTemp = GetDisplayName(pDialData->pCallInfo->GetAccountID());
                                }

                            }
                            else if (2 == iDisplayMethodOnDialing)
                            {
                                strTemp = _TalkUIDelegate.GetDisplayNameFromLogic(pDialData->pCallInfo->GetAccountID());

                                if (strTemp.empty())
                                {
                                    strTemp = GetUserName(pDialData->pCallInfo->GetAccountID());
                                }
                            }
                            else
                            {
                                strTemp = _TalkUIDelegate.GetUserNameFromLogic(pDialData->pCallInfo->GetAccountID());
                            }

                            //如果获取的UserName 是空的 那么就不应该显示  有的时候 账号Label、DisplayName有填  但是UserName 没写 这个时候按dsskey可能还是可以进入拨号界面的
                            //这个时候title 就不应该显示有内容
                            if (_TalkUIDelegate.GetUserNameFromLogic(pDialData->pCallInfo->GetAccountID()).empty())
                            {
                                strTemp = "";
                            }



                        }

                        m_strTitleInfo = strTemp;
                    }
                }
                break;
#endif
            case DIAL_UI_TRANSFER:
                {
                    //设置穿梭数据
                    SetShuttleList(pDialData->listTranCallInfo);

                    m_dialPad.SetDialBtnAction(TALK_UI_ACTION_TRANSFER);
                    m_dialPad.SetDialBtnText(LANG_TRANSLATE(TRID_BLIND_TRANSFER));
                    m_dialPad.SetDialBtnIcon(PIC_WIDGET_BLIND_TRANSFER);
                    m_dialPad.SetTransSendBtnAction(TALK_UI_ACTION_SEND);
                    m_dialPad.SetTransSendBtnText(LANG_TRANSLATE(TRID_SEND));
                    m_dialPad.SetTransSendBtnIcon(PIC_WIDGET_TRANSFER_SMALL);
                    m_dialPad.SetTransferState(true);
                    //http://10.2.1.199/Bug.php?BugID=89076 Transfer to 拨号界面显示 Transfer to
                    m_strTitleInfo = TRID_TRANSFER_TO;
                }
                break;
            case DIAL_UI_CONFERENCE:
                {
                    //会议界面

                    //设置穿梭数据
                    SetShuttleList(pDialData->listConfCallInfo);

                    m_dialPad.SetDialBtnText(LANG_TRANSLATE(TRID_CONFERENCE));
                    m_dialPad.SetDialBtnIcon(PIC_WIDGET_CONFERENCE);

                    //http://10.2.1.199/Bug.php?BugID=89076 Conference with 拨号界面显示 Conference with
                    m_strTitleInfo = TRID_CONFERENCE_WITH;
                }
                break;
            case DIAL_UI_BLUETOOTH_DIALING:     //蓝牙拨号状态  http://10.2.1.199/Bug.php?BugID=69647
                {
                    m_strTitleInfo = TRID_MOBILE_ACCOUNT;
                }
                break;
            case DIAL_UI_CALLPARK:
                {
                    //Park通话

                    m_dialPad.SetDialBtnText(LANG_TRANSLATE(TRID_PARK_CALL));
                    m_dialPad.SetDialBtnIcon(PIC_WIDGET_CALL_PARK);
                }
                break;
            case DIAL_UI_GROUPCALLPARK:
                {
                    //GourpCallPark通话
                    m_dialPad.SetDialBtnText(LANG_TRANSLATE(TRID_GROUP_CALL_PARK));
                    m_dialPad.SetDialBtnIcon(PIC_WIDGET_CALL_PARK);
                }
                break;
            case DIAL_UI_RETRIEVE_CALLPARK:
                {
                    //取回Park

                    m_dialPad.SetDialBtnText(LANG_TRANSLATE(TRID_RETRIEVE));
                    m_dialPad.SetDialBtnIcon(PIC_WIDGET_RETRIEVE_CALL_PARK);
                }
                break;
            case DIAL_UI_DPICKUP:
                {
                    //Pickup状态

                    m_dialPad.SetDialBtnText(LANG_TRANSLATE(TRID_DIRECT_PICKUP));
                    m_dialPad.SetDialBtnIcon(PIC_WIDGET_DPICKUI);
                }
                break;
            default:
                {
                    //未知类型
                    chWARNING2(FALSE, commonAPI_FormatString("Dial ui type(%d) is unknown!",
                               pDialData->eDialUIType).c_str());

                }
                break;
            }
        }

        // Conference类型的需要同时显示穿梭数据和搜索内容
        if (m_eDialUIType == DIAL_UI_CONFERENCE
                || m_eDialUIType == DIAL_UI_TRANSFER)
        {
            m_pSearchTable->SetMergeData(true);
        }
        else
        {
            m_pSearchTable->SetMergeData(false);
        }

        if (pDialData->bSeizeFailed)
        {
            m_strTitleInfo = TRID_EMERGENCY_CALL_ONLY;
        }

        //如果 编辑框中的数据 和 逻辑层中保存的数据不相同 那么 把逻辑层数据设置给编辑框就可以了
        if (m_pPswLineEdit->text() != toQString(pDialData->strNumber.c_str()))
        {
#ifdef IF_FEATURE_LONG_PRESS_DIGIT
            if (pDialData->strNumber.size() == 1 && IsDigitKey(pDialData->iKeyCode))
            {
                m_timerLongPress.start(LONG_PRESS_TIME);
            }
#endif
            m_pPswLineEdit->setText(toQString(pDialData->strNumber.c_str()));
            //这边不能再调用 OnTextChange函数了  因为本身setText 就会产生编辑框字符改变的信号
        }
        else    //如果编辑框字符与逻辑层中保存的相同  那么不要赋值  但是有可能是都为空
        {
            //如果是为空  那么要给编辑框对应的输入法传事件
            if (pDialData->strNumber.empty())
            {
                if (pDialData->iKeyCode != PHONE_KEY_NONE)
                {
                    QKeyEvent keyEvent(QEvent::KeyPress, pDialData->iKeyCode, Qt::NoModifier);
                    qPassEventToEdit(m_pPswLineEdit, &keyEvent);
                }
                else
                {
                    //这边不用担心 是从别的拨号界面 进入到 当前拨号界面  因为拨号界面只有一个 在进入新的拨号界面的时候 会放回缓存中 执行一次Init，UInit函数
                    //编辑框没有字符  并且输入的为PHONE_KEY_NONE即可能是免提 或者 手柄进入  或者是dsskey进入拨号界面
                    if (NULL != m_pSearchTable)
                    {
                        //这边如果编辑框中的字符为空 并且有穿梭数据 是不需要去搜索的
                        //如果 没有穿梭数据 那么还是要去搜索一下， 因为为空的时候 也可以搜索到数据出来
                        if (m_pSearchTable->GetTotalDataCount() <= 0
                                || m_eDialUIType == DIAL_UI_CONFERENCE
                                || m_eDialUIType == DIAL_UI_TRANSFER)
                        {
                            //###############################
                            //发送编辑框字符变化给逻辑
                            OnSuperSearch("");

                        }
                    }
                }
            }
            else
            {
                //刷新Pick 按钮
                RefreshPickBtn(pDialData);

                //数据内容没有改变，可以直接返回
                //因为后续还有设置光标的操作， 所以 这边要直接返回 防止光标出现问题
                return ;
            }
        }

        //刷新Pick 按钮
        RefreshPickBtn(pDialData);

        // http://10.3.5.199/Bug.php?BugID=63426
        //添加判断 如果是联系人选中了 那么不设置焦点
        if (NULL == m_pSearchTable || !m_pSearchTable->IsSelectItem())
        {
            //给编辑框设置焦点
            m_pPswLineEdit->setFocus();

            // 设置光标到末尾
            m_pPswLineEdit->setCursorPosition(m_pPswLineEdit->text().length());
        }
    }
}

//这边始终是优先获取联系人控件中联系人的数据， 联系人控件获取不到数据的时候 才获取编辑框中的数据
//拨号界面使用
bool CDlgDialingUI::GetInputContent(yl::string & strNum, yl::string & strDisplayname)
{
    if (m_pSearchTable != NULL
            && m_pSearchTable->IsSelectItem())
    {
        SearchItemData objItemData;
        objItemData = m_pSearchTable->GetCurrentItemContent();

        strNum = fromQString(objItemData.strNumber);
        strDisplayname = fromQString(objItemData.strDisplay);
    }
    else
    {
        if (NULL != m_pPswLineEdit)
        {
            strNum = m_pPswLineEdit->text().toUtf8().data();

            if (strNum == m_strSaveNumber)
            {
                strDisplayname = m_strSaveDisplayName;
            }
            else
            {
                strDisplayname = "";
            }
        }
    }

    return true;
}

//获取当前 LineEidt 编辑框中的数据  不强制输出预编辑字符
yl::string CDlgDialingUI::GetLineEditData()
{
    if (NULL != m_pPswLineEdit)
    {
        return m_pPswLineEdit->text().toUtf8().data();
    }

    return "";

}

//把联系人控件中的联系人信息设置到编辑框中
void CDlgDialingUI::SetLineEditDataBySearchTable()
{
    if (m_pSearchTable != NULL
            && m_pSearchTable->IsSelectItem())
    {
        SearchItemData objItemData;
        objItemData = m_pSearchTable->GetCurrentItemContent();

        if (NULL != m_pPswLineEdit)
        {
            m_pPswLineEdit->setText(objItemData.strNumber);
        }

        //保存一下数据
        m_strSaveNumber = fromQString(objItemData.strNumber);
        m_strSaveDisplayName = fromQString(objItemData.strDisplay);
    }
}

//把编辑框中的预编辑字符输入到编辑框中
void CDlgDialingUI::InputLineEditData()
{
    if (NULL != m_pPswLineEdit)
    {
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents |
                                    QEventLoop::ExcludeSocketNotifiers);
        qReturnEdit(m_pPswLineEdit);
    }
}

//联系人控件是否有选中联系人
bool CDlgDialingUI::IsSearchTableSelectItem()
{
    if (NULL != m_pSearchTable && m_pSearchTable->IsSelectItem())
    {
        return true;
    }

    return false;
}

// 获取当前保存的 转接通话ID
int CDlgDialingUI::GetShttleCallID()
{
    //有在联系人控件中选择联系人
    if (IsSearchTableSelectItem())
    {
        if (NULL != m_pSearchTable)
        {
            //焦点为搜索栏，获取焦点数据
            SearchItemData tSearchItemData;

            tSearchItemData = m_pSearchTable->GetCurrentItemContent();

            bool bOk = false;
            int nShuttleCallId = tSearchItemData.varData.toInt(&bOk);
            if (bOk && nShuttleCallId > 0)
            {
                //穿梭
                TALKUI_INFO("Dial UI: Get shuttleCallID !");
                return nShuttleCallId;
            }
        }
    }

    return -1;
}

//联系人控件 往上翻一页
void CDlgDialingUI::OnKeyEventUp()
{
    if (NULL != m_pSearchTable)
    {
        m_pSearchTable->OnNotifyScrollPrevious();
    }
}

//联系人控件 往下翻一页
void CDlgDialingUI::OnKeyEventDown()
{
    if (NULL != m_pSearchTable)
    {
        m_pSearchTable->OnNotifyScrollNext();
    }
}

//数字键处理
bool CDlgDialingUI::OnUIDigitPress(int iKey)
{
#ifdef IF_FEATURE_LONG_PRESS_DIGIT
    m_timerLongPress.start(LONG_PRESS_TIME);
#endif
    if (NULL != m_pPswLineEdit)
    {
        if (!m_pPswLineEdit->hasFocus())
        {
            m_pPswLineEdit->setFocus();

            //模拟按键效果，实现从搜索界面到输入框的输入效果
            QKeyEvent keyEvent(QEvent::KeyPress, iKey, Qt::NoModifier);
            if (qPassEventToEdit(m_pPswLineEdit, &keyEvent))
            {
                return true;
            }
        }
    }

    return false;
}

//搜索联系人
void CDlgDialingUI::SearchContact(const QString & strSearchData)
{
    //调用搜索控件搜索
    if (NULL != m_pSearchTable)
    {
        m_pSearchTable->Search(strSearchData.toUtf8().data());
    }
}

//搜索结果返回
LRESULT CDlgDialingUI::DealSuperSearchMessage(msgObject & refMessage)
{
    // 显示第一页数据
    if (NULL != m_pSearchTable)
    {
        yl::string strKey = "";
        if (NULL != refMessage.GetExtraData())
        {
            strKey = (char *) refMessage.GetExtraData();
        }

        m_pSearchTable->ShowSearchPageByKeyAnIndex(strKey, 0);
    }

    return 0;
}

// 是否允许切换Session
bool CDlgDialingUI::IsChangeSession()
{
    // 当没有搜索数据的时候,允许切换话路
    return (NULL == m_pSearchTable
            || 0 == m_pSearchTable->GetTotalDataCount());
}

//设置Softkey数据Action值
void CDlgDialingUI::RefreshPickBtn(DataDial * pDialData)
{
    // 隐藏Pickup按钮
    ShowDPickupBtn(false);
    ShowGPickupBtn(false);
    ShowRetrieveParkBtn(false);
    // 是否有Delete键
    bool bHaveDel = m_pPswLineEdit != NULL && !m_pPswLineEdit->text().isEmpty();

    TALKUI_INFO("bHaveDel is %d. Text empty is %d.", bHaveDel, m_pPswLineEdit->text().isEmpty());
    // 设置Delete键是否显示
    if (NULL != m_pBtnDelete)
    {
        if (bHaveDel)
        {
            m_pBtnDelete->show();
        }
        else
        {
            m_pBtnDelete->hide();
        }
    }

#if IF_BUG_25055
    m_dialPad.SetExtensionBtnVisible(false);

    // 只有Dialing和Predial界面才需要显示这三个按键
    if (m_eDialUIType != DIAL_UI_DIALING && m_eDialUIType != DIAL_UI_PREDIAL)
    {
        return;
    }

    if (!bHaveDel
            && configParser_GetConfigInt(kszDPickupSwitch) != 0)
    {
        ShowDPickupBtn(true);
    }

    if (!bHaveDel
            && configParser_GetConfigInt(kszGPickupSwitch) != 0)
    {
        ShowGPickupBtn(true);
    }

    if (!bHaveDel
            && ((configParser_GetConfigInt(kszCallParkSwitch) != 0)
                || (configParser_GetConfigInt(kszGroupCallParkSwitch) != 0)))
    {
        m_dialPad.SetExtensionBtnVisible(true);
        m_dialPad.SetExtensionBtnAction(TALK_UI_ACTION_RETRIEVE_PARK);
        m_dialPad.SetExtensionBtnText(LANG_TRANSLATE("Retrieve"));
    }
#else
    //if (m_eDialUIType == DIAL_UI_DPICKUP)
    //{
    //  // DPickup界面无需显示Group Pickup 按键，拨号键就是DPICKUP
    //  return;
    //}

    if (DIAL_UI_DPICKUP != m_eDialUIType)
    {
        // 设置Softkey4
        if (!bHaveDel
                && configParser_GetConfigInt(kszDPickupSwitch) != 0)
        {
            ShowDPickupBtn(true);
        }

        // 设置Softkey
        if (!bHaveDel
                && configParser_GetConfigInt(kszGPickupSwitch) != 0)
        {
            ShowGPickupBtn(true);
        }
    }

    // 显示retrieve
    if (!bHaveDel && m_eDialUIType != DIAL_UI_RETRIEVE_CALLPARK
            && ((configParser_GetConfigInt(kszCallParkSwitch) != 0)
                || (configParser_GetConfigInt(kszGroupCallParkSwitch) != 0)))
    {
        ShowRetrieveParkBtn(true);
    }
#endif
}

// 清空搜索栏
void CDlgDialingUI::ClearSearchTable()
{
    if (m_pSearchTable != NULL)
    {
        // 清空数据
        m_pSearchTable->ClearData();
    }
}

//设置穿梭列表
void CDlgDialingUI::SetShuttleList(YLList<CCallInfo *> listShuttleCall)
{
    if (m_pSearchTable != NULL)
    {
        //添加数据
        ListCustomItem listCustomItem;
        listCustomItem.clear();
        YLList<CCallInfo *>::iterator itTarget = listShuttleCall.begin();
        for (; itTarget != listShuttleCall.end(); ++itTarget)
        {
            CCallInfo * pCallInfo = (CCallInfo *)(*itTarget);
            if (pCallInfo != NULL)
            {
                QString strDisplay = LANG_TRANSLATE(TRID_HOLD) + " " + toQString(GetNameFromCallInfo(pCallInfo));
                SearchItemData tSearchItemData;
                tSearchItemData.strDisplay = strDisplay;
                tSearchItemData.strNumber = toQString(pCallInfo->m_tRemoteInfo.sSIPName.c_str());
                tSearchItemData.varData = pCallInfo->GetCallID();
                listCustomItem.push_back(tSearchItemData);
            }
        }

        // 判断自定义数据有无改变，有改变时才进行刷新
        if (m_pSearchTable->IsCustomListChanged(listCustomItem))
        {
            m_pSearchTable->ClearCustomList();
            m_pSearchTable->SetCustomList(listCustomItem);
        }
    }
}

// 显示DPickup按钮
void CDlgDialingUI::ShowDPickupBtn(bool bShow)
{
    if (NULL == m_pBtnDPickup)
    {
        return;
    }

    if (!bShow)
    {
        m_pBtnDPickup->hide();
        return;
    }

//  bool bOffset = (NULL != m_pBtnGPickup && !m_pBtnGPickup->isHidden());
//  m_pBtnDPickup->setGeometry(bOffset ? 533 : 661, 2, 128, 57);
    m_pBtnDPickup->setGeometry(GetFunctionBtnRect());
    m_pBtnDPickup->show();
}

// 显示GPickup按钮
void CDlgDialingUI::ShowGPickupBtn(bool bShow)
{
    if (NULL == m_pBtnGPickup)
    {
        return;
    }

    if (!bShow)
    {
        m_pBtnGPickup->hide();
        return;
    }

//  bool bOffset = (NULL != m_pBtnDPickup && !m_pBtnDPickup->isHidden());
//  m_pBtnGPickup->setGeometry(bOffset ? 533 : 661, 2, 128, 57);
    m_pBtnGPickup->setGeometry(GetFunctionBtnRect());
    m_pBtnGPickup->show();
}

//显示Retrieve按钮
void CDlgDialingUI::ShowRetrieveParkBtn(bool bShow)
{
    if (NULL == m_pBtnRetrieve)
    {
        return ;
    }

    if (!bShow)
    {
        m_pBtnRetrieve->hide();
        return ;
    }

    m_pBtnRetrieve->setGeometry(GetFunctionBtnRect());
    m_pBtnRetrieve->show();

}

//显示Initiation按钮
void CDlgDialingUI::ShowInitiationBtn()
{
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    if (NULL == m_pBtnInitiation)
    {
        return;
    }

    bool bShow = false;
    if (m_eDialUIType == DIAL_UI_PREDIAL || m_eDialUIType == DIAL_UI_DIALING
            || m_eDialUIType == DIAL_UI_TRANSFER || m_eDialUIType == DIAL_UI_CONFERENCE)
    {
        bShow = BWExeAssis_IsCallInitiationEnable(GetLineId());
    }

    if (bShow)
    {
        m_pBtnInitiation->setGeometry(GetFunctionBtnRect());
        m_pBtnInitiation->show();
    }
    else
    {
        m_pBtnInitiation->hide();
    }
#endif
}

//获取功能按钮左位移
QRect CDlgDialingUI::GetFunctionBtnRect()
{
    QRect btnRect(661, 2, 128, 57);

    if (NULL != m_pBtnDPickup)
    {
        if (!m_pBtnDPickup->isHidden())
        {
            btnRect.setRect(btnRect.left() - btnRect.width(), btnRect.top(), btnRect.width(), btnRect.height());
        }
    }

    if (NULL != m_pBtnGPickup)
    {
        if (!m_pBtnGPickup->isHidden())
        {
            btnRect.setRect(btnRect.left() - btnRect.width(), btnRect.top(), btnRect.width(), btnRect.height());
        }
    }

    if (NULL != m_pBtnRetrieve)
    {
        if (!m_pBtnRetrieve->isHidden())
        {
            btnRect.setRect(btnRect.left() - btnRect.width(), btnRect.top(), btnRect.width(), btnRect.height());
        }
    }

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    if (m_pBtnDelete != NULL && m_pBtnInitiation != NULL && !m_pBtnDelete->isHidden())
    {
        btnRect.setRect(btnRect.left() - btnRect.width(), btnRect.top(), btnRect.width(), btnRect.height());
    }
#endif

    return btnRect;

}

// 重设IME按钮的文字
void CDlgDialingUI::ResetIMEText()
{
    if (NULL != m_pPswLineEdit)
    {
        QString strIMEText = qGetCurrentIMETitle(m_pPswLineEdit);
        m_dialPad.SetIMEBtnText(strIMEText);
    }
}

//构造函数调用初始化数据
void CDlgDialingUI::InitData()
{
    //隐藏联系人控件
    m_cConInfoFrame.hide();

    m_pPswLineEdit = new qPswLineEdit(this);
    if (NULL != m_pPswLineEdit)
    {
        QPalette pal = m_pPswLineEdit->palette();
        pal.setColor(QPalette::Text, QColor(40, 49, 58));

        qWidgetUtility::setFontStyle(m_pPswLineEdit, 32, -1);
        qLineEditUtility::SetMaxLength(m_pPswLineEdit, EDIT_TYPE_DIAL);
        qLineEditUtility::SetLineEditTransparent(m_pPswLineEdit);

        qInstallIME(m_pPswLineEdit, IME_DIAL);

        qInstallKeyBoard(m_pPswLineEdit, false, INPUT_DLGTYPE_CUSTOM, INPUT_DIALOG_DIAL);

        // 连接相应的信号、槽
        connect(m_pPswLineEdit, SIGNAL(TextChanged(const QString &)), this,
                SLOT(OnTextChanged(const QString &)));

        m_pPswLineEdit->setGeometry(275, 2, 386, 57);

        ResetIMEText();
    }

    m_pBtnDelete = new qUIPushButton(this);
    if (NULL != m_pBtnDelete)
    {
        m_pBtnDelete->setObjectName("BtnDelete");
        m_pBtnDelete->setGeometry(661, 2, 128, 57);
        m_pBtnDelete->set3StatePic(PIC_WIDGET_DIALING_DELETE_BTN, PIC_WIDGET_DIALING_DELETE_BTN,
                                   PIC_WIDGET_DIALING_DELETE_BTN);
        m_pBtnDelete->setBtnAction(TALK_UI_ACTION_DELETE);
        m_pBtnDelete->SetMask(PIC_WIDGET_DIALING_CLICK_MASK);
        connect(m_pBtnDelete, SIGNAL(clicked(const QString &)), this, SLOT(OnBtnClicked(const QString &)));
    }

    m_pBtnDPickup = new qUIPushButton(this);
    if (NULL != m_pBtnDPickup)
    {
        m_pBtnDPickup->setObjectName("BtnDPickup");
        m_pBtnDPickup->set3StatePic(PIC_WIDGET_DIALING_PICKUI_BTN, PIC_WIDGET_DIALING_PICKUI_BTN,
                                    PIC_WIDGET_DIALING_PICKUI_BTN);
        m_pBtnDPickup->setBtnAction(TALK_UI_ACTION_DPICKUP);
        m_pBtnDPickup->setTextAlign(Qt::AlignCenter);
        m_pBtnDPickup->setText(LANG_TRANSLATE(TRID_DIRECT_PICKUP));
        m_pBtnDPickup->SetMask(PIC_WIDGET_DIALING_CLICK_MASK);
        connect(m_pBtnDPickup, SIGNAL(clicked(const QString &)), this, SLOT(OnBtnClicked(const QString &)));
        m_pBtnDPickup->hide();
    }

    m_pBtnGPickup = new qUIPushButton(this);
    if (NULL != m_pBtnGPickup)
    {
        m_pBtnGPickup->setObjectName("BtnGPickup");
        m_pBtnGPickup->set3StatePic(PIC_WIDGET_DIALING_PICKUI_BTN, PIC_WIDGET_DIALING_PICKUI_BTN,
                                    PIC_WIDGET_DIALING_PICKUI_BTN);
        m_pBtnGPickup->setBtnAction(TALK_UI_ACTION_GPICKUP);
        m_pBtnGPickup->setTextAlign(Qt::AlignCenter);
        m_pBtnGPickup->setText(LANG_TRANSLATE(TRID_GROUP_CALL_PICKUP));
        m_pBtnGPickup->SetMask(PIC_WIDGET_DIALING_CLICK_MASK);
        connect(m_pBtnGPickup, SIGNAL(clicked(const QString &)), this, SLOT(OnBtnClicked(const QString &)));
        m_pBtnGPickup->hide();
    }

    m_pBtnRetrieve = new qUIPushButton(this);
    if (NULL != m_pBtnRetrieve)
    {
        m_pBtnRetrieve->setObjectName("BtnRetrieve");
        m_pBtnRetrieve->set3StatePic(PIC_WIDGET_DIALING_PICKUI_BTN, PIC_WIDGET_DIALING_PICKUI_BTN,
                                     PIC_WIDGET_DIALING_PICKUI_BTN);
        m_pBtnRetrieve->setBtnAction(TALK_UI_ACTION_RETRIEVE_PARK);
        m_pBtnRetrieve->setTextAlign(Qt::AlignCenter);
        m_pBtnRetrieve->setText(LANG_TRANSLATE(TRID_RETRIEVE));
        m_pBtnRetrieve->SetMask(PIC_WIDGET_DIALING_CLICK_MASK);
        connect(m_pBtnRetrieve, SIGNAL(clicked(const QString &)), this,
                SLOT(OnBtnClicked(const QString &)));
        m_pBtnRetrieve->hide();
    }

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    //if (BWExeAssis_IsCallInitiationEnable(m_iCurrentAccountId))
    {
        m_pBtnInitiation = new qUIPushButton(this);
        m_pBtnInitiation->setObjectName("BtnInitiation");
        m_pBtnInitiation->set3StatePic(PIC_WIDGET_DIALING_PICKUI_BTN, PIC_WIDGET_DIALING_PICKUI_BTN,
                                       PIC_WIDGET_DIALING_PICKUI_BTN);
        m_pBtnInitiation->setBtnAction(TALK_UI_ACTION_CALL_INITIATION);
        m_pBtnInitiation->setTextAlign(Qt::AlignCenter);
        m_pBtnInitiation->setText(LANG_TRANSLATE(TRID_DSSKEY_CALL_INITIATION));
        m_pBtnInitiation->SetMask(PIC_WIDGET_DIALING_CLICK_MASK);
        connect(m_pBtnInitiation, SIGNAL(clicked(const QString &)), this,
                SLOT(OnBtnClicked(const QString &)));
        m_pBtnInitiation->hide();
    }
#endif

    // 初始化搜索窗口
    m_pSearchTable = new CSearchTable(this);
    if (NULL != m_pSearchTable)
    {
        connect(m_pSearchTable, SIGNAL(clickItem(const QString &, const QVariant &)),
                this, SLOT(OnSearchItemClicked(const QString &, const QVariant &)));
        m_pSearchTable->show();
    }

    m_dialPad.setParent(this);
    m_dialPad.setGeometry(273, 63, 517, 273);
    connect(&m_dialPad, SIGNAL(BtnClicked(const QString &)), this, SLOT(OnBtnClicked(const QString &)));

#if IF_BUG_25055
    m_dialPad.SetIMEBtnVisible(false);
#endif
}

void CDlgDialingUI::OnSuperSearch(const QString & sText)
{
    RefreshPickBtn(NULL);

    //基类响应textchange 操作
    CDlgDialingBase::OnTextChanged(sText);

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    QTimer::singleShot(20, this, SLOT(ShowInitiationBtn()));
#endif
}

// Text变化的时候响应的槽
void CDlgDialingUI::OnTextChanged(const QString & sText)
{
    OnSuperSearch(sText);

    ////发送编辑框字符变化给逻辑
    //SendParentAction(TCT_DIALING_TEXTCHANGE);

    //再发送text 改变的消息给逻辑层  因为这个时候可能进入test模式或其他界面  这个时候再执行后面的操作的话 容易引出其他问题
    OnUIAction(TALK_UI_ACTION_TEXT_CHANGE);
}

// 当DialPad点击的时候
void CDlgDialingUI::OnBtnClicked(const QString & strAction)
{
    //基类先执行 actionpress 操作  暂时是  delete 和 send 发声
    CDlgDialingBase::OnUIActionPress(strAction.toUtf8().data());

    //删除
    if (TALK_UI_ACTION_DELETE == strAction)
    {
        //强制让 编辑框获得焦点
        OnSearchTabNoFocus();

        //因为可能删除预编辑字符 所以这边不需要判断了
        //删除字符
        qOnBackspaceClicked(m_pPswLineEdit);
#if IF_FEATURE_DIAL_ON_HOLD
        if (m_eDialUIType == DIAL_UI_ON_HOLD
                && m_pPswLineEdit != NULL
                && m_pPswLineEdit->text().isEmpty())
        {
            SendUIRequestAction(TALK_ACTION_CANCEL);
        }
#endif
    }
    else if (TALK_UI_ACTION_IME == strAction)
    {
        //切换输入法
        qShift2NextIME(m_pPswLineEdit);

        ResetIMEText();
    }
    else if (DIALPAD_SHOW_KEYPAD == strAction)
    {
        bool bShowWnd = !qGetIsShowWnd();
        qSetIsShowWnd(m_pPswLineEdit, bShowWnd);
    }
}

// 当搜索框失去焦点
void CDlgDialingUI::OnSearchTabNoFocus()
{
    // 设置搜索框焦点与鼠标
    if (NULL != m_pPswLineEdit)
    {
        m_pPswLineEdit->setFocus();
    }

}

// 处理点击Item的事件
void CDlgDialingUI::OnSearchItemClicked(const QString & strNumber, const QVariant & varData)
{
    if (NULL == m_pPswLineEdit)
    {
        return;
    }

    // 将对应的号码设置到editDial上
    m_pPswLineEdit->setText(strNumber);

    if (m_pPswLineEdit->text() != "")
    {
        m_bSearchTransfer = true;

        if (m_eDialUIType == DIAL_UI_TRANSFER)
        {
            //Transfer To情况为穿梭
            bool bOk = false;
            int nShuttleCallId = varData.toInt(&bOk);
            if (bOk && nShuttleCallId > 0)
            {
                //穿梭
                TALKUI_INFO("Dial UI: Enter shuttle!");
                //保存在实例中
                //根据通话ID transfer通话
                int iShuttleCallID = GetShttleCallID();
                if (iShuttleCallID > 0)
                {
                    //有预编辑字符 就先让预编辑字符输出  避免出现其他问题
                    InputLineEditData();

                    OnUIAction(TALK_UI_ACTION_TRANSFER);

//                  SendUIRequestAction(TALK_ACTION_TRAN, iShuttleCallID);
                }
                /*          SendParentAction(TCT_DIALING_TRANSFERBYCALLID);*/
            }
            else
            {
                //Transfer呼出
                OnUIAction(TALK_UI_ACTION_SEND);
                //  SendParentAction(TCT_DIALING_SEND);
            }
        }
        else if (m_eDialUIType == DIAL_UI_CONFERENCE)
        {
            //Transfer To情况为穿梭
            bool bOk = false;
            int nShuttleCallId = varData.toInt(&bOk);
            if (bOk && nShuttleCallId > 0)
            {
                //穿梭
                TALKUI_INFO("Dial UI: Enter shuttle!");
                OnUIAction(TALK_UI_ACTION_CONFERENCE);
            }
            else
            {
                //呼出
                OnUIAction(TALK_UI_ACTION_SEND);
            }
        }
        else
        {
            //呼出号码
            OnUIAction(TALK_UI_ACTION_SEND);
            //  SendParentAction(TCT_DIALING_SEND);
        }
    }
}

#ifdef IF_FEATURE_LONG_PRESS_DIGIT
void CDlgDialingUI::OnExtendedDigitKeyPress()
{
    m_timerLongPress.stop();

    if (m_pPswLineEdit != NULL)
    {
        int iCursorPos = m_pPswLineEdit->cursorPosition();
        if (iCursorPos > 0)
        {
            yl::string strText = m_pPswLineEdit->text().toUtf8().data();
            yl::string strKey = GetStringByDigitKeyLongPress((char)('0' + strText[iCursorPos - 1] -
                                PHONE_KEY_0));

            if (!strKey.empty())
            {
                strText.replace(iCursorPos - 1, 1, strKey.c_str());

                m_pPswLineEdit->setText(toQString(strText.c_str()));
            }
        }
    }
}

yl::string CDlgDialingUI::GetStringByDigitKeyLongPress(char chKey)
{
    switch (chKey)
    {
    case '0':
        return "+";
    default:
        return "";
    }
    return "";
}
#endif
