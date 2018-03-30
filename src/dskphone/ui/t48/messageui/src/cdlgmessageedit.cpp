#include "cdlgmessageedit.h"
#include "cmsguilogic.h"

#include "include/uicommon.h"
#include "setting/include/common.h"
#include "setting/src/textmessagecontroller.h"
#include "qtcommon/qmisc.h"
#include "mainwnd/mainwnd.h"
#include "baseui/fontcolor.h"

#include "account/include/modaccount.h"

#include "etlmsghandler/modetlmsghandler.h"
#include "inputmethod/inputmethod.h"


#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcomboboxutility.h"

#include "uimanager/rectdefine.h"
#include "uimanager/uimanager_inc.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"

#include "keymap.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "directoryui/include/moddirectoryui.h"

namespace
{
const QRect RECT_DLG_LOCATION(15, 107, 501, 218);

const QRect RECT_TEXT_ALPHNUM(433, 296, 77, 24);

//点击左下角back或按x键
#define MSGBOX_ID_EXIT_BACK         70
//点击右下角home
#define MSGBOX_ID_EXIT_HOME         71
//界面转换导致的关闭，自动保存
#define MSGBOX_ID_AUTOSAVE          72

#define MSGBOX_ID_SEND_MSG          80
#define MSGBOX_ID_SAVE_MSG          81

}

void DlgMessageEdit_DirectoryCallBack(yl::string strContactName, yl::string strContactNumber)
{

    CDlgMessageEdit * pMessEdit = qobject_cast<CDlgMessageEdit *>(UIManager_GetPrivateDlg(
                                      DLG_CDlgMessageEdit));
    if (NULL == pMessEdit)
    {
        return;
    }
    QString name = toQString(strContactName);
    QString number = toQString(strContactNumber);

    pMessEdit->writeData(name, number);
}

///////////////////////////////////////////////////////////////////////////////

CDlgMessageEdit::CDlgMessageEdit(QWidget * parent /*= 0*/)
    : CDlgBaseSubPage(parent)
    , m_pCmbFrom(NULL)
    , m_bIsAccountEmpty(true)
    , m_pLineEditTo(NULL)
    , m_iMsgIdx(-1)
    , m_bAutoSave(true)
    , m_nInstallCmbIndex(0)
    , m_strInstallTo("")
    , m_strInstallText("")
{
    // 设置小窗口的风格为Message
    SetWindowType(eWT_Message);

    InitData();
}

CDlgMessageEdit::~CDlgMessageEdit()
{
    UninstallAllIME();
    qUninstallIME(m_pTextEditor);
}

