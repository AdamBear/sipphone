#include "model.h"
#include "dlghotline.h"
#include "setting/src/advancedcallfeaturecontroller.h"
#include "qtcommon/qmisc.h"
#include "inputmethod/inputmethod.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "uikernel/qlineeditutility.h"
#include "directoryui/include/moddirectoryui.h"

void DlgHotLine_DirectoryCallBack(yl::string strContactName, yl::string strContactNumber)
{

    CDlgHotline * pHotline = qobject_cast<CDlgHotline *>(UIManager_GetPrivateDlg(
                                 DLG_CDlgHotline));
    if (NULL == pHotline)
    {
        return;
    }
    QString name = toQString(strContactName);
    QString number = toQString(strContactNumber);

    pHotline->writeData(name, number);
}

//构造函数
CDlgHotline::CDlgHotline(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pEditHotlineDelay(NULL)
    , m_pEditNumber(NULL)
{
    InitData();
}

//析构函数
CDlgHotline::~CDlgHotline()
{

}

//界面初始化
void CDlgHotline::InitData()
{
    m_pEditHotlineDelay = new QLineEdit(this);
    m_pEditNumber = new QLineEdit(this);

    //指针判空
    if (m_pEditHotlineDelay == NULL || m_pEditNumber == NULL)
    {
        return;
    }

    qLineEditUtility::SetLineEditStyle(m_pEditHotlineDelay, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditNumber, Qt::black);

    //为Number安装输入法
    qInstallIME(m_pEditNumber, IME_DIAL);

    //为Hotline Delay安装输入法
    qInstallIME(m_pEditHotlineDelay, IME_123);

    //最大输入长度
    qLineEditUtility::SetMaxLength(m_pEditNumber, EDIT_TYPE_DIAL);
    qLineEditUtility::SetMaxLength(m_pEditHotlineDelay, EDIT_TYPE_NUMBER_SHORT);



    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_HOT_NUMBER);
    qPair.first = strLab;
    qPair.second = m_pEditNumber;
    qPair.iExtraButten = DIRECTORY_EXTRA;
    m_vecWidgetPair.push_back(qPair);
    qPair.iExtraButten = NONE_EXTRA;

    strLab = LANG_TRANSLATE(TRID_HOT_LINE_DELAY);
    qPair.first = strLab;
    qPair.second = m_pEditHotlineDelay;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //初始化hotline账号为空
    m_pEditNumber->setText(LANG_TRANSLATE(""));

    //为界面上各输入控件安装事件过滤器
    m_pEditNumber->installEventFilter(this);
    m_pEditHotlineDelay->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

//加载数据
bool CDlgHotline::LoadPageData()
{
    this->setFocus();
    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditNumber, EDIT_TYPE_PHONE_NUMBER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditHotlineDelay, EDIT_TYPE_NUMBER);

    bool bIsSuccess =
        g_pAdvancedCallFeatureController->GetHotlineData(&m_featureHotlineData);
    if (!bIsSuccess)
    {
        return false;
    }
    else
    {
        m_hotlinePageData.m_strNumber =
            m_featureHotlineData.m_strHotlineNumber.c_str();
        m_hotlinePageData.m_strDelay =
            QString::number(m_featureHotlineData.m_nHotlineDelay);
        SetCurrentPageData(m_hotlinePageData);
        return true;
    }
}

//获取数据状态是否改变
bool CDlgHotline::IsStatusChanged()
{
    HotlinePageData hotlinePageData;
    GetCurrentPageData(hotlinePageData);
    if (ComparePageData(m_hotlinePageData, hotlinePageData))
    {
        //页面数据相等
        return false;
    }
    else
    {
        //页面数据前后不相等
        CopyPageData(m_hotlinePageData, hotlinePageData);
        return true;
    }
}

bool CDlgHotline::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    //指针判空
    if (m_pEditHotlineDelay == NULL || m_pEditNumber == NULL)
    {
        return false;
    }

    if (!m_pEditNumber->text().isEmpty()
            && !qLineEditUtility::IsFormatValid(m_pEditNumber->text(), EDIT_TYPE_DIAL))
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_NUMBER);
        objErrorContent.pLineEdit = m_pEditNumber;
        return false;
    }
    //数字判断
    if (!m_pEditHotlineDelay->text().isEmpty()
            && !qLineEditUtility::IsFormatValid(m_pEditHotlineDelay->text(), EDIT_TYPE_NUMBER))
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_HOT_LINE_DELAY_IS_INVALID);
        objErrorContent.pLineEdit = m_pEditHotlineDelay;
        return false;
    }
    if (m_pEditHotlineDelay->text().toInt() < 0
            || m_pEditHotlineDelay->text().toInt() > 10)
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_HOT_LINE_DELAY_SHOULD_BE_1_TO_10);
        objErrorContent.pLineEdit = m_pEditHotlineDelay;
        return false;
    }
    return true;
}

//保存数据
bool CDlgHotline::SavePageData()
{
    GetCurrentPageData(m_hotlinePageData);
    m_featureHotlineData.m_strHotlineNumber =
        m_hotlinePageData.m_strNumber.trimmed().toUtf8().data();

    m_featureHotlineData.m_nHotlineDelay =
        m_hotlinePageData.m_strDelay.trimmed().toInt();
    g_pAdvancedCallFeatureController->SetHotlineData(m_featureHotlineData);

    return true;
}

//拷贝页面数据
void CDlgHotline::CopyPageData(HotlinePageData & dest_HotlinePageData,
                               const HotlinePageData & src_HotlinePageData)
{
    dest_HotlinePageData.m_strNumber = src_HotlinePageData.m_strNumber;
    dest_HotlinePageData.m_strDelay = src_HotlinePageData.m_strDelay;
}

//比较页面前后数据
bool CDlgHotline::ComparePageData(const HotlinePageData & before_HotlinePageData,
                                  const HotlinePageData & after_HotlinePageData) const
{
    if (before_HotlinePageData.m_strNumber != after_HotlinePageData.m_strNumber
            || before_HotlinePageData.m_strDelay != after_HotlinePageData.m_strDelay)
    {
        //前后页面数据不相等
        return false;
    }
    else
    {
        //前后页面数据相等
        return true;
    }
}

//取得当前页面数据
HotlinePageData & CDlgHotline::GetCurrentPageData(HotlinePageData & current_HotlinePageData) const
{
    //取得账号数据
    current_HotlinePageData.m_strNumber = m_pEditNumber->text();
    //取得延迟时间,需要做数据校验，可取值范围0-180秒
    current_HotlinePageData.m_strDelay = m_pEditHotlineDelay->text();

    return current_HotlinePageData;
}

//设置当前页面数据
void CDlgHotline::SetCurrentPageData(HotlinePageData & current_HotlinePageData)
{
    //指针判空
    if (m_pEditHotlineDelay == NULL || m_pEditNumber == NULL)
    {
        return;
    }

    //设置账号数据
    m_pEditNumber->setText(current_HotlinePageData.m_strNumber);
    //设置延迟时间，需要作限定数字输入
    m_pEditHotlineDelay->setText(current_HotlinePageData.m_strDelay);
}

void CDlgHotline::OnExtraAction(int eActionType)
{
    if (eActionType == DIRECTORY_EXTRA)
    {
        DM_EnterSelectDirectoryUI(DlgHotLine_DirectoryCallBack);
    }
}

void CDlgHotline::writeData(QString name, QString number)
{
    m_pEditNumber->setText(number);
}
