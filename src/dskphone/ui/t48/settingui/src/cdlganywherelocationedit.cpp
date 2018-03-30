#include "model.h"
#include "cdlganywherelocationedit.h"
#include "messagebox/modmessagebox.h"
#include "canywhereuilogic.h"

#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "uikernel/qlineeditutility.h"
#include "actiondefinecommon.h"
#include "baseui/t4xpicpath.h"
#include "settinguicommon.h"
#include "uikernel/qcheckbox.h"
#include "broadsoft/anywhere/include/anywherecontroller.h"

//设定edit控件输入长度的最大值
namespace
{
#define MSG_ID_CONFIRM_DELETE_LOCATION  100
const int EDIT_MAX_LENGTH = 99;
}

CDlgAnyWhereLocationEdit::CDlgAnyWhereLocationEdit(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pEditName(NULL)
    , m_pEditNumber(NULL)
    , m_pEditOutBndNumber(NULL)
    , m_pCheckBoxActive(NULL)
    , m_pCheckBoxEnabInhb(NULL)
    , m_pCheckBoxReqAnsConf(NULL)
    , m_pCheckBoxCallCtrl(NULL)
{
    InitData();
}

void CDlgAnyWhereLocationEdit::InitData()
{
    //设置标题
    m_strTitle = LANG_TRANSLATE(TRID_EDIT_ANYWHERE_LOCATION).toUtf8().data();

    //初始化 下面会改变父窗口，这里用this初始化是为了防止内存泄漏问题

    m_pEditName = new QLineEdit(this);
    m_pEditNumber = new QLineEdit(this);
    m_pEditOutBndNumber = new QLineEdit(this);

    m_pCheckBoxActive = new qCheckBox(this);
    m_pCheckBoxEnabInhb = new qCheckBox(this);
    m_pCheckBoxReqAnsConf = new qCheckBox(this);
    m_pCheckBoxCallCtrl = new qCheckBox(this);

    //指针判空
    if (m_pEditName == NULL || m_pEditNumber == NULL
            || m_pEditOutBndNumber == NULL || m_pCheckBoxActive == NULL
            || m_pCheckBoxEnabInhb == NULL || m_pCheckBoxReqAnsConf == NULL
            || m_pCheckBoxCallCtrl == NULL)
    {
        return;
    }

    m_pCheckBoxActive->SetData(false);
    m_pCheckBoxEnabInhb->SetData(false);
    m_pCheckBoxReqAnsConf->SetData(false);
    m_pCheckBoxCallCtrl->SetData(false);


    qLineEditUtility::SetLineEditStyle(m_pEditName, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditNumber, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditOutBndNumber, Qt::black);

    //设置输入框输入的最大长度
    m_pEditName->setMaxLength(EDIT_MAX_LENGTH_ANYWHERE_NAME);
    m_pEditNumber->setMaxLength(EDIT_MAX_LENGTH_ANYWHERE_NUMBER);
    m_pEditOutBndNumber->setMaxLength(EDIT_MAX_LENGTH_ANYWHERE_ALTNUMBER);

    //为输入框安装输入法
    qInstallIME(m_pEditName, IME_Name);
    qInstallIME(m_pEditNumber, IME_DIAL);
    qInstallIME(m_pEditOutBndNumber, IME_DIAL);

    //设置Activation标签
    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_NAME);
    qPair.first = strLab;
    qPair.second = m_pEditName;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_PHONE_NUMBER);
    qPair.first = strLab;
    qPair.second = m_pEditNumber;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_ENABLE_THIS_LOCATION);
    qPair.first = strLab;
    qPair.second = m_pCheckBoxActive;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_OUTBOUND_ALTERNATE_PHONE_NUMBER);
    qPair.first = strLab;
    qPair.second = m_pEditOutBndNumber;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_ENABLE_DIVERSION_INHIBITOR);
    qPair.first = strLab;
    qPair.second = m_pCheckBoxEnabInhb;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_REQUIRE_ANSWER_CONFIRMATION);
    qPair.first = strLab;
    qPair.second = m_pCheckBoxReqAnsConf;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_USE_SERVER_BASED_CALL_CONTROL);
    qPair.first = strLab;
    qPair.second = m_pCheckBoxCallCtrl;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //安装事件过滤器
    m_pEditName->installEventFilter(this);
    m_pEditNumber->installEventFilter(this);
    m_pEditOutBndNumber->installEventFilter(this);

    m_pCheckBoxActive->installEventFilter(this);
    m_pCheckBoxEnabInhb->installEventFilter(this);
    m_pCheckBoxReqAnsConf->installEventFilter(this);
    m_pCheckBoxCallCtrl->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}