void CDlgMessageEdit::InitData()
{
    //发送账号选择和目的账号填写框
    m_pCmbFrom = new QComboBox(this);
    m_pLineEditTo = new QLineEdit(this);
    m_pTextEditorBackground = new QLabel(this);
    m_pTextEditor = new QTextEdit(this);
    m_pLabelWordNum = new QLabel(this);
    if (NULL == m_pCmbFrom
            || NULL == m_pLineEditTo
            || NULL == m_pTextEditorBackground
            || NULL == m_pTextEditor
            || NULL == m_pLabelWordNum)
    {
        return;
    }

    this->installEventFilter(this);
    m_pCmbFrom->installEventFilter(this);
    m_pLineEditTo->installEventFilter(this);
    m_pTextEditor->installEventFilter(this);

    //安装输入法
    qInstallIME(m_pLineEditTo, IME_ALL);
    qSetCurrentIME(m_pLineEditTo, IME_123);
    //设置下拉框样式
    qComboBoxUtility::SetComboBoxStyle(m_pCmbFrom, Qt::black);
    //设置样式
    qLineEditUtility::SetLineEditStyle(m_pLineEditTo, Qt::black);
    //设置最大输入长度
    qLineEditUtility::SetMaxLength(m_pLineEditTo, EDIT_TYPE_PHONE_NUMBER);

    m_vecWidgetPair.clear();
    QWidgetPair widgetPair;

    //添加各项
    //from
    widgetPair.first    = LANG_TRANSLATE(TRID_FROM);
    widgetPair.second   = m_pCmbFrom;
    m_vecWidgetPair.push_back(widgetPair);

    //to
    widgetPair.first    = LANG_TRANSLATE(TRID_TO);
    widgetPair.second   = m_pLineEditTo;
    widgetPair.iExtraButten = DIRECTORY_EXTRA;
    m_vecWidgetPair.push_back(widgetPair);
    widgetPair.iExtraButten = NONE_EXTRA;

    //添加到显示视图中
    AddWidget(m_vecWidgetPair);

    m_pTextEditorBackground->setPixmap(THEME_GET_BMP(PIC_TEXT_MSG_EDIT));
    m_pTextEditorBackground->setGeometry(RECT_DLG_LOCATION);

    //编辑框
    //安装输入法
    qInstallIME(m_pTextEditor, IME_ALL);
    qSetCurrentIME(m_pTextEditor, IME_abc);
    qSetQTextEditMaxLength(m_pTextEditor, DEFAULT_QTEXTEDIT_MAXLENGTH);

    if (FullKeyboardEnabled())
    {
        qUseKeyBoardFromIME(m_pTextEditor);
        m_pGeneralFrame->setEnableAdjust(false);
    }

    m_pTextEditor->setTextBackgroundColor(QColor(0, 0, 0, 0));
    m_pTextEditor->setTextColor(THEME_TEXT_COLOR);
    m_pTextEditor->setGeometry(RECT_DLG_LOCATION);

    QPalette pal;
    m_pLabelWordNum->setText("0/" + QString::number(DEFAULT_QTEXTEDIT_MAXLENGTH, 10));
    const QColor color = Qt::darkGray;
    pal = m_pLabelWordNum->palette();
    pal.setBrush(QPalette::Text, QBrush(color));
    pal.setBrush(QPalette::WindowText, QBrush(color));
    m_pLabelWordNum->setPalette(pal);
    m_pLabelWordNum->setGeometry(RECT_TEXT_ALPHNUM);
    m_pLabelWordNum->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    connect(m_pTextEditor, SIGNAL(textChanged()), this, SLOT(SetAlphNumText()));
}

// 初始化
void CDlgMessageEdit::Init()
{
    if (NULL == m_pCmbFrom || NULL == m_pLineEditTo)
    {
        return;
    }

    //设置From号码列表
    m_pCmbFrom->clear();
    int iAccountNum = acc_AccountNum();
    yl::string strValue;
    yl::string strUserName;
    for (int i = 0; i < iAccountNum; i++)
    {
        BOOL bAccountEnable = (1 == configParser_GetCfgItemIntValue(kszAccountEnable, i));

        // 帐号开启才可以发送
        if (bAccountEnable)
        {
            strValue = acc_GetAccountShowText(i);

            strUserName = configParser_GetCfgItemStringValue(kszAccountUserName, i);

            int nAccountID = acc_GetAccountIDByName(strUserName.c_str());
            if (acc_IsAccountAvailable(nAccountID))
            {
                m_pCmbFrom->addItem(toQString(strValue.c_str()), nAccountID);
            }
        }
    }

    //本地帐号为空
    if (m_pCmbFrom->count() == 0)
    {
        m_bIsAccountEmpty = true;
        m_pCmbFrom->addItem(LANG_TRANSLATE(TRID_ACCOUNT_EMPTY), -1);
    }
    else
    {
        m_bIsAccountEmpty = false;
    }

    m_iMsgIdx = g_pMsgUILogic->GetCurrentMsgId();

    //如果m_iMsgIdx大于-1说明该窗口要显示的不是新消息，而是要编辑一条待转发的消息或回复或草稿
    if (m_iMsgIdx > -1)
    {
        TextMsgItemDetailData pageData;
        yl::string strMsgBoxAction = g_pMsgUILogic->GetBoxAction();
        if (g_pMsgUILogic->GetMessageDetail(&pageData, strMsgBoxAction, m_iMsgIdx))
        {
            SetMessageDetail(pageData);
        }
    }
    else
    {
        m_pLineEditTo->setText("");
        m_pTextEditor->setText("");
        SetWindowTitle(LANG_TRANSLATE(TRID_NEW_MESSAGE));
    }
}

