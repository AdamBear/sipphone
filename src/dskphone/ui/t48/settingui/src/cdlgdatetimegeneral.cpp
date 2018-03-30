#include "model.h"
#include "cdlgdatetimegeneral.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "uikernel/languagehelper.h"
#include "setting/src/dateandtimecontroller.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "messagebox/modmessagebox.h"
#include "uicommon.h"
#include "keymap.h"
#include "mainwnd/mainwnd.h"
#include "etlmsghandler/modetlmsghandler.h"

#include "uikernel/qlineeditutility.h"
#include "uikernel/qcomboboxutility.h"
#include "uikernel/timeeditframe.h"
#include "uikernel/dataeditframe.h"
#include "setting/include/common.h"
#include "interfacedefine.h"
#include "service_system.h"
#include "uicommon/uikernel/qcomboboxex.h"

CDlgDateTimeGeneral::CDlgDateTimeGeneral(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pDataFrame(NULL)
    , m_pTimeFrame(NULL)
    , m_pCmbType(NULL)
    , m_pCmbBoxTimeZone(NULL)
    , m_pEditNTPServer1(NULL)
    , m_pEditNTPServer2(NULL)
    , m_pCmbBoxDaylightSaving(NULL)
    , m_ePageType(DATE_AND_TIME_TYPE_NONE)
    , m_pCmbLocation(NULL)
{
    InitData();
}

void CDlgDateTimeGeneral::InitData()
{
    m_pCmbType = new QComboBox(this);
    m_pDataFrame = new DataEditFrame(this);
    m_pTimeFrame = new TimeEditFrame(this);
    m_pCmbBoxTimeZone = new QComboBox(this);
    m_pEditNTPServer1 = new QLineEdit(this);
    m_pEditNTPServer2 = new QLineEdit(this);
    m_pCmbBoxDaylightSaving = new QComboBox(this);
    m_pCmbLocation = new qComboBoxEx(this);

    //指针判空
    if (NULL == m_pDataFrame
            || NULL == m_pTimeFrame
            || NULL == m_pCmbType
            || NULL == m_pCmbBoxTimeZone
            || NULL == m_pEditNTPServer1
            || NULL == m_pEditNTPServer2
            || NULL == m_pCmbBoxDaylightSaving
            || NULL == m_pCmbLocation)
    {
        return;
    }

    this->setFocusPolicy(Qt::StrongFocus);

    //先主动更新逻辑层数据
    g_pDateAndTimeController->ReloadTimeZoneList();
    //添加下拉框数据
    UpdateTimeZoneListContent();

    m_pCmbType->addItem(LANG_TRANSLATE(TRID_SNTP_SETTING), DATE_AND_TIME_SNTP);
    m_pCmbType->addItem(LANG_TRANSLATE(TRID_MANUAL_SETTING), DATE_AND_TIME_MANUAL);
    m_pCmbBoxDaylightSaving->addItem(LANG_TRANSLATE(TRID_AUTOMATIC), DAYLIGHTING_SAVING_AUTO);
    m_pCmbBoxDaylightSaving->addItem(LANG_TRANSLATE(TRID_ENABLED), DAYLIGHTING_SAVING_ON);
    m_pCmbBoxDaylightSaving->addItem(LANG_TRANSLATE(TRID_DISABLED), DAYLIGHTING_SAVING_OFF);

    qComboBoxUtility::SetComboBoxStyle(m_pCmbType, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxTimeZone, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxDaylightSaving, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCmbLocation, Qt::black);

    m_pCmbBoxTimeZone->setCurrentIndex(-1);

    qLineEditUtility::SetLineEditStyle(m_pEditNTPServer1, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditNTPServer2, Qt::black);

    //安装输入法
    qInstallIME(m_pEditNTPServer1, IME_ALL);
    qInstallIME(m_pEditNTPServer2, IME_ALL);
    qInstallIME(m_pDataFrame->m_plineYear, IME_123);
    qInstallIME(m_pDataFrame->m_plineMonth, IME_123);
    qInstallIME(m_pDataFrame->m_plineDay, IME_123);
    qInstallIME(m_pTimeFrame->m_plineHour, IME_123);
    qInstallIME(m_pTimeFrame->m_plineMinute, IME_123);
    qInstallIME(m_pTimeFrame->m_plineSecond, IME_123);

    qUseKeyBoardFromIME(m_pDataFrame->m_plineYear, INPUT_DIALOG_NORMAL, false);
    qUseKeyBoardFromIME(m_pDataFrame->m_plineMonth, INPUT_DIALOG_NORMAL, false);
    qUseKeyBoardFromIME(m_pDataFrame->m_plineDay, INPUT_DIALOG_NORMAL, false);

    qUseKeyBoardFromIME(m_pTimeFrame->m_plineHour, INPUT_DIALOG_NORMAL, false);
    qUseKeyBoardFromIME(m_pTimeFrame->m_plineMinute, INPUT_DIALOG_NORMAL, false);
    qUseKeyBoardFromIME(m_pTimeFrame->m_plineSecond, INPUT_DIALOG_NORMAL, false);

    //最大输入长度限制
    qLineEditUtility::SetMaxLength(m_pEditNTPServer1, EDIT_TYPE_SERVER);
    qLineEditUtility::SetMaxLength(m_pEditNTPServer2, EDIT_TYPE_SERVER);

    connect(m_pCmbType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(OnCmbTypeChanged(int)));
    connect(m_pCmbBoxDaylightSaving, SIGNAL(currentIndexChanged(int)),
            this, SLOT(OnCmbDaylightSavingChanged(int)));
    connect(m_pCmbBoxTimeZone, SIGNAL(currentIndexChanged(int)),
            this, SLOT(UpdateTimeZoneCityListContent(int)));

    m_pCmbType->installEventFilter(this);
    m_pCmbBoxTimeZone->installEventFilter(this);
    m_pEditNTPServer1->installEventFilter(this);
    m_pEditNTPServer2->installEventFilter(this);
    m_pCmbBoxDaylightSaving->installEventFilter(this);
    m_pCmbLocation->installEventFilter(this);
    m_pTimeFrame->installEventFilter(this);
    m_pDataFrame->installEventFilter(this);

    m_pDataFrame->m_plineYear->installEventFilter(this);
    m_pDataFrame->m_plineMonth->installEventFilter(this);
    m_pDataFrame->m_plineDay->installEventFilter(this);
    m_pTimeFrame->m_plineHour->installEventFilter(this);
    m_pTimeFrame->m_plineMinute->installEventFilter(this);
    m_pTimeFrame->m_plineSecond->installEventFilter(this);

    this->installEventFilter(this);

    //初次进入的时候MakeReadyForRelayout无法隐藏控件
    m_pCmbBoxTimeZone->hide();
    m_pCmbBoxDaylightSaving->hide();
    m_pEditNTPServer1->hide();
    m_pEditNTPServer2->hide();
    m_pTimeFrame->hide();
    m_pDataFrame->hide();
    m_pCmbLocation->hide();
}

void CDlgDateTimeGeneral::UpdateTimeZoneListContent()
{
    if (g_pDateAndTimeController != NULL)
    {
        m_pCmbBoxTimeZone->clear();

        TimeZoneListData tzData;
        g_pDateAndTimeController->GetTimeZoneList(&tzData);
        YLList<yl::string>::const_iterator it;
        for (it = tzData.m_listTimeZone.begin(); it != tzData.m_listTimeZone.end(); ++it)
        {
            yl::string strContent = *it;
            m_pCmbBoxTimeZone->addItem(toQString(strContent.c_str()));
        }
    }
}

void CDlgDateTimeGeneral::UpdateTimeZoneCityListContent(int iIndex)
{
    if (g_pDateAndTimeController != NULL)
    {
        m_pCmbLocation->clear();

        yl::string strTimezone = m_pCmbBoxTimeZone->itemText(iIndex).toUtf8().data();

        TimeZoneCityListData cityData;
        g_pDateAndTimeController->GetTimeZoneCityList(&cityData, strTimezone);

        m_pCmbLocation->addItem(QString(TRID_NONE));

        YLList<yl::string>::const_iterator it;
        for (it = cityData.m_listTimeZoneCity.begin(); it != cityData.m_listTimeZoneCity.end(); ++it)
        {
            yl::string strContent = *it;
            m_pCmbLocation->addItem(toQString(strContent.c_str()));
        }
    }
}

bool CDlgDateTimeGeneral::GetPageData(DateTimeGeneralData * pData)
{
    if (NULL == pData)
    {
        return false;
    }
    //指针判空
    if (NULL == m_pCmbType || NULL == m_pCmbBoxTimeZone
            || NULL == m_pEditNTPServer1 || NULL == m_pEditNTPServer2
            || NULL == m_pCmbBoxDaylightSaving
            || NULL == m_pCmbLocation)
    {
        return false;
    }

    //获取自动设置的界面显示数据，存入pData
    pData->m_eDateTimeType = (DateAndTimeType)m_pCmbType
                             ->itemData(m_pCmbType->currentIndex(), Qt::UserRole).value<int>();
    pData->m_strTimeZone = m_pCmbBoxTimeZone->currentText().toUtf8().data();
    pData->m_strNtpServer1 = m_pEditNTPServer1->text().toUtf8().data();
    pData->m_strNtpServer2 = m_pEditNTPServer2->text().toUtf8().data();
    pData->m_eDayLightType = (DaylightSavingType)m_pCmbBoxDaylightSaving
                             ->itemData(m_pCmbBoxDaylightSaving->currentIndex(), Qt::UserRole).value<int>();
    pData->m_strTimeZoneCity = m_pCmbLocation
                               ->itemText(m_pCmbLocation->currentIndex()).toUtf8().data();

    return true;
}

bool CDlgDateTimeGeneral::IsStatusChanged()
{
    DateTimeGeneralData objPageData;
    DateTimeGeneralData objConfigData;

    //获取自动设置界面已有的配置数据，存入objConfigData
    bool bResult = g_pDateAndTimeController->GetGeneralData(&objConfigData);

    //容错处理，保存首项设置的合法性
    if (objConfigData.m_eDateTimeType != DATE_AND_TIME_MANUAL
            && objConfigData.m_eDateTimeType != DATE_AND_TIME_SNTP)
    {
        objConfigData.m_eDateTimeType = DATE_AND_TIME_SNTP;
    }

    //获取手动设置界面的页面显示数据

    //利用GetPageData获取自动设置界面显示数据，存入objPageData
    bResult = bResult && GetPageData(&objPageData);
    if (bResult)
    {
        /* NTP-Manual  Manual-NTP */
        if (objPageData.m_eDateTimeType != objConfigData.m_eDateTimeType)
        {
            return true;
        }
        /* NTP-NTP */
        else if (DATE_AND_TIME_SNTP == objConfigData.m_eDateTimeType)
        {
            if (objPageData.m_strTimeZone != objConfigData.m_strTimeZone
                    || objPageData.m_strNtpServer1 != objConfigData.m_strNtpServer1
                    || objPageData.m_strNtpServer2 != objConfigData.m_strNtpServer2
                    || objPageData.m_eDayLightType != objConfigData.m_eDayLightType
                    || objConfigData.m_strTimeZoneCity != objConfigData.m_strTimeZoneCity)
            {
                return true;
            }
        }
        /* Manual-Manual */
        else
        {
            //如果是手动设置状态,与之前的输入比较
            return m_pPreDatatime.date().year() != m_pDataFrame->m_plineYear->text().toInt()
                   || m_pPreDatatime.date().month() != m_pDataFrame->m_plineMonth->text().toInt()
                   || m_pPreDatatime.date().day() != m_pDataFrame->m_plineDay->text().toInt()
                   || m_pPreDatatime.time().hour() != m_pTimeFrame->m_plineHour->text().toInt()
                   || m_pPreDatatime.time().minute() != m_pTimeFrame->m_plineMinute->text().toInt()
                   || m_pPreDatatime.time().second() != m_pTimeFrame->m_plineSecond->text().toInt();
        }
    }
    return false;
}

bool CDlgDateTimeGeneral::LoadPageData()
{
    this->setFocus();

    //指针判空
    if (NULL == m_pCmbType
            || NULL == m_pDataFrame || NULL == m_pTimeFrame
            || NULL == m_pCmbBoxTimeZone || NULL == m_pEditNTPServer1
            || NULL == m_pEditNTPServer2 || NULL == m_pCmbBoxDaylightSaving
            || NULL == m_pCmbLocation)
    {
        return false;
    }

    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditNTPServer1, EDIT_TYPE_SERVER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditNTPServer2, EDIT_TYPE_SERVER);

    DateTimeGeneralData m_eDataTimeData;

    //获取自动设置界面已有的配置数据，存入objData
    bool bResult = g_pDateAndTimeController->GetGeneralData(&m_eDataTimeData);

    if (bResult)
    {
        int iIndex = m_pCmbBoxDaylightSaving->findData(m_eDataTimeData.m_eDayLightType, Qt::UserRole);
        // 校正焦点值
        if (iIndex < 0 || iIndex >= m_pCmbBoxDaylightSaving->count())
        {
            iIndex = 0;
        }
        m_pCmbBoxDaylightSaving->blockSignals(true); //暂时屏蔽信号，避免不必要的重新布局
        m_pCmbBoxDaylightSaving->setCurrentIndex(iIndex);
        m_pCmbBoxDaylightSaving->blockSignals(false);

        iIndex = m_pCmbType->findData(m_eDataTimeData.m_eDateTimeType, Qt::UserRole);
        if (iIndex < 0 || iIndex >= m_pCmbType->count())
        {
            iIndex = 0;
        }
        m_pCmbType->setCurrentIndex(iIndex);

        RefreshByPageType(m_eDataTimeData.m_eDateTimeType);

        SetCurrentTimezoneCmbIndex(m_eDataTimeData.m_strTimeZone);

        m_pEditNTPServer1->setText(toQString(m_eDataTimeData.m_strNtpServer1.c_str()));
        m_pEditNTPServer2->setText(toQString(m_eDataTimeData.m_strNtpServer2.c_str()));

        iIndex = m_pCmbLocation->findText(toQString(m_eDataTimeData.m_strTimeZoneCity.c_str()));
        // 校正焦点值
        if (iIndex < 0 || iIndex >= m_pCmbLocation->count())
        {
            iIndex = 0;
        }
        m_pCmbLocation->setCurrentIndex(iIndex);

        return true;
    }
    return false;
}