//设置页面数据
void CDlgAnyWhereLocationEdit::SetDetailData(const SAYWLocation & propSet)
{
    m_pEditName->setText(propSet.m_strDescription.c_str());
    m_pEditNumber->setText(propSet.m_strPhoneNum.c_str());
    m_pEditOutBndNumber->setText(propSet.m_strOutBoundPhoneNum.c_str());
    m_pCheckBoxActive->SetData(propSet.m_bActive);
    m_pCheckBoxEnabInhb->SetData(propSet.m_bEnableDiversionInhibitor);
    m_pCheckBoxReqAnsConf->SetData(propSet.m_bRequireAnswerConfirmation);
    m_pCheckBoxCallCtrl->SetData(propSet.m_bUseBbasedCallControlServices);
    m_LocationData = propSet;

    //没有设置号码的时候不会显示删除键，此处刷新出来
    emit RefreshSoftkey();
}

//获取页面数据
SAYWLocation CDlgAnyWhereLocationEdit::GetPageData()
{
    SAYWLocation pageInfo;
    pageInfo.m_strDescription = m_pEditName->text().toUtf8().data();
    //号码以传入的数据为准
    pageInfo.m_strPhoneNum = m_pEditNumber->text().toUtf8().data();
    pageInfo.m_strOutBoundPhoneNum = m_pEditOutBndNumber->text().toUtf8().data();
    pageInfo.m_bActive = m_pCheckBoxActive->GetData();
    pageInfo.m_bEnableDiversionInhibitor = m_pCheckBoxEnabInhb->GetData();
    pageInfo.m_bRequireAnswerConfirmation = m_pCheckBoxReqAnsConf->GetData();
    pageInfo.m_bUseBbasedCallControlServices = m_pCheckBoxCallCtrl->GetData();
    return pageInfo;
}

bool CDlgAnyWhereLocationEdit::IsInputDataValid()
{
    string strAltPhoneNum = m_pEditOutBndNumber->text().toUtf8().data();
    //检测Outbound Alternate Phone Number/SIP URI是否合法
    if (!strAltPhoneNum.empty()
            && !(BWCallControl_CheckPhoneNumberValid(strAltPhoneNum.c_str())
                 || CheckIsSipURI(strAltPhoneNum))
            || strAltPhoneNum.size() == 1)   //这个字段要大于1
    {
        m_pGeneralFrame->SelectItemByDataIndex(3);
        m_pEditOutBndNumber->setFocus();
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_OUTBOUND_ALTERNATE_NUMBER_INVALID),
                                    MESSAGEBOX_NOTE, SHORT_TIME);
        return false;
    }

    //判断输入的号码是否合法
    if (!BWCallControl_CheckPhoneNumberValid(m_pEditNumber->text().toUtf8().data(), 19))
    {
        m_pGeneralFrame->SelectItemByDataIndex(1);
        m_pEditNumber->setFocus();
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_NUMBER_INVALID),
                                    MESSAGEBOX_NOTE, SHORT_TIME);
        return false;
    }

    //判断号码是否已存在
    if (m_LocationData.m_strPhoneNum != m_pEditNumber->text().toUtf8().data()
            && AnyWhere_CheckNumberExit(m_pEditNumber->text().toUtf8().data()))
    {
        m_pGeneralFrame->SelectItemByDataIndex(1);
        m_pEditNumber->setFocus();
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_NUMBER_ALREADY_EXISTS),
                                    MESSAGEBOX_NOTE, SHORT_TIME);

        return false;
    }

    return true;
}

