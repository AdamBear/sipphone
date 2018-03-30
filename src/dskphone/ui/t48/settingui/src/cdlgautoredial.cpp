#include "model.h"
#include "cdlgautoredial.h"
#include "qtcommon/qmisc.h"

#include "uikernel/languagehelper.h"
#include "setting/include/common.h"
#include "setting/src/advancedcallfeaturecontroller.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcheckbox.h"

CDlgAutoRedial::CDlgAutoRedial(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_qCheckBoxAutoRedial(NULL)
    , m_pEditInterval(NULL)
    , m_pEditTimes(NULL)
{
    InitData();
}

void CDlgAutoRedial::InitData()
{
    m_qCheckBoxAutoRedial = new qCheckBox(this);
    m_pEditInterval = new QLineEdit(this);
    m_pEditTimes = new QLineEdit(this);

    if (m_qCheckBoxAutoRedial == NULL || m_pEditInterval == NULL
            || m_pEditTimes == NULL)
    {
        return;
    }

    qLineEditUtility::SetLineEditStyle(m_pEditInterval, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditTimes, Qt::black);

    //editLine控件输入的文字不能超过EDIT_MAX_LENGTH长度
    //ui->editForwardTo->setMaxLength(EDIT_MAX_LENGTH);
    qLineEditUtility::SetMaxLength(m_pEditInterval, EDIT_TYPE_NUMBER);
    qLineEditUtility::SetMaxLength(m_pEditTimes, EDIT_TYPE_NUMBER);

    //为Redial Interval安装输入法
    qInstallIME(m_pEditInterval, IME_123);

    //为Redial Time安装输入法
    qInstallIME(m_pEditTimes, IME_123);


    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_AUTO_REDIAL);
    qPair.first = strLab;
    qPair.second = m_qCheckBoxAutoRedial;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_REDIAL_INTERVAL);
    qPair.first = strLab;
    qPair.second = m_pEditInterval;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_REDIAL_TIMES);
    qPair.first = strLab;
    qPair.second = m_pEditTimes;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //为界面上各输入控件安装事件过滤器
    m_qCheckBoxAutoRedial->installEventFilter(this);
    m_pEditInterval->installEventFilter(this);
    m_pEditTimes->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

bool CDlgAutoRedial::GetPageData(AdvancedCallFeatureAutoRedialData * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    //获取AutoRdial界面的当前显示值，存入pData
    if (m_qCheckBoxAutoRedial != NULL)
    {
        //从页面滑块获取开关值
        pData->m_isOn = m_qCheckBoxAutoRedial->GetData();
    }
    else
    {
        return false;
    }

    if (m_pEditTimes == NULL || m_pEditInterval == NULL)
    {
        return false;
    }

    //获取页面editLine中输入的文字
    pData->m_nRedialInterval = m_pEditInterval->text().toInt();
    pData->m_nRedialTimes = m_pEditTimes->text().toInt();

    return true;
}

bool CDlgAutoRedial::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (m_qCheckBoxAutoRedial == NULL || m_pEditInterval == NULL
            || m_pEditTimes == NULL)
    {
        return false;
    }

    if (m_pEditInterval->text().trimmed().isEmpty()
            || !qLineEditUtility::IsFormatValid(m_pEditInterval->text(), EDIT_TYPE_NUMBER))
    {
        objErrorContent.strErrorContent = TRID_REDIAL_INTERVAL_IS_INVALID;
        objErrorContent.pLineEdit = m_pEditInterval;
        return false;
    }
    //判断大小
    int iInterval = m_pEditInterval->text().toInt();
    if (iInterval < 1 || iInterval > 300)
    {
        objErrorContent.strErrorContent = TRID_REDIAL_INTERVAL_IS_INVALID;
        objErrorContent.pLineEdit = m_pEditInterval;
        return false;
    }

    if (m_pEditTimes->text().trimmed().isEmpty()
            || !qLineEditUtility::IsFormatValid(m_pEditTimes->text(), EDIT_TYPE_NUMBER))
    {
        objErrorContent.strErrorContent = TRID_REDIAL_TIME_IS_INVALID;
        objErrorContent.pLineEdit = m_pEditTimes;
        return false;
    }
    //判断大小
    int iRedialTime = m_pEditTimes->text().toInt();
    if (iRedialTime < 1 || iRedialTime > 300)
    {
        objErrorContent.strErrorContent = TRID_REDIAL_TIME_IS_INVALID;
        objErrorContent.pLineEdit = m_pEditTimes;
        return false;
    }

    return true;
}

bool CDlgAutoRedial::IsStatusChanged()
{
    AdvancedCallFeatureAutoRedialData objPageData;
    AdvancedCallFeatureAutoRedialData objConfigData;

    //Controller层获取Auto Redial界面已有的配置值，存入objConfigData
    bool bResult = g_pAdvancedCallFeatureController->GetAutoRedialData(&objConfigData);

    //利用GetPageData获取Auto Redial界面显示的数据，存入objPageData
    bResult = bResult && GetPageData(&objPageData);
    if (bResult)
    {
        //判断对应项数据是否存在改变，只要有一处改变，就返回true
        if (objPageData.m_isOn != objConfigData.m_isOn
                || objPageData.m_nRedialInterval != objConfigData.m_nRedialInterval
                || objPageData.m_nRedialTimes != objConfigData.m_nRedialTimes)
        {
            return true;
        }
    }

    return false;
}

bool CDlgAutoRedial::LoadPageData()
{
    //指针判空
    if (m_qCheckBoxAutoRedial == NULL || m_pEditInterval == NULL
            || m_pEditTimes == NULL)
    {
        return false;
    }

    this->setFocus();
    AdvancedCallFeatureAutoRedialData objData;

    //利用Controller获取Auto Redial的已有配置值，存入objData
    bool bResult = g_pAdvancedCallFeatureController->GetAutoRedialData(&objData);
    if (bResult)
    {
        if (NULL == m_qCheckBoxAutoRedial)
        {
            return false;
        }

        //将objData中的值在界面上对应的区域显示出来
        m_qCheckBoxAutoRedial->SetData(objData.m_isOn);

        //需要将配置中所存的的整形转化为界面显示所需要的QString类型
        QString strRedial;
        m_pEditInterval->setText(strRedial.setNum(objData.m_nRedialInterval));
        m_pEditTimes->setText(strRedial.setNum(objData.m_nRedialTimes));

        return true;
    }
    else
    {
        return false;
    }
}

bool CDlgAutoRedial::SavePageData()
{
    if (NULL != m_qCheckBoxAutoRedial)
    {
        AdvancedCallFeatureAutoRedialData pageData;
        GetPageData(&pageData);

        /*
        将界面显示值存入pData
        利用Controller将pageData值存入Auto Redial界面的配置中
        */
        g_pAdvancedCallFeatureController->SetAutoRedialData(pageData);
        return true;
    }
    else
    {
        return false;
    }
}

CDlgAutoRedial::~CDlgAutoRedial()
{

}