void CDlgMessageEdit::SetFromCmbCurrentIndex(QString addrText)
{
    if (m_pCmbFrom != NULL)
    {
        for (int i = 0; i < m_pCmbFrom->count(); ++i)
        {
            if (addrText.compare(m_pCmbFrom->itemText(i), Qt::CaseInsensitive) == 0)
            {
                m_pCmbFrom->setCurrentIndex(i);
                //记录到成员变量中
                m_nInstallCmbIndex = i;
                break;
            }
        }
    }
}

void CDlgMessageEdit::SetMessageDetail(const TextMsgItemDetailData & sDetail)
{
    if (NULL == m_pLineEditTo || NULL == m_pCmbFrom || NULL == m_pTextEditor)
    {
        return;
    }

    //获取当前box类型
    yl::string strCurrentBox = g_pMsgUILogic->GetBoxAction();

    if (kszCAInBox == strCurrentBox)
    {
        //如果是收件箱表示现在弹出的edit窗口是回复

        //所以将原消息的收件人设为当前的发件人
        //获取当前消息的收件人信息
        QString addrText = QString::fromUtf8(sDetail.m_strToUserName.c_str());
        //如果当前列表中有对应账号则将其设置为新消息的发件人
        SetFromCmbCurrentIndex(addrText);

        //将原消息的发件人设为当前的收件人
        m_strInstallTo = QString::fromUtf8(sDetail.m_strFromUserName.c_str());
        m_pLineEditTo->setText(m_strInstallTo);

        //并且不需要设置消息内容
        m_strInstallText = "";
        m_pTextEditor->setText(m_strInstallText);
        SetWindowTitle(LANG_TRANSLATE(TRID_REPLAY));
    }
    else
    {
        m_strInstallText = QString::fromUtf8(sDetail.m_strText.c_str());
        //如果是其他情况，即发件箱或已发送或草稿，则加载原消息内容
        m_pTextEditor->setText(m_strInstallText);

        //将光标设置到文本最后
        QTextCursor cursor = m_pTextEditor->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_pTextEditor->setTextCursor(cursor);

        //如果是草稿箱则照草稿中保存的内容加载
        if (kszCADraftBox == strCurrentBox)
        {
            //获取当前消息的收件人信息
            QString addrText = QString::fromUtf8(sDetail.m_strFromUserName.c_str());
            //如果当前列表中有对应账号则将其设置为新消息的发件人
            SetFromCmbCurrentIndex(addrText);

            m_strInstallTo = QString::fromUtf8(sDetail.m_strToUserName.c_str());
            m_pLineEditTo->setText(m_strInstallTo);

            SetWindowTitle(LANG_TRANSLATE(TRID_EDIT));
        }
        //如果是转发
        else // if (kszCASentBox == strCurrentBox || kszCAOutBox == strCurrentBox)
        {
            //获取当前消息的收件人信息
            QString addrText = QString::fromUtf8(sDetail.m_strToUserName.c_str());
            //如果当前列表中有对应账号则将其设置为新消息的发件人
            SetFromCmbCurrentIndex(addrText);

            //不需要设置收件人
            m_strInstallTo = "";
            m_pLineEditTo->setText(m_strInstallTo);

            SetWindowTitle(LANG_TRANSLATE(TRID_FORWARD));
        }
    }
}

// 关闭窗口时自动保存未保存的消息
void CDlgMessageEdit::Uninit()
{
    //需要自动保存（非手动退出）和有内容修改的情况才进行自动保存
    if (m_bAutoSave && ProcessInputs(MSGBOX_ID_AUTOSAVE))
    {
        g_pMsgUILogic->SaveDraftMessage(m_iMsgIdx, true, true, this);
    }
}

