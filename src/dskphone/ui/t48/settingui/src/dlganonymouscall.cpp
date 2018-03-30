#include "model.h"
#include "dlganonymouscall.h"
#include "setting/include/common.h"

#include "setting/src/anonymouscallcontroller.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "uikernel/qcheckbox.h"
#include "uikernel/qcomboboxutility.h"
#include "uikernel/qlineeditutility.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

CDlgAnonymousCall::CDlgAnonymousCall(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCheckBoxCode(NULL)
    , m_pCheckBoxAnonymousCall(NULL)
    , m_pCheckBoxReject(NULL)
    , m_pCheckBoxAnonymousRejectCode(NULL)
    , m_pEditCallOffCode(NULL)
    , m_pEditCallOnCode(NULL)
    , m_pEditRejectOffCode(NULL)
    , m_pEditRejectOnCode(NULL)
    , m_bInitSuccess(false)
{
    InitData();
}

CDlgAnonymousCall::~CDlgAnonymousCall()
{
}

//界面初始化
void CDlgAnonymousCall::InitData()
{
    m_pCheckBoxAnonymousCall = new qCheckBox(this);
    m_pCheckBoxCode = new qCheckBox(this);
    m_pCheckBoxReject = new qCheckBox(this);
    m_pCheckBoxAnonymousRejectCode = new qCheckBox(this);
    m_pEditCallOffCode = new QLineEdit(this);
    m_pEditCallOnCode = new QLineEdit(this);
    m_pEditRejectOffCode = new QLineEdit(this);
    m_pEditRejectOnCode = new QLineEdit(this);

    if (m_pCheckBoxCode == NULL || m_pCheckBoxReject == NULL
            || m_pEditCallOffCode == NULL || m_pEditCallOnCode == NULL
            || m_pEditRejectOffCode == NULL || m_pEditRejectOnCode == NULL
            || m_pCheckBoxAnonymousCall == NULL || m_pCheckBoxAnonymousRejectCode == NULL)
    {
        m_bInitSuccess = false;
        return;
    }

    m_bInitSuccess = true;

#if IF_BUG_28909
    //http://192.168.1.99/Bug.php?BugID=28909
    //QLOOG: 翻译成日语会显示不下，因此不需要添加翻译
    m_pCheckBoxCode->SetContent(TRID_ON_CODE, TRID_OFF_CODE);
    m_pCheckBoxAnonymousRejectCode->SetContent(TRID_ON_CODE, TRID_OFF_CODE);
#else
    // 需要添加翻译
    m_pCheckBoxCode->SetContent(LANG_TRANSLATE(TRID_ON_CODE), LANG_TRANSLATE(TRID_OFF_CODE));
    m_pCheckBoxAnonymousRejectCode->SetContent(LANG_TRANSLATE(TRID_ON_CODE),
            LANG_TRANSLATE(TRID_OFF_CODE));
#endif

    qLineEditUtility::SetLineEditStyle(m_pEditCallOffCode, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditCallOnCode, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditRejectOffCode, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditRejectOnCode, Qt::black);

    //设置最大输入长度
    qLineEditUtility::SetMaxLength(m_pEditCallOffCode, EDIT_TYPE_CODE);
    qLineEditUtility::SetMaxLength(m_pEditCallOnCode, EDIT_TYPE_CODE);
    qLineEditUtility::SetMaxLength(m_pEditRejectOffCode, EDIT_TYPE_CODE);
    qLineEditUtility::SetMaxLength(m_pEditRejectOnCode, EDIT_TYPE_CODE);

    qInstallIME(m_pEditCallOffCode, IME_DIAL);
    qInstallIME(m_pEditCallOnCode, IME_DIAL);
    qInstallIME(m_pEditRejectOffCode, IME_DIAL);
    qInstallIME(m_pEditRejectOnCode, IME_DIAL);

    //为界面上各输入控件安装事件过滤器
    m_pCheckBoxAnonymousCall->installEventFilter(this);
    m_pCheckBoxCode->installEventFilter(this);
    m_pEditCallOffCode->installEventFilter(this);
    m_pEditCallOffCode->installEventFilter(this);
    m_pCheckBoxReject->installEventFilter(this);
    m_pCheckBoxAnonymousRejectCode->installEventFilter(this);
    m_pEditRejectOffCode->installEventFilter(this);
    m_pEditRejectOnCode->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

//加载数据
bool CDlgAnonymousCall::LoadPageData()
{
    this->setFocus();

    InitListItem();

    qLineEditUtility::SetCurrentInputMethod(m_pEditCallOnCode, EDIT_TYPE_PHONE_NUMBER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditCallOffCode, EDIT_TYPE_PHONE_NUMBER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditRejectOnCode, EDIT_TYPE_PHONE_NUMBER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditRejectOffCode, EDIT_TYPE_PHONE_NUMBER);

    m_anonymousCallData.Reset();
    bool bIsSuccess =
        g_pAnonymousCallController->GetAnonymousCallDataByClickAction(&m_anonymousCallData,
                m_strSubPageAction);
    //设置当前页面数据
    if (bIsSuccess)
    {
        SetCurrentPageData(m_anonymousCallData);
        return true;
    }
    else
    {
        return false;
    }
}

//获取数据状态是否改变
bool CDlgAnonymousCall::IsStatusChanged()
{
    AnonymousCallData anonymousCallData;
    //取得当前页面数据
    GetCurrentPageData(anonymousCallData);
    //比较前后页面数据
    if (ComparePageData(m_anonymousCallData, anonymousCallData))
    {
        //相等情况
        return false;
    }

    return true;
}

bool CDlgAnonymousCall::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (!m_bInitSuccess)
    {
        return false;
    }

    //broadsoft开启之后不显示code，http://10.2.1.199/Bug.php?BugID=44441
    if (configParser_GetConfigInt(kszBroadsoftActive) == 0
#if IF_FEATURE_METASWITCH
            && configParser_GetConfigInt(kszMetaSwitchActive) == 0
#endif
       )
    {
        //on code与off code需要同时可用或同时不可用
        if (m_pEditCallOnCode->text().isEmpty()
                && !m_pEditCallOffCode->text().isEmpty())
        {
            if (!qLineEditUtility::IsFormatValid(m_pEditCallOffCode->text(), EDIT_TYPE_CODE))
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_CALL_OFF_CODE_IS_INVALID);
                objErrorContent.pLineEdit = m_pEditCallOffCode;
                return false;
            }
        }
        if (m_pEditCallOffCode->text().isEmpty()
                && !m_pEditCallOnCode->text().isEmpty())
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_OFF_CODE_CANNOT_BE_EMPTY);
            objErrorContent.pLineEdit = m_pEditCallOffCode;
            return false;
        }
        if (!m_pEditCallOnCode->text().isEmpty()
                && !m_pEditCallOffCode->text().isEmpty())
        {
            if (!qLineEditUtility::IsFormatValid(m_pEditCallOnCode->text(), EDIT_TYPE_CODE))
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_CALL_ON_CODE_IS_INVALID);
                objErrorContent.pLineEdit = m_pEditCallOnCode;
                return false;
            }
            if (!qLineEditUtility::IsFormatValid(m_pEditCallOffCode->text(), EDIT_TYPE_CODE))
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_CALL_OFF_CODE_IS_INVALID);
                objErrorContent.pLineEdit = m_pEditCallOffCode;
                return false;
            }
        }

        //on code与off code需要同时可用或同时不可用
        if (m_pEditRejectOnCode->text().isEmpty()
                && !m_pEditRejectOffCode->text().isEmpty())
        {
            if (!qLineEditUtility::IsFormatValid(m_pEditRejectOffCode->text(), EDIT_TYPE_PHONE_NUMBER))
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_REJECT_OFF_CODE_IS_INVALID);
                objErrorContent.pLineEdit = m_pEditRejectOffCode;
                return false;
            }
        }
        if (m_pEditRejectOffCode->text().isEmpty()
                && !m_pEditRejectOnCode->text().isEmpty())
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_OFF_CODE_CANNOT_BE_EMPTY);
            objErrorContent.pLineEdit = m_pEditRejectOffCode;
            return false;
        }
        if (!m_pEditRejectOnCode->text().isEmpty()
                && !m_pEditRejectOffCode->text().isEmpty())
        {
            if (!qLineEditUtility::IsFormatValid(m_pEditRejectOnCode->text(), EDIT_TYPE_PHONE_NUMBER))
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_REJECT_ON_CODE_IS_INVALID);
                objErrorContent.pLineEdit = m_pEditRejectOnCode;
                return false;
            }
            if (!qLineEditUtility::IsFormatValid(m_pEditRejectOffCode->text(), EDIT_TYPE_PHONE_NUMBER))
            {
                objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_REJECT_OFF_CODE_IS_INVALID);
                objErrorContent.pLineEdit = m_pEditRejectOffCode;
                return false;
            }
        }
    }

    return true;
}