void CDlgDateTimeGeneral::RefreshByPageType(DateAndTimeType ePageType,
        bool bRefreshByDaylightSaving/* = false*/)
{
    if (m_ePageType == ePageType && !bRefreshByDaylightSaving)
    {
        return;
    }

    if (NULL == m_pGeneralFrame
            || NULL == m_pCmbType
            || NULL == m_pTimeFrame
            || NULL == m_pCmbBoxTimeZone
            || NULL == m_pCmbBoxDaylightSaving
            || NULL == m_pEditNTPServer1
            || NULL == m_pEditNTPServer2
            || NULL == m_pDataFrame || NULL == m_pTimeFrame
            || NULL == m_pCmbLocation)
    {
        return;
    }

    MakeReadyForRelayout();

    m_ePageType = ePageType;
    QWidgetPair qPair;
    qPair.first = LANG_TRANSLATE(TRID_TYPE);
    qPair.second = m_pCmbType;
    m_vecWidgetPair.push_back(qPair);

    if (DATE_AND_TIME_MANUAL == m_ePageType)
    {
        QWidgetPair qPair;
        QString strLab;

        strLab = LANG_TRANSLATE(TRID_DATE);
        qPair.first = strLab;
        qPair.second = m_pDataFrame;
        m_vecWidgetPair.push_back(qPair);
        m_pDataFrame->raise();

        strLab = LANG_TRANSLATE(TRID_TIME);
        qPair.first = strLab;
        qPair.second = m_pTimeFrame;
        m_vecWidgetPair.push_back(qPair);
        m_pTimeFrame->raise();

        //进入手动设置时间界面，即时刷新时间
        RefreshDateAndTime();
    }
    else
    {
        QWidgetPair qPair;
        QString strLab;

        strLab = LANG_TRANSLATE(TRID_TIME_ZONE);
        qPair.first = strLab;
        qPair.second = m_pCmbBoxTimeZone;
        m_vecWidgetPair.push_back(qPair);
        m_pCmbBoxTimeZone->raise();

        strLab = LANG_TRANSLATE(TRID_NTP_SERVER1);
        qPair.first = strLab;
        qPair.second = m_pEditNTPServer1;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE(TRID_NTP_SERVER2);
        qPair.first = strLab;
        qPair.second = m_pEditNTPServer2;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE(TRID_DAYLIGHT_SAVING);
        qPair.first = strLab;
        qPair.second = m_pCmbBoxDaylightSaving;
        m_vecWidgetPair.push_back(qPair);
        m_pCmbBoxDaylightSaving->raise();

        if (m_eDataTimeData.m_eDateTimeType != DATE_AND_TIME_TYPE_NONE)
        {
            //更新数据
            SetCurrentTimezoneCmbIndex(m_eDataTimeData.m_strTimeZone);

            m_pEditNTPServer1->setText(toQString(m_eDataTimeData.m_strNtpServer1.c_str()));
            m_pEditNTPServer2->setText(toQString(m_eDataTimeData.m_strNtpServer2.c_str()));

            int iIndex = m_pCmbBoxDaylightSaving->findData(m_eDataTimeData.m_eDayLightType, Qt::UserRole);
            // 校正焦点值
            if (iIndex < 0 || iIndex >= m_pCmbBoxDaylightSaving->count())
            {
                iIndex = 0;
            }
            m_pCmbBoxDaylightSaving->blockSignals(true);
            m_pCmbBoxDaylightSaving->setCurrentIndex(iIndex);
            m_pCmbBoxDaylightSaving->blockSignals(false);
        }

        int iDaylightType = m_pCmbBoxDaylightSaving->itemData(
                                m_pCmbBoxDaylightSaving->currentIndex()).value<int>();
        //夏令时为auto时添加location选项
        if (DAYLIGHTING_SAVING_AUTO == iDaylightType)
        {
            strLab = LANG_TRANSLATE(TRID_TIME_ZONE_LOCATION);
            qPair.first = strLab;
            qPair.second = m_pCmbLocation;
            m_vecWidgetPair.push_back(qPair);
        }
    }

    AddWidget(m_vecWidgetPair);

    m_pGeneralFrame->RefreshData(0);

    m_pGeneralFrame->show();

    // 刷新Softkey显示
    emit RefreshSoftkey();
}

