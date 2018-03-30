#include "dlgsetscreensaver.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uikernel/languagehelper.h"
#include "keymap.h"
#include "uikernel/qcomboboxutility.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "service_config.h"
#include "resource/modresource.h"
#include "dsklog/log.h"

namespace
{
//屏保等待时间
// 产品部需求变更，去除Never项目
//#define SCREEN_SAVE_WAIT_TIME_NEVER           0
#define SCREEN_SAVE_WAIT_TIME_FIFTEEN_SECONDS   15
#define SCREEN_SAVE_WAIT_TIME_THIRTY_SECONDS    30
#define SCREEN_SAVE_WAIT_TIME_ONE_MINUTE        60
#define SCREEN_SAVE_WAIT_TIME_TWO_MINUTES       120
#define SCREEN_SAVE_WAIT_TIME_FIVE_MINUTES      300
#define SCREEN_SAVE_WAIT_TIME_TEN_MINUTES       600
#define SCREEN_SAVE_WAIT_TIME_HALF_HOUR         1800
#define SCREEN_SAVE_WAIT_TIME_ONE_HOUR          3600
#define SCREEN_SAVE_WAIT_TIME_TWO_HOURS         7200
#define SCREEN_SAVE_WAIT_TIME_THREE_HOURS       10800
#define SCREEN_SAVE_WAIT_TIME_SIX_HOURS         21600

enum
{
    SCREEN_SAVER_TYPE_SYSTEM = 0,
    SCREEN_SAVER_TYPE_CUSTOM,
    SCREEN_SAVER_TYPE_XMLBROWSER,
};
}

CDlgSetScreenSaver::CDlgSetScreenSaver(QWidget * pParent /* = NULL */)
    : CDlgBaseSubPage(pParent)
    , m_bInitSucceed(true)
{
    InitData();

    InitListItemData();
}

CDlgSetScreenSaver::~CDlgSetScreenSaver()
{
}

bool CDlgSetScreenSaver::LoadPageData()
{
    this->setFocus();

    if (!IsInitSucceed())
    {
        return false;
    }

    int nWaitTime = GetWaitTime();
    AdjustWaitTime(nWaitTime);
    m_pCmbWaitTime->setCurrentIndex(m_pCmbWaitTime->findData(nWaitTime));

    if (GetShowClockType() != 0)
    {
        m_pChBoxShowSysClock->SetData(true);
    }
    else
    {
        m_pChBoxShowSysClock->SetData(false);
    }

    int nScreenType = GetScreensaverType();
    m_pCmbSaverType->setCurrentIndex(m_pCmbSaverType->findData(nScreenType));

#if !IF_FEATURE_XMLB_SCREENSAVER
    if (SCREEN_SAVER_TYPE_SYSTEM == nScreenType && !IsHasCumtomPicture())
    {
        m_pCmbSaverType->setEnabled(false);
    }
#endif

#if IF_FEATURE_XMLB_SCREENSAVER
    if (SCREEN_SAVER_TYPE_XMLBROWSER != nScreenType)
    {
        m_pChBoxShowSysClock->setEnabled(true);

    }
    else
    {
        m_pChBoxShowSysClock->setEnabled(false);
    }
#endif
    return true;
}

bool CDlgSetScreenSaver::IsStatusChanged()
{
    if (!IsInitSucceed())
    {
        return false;
    }

    int nCurWaitTimes = m_pCmbWaitTime->itemData(m_pCmbWaitTime->currentIndex()).value<int>();
    int nSavedWaitTimes = GetWaitTime();

    if (nCurWaitTimes != nSavedWaitTimes)
    {
        return true;
    }

    int nCurShowClock = int(m_pChBoxShowSysClock->GetData());
    int nSavedShowClock = GetShowClockType();

    if (nCurShowClock != nSavedShowClock)
    {
        return true;
    }

    int nCurSaverType = m_pCmbSaverType->itemData(m_pCmbSaverType->currentIndex()).value<int>();
    int nSavedSaverType = GetScreensaverType();

    if (nCurSaverType != nSavedSaverType)
    {
        return true;
    }

    return false;
}