//保存数据
bool CDlgAnonymousCall::SavePageData()
{
    GetCurrentPageData(m_anonymousCallData);
    g_pAnonymousCallController->SetAnonymousCallDataByClickAction(m_strSubPageAction,
            m_anonymousCallData);
    return true;
}

//拷贝页面数据
void CDlgAnonymousCall::CopyPageData(AnonymousCallData & dest_anonymousCallData,
                                     const AnonymousCallData & src_anonymousCallData)
{
    //拷贝匿名呼出开功能
    dest_anonymousCallData.m_isAnonymousCallOn =
        src_anonymousCallData.m_isAnonymousCallOn;

    //拷贝匿名发送特征码功能
    dest_anonymousCallData.m_isAnonymousSendCode =
        src_anonymousCallData.m_isAnonymousSendCode;

    //拷贝拒绝匿名呼入开功能
    dest_anonymousCallData.m_isRejectAnonymousCallOn =
        src_anonymousCallData.m_isRejectAnonymousCallOn;

    //拷贝拒绝匿名呼入特征码功能
    dest_anonymousCallData.m_AnonymousRejectSendCode =
        src_anonymousCallData.m_AnonymousRejectSendCode;

    //拷贝匿名呼出开启码
    strcpy(dest_anonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOn,
           src_anonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOn);
    //拷贝匿名呼出关闭码
    strcpy(dest_anonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOff,
           src_anonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOff);
    //拷贝拒绝匿名呼入开启码
    strcpy(dest_anonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOn,
           src_anonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOn);
    //拷贝拒绝匿名呼入关闭码
    strcpy(dest_anonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOff,
           src_anonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOff);
}