void CDlgDateTimeGeneral::Init()
{
    //
}

void CDlgDateTimeGeneral::Uninit()
{

}

bool CDlgDateTimeGeneral::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL || pEvent == NULL)
    {
        return false;
    }

    if (pEvent->type() ==  QEvent::FocusIn)
    {
        QWidget * pFocusWgt = focusWidget();

        if (NULL != pFocusWgt)
        {
            if (FullKeyboardEnabled() && pFocusWgt->isEnabled() && m_pListDelegate != NULL)
            {
                CSettingItem* pItem = m_pListDelegate->FindItemByContentWgt(pFocusWgt);
                if (pItem != NULL && pItem->GetDataIndex() != NULL && isListReady())
                {
                    this->SetCurrentIndex(pItem->GetDataIndex());
                    qSetIsShowWnd(pFocusWgt, true);
                }
            }

            if (pFocusWgt->inherits("QLineEdit"))
            {
                emit RefreshSoftkey();
            }
        }
    }

    if (pEvent->type() == QEvent::MouseButtonPress)
    {
        if (pObject->inherits("QLineEdit"))
        {
            QLineEdit * pEdit = qobject_cast<QLineEdit*>(pObject);
            if (FullKeyboardEnabled() && pEdit->isEnabled())
            {
                if (!qGetIsShowWnd())
                {
                    qSetIsShowWnd(pEdit, true);
                }
                emit RefreshSoftkey();
            }
        }
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

void CDlgDateTimeGeneral::RefreshDateAndTime()
{
    if (m_pDataFrame != NULL
            && m_pTimeFrame != NULL)
    {
        QString strTemp;
        // 即时刷新时间
        m_pPreDatatime.setTime_t(
            g_pDateAndTimeController->GetLocalTime());// 通过获取系统时间设置当前时间

        m_pDataFrame->m_plineYear->setText(strTemp.setNum(m_pPreDatatime.date().year()));

        strTemp.sprintf("%.2d", m_pPreDatatime.date().month());
        m_pDataFrame->m_plineMonth->setText(strTemp);

        strTemp.sprintf("%.2d", m_pPreDatatime.date().day());
        m_pDataFrame->m_plineDay->setText(strTemp);

        strTemp.sprintf("%.2d", m_pPreDatatime.time().hour());
        m_pTimeFrame->m_plineHour->setText(strTemp);

        strTemp.sprintf("%.2d", m_pPreDatatime.time().minute());
        m_pTimeFrame->m_plineMinute->setText(strTemp);

        strTemp.sprintf("%.2d", m_pPreDatatime.time().second());
        m_pTimeFrame->m_plineSecond->setText(strTemp);
    }
}

bool CDlgDateTimeGeneral::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (DATE_AND_TIME_SNTP == m_ePageType)
    {
        if (m_pEditNTPServer1->text().isEmpty()
                || !qLineEditUtility::IsFormatValid(m_pEditNTPServer1->text(), EDIT_TYPE_SERVER))
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_NTP_SERVER1_IS_INVALID);
            objErrorContent.pLineEdit = m_pEditNTPServer1;
            return false;
        }
        if (!m_pEditNTPServer2->text().isEmpty()
                && !qLineEditUtility::IsFormatValid(m_pEditNTPServer2->text(), EDIT_TYPE_SERVER))
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_NTP_SERVER2_IS_INVALID);
            objErrorContent.pLineEdit = m_pEditNTPServer2;
            return false;
        }
    }
    return true;
}

