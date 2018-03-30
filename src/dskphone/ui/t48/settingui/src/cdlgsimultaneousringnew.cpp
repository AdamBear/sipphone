#include "model.h"
#include "cdlgsimultaneousringnew.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "uikernel/qlineeditutility.h"
#include "actiondefinecommon.h"
#include "baseui/t4xpicpath.h"
#include "inputmethod/inputmethod.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"

namespace
{
//区别振铃号码输入长度上限
#define EDIT_MAX_LENGTH_SIMNUM 30

enum
{
    MSGBOX_ID_DELETING = 100,
    MSGBOX_ID_CONFIRM_DELETE
};
}

static LRESULT OnSimultaneousRingNewMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("OnSimultaneousRingNewMsg [%d] [%d]",
                   objMessage.message, objMessage.wParam);

    CDlgSimultaneousRingNew * pSRNew = qobject_cast<CDlgSimultaneousRingNew *>(UIManager_GetPrivateDlg(
                                           DLG_CDlgSimultaneousRingNew));

    if (NULL == pSRNew || !pSRNew->IsTopPage())
    {
        return 0;
    }

    pSRNew->OnReceiveMsg(objMessage);
    return 0;
}


CDlgSimultaneousRingNew::CDlgSimultaneousRingNew(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_bIsEditPage(false)
    , m_iSelectedIndex(-1)
    , m_eMessageBoxType(EMBT_NONE)
    , m_iAccountId(-1)
    , m_pStatusCheckBox(NULL)
{
    InitData();
}

CDlgSimultaneousRingNew::~CDlgSimultaneousRingNew()
{
}

void CDlgSimultaneousRingNew::Init()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN,
                          XSI_MESSAGE_END, OnSimultaneousRingNewMsg);
}

void CDlgSimultaneousRingNew::Uninit()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN,
                            XSI_MESSAGE_END, OnSimultaneousRingNewMsg);
}

void CDlgSimultaneousRingNew::InitData()
{
    //设置标题
    m_strTitle = LANG_TRANSLATE(TRID_ADD_NUMBER).toUtf8().data();

    //初始化 下面会改变父窗口，这里用this初始化是为了防止内存泄漏问题
    m_pStatusCheckBox = new qCheckBox(this);
    m_pEditNumber = new QLineEdit(this);

    //指针判空
    if (m_pStatusCheckBox == NULL
            || m_pEditNumber == NULL)
    {
        return;
    }

    m_pStatusCheckBox->SetData(false);

    qLineEditUtility::SetLineEditStyle(m_pEditNumber, Qt::black);

    //长度特殊，不能用lineeditutility设置
    m_pEditNumber->setMaxLength(EDIT_MAX_LENGTH_SIMNUM);

    //为输入框安装输入法
    qInstallIME(m_pEditNumber, IME_DIAL);

    //设置Activation标签
    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_PHONE_NUMBER);
    qPair.first = strLab;
    qPair.second = m_pEditNumber;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_REQUIRE_ANSWER_CONFIRMATION);
    qPair.first = strLab;
    qPair.second = m_pStatusCheckBox;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //安装事件过滤器
    m_pStatusCheckBox->installEventFilter(this);
    m_pEditNumber->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);

    //获取数据
    GetSimultaneousRingNumbers();
}


//收到logic层返回的消息
void CDlgSimultaneousRingNew::OnReceiveMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    //保存或删除成功
    case XSI_MESSAGE_REQ_FINISH:
        {
            if (XSI_SIMULTANEOUSRING_PUT == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_OK);
                emit BtnBackClick();
            }
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            if (XSI_SIMULTANEOUSRING_PUT != objMessage.wParam)
            {
                return ;
            }

            QString strErr;
            if (EMBT_DELETE == m_eMessageBoxType)
            {
                strErr = LANG_TRANSLATE(TRID_DELETE_FIALED);
            }
            else if (EMBT_SAVE == m_eMessageBoxType)
            {
                strErr = LANG_TRANSLATE(TRID_SAVE_FAILED);
            }
            else
            {
                break;
            }

            m_eMessageBoxType = EMBT_NONE;
            MessageBox_RemoveTopMessageBox(MessageBox_Destory);
            MessageBox_ShowCommonMsgBox(this, strErr, MESSAGEBOX_NOTE, SHORT_TIME, "", "",
                                        MSG_ID_SHOW_NOTE_BEFORE_BACK);
        }
        break;
    default:
        break;
    }
}