bool CDlgMessageEdit::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SAVE);
    objWndData[0].m_strSoftkeyAction = DM_SOFTKEY_CLICK_SAVE;
    objWndData[0].m_strIconPath = PIC_SOFTKEY_SAVE;

    objWndData[1].m_strSoftkeyTitle = "";
    objWndData[1].m_strSoftkeyAction = DM_SOFTKEY_SWITCH_IME;

    objWndData[2].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE);
    objWndData[2].m_strSoftkeyAction = DM_SOFTKEY_DELETE_IME;
    objWndData[2].m_strIconPath = PIC_SOFTKEY_DELETE;

    objWndData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SEND_MESSAGE);
    objWndData[3].m_strSoftkeyAction = DM_SOFTKEY_CLICK_SEND;
    objWndData[3].m_strIconPath = PIC_SOFTKEY_MSG_SEND;

    return true;
}

int CDlgMessageEdit::GetFromAccountID() const
{
    if (m_pCmbFrom != NULL)
    {
        int iIdx = m_pCmbFrom->currentIndex();

        if (iIdx < 0)
        {
            return -1;
        }
        else
        {
            return m_pCmbFrom->itemData(iIdx).toInt();
        }
    }
    else
    {
        return -1;
    }
}

//TO联系人地址
QString CDlgMessageEdit::GetToAddr() const
{
    if (m_pLineEditTo != NULL)
    {
        return m_pLineEditTo->text().trimmed();
    }
    else
    {
        return "";
    }
}

QString CDlgMessageEdit::GetTextContent() const
{
    if (NULL != m_pTextEditor)
    {
        return m_pTextEditor->toPlainText();
    }

    return "";
}

void CDlgMessageEdit::UpdateWnd()
{
    CDlgBaseSubPage::UpdateWnd();

    if (NULL != m_pTextEditor)
    {
        if (m_bPageEnable)
        {
            m_pTextEditor->setFocus();
        }
        else
        {
            m_pTextEditor->clearFocus();
            m_pTextEditor->hide();

            if (NULL != m_pTextEditorBackground)
            {
                m_pTextEditorBackground->hide();
            }

            if (NULL != m_pLabelWordNum)
            {
                m_pLabelWordNum->hide();
            }
        }
    }
}

bool CDlgMessageEdit::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (NULL == pObj
            || NULL == pEvent)
    {
        return true;
    }

    //判断事件类型
    switch (pEvent->type())
    {
    case QEvent::FocusIn:
        {
            if (NULL != m_pTextEditor && pObj == m_pTextEditor)
            {
                emit RefreshSoftkey();

                qSetIsShowWnd(m_pTextEditor, true);
                if (qGetIsShowWnd())
                {
                    AdjustMsgEdit(true);
                }
            }
        }
        break;
    case QEvent::FocusOut:
        {
            if (NULL != m_pTextEditor && pObj == m_pTextEditor)
            {
                emit RefreshSoftkey();
                qSetIsShowWnd(m_pTextEditor, false);
            }
        }
        break;
    case QEvent::MouseButtonPress:
        {
            if (NULL != m_pTextEditor && pObj == m_pTextEditor->viewport() && FullKeyboardEnabled())
            {
                if (!qGetIsShowWnd() || m_pTextEditor != GetCurrInputWgt())
                {
                    qSetIsShowWnd(m_pTextEditor, true);
                }
            }
        }
        break;
    //键盘输入事件
    case  QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;

            // 特殊处理上下键，移动到textedit
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_DOWN:
                {
                    if (pObj == m_pLineEditTo)
                    {
                        if (NULL != m_pTextEditor)
                        {
                            m_pTextEditor->setFocus();
                            return true;
                        }
                    }
                }
                break;
            case PHONE_KEY_UP:
                {
                    if (pObj == m_pCmbFrom)
                    {
                        if (NULL != m_pTextEditor)
                        {
                            m_pTextEditor->setFocus();
                            return true;
                        }
                    }
                }
                break;
            }
            //如果焦点在输入框上则先进行输入框的处理
            if (NULL != m_pTextEditor && focusWidget() == m_pTextEditor)
            {
                switch (pKeyEvent->key())
                {
                //如果是ok、cancel的话，输入框不用处理
                case PHONE_KEY_OK:
                case PHONE_KEY_CANCEL:
                    {
                        break;
                    }
                //如果是输入键和左右键则传给输入框
                case PHONE_KEY_0:
                case PHONE_KEY_1:
                case PHONE_KEY_2:
                case PHONE_KEY_3:
                case PHONE_KEY_4:
                case PHONE_KEY_5:
                case PHONE_KEY_6:
                case PHONE_KEY_7:
                case PHONE_KEY_8:
                case PHONE_KEY_9:
                case PHONE_KEY_STAR:
                case PHONE_KEY_POUND:
                case PHONE_KEY_LEFT:
                case PHONE_KEY_RIGHT:
                    {
                        qPassEventToEdit(m_pTextEditor, pKeyEvent);
                        return true;
                    }
                    break;
                default:
                    if (!FullKeyboardEnabled())
                    {
                        // 其他所有键均屏蔽
                        return true;
                    }
                    break;
                }
            }

            switch (pKeyEvent->key())
            {
            //焦点不再输入框时，处理ok、cancel按键
            case PHONE_KEY_OK:
                {
                    OnSendBtnClicked();
                    return true;
                }
                break;
            case PHONE_KEY_CANCEL:
                {
                    OnBtnBackClick();
                    return true;
                }
                break;
            //其他情况由父类的eventfilter处理
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObj, pEvent);
}

