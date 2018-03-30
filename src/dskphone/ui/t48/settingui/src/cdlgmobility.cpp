#include "model.h"
#include "cdlgmobility.h"
#include "canywhereuilogic.h"
#include "uicommon/messagebox/modmessagebox.h"

#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "setting/src/callforwardcontroller.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "cdlgcallcontrol.h"
#include "modsettingui.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "uikernel/qcomboboxutility.h"
#include "settinguicommon.h"

namespace
{
#define MSG_ID_LOADING 100

//设定edit控件输入长度的最大值
#define EDIT_MAX_LENGTH  30
}

// Date format.
enum PhoneRingType
{
    PHONE_RING_TYPE_NONE = -1,  // Invalid phone ring type.
    PHONE_RING_TYPE_FIXED = 0,  // Fixed.
    PHONE_RING_TYPE_MOBILE = 1,  // Mobile.
    PHONE_RING_TYPE_BOTH = 2,  // Both.
    PHONE_RING_TYPE_MAX
};

static LRESULT OnMobility(msgObject & objMessage)
{
    SETTINGUI_INFO("OnMobility 0x[%x] 0x[%x]",
                   objMessage.message, objMessage.wParam);

    CDlgMobility * pMobilityNew = qobject_cast<CDlgMobility *>(UIManager_GetPrivateDlg(
                                      DLG_CDlgMOBILITY));
    if (NULL == pMobilityNew || !pMobilityNew->IsTopPage())
    {
        return 0;
    }

    pMobilityNew->OnReceiveMsg(objMessage);
    return 0;
}


CDlgMobility::CDlgMobility(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCbMobility(NULL)
    , m_pCbRinging(NULL)
    , m_pCbDiverseInhibite(NULL)
    , m_pCbRequireConfirm(NULL)
    , m_pCbAlertClick(NULL)
    , m_pCbAlertPage(NULL)
    , m_pCbCallCenter(NULL)
    , m_pEditNumber(NULL)
    , m_iAccountId(-1)
{
    InitData();
}

CDlgMobility::~CDlgMobility()
{
}

void CDlgMobility::Init()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN,
                          XSI_MESSAGE_END, OnMobility);
}

void CDlgMobility::Uninit()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN,
                            XSI_MESSAGE_END, OnMobility);
}

void CDlgMobility::InitData()
{
    // 设置标题
    m_strTitle = LANG_TRANSLATE(TRID_MOBILITY).toUtf8().data();

    //初始化 下面会改变父窗口，这里用this初始化是为了防止内存泄漏问题
    m_pCbMobility = new qCheckBox(this);
    m_pCbRinging = new QComboBox(this);
    m_pCbDiverseInhibite = new qCheckBox(this);
    m_pCbRequireConfirm = new qCheckBox(this);
    m_pCbAlertClick = new qCheckBox(this);
    m_pCbAlertPage = new qCheckBox(this);
    m_pCbCallCenter = new qCheckBox(this);

    m_pEditNumber = new QLineEdit(this);

    //指针判空
    if (m_pCbMobility == NULL
            || m_pCbRinging == NULL
            || m_pCbDiverseInhibite == NULL
            || m_pCbRequireConfirm == NULL
            || m_pCbAlertClick == NULL
            || m_pCbAlertPage == NULL
            || m_pCbCallCenter == NULL
            || m_pEditNumber == NULL)
    {
        return;
    }

    m_pCbMobility->SetData(false);
    m_pCbDiverseInhibite->SetData(false);
    m_pCbRequireConfirm->SetData(false);
    m_pCbAlertClick->SetData(false);
    m_pCbAlertPage->SetData(false);
    m_pCbCallCenter->SetData(false);

    qComboBoxUtility::SetComboBoxStyle(m_pCbRinging, Qt::black);
    qComboBoxUtility::SetUpComboBox(m_pCbRinging);

    m_pCbRinging->addItem(LANG_TRANSLATE(TRID_MOBILITY_RING_FIXED), PHONE_RING_TYPE_FIXED);
    m_pCbRinging->addItem(LANG_TRANSLATE(TRID_MOBILITY_RING_MOBILE), PHONE_RING_TYPE_MOBILE);
    m_pCbRinging->addItem(LANG_TRANSLATE(TRID_MOBILITY_RING_BOTH), PHONE_RING_TYPE_BOTH);


    //设置输入框输入的最大长度
    m_pEditNumber->setMaxLength(EDIT_MAX_LENGTH);

    //为输入框安装输入法
    qInstallIME(m_pEditNumber, IME_DIAL);

    /*
    设置标签上文字的大小及标签名
    LANG_TRANSLATE是翻译宏，为了方便语言的转化
    */
    //设置Activation标签
    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_MOBILITY);
    qPair.first = strLab;
    qPair.second = m_pCbMobility;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_MOBILITY_PHONE_NUMBER);
    qPair.first = strLab;
    qPair.second = m_pEditNumber;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_PHONE_TO_RING);
    qPair.first = strLab;
    qPair.second = m_pCbRinging;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_ALERT_CLICK_TO_DIAL_CALLS);
    qPair.first = strLab;
    qPair.second = m_pCbAlertClick;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_ALERT_GOURP_PAGING_CALLS);
    qPair.first = strLab;
    qPair.second = m_pCbAlertPage;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_ENABLE_DIVERSION_INHIBITOR);
    qPair.first = strLab;
    qPair.second = m_pCbDiverseInhibite;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_REQUIRE_ANSWER_CONFIRMATION);
    qPair.first = strLab;
    qPair.second = m_pCbRequireConfirm;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_USE_SERVER_BASED_CALL_CONTROL);
    qPair.first = strLab;
    qPair.second = m_pCbCallCenter;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //安装事件过滤器
    m_pCbMobility->installEventFilter(this);
    m_pCbRinging->installEventFilter(this);
    m_pCbDiverseInhibite->installEventFilter(this);
    m_pCbRequireConfirm->installEventFilter(this);
    m_pCbAlertClick->installEventFilter(this);
    m_pCbAlertPage->installEventFilter(this);
    m_pCbCallCenter->installEventFilter(this);
    m_pEditNumber->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