//获取SimultaneousRing号码列表
void CDlgSimultaneousRingNew::GetSimultaneousRingNumbers()
{
    m_eMessageBoxType = EMBT_LOAD;
    SRP_GetSimultaneousRingInfo(m_RingNumbers);
}

bool CDlgSimultaneousRingNew::IsInputDataValid()
{
    if (m_pEditNumber == NULL
            || m_pStatusCheckBox == NULL)
    {
        return false;
    }

    //判断Name和Number是否为空，如果为空则返回
    if (!BWCallControl_CheckPhoneNumberValid(m_pEditNumber->text().toUtf8().data()))
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_INVALID_NUMBER), MESSAGEBOX_NOTE, SHORT_TIME);
        m_pGeneralFrame->SelectItemByIndex(0);
        m_pEditNumber->setFocus();
        return false;
    }
    else if (SRP_CheckNumberExit(m_pEditNumber->text().toUtf8().data()))
    {
        if (!m_bIsEditPage
                || (m_bIsEditPage
                    && m_iSelectedIndex >= 0
                    && m_iSelectedIndex < m_RingNumbers.simRingLocationArry.size()
                    && m_RingNumbers.simRingLocationArry[m_iSelectedIndex].strAddress !=
                    m_pEditNumber->text().toUtf8().data()))
        {
            MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_NUMBER_ALREADY_EXISTS), MESSAGEBOX_NOTE,
                                        SHORT_TIME);
            m_pGeneralFrame->SelectItemByIndex(0);
            m_pEditNumber->setFocus();
            return false;
        }
    }
    return true;
}

bool CDlgSimultaneousRingNew::IsStatusChanged()
{
    if (m_pEditNumber == NULL
            || m_pStatusCheckBox == NULL)
    {
        return false;
    }

    if (!m_bIsEditPage)
    {
        return !m_pEditNumber->text().isEmpty();
    }

    if (m_iSelectedIndex < 0 || m_iSelectedIndex >= m_RingNumbers.simRingLocationArry.size())
    {
        SETTINGUI_INFO("imultaneousRingNew invalid index");
        return false;
    }

    return  m_RingNumbers.simRingLocationArry[m_iSelectedIndex].strAddress != fromQString(
                m_pEditNumber->text())
            || m_pStatusCheckBox->GetData() !=
            m_RingNumbers.simRingLocationArry[m_iSelectedIndex].bAnswerConfirmationRequired;
}

bool CDlgSimultaneousRingNew::LoadPageData()
{
    return false;
}

//iSelectedIndex：CDlgSimultaneousRingNumbers界面进入编辑界面时的选中项数据
void  CDlgSimultaneousRingNew::SetSelectedData(int iSelectedIndex)
{
    //设置标题
    m_strTitle = LANG_TRANSLATE(TRID_EDIT_NUMBER).toUtf8().data();

    if (iSelectedIndex < 0 || iSelectedIndex >= m_RingNumbers.simRingLocationArry.size())
    {
        return ;
    }

    m_bIsEditPage = true;
    m_iSelectedIndex = iSelectedIndex;
    //设置Number
    m_pEditNumber->setText(toQString(m_RingNumbers.simRingLocationArry[m_iSelectedIndex].strAddress));
    //设置Answer Comfimation
    bool bStatus = m_RingNumbers.simRingLocationArry[m_iSelectedIndex].bAnswerConfirmationRequired;
    m_pStatusCheckBox->SetData(bStatus);
}