bool CDlgMessageEdit::OnAction(const QString & strAction)
{
    if (strAction == DM_SOFTKEY_CLICK_SAVE)
    {
        //保存到草稿
        SaveToDraftBox();
        return true;
    }
    else if (strAction == DM_SOFTKEY_SWITCH_IME)
    {
        //切换输入法
        OnIMEBtnClicked();
        return true;
    }
    else if (strAction == DM_SOFTKEY_DELETE_IME)
    {
        //退格
        OnIMEDeleteBtnClicked();
        return true;
    }
    else if (strAction == DM_SOFTKEY_CLICK_SEND)
    {
        //信息发送
        OnSendBtnClicked();
        return true;
    }
    else if (strAction == ACTION_BACK)
    {
        OnBtnBackClick();
        return true;
    }

    else
    {
        return false;
    }
}

void CDlgMessageEdit::OnBtnBackClick()
{
    //m_bAutoSave用来判断是否需要弹出discard弹出框
    //ProcessInputs用来判断内容是否被修改并负责弹出discard框
    if (m_bAutoSave && ProcessInputs(MSGBOX_ID_EXIT_BACK))
    {
        //ProcessInputs返回true说明弹出了discard弹出框，则不退出界面
    }
    else
    {
        emit BtnBackClick();
    }
}

bool CDlgMessageEdit::PorcessHomeBtnClick()
{
    if (m_bAutoSave && ProcessInputs(MSGBOX_ID_EXIT_HOME))
    {
        //返回true表示已被处理，不需要settingpage去处理了
        return true;
    }
    else
    {
        return false;
    }
}

//保存到草稿
void CDlgMessageEdit::SaveToDraftBox()
{
    if (NULL != m_pTextEditor)
    {
        InputPreeditContent(m_pTextEditor);
    }

    MessageBox_ShowCommonMsgBox(this,
                                LANG_TRANSLATE(TRID_SAVED),
                                MESSAGEBOX_NOTE, 1000,  "", "", MSGBOX_ID_SAVE_MSG);
}

//输入法切换
void CDlgMessageEdit::OnIMEBtnClicked()
{
    _MainWND.SwitchNextIMEName(this->focusWidget());
}

//输入法退格
void CDlgMessageEdit::OnIMEDeleteBtnClicked()
{
    qOnBackspaceClicked(this);
}

