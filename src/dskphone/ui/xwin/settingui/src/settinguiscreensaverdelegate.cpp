#include "settinguiscreensaverdelegate.h"

#if IF_SUPPORT_SCREEN_SAVER
#include "settinguimanager.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "setuidatadefine.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/modsettingui.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "resource/modresource.h"
#include "uicallbackfun.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
//屏保等待时间
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

#define WAIT_TIME_LEVEL_1                       22
#define WAIT_TIME_LEVEL_2                       44
#define WAIT_TIME_LEVEL_3                       89
#define WAIT_TIME_LEVEL_4                       209
#define WAIT_TIME_LEVEL_5                       449
#define WAIT_TIME_LEVEL_6                       1199
#define WAIT_TIME_LEVEL_7                       2699
#define WAIT_TIME_LEVEL_8                       5399
#define WAIT_TIME_LEVEL_9                       8999
#define WAIT_TIME_LEVEL_10                      16199

enum
{
    SCREEN_SAVER_TYPE_SYSTEM = 0,
    SCREEN_SAVER_TYPE_CUSTOM,
    SCREEN_SAVER_TYPE_XMLBROWSER,
};

typedef YLList< YLSmartPtr<BaseResItem> >           LIST_RES_ITEM_SMART_PTR;
}

CSettingUIScreenSaverDelegate::CSettingUIScreenSaverDelegate()
{

}

CSettingUIScreenSaverDelegate::~CSettingUIScreenSaverDelegate()
{

}

CSettingUIDelegateBase* CSettingUIScreenSaverDelegate::CreateSettingUIScreenSaverDelegate()
{
    CSettingUIScreenSaverDelegate* pDelegate = new CSettingUIScreenSaverDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase* CSettingUIScreenSaverDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL == m_pPageData || !bReLoad)
    {
        return NULL;
    }

    int nWaitTime = configParser_GetConfigInt(kszScreenSaverWaitTime);
    AdjustWaitTime(nWaitTime);
    SetComboboxValue(SCREEN_SAVER_WAIT_TIME, nWaitTime);

    return m_pPageData;
}

bool CSettingUIScreenSaverDelegate::PageDataFilter(CSettingUIPageDataBase* pPagedata, bool bReload /*= false*/)
{
    if (NULL == m_pPageData)
    {
        return false;
    }

#if !IF_FEATURE_XMLB_SCREENSAVER
    if (SCREEN_SAVER_TYPE_SYSTEM == nScreenType && !IsHasCumtomPicture())
    {
        SetItemEnable(m_pPageData, SCREEN_SAVER_TYPE, false);
    }
#endif

    int nScreenType = configParser_GetConfigInt(kszScreenSaverType);

#if IF_FEATURE_XMLB_SCREENSAVER
    if (SCREEN_SAVER_TYPE_XMLBROWSER != nScreenType)
    {
        SetItemEnable(m_pPageData, SCREEN_SAVER_SHOW_SYSTEM_CLOCK, true);
    }
    else
    {
        SetItemEnable(m_pPageData, SCREEN_SAVER_SHOW_SYSTEM_CLOCK, false);
    }
#endif

    return true;
}

bool CSettingUIScreenSaverDelegate::SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    int nCurSaverType = GetComboboxValue(SCREEN_SAVER_TYPE);

    bool bInvalid = false;
    yl::string strValidNote;

    if (SCREEN_SAVER_TYPE_CUSTOM == nCurSaverType && !IsHasCumtomPicture())
    {
        bInvalid = true;
        strValidNote = _LANG(TRID_YOU_HAVE_TO_UPLOAD_YOUR_PICTURE_WHEN_CHOOSE_THIS_TYLE);
    }
#if IF_FEATURE_XMLB_SCREENSAVER
    else if (SCREEN_SAVER_TYPE_XMLBROWSER == nCurSaverType
             && configParser_GetConfigString(kszXmlBrowserScreenSaverURL).empty())
    {
        bInvalid = true;
        strValidNote = _LANG(TRID_XML_BROWSER_URL_CANNOT_BE_EMPTY);
    }
#endif

    if (bInvalid)
    {
        SetComboboxValue(SCREEN_SAVER_TYPE, SCREEN_SAVER_TYPE_SYSTEM);
        g_SetUICallBackFun.RefreshUI(false, GetPageID());

        CSettingUIDataValidInfo dataValid;
        int nDataIndex = GetItemDataIndexByItemId(SCREEN_SAVER_TYPE);
        dataValid.SetValidIndex(nDataIndex);
        dataValid.SetValidNote(strValidNote);
        g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
        return false;
    }

    return CSettinguiDataDelegateBase::SavePageData(pPageData);
}

bool CSettingUIScreenSaverDelegate::IsHasCumtomPicture()
{
    LIST_RES_ITEM_SMART_PTR ylListUserPic;
    res_getResItemsByItemType(ylListUserPic, SCREENSAVER_TYPE, IMPORT_TYPE);

    if (ylListUserPic.size() > 0)
    {
        return true;
    }

    return false;
}

void CSettingUIScreenSaverDelegate::AdjustWaitTime(int& nTimes)
{
    if (nTimes <= WAIT_TIME_LEVEL_1)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_FIFTEEN_SECONDS;
    }
    else if (nTimes > WAIT_TIME_LEVEL_1 && nTimes <= WAIT_TIME_LEVEL_2)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_THIRTY_SECONDS;
    }
    else if (nTimes > WAIT_TIME_LEVEL_2 && nTimes <= WAIT_TIME_LEVEL_3)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_ONE_MINUTE;
    }
    else if (nTimes > WAIT_TIME_LEVEL_3 && nTimes <= WAIT_TIME_LEVEL_4)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_TWO_MINUTES;
    }
    else if (nTimes > WAIT_TIME_LEVEL_4 && nTimes <= WAIT_TIME_LEVEL_5)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_FIVE_MINUTES;
    }
    else if (nTimes > WAIT_TIME_LEVEL_5 && nTimes <= WAIT_TIME_LEVEL_6)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_TEN_MINUTES;
    }
    else if (nTimes > WAIT_TIME_LEVEL_6 && nTimes <= WAIT_TIME_LEVEL_7)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_HALF_HOUR;
    }
    else if (nTimes > WAIT_TIME_LEVEL_7 && nTimes <= WAIT_TIME_LEVEL_8)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_ONE_HOUR;
    }
    else if (nTimes > WAIT_TIME_LEVEL_8 && nTimes <= WAIT_TIME_LEVEL_9)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_TWO_HOURS;
    }
    else if (nTimes > WAIT_TIME_LEVEL_9 && nTimes <= WAIT_TIME_LEVEL_10)
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_THREE_HOURS;
    }
    else
    {
        nTimes = SCREEN_SAVE_WAIT_TIME_SIX_HOURS;
    }
}
#endif