//比较页面前后数据
bool CDlgAnonymousCall::ComparePageData(const AnonymousCallData & before_anonymousCallData,
                                        const AnonymousCallData & after_anonymousCallData) const
{
    //添加比较语句
    if (before_anonymousCallData.m_isAnonymousCallOn != after_anonymousCallData.m_isAnonymousCallOn
            || before_anonymousCallData.m_isAnonymousSendCode != after_anonymousCallData.m_isAnonymousSendCode
            || before_anonymousCallData.m_isRejectAnonymousCallOn !=
            after_anonymousCallData.m_isRejectAnonymousCallOn
            || before_anonymousCallData.m_AnonymousRejectSendCode !=
            after_anonymousCallData.m_AnonymousRejectSendCode)
    {
        return false;
    }

    if (0 != strcmp(before_anonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOn,
                    after_anonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOn)
            || 0 != strcmp(before_anonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOff,
                           after_anonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOff)
            || 0 != strcmp(before_anonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOn,
                           after_anonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOn)
            || 0 != strcmp(before_anonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOff,
                           after_anonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOff))
    {
        //页面数据发生了改变
        return false;
    }
    else
    {
        //前后页面数据没有发生改变
        return true;
    }
}

//取得当前页面数据
AnonymousCallData & CDlgAnonymousCall::GetCurrentPageData(
    AnonymousCallData & current_anonymousCallData) const
{
    if (m_bInitSuccess)
    {
        current_anonymousCallData.m_isAnonymousCallOn = m_pCheckBoxAnonymousCall->GetData();
        current_anonymousCallData.m_isAnonymousSendCode = m_pCheckBoxCode->GetData();
        current_anonymousCallData.m_isRejectAnonymousCallOn = m_pCheckBoxReject->GetData();
        current_anonymousCallData.m_AnonymousRejectSendCode = m_pCheckBoxAnonymousRejectCode->GetData();

        //电话呼出匿名开启码
        strcpy(current_anonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOn,
               m_pEditCallOnCode->text().trimmed().toUtf8().data());
        //电话呼出匿名关闭码
        strcpy(current_anonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOff,
               m_pEditCallOffCode->text().trimmed().toUtf8().data());

        //电话匿名呼入拒绝开启码
        strcpy(current_anonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOn,
               m_pEditRejectOnCode->text().trimmed().toUtf8().data());
        //电话匿名呼入拒绝关闭码
        strcpy(current_anonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOff,
               m_pEditRejectOffCode->text().trimmed().toUtf8().data());
    }

    //返回当前页面数据
    return current_anonymousCallData;
}