/*
完成页面显示数据的保存
*/
bool CDlgDateTimeGeneral::SavePageData()
{
    //指针判空
    if (NULL == m_pCmbType)
    {
        return false;
    }

    //保存自动设置数据到pageDate
    DateTimeGeneralData pageData;
    bool bResult = GetPageData(&pageData);

    if (bResult)
    {
        //设置pageData为配置数据,手动设置也要保存自动设置界面的值
        g_pDateAndTimeController->SetGeneralData(pageData);
        //当前为手动设置
        if (pageData.m_eDateTimeType == DATE_AND_TIME_MANUAL)
        {
            if (m_pDataFrame != NULL
                    && m_pTimeFrame != NULL)
            {
                //保存手动设置数据到pageDateTime
                tm pageDateTime;
                pageDateTime.tm_year = m_pDataFrame->m_plineYear->text().toInt();
                pageDateTime.tm_mon = m_pDataFrame->m_plineMonth->text().toInt();
                pageDateTime.tm_mday = m_pDataFrame->m_plineDay->text().toInt();

                pageDateTime.tm_hour = m_pTimeFrame->m_plineHour->text().toInt();
                pageDateTime.tm_min = m_pTimeFrame->m_plineMinute->text().toInt();
                pageDateTime.tm_sec = m_pTimeFrame->m_plineSecond->text().toInt();

                //设置时间前需要先设置时间模式http://10.3.5.199/Bug.php?BugID=61792
                g_pDateAndTimeController->SetTimeType(DATE_AND_TIME_MANUAL);
                //设置pageDateTime为配置数据
                g_pDateAndTimeController->SetLocalTime(pageDateTime);

                m_pPreDatatime.setDate(QDate(pageDateTime.tm_year, pageDateTime.tm_mon, pageDateTime.tm_mday));
                m_pPreDatatime.setTime(QTime(pageDateTime.tm_hour, pageDateTime.tm_min, pageDateTime.tm_sec));
            }
        }
    }


    // 刷新时间的显示
    etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SETTIME, 0);
    msgBroadpostThreadMsg(SYS_MSG_NOTIFY_TIME_CHANGED, 0, 0);
    etl_NotifyApp(FALSE, SYS_MSG_NOTIFY_TIME_CHANGED, 0, 0);

    //给rtserver发消息
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SETTIME, 0);

    return true;
}