bool CDlgSetScreenSaver::SavePageData()
{
    if (IsStatusChanged())
    {
        int nCurWaitTimes = m_pCmbWaitTime->itemData(m_pCmbWaitTime->currentIndex()).value<int>();
        configParser_SetConfigInt(kszScreenSaverWaitTime, nCurWaitTimes, CONFIG_LEVEL_PHONE);
        etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_SCREEN_SAVER_TIME, 0);

        int nCurIsShowSysClock = int(m_pChBoxShowSysClock->GetData());
        configParser_SetConfigInt(kszScreensaverDisplayClock, nCurIsShowSysClock, CONFIG_LEVEL_PHONE);
        etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_SCREENSAVER_SHOW_CLOCK, 0);

        int nCurSaverType = m_pCmbSaverType->itemData(m_pCmbSaverType->currentIndex()).value<int>();
        configParser_SetConfigInt(kszScreenSaverType, nCurSaverType, CONFIG_LEVEL_PHONE);
        etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_SCREEN_SAVER_MODE, 0);
    }

    return true;
}

bool CDlgSetScreenSaver::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (!IsInitSucceed())
    {
        return true;
    }

    int nCurSaverType = m_pCmbSaverType->itemData(m_pCmbSaverType->currentIndex()).value<int>();

    if (SCREEN_SAVER_TYPE_CUSTOM == nCurSaverType && !IsHasCumtomPicture())
    {
        m_pCmbSaverType->setCurrentIndex(m_pCmbSaverType->findData(SCREEN_SAVER_TYPE_SYSTEM));

        objErrorContent.strErrorContent = TRID_YOU_HAVE_TO_UPLOAD_YOUR_PICTURE_WHEN_CHOOSE_THIS_TYLE;
        return false;
    }
#if IF_FEATURE_XMLB_SCREENSAVER
    else if (SCREEN_SAVER_TYPE_XMLBROWSER == nCurSaverType
             && configParser_GetConfigString(kszXmlBrowserScreenSaverURL).empty())
    {
        m_pCmbSaverType->setCurrentIndex(m_pCmbSaverType->findData(SCREEN_SAVER_TYPE_SYSTEM));

        objErrorContent.strErrorContent = TRID_XML_BROWSER_URL_CANNOT_BE_EMPTY;
        return false;
    }
#endif

    return true;
}

void CDlgSetScreenSaver::InitData()
{
    m_pCmbWaitTime = new QComboBox(this);
    m_pChBoxShowSysClock = new qCheckBox(this);
    m_pCmbSaverType = new QComboBox(this);

    if (NULL == m_pCmbWaitTime
            || NULL == m_pChBoxShowSysClock
            || NULL == m_pCmbSaverType)
    {
        m_bInitSucceed = false;
        return;
    }

    qComboBoxUtility::SetComboBoxStyle(m_pCmbWaitTime, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCmbSaverType, Qt::black);

    FillWaitTimeCmb();

    m_pCmbSaverType->addItem(LANG_TRANSLATE(TRID_SYSTEM),
                             QVariant::fromValue(int(SCREEN_SAVER_TYPE_SYSTEM)));
    m_pCmbSaverType->addItem(LANG_TRANSLATE(TRID_CUSTOM),
                             QVariant::fromValue(int(SCREEN_SAVER_TYPE_CUSTOM)));
#if IF_FEATURE_XMLB_SCREENSAVER
    m_pCmbSaverType->addItem(LANG_TRANSLATE(TRID_SERVER_XML),
                             QVariant::fromValue(int(SCREEN_SAVER_TYPE_XMLBROWSER)));
#endif

    m_pCmbWaitTime->installEventFilter(this);
    m_pChBoxShowSysClock->installEventFilter(this);
    m_pCmbSaverType->installEventFilter(this);
}

void CDlgSetScreenSaver::InitListItemData()
{
    if (!IsInitSucceed())
    {
        return;
    }

    MakeReadyForRelayout();

    QWidgetPair qPair;

    qPair.first = LANG_TRANSLATE(TIRD_SCREENSAVER_WAIT_TIME);
    qPair.second = m_pCmbWaitTime;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_DISPLAY_CLOCK);
    qPair.second = m_pChBoxShowSysClock;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_SCREENSAVER_TYPE);
    qPair.second = m_pCmbSaverType;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
}