//设置当前页面数据
void CDlgAnonymousCall::SetCurrentPageData(
    AnonymousCallData & current_anonymousCallData)
{
    if (!m_bInitSuccess)
    {
        return;
    }

    m_pCheckBoxAnonymousCall->SetData(current_anonymousCallData.m_isAnonymousCallOn);
    m_pCheckBoxCode->SetData(current_anonymousCallData.m_isAnonymousSendCode);
    m_pCheckBoxReject->SetData(current_anonymousCallData.m_isRejectAnonymousCallOn);
    m_pCheckBoxAnonymousRejectCode->SetData(current_anonymousCallData.m_AnonymousRejectSendCode);

    //电话呼出匿名开启码
    m_pEditCallOnCode->setText(toQString(m_anonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOn));

    //电话呼出匿名关闭码
    m_pEditCallOffCode->setText(toQString(m_anonymousCallData.m_stCodeInfoAnonymousCall.m_szCodeOff));

    //电话匿名呼入拒绝开启码
    m_pEditRejectOnCode->setText(toQString(
                                     m_anonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOn));

    //电话匿名呼入拒绝关闭码
    m_pEditRejectOffCode->setText(toQString(
                                      m_anonymousCallData.m_stCodeInfoRejectAnonymousCall.m_szCodeOff));
}

void CDlgAnonymousCall::InitListItem()
{
    MakeReadyForRelayout();

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_ALLOW_SEND_CALL_AS_ANONYMOUS);
    if (g_pAnonymousCallController->IsAdministratorHasSet(m_strSubPageAction))
    {
        strLab = LANG_TRANSLATE(TRID_ANONYMOUS);
    }

    qPair.first = strLab;
    qPair.second = m_pCheckBoxAnonymousCall;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_LOCAL_ANONYMOUS_REJECTION);
    if (g_pAnonymousCallController->IsAdministratorHasSet(m_strSubPageAction))
    {
        strLab = LANG_TRANSLATE(TRID_ANONYMOUS_REJECTION);
    }

    qPair.first = strLab;
    qPair.second = m_pCheckBoxReject;
    m_vecWidgetPair.push_back(qPair);

    //broadsoft开启之后不显示code，http://10.2.1.199/Bug.php?BugID=44441
    if (configParser_GetConfigInt(kszBroadsoftActive) == 0
#if IF_FEATURE_METASWITCH
            && configParser_GetConfigInt(kszMetaSwitchActive) == 0
#endif
            && !g_pAnonymousCallController->IsAdministratorHasSet(m_strSubPageAction))
    {
        strLab = LANG_TRANSLATE(TRID_SEND_ANONYMOUS_CODE);
        qPair.first = strLab;
        qPair.second = m_pCheckBoxCode;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE(TRID_ON_CODE);
        qPair.first = strLab;
        qPair.second = m_pEditCallOnCode;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE(TRID_OFF_CODE);
        qPair.first = strLab;
        qPair.second = m_pEditCallOffCode;
        m_vecWidgetPair.push_back(qPair);
    }
    else
    {
        m_pCheckBoxCode->hide();
        m_pEditCallOnCode->hide();
        m_pEditCallOffCode->hide();
    }

    //broadsoft开启之后不显示code，http://10.2.1.199/Bug.php?BugID=44441
    if (configParser_GetConfigInt(kszBroadsoftActive) == 0
#if IF_FEATURE_METASWITCH
            && configParser_GetConfigInt(kszMetaSwitchActive) == 0
#endif
            && !g_pAnonymousCallController->IsAdministratorHasSet(m_strSubPageAction))
    {
        strLab = LANG_TRANSLATE(TRID_SEND_ANONYMOUS_REJECT_CODE);
        qPair.first = strLab;
        qPair.second = m_pCheckBoxAnonymousRejectCode;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE(TRID_ON_CODE);
        qPair.first = strLab;
        qPair.second = m_pEditRejectOnCode;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE(TRID_OFF_CODE);
        qPair.first = strLab;
        qPair.second = m_pEditRejectOffCode;
        m_vecWidgetPair.push_back(qPair);
    }
    else
    {
        m_pCheckBoxAnonymousRejectCode->hide();
        m_pEditRejectOnCode->hide();
        m_pEditRejectOffCode->hide();
    }

    AddWidget(m_vecWidgetPair);
    UpdateWnd();
}