//发送
void CDlgMessageEdit::OnSendBtnClicked()
{
    if (NULL == m_pGeneralFrame || NULL == m_pCmbFrom || NULL == m_pLineEditTo)
    {
        return;
    }
    //内容为空不能发送
    QString strContent = GetTextContent();
    if (strContent.isEmpty())
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_TEXT_CANNOT_BE_EMPTY),
                                    MESSAGEBOX_Cancel, 2000);
        m_pTextEditor->setFocus();
        return;
    }

    //发送账号不可用则不能发送
    int iAccountId = GetFromAccountID();
    if (iAccountId < 0)
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_INVALID_ACCOUNT), MESSAGEBOX_Cancel, 2000);
        m_pGeneralFrame->SelectItemByIndex(0);
        m_pCmbFrom->setFocus();
        return;
    }

    //接收账号为空不能发送
    if (GetToAddr().isEmpty())
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_TARGET_CANNOT_BE_EMPTY), MESSAGEBOX_Cancel, 2000);

        m_pGeneralFrame->SelectItemByIndex(1);
        m_pLineEditTo->setFocus();
        return;
    }

    //弹出发送弹出框，在回调时执行发送操作
    MessageBox_ShowCommonMsgBox(this,
                                LANG_TRANSLATE(TRID_SENDING_MESSAGE),
                                MESSAGEBOX_NOTE, 1000,  "", "", MSGBOX_ID_SEND_MSG);
}

//当编辑中的文字数量发生变化时更新右下角标签的显示
void CDlgMessageEdit::SetAlphNumText()
{
    if (NULL == m_pTextEditor || NULL == m_pLabelWordNum)
    {
        return;
    }

    //获取文字数量
    int iAlphNum = (m_pTextEditor->toPlainText()).size();

    //超过200的字符删掉
    if (iAlphNum > 200)
    {
        QTextCursor cursor = m_pTextEditor->textCursor();
        cursor.deletePreviousChar();
        m_pTextEditor->setTextCursor(cursor);
    }

    //重新获取数字
    iAlphNum = (m_pTextEditor->toPlainText()).size();
    QString strAlph = QString::number(iAlphNum, 10);
    //生成标签应显示的文字
    strAlph += "/" + QString::number(DEFAULT_QTEXTEDIT_MAXLENGTH, 10);
    //更新标签
    m_pLabelWordNum->setText(strAlph);

    AdjustMsgEdit(true);
}

bool CDlgMessageEdit::ProcessInputs(int nUIAction)
{
    if (NULL != m_pTextEditor)
    {
        InputPreeditContent(m_pTextEditor);
    }

    //如果可编辑的三个控件内容都与加载时没有变化则不保存
    //如果没有注册账号则不判断发送账号是否有修改
    if (m_bIsAccountEmpty)
    {
        if (GetToAddr() == m_strInstallTo
                && GetTextContent() == m_strInstallText)
        {
            return false;
        }
    }
    else
    {
        if (GetFromAccountID() == m_nInstallCmbIndex
                && GetToAddr() == m_strInstallTo
                && GetTextContent() == m_strInstallText)
        {
            return false;
        }
    }

    //如果是界面转换导致的关闭则返回true直接保存，不弹出save框
    if (MSGBOX_ID_AUTOSAVE == nUIAction)
    {
        return true;
    }

    MsgBoxSoftkey softkeyCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
    MsgBoxSoftkey softkeyDiscard(LANG_TRANSLATE(TRID_DISCARD), MessageBox_Discard);
    MsgBoxSoftkey softkeySave(LANG_TRANSLATE(TRID_SAVE), MessageBox_OK);
    VecMsgBoxSoftkey vecSoftKey;

    vecSoftKey.push_back(softkeyCancel);
    vecSoftKey.push_back(softkeyDiscard);
    vecSoftKey.push_back(softkeySave);

    QString strNote = LANG_TRANSLATE(TRID_DO_YOU_WANT_TO_SAVE_THE_MESSAGE_OR_DISCARD_IT);

    MessageBox_ShowCommonMsgBox(this, strNote, vecSoftKey, -1, nUIAction);

    return true;
}