//处理XSI密码
void CDlgMobility::OnSubmitXSIPassword(const yl::string & strPswd)
{
    BWCallControl_SetAccountXSIPassword(strPswd.c_str(), m_iAccountId);
    ExtraDialogInitData();
}

void CDlgMobility::OnCancelInputXSIPassword()
{
    //回到上一层界面
    emit BtnBackClick();
}

//收到logic层返回的消息
void CDlgMobility::OnReceiveMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case XSI_MESSAGE_REQ_FINISH:
        {
            if (XSI_MOBILITY_GET == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_Destory);
                GetMobilityData();
            }
            else if (XSI_MOBILITY_PUT == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_OK);
                emit BtnBackClick();
            }
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            if (XSI_MOBILITY_GET != objMessage.wParam
                    && XSI_MOBILITY_PUT != objMessage.wParam)
            {
                return ;
            }

            MessageBox_RemoveTopMessageBox(MessageBox_Destory);

            SXsiExtenInfo * pErrInfo = (SXsiExtenInfo *)objMessage.GetExtraData();

            if (NULL == pErrInfo)
            {
                return ;
            }

            if (pErrInfo->m_nError == 401)
            {
                ProcessMsgBoxPassword(m_iAccountId);
            }
            else
            {
                if (m_bIsInLoad)
                {
                    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOAD_FAILED), MESSAGEBOX_NOTE, 1500);
                }
                else
                {
                    // 如果所有的账号都下载失败，则退出
                    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SAVE_FAILED),
                                                MESSAGEBOX_NOTE, 1500, "", "",
                                                MSG_ID_SHOW_NOTE_BEFORE_BACK);
                }
            }


        }
        break;
    default:
        break;
    }
}

//获取FWDNotReach数据
void CDlgMobility::GetMobilityData()
{
    SMobility MobilityInfo;
    // 获取mobility信息
    MBLT_GetMobilityInfo(MobilityInfo, m_iAccountId);

    // 设置mobility对应选项功能配置
    m_pCbMobility->SetData(MobilityInfo.bActive);
    m_pCbRinging->setCurrentIndex(m_pCbRinging->findData(MobilityInfo.nPhoneToRing, Qt::UserRole));
    m_pCbDiverseInhibite->SetData(MobilityInfo.bEnableDiversionInhibitor);
    m_pCbRequireConfirm->SetData(MobilityInfo.bRequireAnswerConfirmation);
    m_pCbAlertClick->SetData(MobilityInfo.bAlertClickToDialCalls);
    m_pCbAlertPage->SetData(MobilityInfo.bAlertGroupPagingCalls);
    m_pCbCallCenter->SetData(MobilityInfo.bBroadworksCallControl);

    m_pEditNumber->setText(toQString(MobilityInfo.strMobilePhoneNumber));
}

