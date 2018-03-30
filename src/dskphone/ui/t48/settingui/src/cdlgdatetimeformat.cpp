#include "model.h"
#include "cdlgdatetimeformat.h"
#include "interfacedefine.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "uikernel/languagehelper.h"
#include "setting/src/dateandtimecontroller.h"
#include "baseui/fontcolor.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "uikernel/qcomboboxutility.h"
#include "language/input/include/modInput.h"

//枚举类型的注册
Q_DECLARE_METATYPE(DateFormatType);
Q_DECLARE_METATYPE(TimeFormatType);

CDlgDateTimeFormat::CDlgDateTimeFormat(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbBoxDateFormat(NULL)
    , m_pCmbBoxTimeFormat(NULL)
{
    InitData();
}

void CDlgDateTimeFormat::InitData()
{
    //设置Date Format标签
    m_pCmbBoxDateFormat = new QComboBox(this);
    m_pCmbBoxTimeFormat = new QComboBox(this);

    //指针判空
    if (m_pCmbBoxDateFormat != NULL
            && m_pCmbBoxTimeFormat != NULL)
    {
        qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxDateFormat, Qt::black);
        qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxTimeFormat, Qt::black);

        //设置下拉框内容
        m_pCmbBoxTimeFormat->addItem(LANG_TRANSLATE(TRID_24_HOUR_SYSTEM), TIME_FORMAT_24HOUR);
        m_pCmbBoxTimeFormat->addItem(LANG_TRANSLATE(TRID_12_HOUR_SYSTEM), TIME_FORMAT_12HOUR);

        if (Lang_IsChineseStyleDate())
        {
            m_pCmbBoxDateFormat->addItem(LANG_TRANSLATE(TRID_DATE_FORMAT_MM_DD_WWW),
                                         DATE_FORMAT_MM_DD_WWW_SPACE);
            m_pCmbBoxDateFormat->addItem(LANG_TRANSLATE(TRID_DATE_FORMAT_YY_DASH_MM_DASH_DD),
                                         DATE_FORMAT_YY_MM_DD_HYPHEN);
            m_pCmbBoxDateFormat->addItem(LANG_TRANSLATE(TRID_DATE_FORMAT_YYYY_DASH_MM_DASH_DD),
                                         DATE_FORMAT_YYYY_MM_DD);
            m_pCmbBoxDateFormat->addItem(LANG_TRANSLATE(TRID_DATE_FORMAT_YYYY_SLASH_MM_SLASH_DD),
                                         DATE_FORMAT_YYYY_MM_DD_VIRGULE);
            m_pCmbBoxDateFormat->addItem(LANG_TRANSLATE(TRID_DATE_FORMAT_YY_SLASH_MM_SLASH_DD),
                                         DATE_FORMAT_YY_MM_DD_VIRGULE);
            m_pCmbBoxDateFormat->addItem(LANG_TRANSLATE(TRID_DATE_FORMAT_YYYY_MM_DD),
                                         DATE_FORMAT_YYYY_MM_DD_SPACE);
            m_pCmbBoxDateFormat->addItem(LANG_TRANSLATE(TRID_DATE_FORMAT_MM_DD_WW), DATE_FORMAT_MM_DD_WW_SPACE);
        }
        else
        {
            m_pCmbBoxDateFormat->addItem(LANG_TRANSLATE(TRID_DATE_FORMAT_WWW_MMM_DD), DATE_FORMAT_WWW_MMM_DD);
            m_pCmbBoxDateFormat->addItem(LANG_TRANSLATE(TRID_DATE_FORMATE_DD_DASH_MMM_DASH_YYYY),
                                         DATE_FORMAT_DD_MMM_YY);
            m_pCmbBoxDateFormat->addItem(LANG_TRANSLATE(TRID_DATE_FORMAT_YYYY_DASH_MM_DASH_DD),
                                         DATE_FORMAT_YYYY_MM_DD);
            m_pCmbBoxDateFormat->addItem(LANG_TRANSLATE(TRID_DATE_FORMATE_DD_SLASH_MM_SLASH_YYYY),
                                         DATE_FORMAT_DD_MM_YYYY);
            m_pCmbBoxDateFormat->addItem(LANG_TRANSLATE(TRID_DATE_FORMATE_MM_SLASH_DD_SLASH_YY),
                                         DATE_FORMAT_MM_DD_YY);
            m_pCmbBoxDateFormat->addItem(LANG_TRANSLATE(TRID_DATE_FORMATE_DD_MMM_YYYY),
                                         DATE_FORMAT_DD_MMM_YYYY);
            m_pCmbBoxDateFormat->addItem(LANG_TRANSLATE(TRID_DATE_FORMAT_WWW_DD_MMM), DATE_FORMAT_WWW_DD_MMM);
        }

        QWidgetPair qPair;
        QString strLab;

        strLab = LANG_TRANSLATE(TRID_DATE_FORMATE);
        qPair.first = strLab;
        qPair.second = m_pCmbBoxDateFormat;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE(TRID_TIME_FORMAT);
        qPair.first = strLab;
        qPair.second = m_pCmbBoxTimeFormat;
        m_vecWidgetPair.push_back(qPair);

        AddWidget(m_vecWidgetPair);
    }

    //为界面自己安装事件过滤器
    m_pCmbBoxDateFormat->installEventFilter(this);
    m_pCmbBoxTimeFormat->installEventFilter(this);
    this->installEventFilter(this);
}