bool CDlgSimultaneousRingNew::SavePageData()
{
    if (IsInputDataValid())
    {
        bool bStatus  = m_pStatusCheckBox->GetData();
        yl::string strNumbers = m_pEditNumber->text().toUtf8().data();

        if (!m_bIsEditPage)
        {
            //Add界面
            SSimRingLocation strRingLocation;
            strRingLocation.strAddress = strNumbers.c_str();
            strRingLocation.bAnswerConfirmationRequired = bStatus;
            m_RingNumbers.simRingLocationArry.push_back(strRingLocation);
        }
        else
        {
            //Edit界面
            if (m_iSelectedIndex < 0 || m_iSelectedIndex >= m_RingNumbers.simRingLocationArry.size())
            {
                return false;
            }
            YLVector<SSimRingLocation>::iterator iter = m_RingNumbers.simRingLocationArry.begin();
            iter += m_iSelectedIndex;
            iter->strAddress = strNumbers.c_str();
            iter->bAnswerConfirmationRequired = bStatus;
        }

        m_eMessageBoxType = EMBT_SAVE;
        //执行添加操作
        SRP_SetSimultaneousRing(m_RingNumbers);
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SAVING_PLEASE_WAIT), MESSAGEBOX_NOTE);

        return true;
    }

    return false;
}

// 设置子页面前的操作
void CDlgSimultaneousRingNew::BeforeSetSubPage(void * pData)
{
    if (NULL != pData)
    {
        m_bIsEditPage = true;
        SetSelectedData(*(int *)pData);
    }
    else
    {
        m_bIsEditPage = false;
    }
}

// 设置子页面后的操作
void CDlgSimultaneousRingNew::AfterSetSubPage(void * pData)
{
    if (NULL == pData)
    {
        return;
    }
    if (m_bIsEditPage)
    {
        m_iAccountId = *(int *)pData;
    }
    else
    {
        m_RingNumbers.bActive = *(bool *)pData;
    }
}

// 设置页面的SoftKey
bool CDlgSimultaneousRingNew::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (m_bIsEditPage)
    {
        objWndData[0].m_strSoftkeyAction = OPTION_ACTION_DELETE;
        objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_REMOVE);
        objWndData[0].m_strIconPath = PIC_SOFTKEY_REMOVE;
    }

    return true;
}

void CDlgSimultaneousRingNew::OnDeleteSimultaneousRingNumber()
{
    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_IF_DELETE_THE_NUMBER),
                                MESSAGEBOX_OkCancel, -1, "", "",
                                MSGBOX_ID_CONFIRM_DELETE);
}

bool CDlgSimultaneousRingNew::OnAction(const QString & strAction)
{
    if (strAction == OPTION_ACTION_DELETE)
    {
        OnDeleteSimultaneousRingNumber();
        return true;
    }
    else if (strAction == ACTION_SAVE_DATA)
    {
        SavePageData();
        return true;
    }

    return false;
}

void CDlgSimultaneousRingNew::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iRet = pMessageBox->GetResult();
    switch (pMessageBox->GetID())
    {
    case MSGBOX_ID_CONFIRM_DELETE:
        {
            if (iRet == MessageBox_OK)
            {
                if (NULL == m_pGeneralFrame || m_iSelectedIndex < 0)
                {
                    return;
                }

                m_RingNumbers.simRingLocationArry.removeAt(m_iSelectedIndex);

                m_eMessageBoxType = EMBT_DELETE;
                SRP_SetSimultaneousRing(m_RingNumbers);

                MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_DELETING_PLEASE_WAIT),
                                            MESSAGEBOX_Cancel, -1, "", "",
                                            MSGBOX_ID_DELETING);
            }
        }
        break;
    case MSGBOX_ID_DELETING:
        {
            if (iRet == MessageBox_Cancel)
            {
                SRP_CancelRequest();
            }
        }
        break;
    default:
        break;
    }
}
