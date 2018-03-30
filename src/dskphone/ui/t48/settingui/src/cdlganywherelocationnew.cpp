#include "model.h"
#include "cdlganywherelocationnew.h"
#include "canywhereuilogic.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "broadsoft/anywhere/include/anywherecontroller.h"
#include "uikernel/qlineeditutility.h"


CDlgAnyWhereLocationNew::CDlgAnyWhereLocationNew(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pEditName(NULL)
    , m_pEditNumber(NULL)
{
    InitData();
}

void CDlgAnyWhereLocationNew::InitData()
{
    m_strTitle = LANG_TRANSLATE(TRID_NEW_ANYWHERE_LOCATION);

    //初始化 下面会改变父窗口，这里用this初始化是为了防止内存泄漏问题
    m_pEditName = new QLineEdit(this);
    m_pEditNumber = new QLineEdit(this);

    //指针判空
    if (m_pEditName == NULL
            || m_pEditNumber == NULL)
    {
        return;
    }

    qLineEditUtility::SetLineEditStyle(m_pEditName, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditNumber, Qt::black);

    //设置输入框输入的最大长度
    m_pEditName->setMaxLength(EDIT_MAX_LENGTH_ANYWHERE_NAME);
    m_pEditNumber->setMaxLength(EDIT_MAX_LENGTH_ANYWHERE_NUMBER);

    //为输入框安装输入法
    qInstallIME(m_pEditName, IME_Name);
    qInstallIME(m_pEditNumber, IME_DIAL);

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


    AddWidget(m_vecWidgetPair);

    //安装事件过滤器
    m_pEditName->installEventFilter(this);
    m_pEditNumber->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

//号码已存在的处理
void CDlgAnyWhereLocationNew::NotifyLocationAlreadyExist()
{
    QString number = GetNumber();
    QString strContent = LANG_TRANSLATE(TRID_NUMBER_ALREADY_EXISTS) + QString(": %1").arg(number);

    MessageBox_ShowCommonMsgBox(NULL, strContent, MESSAGEBOX_NOTE, SHORT_TIME);
}


QString CDlgAnyWhereLocationNew::GetNumber()
{
    //输入预编辑字符
    ReturnAllEdit();

    return m_pEditNumber->text();
}

//获取页面数据
SAYWLocation CDlgAnyWhereLocationNew::GetPageData()
{
    //先把编辑状态的字符输入到输入框
    ReturnAllEdit();

    SAYWLocation pageInfo;
    //从控件中获取属性
    pageInfo.m_strDescription = m_pEditName->text().toUtf8().data();
    pageInfo.m_strPhoneNum = m_pEditNumber->text().toUtf8().data();

    //设置默认属性
    pageInfo.m_bActive = true;
    pageInfo.m_bEnableDiversionInhibitor = false;
    pageInfo.m_bRequireAnswerConfirmation = false;
    pageInfo.m_bUseBbasedCallControlServices = false;

    return pageInfo;
}

bool CDlgAnyWhereLocationNew::IsInputDataValid()
{
    //判断输入的号码是否合法
    if (!BWCallControl_CheckPhoneNumberValid(m_pEditNumber->text().toUtf8().data(), 19))
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_NUMBER_INVALID), MESSAGEBOX_NOTE, SHORT_TIME);
        SelectIndex(1);
        m_pEditNumber->setFocus();
        return false;
    }

    //判断号码是否已存在
    if (AnyWhere_CheckNumberExit(m_pEditNumber->text().toUtf8().data()))
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_NUMBER_ALREADY_EXISTS), MESSAGEBOX_NOTE,
                                    SHORT_TIME);
        SelectIndex(1);
        m_pEditNumber->setFocus();
        return false;
    }

    return true;
}

bool CDlgAnyWhereLocationNew::IsStatusChanged()
{
    return !m_pEditName->text().isEmpty() || !m_pEditNumber->text().isEmpty();
}

bool CDlgAnyWhereLocationNew::LoadPageData()
{
    return false;
}

bool CDlgAnyWhereLocationNew::SavePageData()
{
    //输入合法才保存
    if (IsInputDataValid())
    {
        SAYWLocation pageInfo = GetPageData();

        //调用save接口
        g_pAnywhereUILogic->RequestAddLocation(pageInfo, true);
    }

    return false;
}

CDlgAnyWhereLocationNew::~CDlgAnyWhereLocationNew()
{
}

void CDlgAnyWhereLocationNew::OnCustomBack()
{
    // 用于自定义退出界面操作（只回退一个界面）
    // 回到上一层界面
    emit BtnBackClick();
}