void CDlgSetScreenSaver::FillWaitTimeCmb()
{
    if (NULL == m_pCmbWaitTime)
    {
        return;
    }

//  m_pCmbWaitTime->addItem(LANG_TRANSLATE(TRID_NEVER), QVariant::fromValue(SCREEN_SAVE_WAIT_TIME_NEVER));
    m_pCmbWaitTime->addItem(LANG_TRANSLATE(TRID_15_SECONDS),
                            QVariant::fromValue(SCREEN_SAVE_WAIT_TIME_FIFTEEN_SECONDS));
    m_pCmbWaitTime->addItem(LANG_TRANSLATE(TRID_30_SECONDS),
                            QVariant::fromValue(SCREEN_SAVE_WAIT_TIME_THIRTY_SECONDS));
    m_pCmbWaitTime->addItem(LANG_TRANSLATE(TRID_1_MINUT),
                            QVariant::fromValue(SCREEN_SAVE_WAIT_TIME_ONE_MINUTE));
    m_pCmbWaitTime->addItem(LANG_TRANSLATE(TRID_2_MINUTS),
                            QVariant::fromValue(SCREEN_SAVE_WAIT_TIME_TWO_MINUTES));
    m_pCmbWaitTime->addItem(LANG_TRANSLATE(TRID_5_MINUTS),
                            QVariant::fromValue(SCREEN_SAVE_WAIT_TIME_FIVE_MINUTES));
    m_pCmbWaitTime->addItem(LANG_TRANSLATE(TRID_10_MINUTS),
                            QVariant::fromValue(SCREEN_SAVE_WAIT_TIME_TEN_MINUTES));
    m_pCmbWaitTime->addItem(LANG_TRANSLATE(TRID_30_MINUTS),
                            QVariant::fromValue(SCREEN_SAVE_WAIT_TIME_HALF_HOUR));
    m_pCmbWaitTime->addItem(LANG_TRANSLATE(TRID_1_HOUR),
                            QVariant::fromValue(SCREEN_SAVE_WAIT_TIME_ONE_HOUR));
    m_pCmbWaitTime->addItem(LANG_TRANSLATE(TRID_2_HOUR),
                            QVariant::fromValue(SCREEN_SAVE_WAIT_TIME_TWO_HOURS));
    m_pCmbWaitTime->addItem(LANG_TRANSLATE(TRID_3_HOUR),
                            QVariant::fromValue(SCREEN_SAVE_WAIT_TIME_THREE_HOURS));
    m_pCmbWaitTime->addItem(LANG_TRANSLATE(TRID_6_HOUR),
                            QVariant::fromValue(SCREEN_SAVE_WAIT_TIME_SIX_HOURS));
}

bool CDlgSetScreenSaver::IsInitSucceed() const
{
    return m_bInitSucceed;
}

void CDlgSetScreenSaver::AdjustWaitTime(int & nTimes)
{
    /*
        if (nTimes <= 0)
        {
            nTimes = SCREEN_SAVE_WAIT_TIME_NEVER;
        }
        else
    */
    if (nTimes <= 22)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_FIFTEEN_SECONDS;
    }
    else if (nTimes > 22 && nTimes <= 44)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_THIRTY_SECONDS;
    }
    else if (nTimes > 44 && nTimes <= 89)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_ONE_MINUTE;
    }
    else if (nTimes > 89 && nTimes <= 209)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_TWO_MINUTES;
    }
    else if (nTimes > 209 && nTimes <= 449)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_FIVE_MINUTES;
    }
    else if (nTimes > 449 && nTimes <= 1199)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_TEN_MINUTES;
    }
    else if (nTimes > 1199 && nTimes <= 2699)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_HALF_HOUR;
    }
    else if (nTimes > 2699 && nTimes <= 5399)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_ONE_HOUR;
    }
    else if (nTimes > 5399 && nTimes <= 8999)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_TWO_HOURS;
    }
    else if (nTimes > 8999 && nTimes <= 16199)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_THREE_HOURS;
    }
    else
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_SIX_HOURS;
    }
}

bool CDlgSetScreenSaver::IsHasCumtomPicture()
{
    YLList< YLSmartPtr<BaseResItem> > ylListUserPic;
    res_getResItemsByItemType(ylListUserPic, SCREENSAVER_TYPE, IMPORT_TYPE);

    if (ylListUserPic.size() > 0)
    {
        return true;
    }

    return false;
}

int CDlgSetScreenSaver::GetWaitTime()
{
    return configParser_GetConfigInt(kszScreenSaverWaitTime);
}

int CDlgSetScreenSaver::GetShowClockType()
{
    return configParser_GetConfigInt(kszScreensaverDisplayClock);
}

int CDlgSetScreenSaver::GetScreensaverType()
{
    return configParser_GetConfigInt(kszScreenSaverType);
}