bool CDlgDateTimeFormat::GetPageData(DateAndTimeFormatData * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    //指针判空
    if (m_pCmbBoxDateFormat == NULL
            && m_pCmbBoxTimeFormat == NULL)
    {
        return false;
    }

    pData->m_eDateFormat = (DateFormatType)m_pCmbBoxDateFormat
                           ->itemData(m_pCmbBoxDateFormat->currentIndex(), Qt::UserRole).value<int>();

    pData->m_eTimeFormat = (TimeFormatType)m_pCmbBoxTimeFormat
                           ->itemData(m_pCmbBoxTimeFormat->currentIndex(), Qt::UserRole).value<int>();

    return true;
}

bool CDlgDateTimeFormat::IsStatusChanged()
{
    DateAndTimeFormatData objPageData;
    DateAndTimeFormatData objConfigData;

    //获取界面已有的配置数据，存入objConfigData;
    bool bResult = g_pDateAndTimeController->GetDateAndTimeFormat(&objConfigData);

    //利用GetPageData获取界面显示的数据，存入objPageData
    bResult = bResult && GetPageData(&objPageData);
    if (bResult)
    {
        //判断对应项数据是否存在改变，只要有一处改变，就返回true
        if (objPageData.m_eDateFormat != objConfigData.m_eDateFormat
                || objPageData.m_eTimeFormat != objConfigData.m_eTimeFormat)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool CDlgDateTimeFormat::LoadPageData()
{
    this->setFocus();
    DateAndTimeFormatData objData;

    //指针判空
    if (m_pCmbBoxDateFormat == NULL
            && m_pCmbBoxTimeFormat == NULL)
    {
        return false;
    }

    //获取界面已有的配置值，存入objData
    bool bResult = g_pDateAndTimeController->GetDateAndTimeFormat(&objData);
    if (bResult)
    {
        m_pCmbBoxDateFormat->setCurrentIndex(m_pCmbBoxDateFormat
                                             ->findData(objData.m_eDateFormat, Qt::UserRole));

        m_pCmbBoxTimeFormat->setCurrentIndex(m_pCmbBoxTimeFormat
                                             ->findData(objData.m_eTimeFormat, Qt::UserRole));
        return true;
    }
    else
    {
        return false;
    }
}

bool CDlgDateTimeFormat::SavePageData()
{
    /*
    将界面显示数据存入pData
    将pData设为当前界面的配置数据
    */
    DateAndTimeFormatData pData;
    GetPageData(&pData);

    g_pDateAndTimeController->SetDateAndTimeFormat(pData);

    // 刷新时间的显示
    etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SETTIME, 0);

    return true;
}

CDlgDateTimeFormat::~CDlgDateTimeFormat()
{
}