CDlgDateTimeGeneral::~CDlgDateTimeGeneral()
{
}

void CDlgDateTimeGeneral::OnCmbTypeChanged(int iIndex)
{
    if (m_pCmbType == NULL)
    {
        return;
    }

    RefreshByPageType((DateAndTimeType)m_pCmbType->itemData(iIndex, Qt::UserRole).value<int>());
}

void CDlgDateTimeGeneral::OnCmbDaylightSavingChanged(int iIndex)
{
    if (m_pCmbType == NULL)
    {
        return;
    }

    RefreshByPageType((DateAndTimeType)m_pCmbType->itemData(m_pCmbType->currentIndex(),
                      Qt::UserRole).value<int>(), true);

    m_pCmbBoxDaylightSaving->setFocus();
}

void CDlgDateTimeGeneral::SetCurrentTimezoneCmbIndex(yl::string strCurrentTimeZone)
{
    if (NULL == m_pCmbBoxTimeZone || strCurrentTimeZone.empty())
    {
        return;
    }

    //更新数据
    int iIndex = m_pCmbBoxTimeZone->findText(toQString(strCurrentTimeZone.c_str()));

    if (iIndex == -1)
    {
        QString strExtTimezone = QString("+") + toQString(strCurrentTimeZone.c_str());
        int iRet = m_pCmbBoxTimeZone->findText(strExtTimezone);
        if (iRet != -1)
        {
            iIndex = iRet;
        }
    }

    // 校正焦点值
    if (iIndex < 0 || iIndex >= m_pCmbBoxTimeZone->count())
    {
        iIndex = 0;
    }
    m_pCmbBoxTimeZone->setCurrentIndex(iIndex);
}

// 卸载界面上所有的输入法
void CDlgDateTimeGeneral::UninstallAllIME()
{
    qUninstallIME(m_pEditNTPServer1);
    qUninstallIME(m_pEditNTPServer2);
    qUninstallIME(m_pDataFrame->m_plineYear);
    qUninstallIME(m_pDataFrame->m_plineMonth);
    qUninstallIME(m_pDataFrame->m_plineDay);
    qUninstallIME(m_pTimeFrame->m_plineHour);
    qUninstallIME(m_pTimeFrame->m_plineMinute);
    qUninstallIME(m_pTimeFrame->m_plineSecond);
}