void CDlgMessageEdit::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    MESSAGEBOX_TYPE eMsgBoxType = pMessageBox->GetType();
    MESSAGEBOX_RESULT eResult = pMessageBox->GetResult();
    int nAction = pMessageBox->GetID();

    switch (eMsgBoxType)
    {
    case MESSAGEBOX_SPECIAL:
        {
            if (MessageBox_Discard == eResult)
            {
                if (MSGBOX_ID_EXIT_BACK == nAction)
                {
                    m_bAutoSave = false;
                    emit BtnBackClick();
                }
                else if (MSGBOX_ID_EXIT_HOME == nAction)
                {
                    m_bAutoSave = false;
                    CBaseDialog::OnHomeButtonClick();
                }
            }
            else if (MessageBox_OK == eResult)
            {
                m_bAutoSave = false;
                SaveToDraftBox();
            }
        }
        break;

    case MESSAGEBOX_NOTE:
        {
            switch (nAction)
            {
            case MSGBOX_ID_SEND_MSG:
                {
                    m_bAutoSave = false;
                    g_pMsgUILogic->DoSendMessage(GetToAddr(), GetFromAccountID(), this);
                }
                break;
            case MSGBOX_ID_SAVE_MSG:
                {
                    m_bAutoSave = false;
                    g_pMsgUILogic->SaveDraftMessage(m_iMsgIdx, true, true, this);
                    //保存后窗口跳转到前一级界面
                    emit BtnBackClick();
                }
                break;
            default:
                break;
            }
        }
    default:
        break;
    }
}

void CDlgMessageEdit::OnKeyboardShow()
{
    AdjustMsgEdit(true);
}

void CDlgMessageEdit::OnKeyboardHide()
{
    AdjustMsgEdit(false);
}

void CDlgMessageEdit::AdjustMsgEdit(bool bShow)
{
    if (!FullKeyboardEnabled())
    {
        return;
    }

    // 判断textEdit光标是否被全键盘挡住
    if (m_pTextEditor == NULL)
    {
        return;
    }
    if (bShow)
    {
        if (m_pTextEditor != GetCurrInputWgt())
        {
            return;
        }

        QRect rcCursor = m_pTextEditor->cursorRect();
        QPoint ptCursor = m_pTextEditor->mapToGlobal(rcCursor.bottomLeft());
        if (ptCursor.y() < g_iFullKeyBoardY - rcCursor.height())
        {
            return;
        }

        QRect rc = m_pTextEditor->geometry();
        rc.setY(rc.y() - rcCursor.height());
        rc.setHeight(RECT_DLG_LOCATION.height());
        m_pTextEditor->setGeometry(rc);
        m_pTextEditorBackground->setGeometry(rc);

        rc = m_pLabelWordNum->geometry();
        rc.setY(rc.y() - rcCursor.height());
        rc.setHeight(RECT_TEXT_ALPHNUM.height());
        m_pLabelWordNum->setGeometry(rc);

        rc = m_pGeneralFrame->geometry();
        rc.setY(rc.y() - rcCursor.height());
        m_pGeneralFrame->setGeometry(rc);
    }
    else
    {
        QRect rc = RECT_DLG_LOCATION;
        m_pTextEditor->setGeometry(rc);
        m_pTextEditorBackground->setGeometry(rc);

        rc = RECT_TEXT_ALPHNUM;
        m_pLabelWordNum->setGeometry(rc);

        m_pListDelegate->AdjustFrameList(true);
    }
}
void CDlgMessageEdit::OnExtraAction(int eActionType)
{
    if (eActionType == DIRECTORY_EXTRA)
    {
        DM_EnterSelectDirectoryUI(DlgMessageEdit_DirectoryCallBack);
    }
}

void CDlgMessageEdit::writeData(QString name, QString number)
{
    m_pLineEditTo->setText(number);
}