void CDlgAnyWhereLocationEdit::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iMsgBboxID = pMessageBox->GetID();
    int iRet = pMessageBox->GetResult();
    switch (iMsgBboxID)
    {
    case MSG_ID_CONFIRM_DELETE_LOCATION:
        {
            if (MessageBox_OK == iRet)
            {
                //调用删除单个location的接口，删除选中行的location
                g_pAnywhereUILogic->RequestDeleteLocation(m_LocationData.m_strPhoneNum, true);
            }
        }
        break;
    default:
        break;
    }

}

bool CDlgAnyWhereLocationEdit::IsStatusChanged()
{
    ReturnAllEdit();
    //只要有属性发生变化就返回true，否则返回false
    if (m_LocationData.m_strDescription != m_pEditName->text().toUtf8().data()
            || m_LocationData.m_strPhoneNum != m_pEditNumber->text().toUtf8().data()
            || m_LocationData.m_strOutBoundPhoneNum != m_pEditOutBndNumber->text().toUtf8().data()
            || m_LocationData.m_bActive != (m_pCheckBoxActive->GetData())
            || m_LocationData.m_bEnableDiversionInhibitor != (m_pCheckBoxEnabInhb->GetData())
            || m_LocationData.m_bRequireAnswerConfirmation != (m_pCheckBoxReqAnsConf->GetData())
            || m_LocationData.m_bUseBbasedCallControlServices != (m_pCheckBoxCallCtrl->GetData())
       )
    {
        return true;
    }
    return false;
}

bool CDlgAnyWhereLocationEdit::LoadPageData()
{
    return false;
}

void CDlgAnyWhereLocationEdit::LoadPageData(const yl::string & strPhoneNumber)
{
    g_pAnywhereUILogic->RequestLocationDetail(strPhoneNumber);
}

bool CDlgAnyWhereLocationEdit::SavePageData()
{
    if (IsStatusChanged())
    {
        if (IsInputDataValid())
        {
            //调用save接口
            SAYWLocation pageInfo = GetPageData();
            if (!pageInfo.m_strPhoneNum.empty())
            {
                ///第二个参数
                g_pAnywhereUILogic->RequestModifyLocation(m_LocationData.m_strPhoneNum, pageInfo);
            }

            return false;
        }
        else
        {
            return false;
        }
    }
    else
    {
        emit BtnBackClick();
        return false;
    }
}

CDlgAnyWhereLocationEdit::~CDlgAnyWhereLocationEdit()
{
}

//判断是否Sip URI格式的号码
// The SIP URI must be in user@domain format.
BOOL CDlgAnyWhereLocationEdit::CheckIsSipURI(const string & strNumber)
{
    if (strNumber.empty())
    {
        return FALSE;
    }

    string::size_type nPos = strNumber.find('@');

    if (nPos != string::npos
            && nPos != 0
            && nPos != strNumber.size() - 1)
    {
        return TRUE;
    }

    return FALSE;
}

// 设置页面的SoftKey
bool CDlgAnyWhereLocationEdit::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (m_pEditNumber != NULL && !m_pEditNumber->text().isEmpty())
    {
        objWndData[0].m_strSoftkeyAction = OPTION_ACTION_DELETE;
        objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_REMOVE);
        objWndData[0].m_strIconPath = PIC_SOFTKEY_REMOVE;
    }
    else
    {
        objWndData[0].m_strSoftkeyAction.clear();
        objWndData[0].m_strSoftkeyTitle.clear();
    }

    return true;
}

bool CDlgAnyWhereLocationEdit::OnAction(const QString & strAction)
{
    if (strAction == OPTION_ACTION_DELETE)
    {
        OnDeleteAnyWhere();
        return true;
    }
    else if (strAction == ACTION_SAVE_DATA)
    {
        SavePageData();
        return true;
    }
    return false;
}

void CDlgAnyWhereLocationEdit::OnDeleteAnyWhere()
{
    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_IF_DELETE_LOCATION),
                                MESSAGEBOX_OkCancel, -1, "", "",
                                MSG_ID_CONFIRM_DELETE_LOCATION);
}

void CDlgAnyWhereLocationEdit::OnCustomBack()
{
    // 用于自定义退出界面操作（只回退一个界面）
    // 回到上一层界面
    emit BtnBackClick();
}