bool CDlgMobility::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    //指针判空
    if (m_pCbMobility == NULL
            || m_pCbRinging == NULL
            || m_pCbDiverseInhibite == NULL
            || m_pCbRequireConfirm == NULL
            || m_pCbAlertClick == NULL
            || m_pCbAlertPage == NULL
            || m_pCbCallCenter == NULL
            || m_pEditNumber == NULL)
    {
        return false;
    }

    bool bStatus = m_pCbMobility->GetData();
    QString strNumber = m_pEditNumber->text();

    //判断Number是否合法
    if (!BWCallControl_CheckPhoneNumberValid(strNumber.toUtf8().data())
            && !(!bStatus && strNumber.isEmpty()))
    {
        objErrorContent.strErrorContent = TRID_INVALID_NUMBER;
        objErrorContent.pLineEdit = m_pEditNumber;
        m_pEditNumber->setFocus();
        return false;
    }

    return true;
}


bool CDlgMobility::IsStatusChanged()
{
    return false;
}

bool CDlgMobility::LoadPageData()
{
    GetMobilityData();
    return true;
}

bool CDlgMobility::SavePageData()
{
    m_bIsInLoad = false;

    SMobility MobilityInfo;
    MobilityInfo.bActive = m_pCbMobility->GetData();
    MobilityInfo.nPhoneToRing = m_pCbRinging->itemData(m_pCbRinging->currentIndex(),
                                Qt::UserRole).value<int>();
    MobilityInfo.bEnableDiversionInhibitor = m_pCbDiverseInhibite->GetData();
    MobilityInfo.bAlertClickToDialCalls = m_pCbAlertClick->GetData();
    MobilityInfo.bAlertGroupPagingCalls = m_pCbAlertPage->GetData();
    MobilityInfo.bRequireAnswerConfirmation = m_pCbRequireConfirm->GetData();
    MobilityInfo.bBroadworksCallControl = m_pCbCallCenter->GetData();
    MobilityInfo.strMobilePhoneNumber = m_pEditNumber->text().toUtf8().data();

    // 保存mobility数据
    MBLT_SetMobility(MobilityInfo, m_iAccountId);

    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SAVING_PLEASE_WAIT), MESSAGEBOX_Cancel);
    return false;
}

// 设置子页面前的操作
void CDlgMobility::BeforeSetSubPage(void * pData)
{
    if (NULL != pData)
    {
        m_iAccountId = *(int *)pData;
    }
    else
    {
        m_iAccountId = acc_GetDefaultAccount();
    }
}

// 设置子页面后的操作
void CDlgMobility::AfterSetSubPage(void * pData)
{

}

void CDlgMobility::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iMsgID = pMessageBox->GetID();
    int iRet = pMessageBox->GetResult();
    int iMsgType = pMessageBox->GetType();

    switch (iMsgID)
    {
    case MSG_ID_LOADING:
        {
            if (iRet == MessageBox_Cancel)
            {
                MBLT_CancelRequest();
            }
        }
        break;
    case MSGBOX_ID_POP_XSI_PWD:
        {
            if (iRet == MessageBox_OK)
            {
                yl::string strPwd = pMessageBox->GetExtraData().toString().toUtf8().data();
                OnSubmitXSIPassword(strPwd);
            }
            else if (iRet == MessageBox_Cancel)
            {
                OnCancelInputXSIPassword();
            }
            return;
        }
        break;
    default:
        break;
    }

    if (iMsgType == MESSAGEBOX_Cancel
            && iRet == MessageBox_Cancel)
    {
        MBLT_CancelRequest();
    }
}

void CDlgMobility::ExtraDialogInitData()
{
    //表示处于加载中
    m_bIsInLoad = true;

    if (BWCallControl_IsAccountXSIEnable(m_iAccountId))
    {
        if (ProcessMsgBoxPassword(m_iAccountId))
        {
            return;
        }
        MBLT_QueryMobility(m_iAccountId);
    }
    else
    {
        m_bIsInLoad = false;
    }

    if (!m_bIsInLoad)
    {
        // 如果所有的账号都下载失败，则退出
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_XSI_INFORMATION_IS_INCOMPLETE),
                                    MESSAGEBOX_NOTE, 1500, "", "",
                                    MSG_ID_SHOW_NOTE_BEFORE_BACK);
    }
    else
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, -1, "", "", MSG_ID_LOADING);
    }
}

// 处理密码弹窗
bool CDlgMobility::ProcessMsgBoxPassword(int nLineId)
{
    if (XSI_SIPACCOUNT_NOT_AVAILABLE == CBaseHelper::GetXSIStatus(nLineId))
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SETTUI_TIP_SIP_XSI_AUTHENTICATION_FAILED),
                                    MESSAGEBOX_NOTE, 1500, "", "",
                                    MSG_ID_SHOW_NOTE_BEFORE_BACK);

        return true;
    }
    else if (XSI_AUTH_ERROR == CBaseHelper::GetXSIStatus(nLineId))
    {
        SettingUI_PopPasswordDlg(this, nLineId);
        return true;
    }
    else
    {
        return false;
    }
}
