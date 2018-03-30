#include "screensavercontroller.h"
#include "screensaverpainter.h"

#include "configparser/modconfigparser.h"
#include "configiddefine/configiddefine.h"
#include "service_config.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "lamp/backlight/include/modbacklight.h"
#include "baselibs/include/dsklog/log.h"
#include "lamp/powersaving/include/powersavingmsgdefine.h"

#if IF_FEATURE_XMLB_SCREENSAVER || IS_COLOR
namespace
{
//使用resource接口获取文件用到的类型定义
#define SCREENSAVER_FILE_TYPE_SYSTEM                SYSTEM_TYPE
#if SCREEN_SAVE_FEATRUE_CUSTOM_MODE
#define SCREENSAVER_FILE_TYPE_USER                  IMPORT_TYPE
#endif

#define SCREENSAVER_BG_CHANGE_MAX_INTERVAL          1200
#define SCREENSAVER_BG_CHANGE_MIN_INTERVAL          5
#define SCREENSAVER_BG_CHANGE_DEFAULT_INTERVAL      600

#define SCREENSAVER_CLOCK_MOVE_MAX_INTERVAL         1200
#define SCREENSAVER_CLOCK_MOVE_MIN_INTERVAL         5
#define SCREENSAVER_CLOCK_MOVE_DEFAULT_INTERVAL     600

}

ScreenSaverController& ScreenSaverController::GetInstance()
{
    static ScreenSaverController s_Instance;

    return s_Instance;
}

ScreenSaverController::ScreenSaverController()
#if IS_COLOR
    : m_eSSType(SCREENSAVER_MODE_CHANGE_TYPE_SYS)
#else
    : m_eSSType(SCREENSAVER_MODE_CHANGE_TYPE_XML)
#endif
    , m_strCurrentPicPath("")
    , m_nLastUsedPicIndex(-1)
    , m_nLastUsedColorIndex(0)
    , m_nBGChangeInterval(0)
    , m_nClockMoveInterval(0)
    , m_bIsShowClock(true)
{
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, &ScreenSaverController::OnMessage);

    etl_RegisterMsgHandle(BACKLIGHT_LEVEL_CHANGED_MSG, BACKLIGHT_LEVEL_CHANGED_MSG, &ScreenSaverController::OnMessage); //屏保移植注释

    etl_RegisterMsgHandle(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVINGMEG_SCREEN_AWAKE, &ScreenSaverController::OnMessage);
}

ScreenSaverController::~ScreenSaverController()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, &ScreenSaverController::OnMessage);

    etl_UnregisterMsgHandle(BACKLIGHT_LEVEL_CHANGED_MSG, BACKLIGHT_LEVEL_CHANGED_MSG, &ScreenSaverController::OnMessage);

    etl_UnregisterMsgHandle(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVINGMEG_SCREEN_AWAKE, &ScreenSaverController::OnMessage);
}


#if IS_COLOR
void ScreenSaverController::ReloadPicFiles()
{
    m_listSysPic.clear();
    res_getResItemsByItemType(m_listSysPic, SCREENSAVER_TYPE, SCREENSAVER_FILE_TYPE_SYSTEM);

#if SCREEN_SAVE_FEATRUE_CUSTOM_MODE
    m_listUserPic.clear();
    res_getResItemsByItemType(m_listUserPic, SCREENSAVER_TYPE, SCREENSAVER_FILE_TYPE_USER);
#endif
}

#endif

void ScreenSaverController::LoadDefault()
{
#if SCREEN_SAVE_FEATRUE_CUSTOM_MODE
    m_eSSType = (SCREENSAVER_MODE_CHANGE_TYPE)configParser_GetConfigInt(kszScreenSaverType);
#else
    m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_SYS;
#endif

    m_nBGChangeInterval = configParser_GetConfigInt(kszPicChangeInterval);
    JudgeIntervalValid(m_nBGChangeInterval, SCREENSAVER_BG_CHANGE_MAX_INTERVAL, SCREENSAVER_BG_CHANGE_MIN_INTERVAL, SCREENSAVER_BG_CHANGE_DEFAULT_INTERVAL);

    m_nClockMoveInterval = configParser_GetConfigInt(kszClockMoveInterval);
    JudgeIntervalValid(m_nClockMoveInterval, SCREENSAVER_CLOCK_MOVE_MAX_INTERVAL, SCREENSAVER_CLOCK_MOVE_MIN_INTERVAL, SCREENSAVER_CLOCK_MOVE_DEFAULT_INTERVAL);

    m_bIsShowClock = configParser_GetConfigInt(kszScreensaverDisplayClock);

#if IS_COLOR
    ReloadPicFiles();
#endif


/////////////////////////////////////
////////////////////////////////

}

#if IS_COLOR

int ScreenSaverController::GetNotRepeatIndex(int nLastIndex, int nIndexCount)
{
    int nRandomIndex;
    //如果列表总数大于1，则可以进行随机
    if (nIndexCount > 1)
    {
        //如果上次使用的index不是-1，即存在上次使用的index
        //则在随机时如果得到相等的index就再随机一次
        if (nLastIndex != -1)
        {
            do
            {
                nRandomIndex = GetRandomNumber(nIndexCount);
            }
            while (nRandomIndex == nLastIndex);
        }
        //如果上次使用的是-1，说明没有上次使用的index，直接返回一个随机index
        else
        {
            nRandomIndex = GetRandomNumber(nIndexCount);
        }
    }
    //否则直接选取第一个
    else
    {
        nRandomIndex = 0;
    }

    //容错处理
    if (nRandomIndex > nIndexCount || nRandomIndex < 0)
    {
        nRandomIndex = 0;
    }

    return nRandomIndex;
}

yl::string ScreenSaverController::GetNextScreenSaverPicPath()
{
    IDLEUI_INFO("ScreenSavers Get Next Pic Path, nSSType = %d", m_eSSType);

    //如果使用系统默认屏保则返回系统默认图片
    bool bUserSysPic = (SCREENSAVER_MODE_CHANGE_TYPE_SYS == m_eSSType);

#if SCREEN_SAVE_FEATURE_SHOW_PICTURE_BY_RANDOM
    return GetRandomPic(bUserSysPic);
#else
    return GetPictureByOrder(bUserSysPic);
#endif
}

yl::string ScreenSaverController::GetRandomPic(bool bUseSysPic)
{
    listScreenSaverPic& listPic = bUseSysPic ? m_listSysPic : m_listUserPic;
    int nListSize = listPic.size();

    IDLEUI_INFO("ScreenSavers Get Random Pic, bUseSysPic = %d, nListSize = %d", bUseSysPic, nListSize);

    //如果获取不到自定义图片则使用系统默认图片
    if (0 >= nListSize)
    {
        if (bUseSysPic)
        {
            m_strCurrentPicPath = "";
        }
        else
        {
            //属于用户到系统的模式
            m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_NO_USER_PIC;
            m_strCurrentPicPath = GetRandomPic(true);
        }
    }
    else
    {
        //选出一个不重复index
        m_nLastUsedPicIndex = GetNotRepeatIndex(m_nLastUsedPicIndex, nListSize);

        //根据index找到对应列表项
        listScreenSaverPic::iterator iter = listPic.begin();
        for (int nIndex = 0
                          ; iter != listPic.end() && nIndex < nListSize && nIndex != m_nLastUsedPicIndex
                ; iter++, ++nIndex)
        {
            //
        }

        YLSmartPtr<BaseResItem>& pItem = (iter != listPic.end())
                                         ? *iter
                                         : *listPic.begin();

        if (pItem.IsEmpty())
        {
            //属于用户到系统的模式
            m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_NO_USER_PIC;
            m_strCurrentPicPath = GetRandomPic(true);
        }
        else
        {
            m_strCurrentPicPath = pItem->resPath;
            IDLEUI_INFO("ScreenSavers Get Random Pic, m_strCurrentPicPath = %s", m_strCurrentPicPath.c_str());
        }
    }

    return m_strCurrentPicPath;
}

yl::string ScreenSaverController::GetPictureByOrder(bool bUseSysPic)
{
    yl::string strNextPicture;

    if (m_eSSType == SCREENSAVER_MODE_CHANGE_TYPE_XML)
    {
        strNextPicture = GetUserPicByOrder();
        if (strNextPicture.empty())
        {
            strNextPicture = GetSysPicByOrder();
        }

        return strNextPicture;
    }
    if (bUseSysPic)
    {
        strNextPicture = GetSysPicByOrder();
    }
    else
#if SCREEN_SAVE_FEATRUE_CUSTOM_MODE
    {
        strNextPicture = GetUserPicByOrder();

        if (strNextPicture.empty())
        {
            m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_NO_USER_PIC;

            strNextPicture = GetSysPicByOrder();
        }
    }
#endif

    return strNextPicture;
}

yl::string ScreenSaverController::GetSysPicByOrder()
{
    listScreenSaverPic& listPic = m_listSysPic;
    int nListSize = listPic.size();

    IDLEUI_INFO("ScreenSavers Get System Pic Size = %d", nListSize);

    if (nListSize <= 0)
    {
        m_strCurrentPicPath = "";
    }
    else
    {
        m_strCurrentPicPath = GetNextPicture(listPic);
    }

    return m_strCurrentPicPath;
}

yl::string ScreenSaverController::GetUserPicByOrder()
{
    listScreenSaverPic& listPic = m_listUserPic;
    int nListSize = listPic.size();

    yl::string strPicPath;

    IDLEUI_INFO("ScreenSavers Get User Pic Size = %d", nListSize);

    if (nListSize <= 0)
    {
        strPicPath = "";
    }
    else
    {
        strPicPath = GetNextPicture(listPic);

        if (!strPicPath.empty())
        {
            m_strCurrentPicPath = strPicPath;
        }
    }

    return strPicPath;
}

yl::string ScreenSaverController::GetNextPicture(listScreenSaverPic& listPic)
{
    int nListSize = listPic.size();

    if (++m_nLastUsedPicIndex >= nListSize)
    {
        m_nLastUsedPicIndex = 0;
    }

    //根据index找到对应列表项
    listScreenSaverPic::iterator iter = listPic.begin();
    for (int nIndex = 0 ; iter != listPic.end() && nIndex < nListSize && nIndex != m_nLastUsedPicIndex;
            iter++, ++nIndex)
    {
        //
    }

    YLSmartPtr<BaseResItem>& pItem = (iter != listPic.end()) ? *iter : *listPic.begin();

    if (pItem.IsEmpty())
    {
        return yl::string("");
    }
    else
    {
        return pItem->resPath;
    }
}

#endif

void ScreenSaverController::JudgeIntervalValid(int& nInterval, const int nMaxInterval, const int nMinInterval, const int nDefault)
{
    if (nInterval > nMaxInterval)
    {
        nInterval = nDefault;
    }

    if (nInterval < nMinInterval)
    {
        nInterval = nDefault;
    }

    nInterval *= 1000;
}

int ScreenSaverController::GetRandomNumber(int nMax)
{
    return (rand() % nMax);
}


void ScreenSaverController::CheckXMLScreenSavers()
{
    if (m_eSSType == SCREENSAVER_MODE_CHANGE_TYPE_XML)
    {
        g_ScreenSaverPainter.IsOnXMLScreenSavers(true);
    }
    else
    {
        g_ScreenSaverPainter.IsOnXMLScreenSavers(false);
    }
}

LRESULT ScreenSaverController::OnReceiveMessage(msgObject & refMsg)
{
    if (refMsg.message == POWERSAVINGMEG_SCREEN_AWAKE)
    {
        g_ScreenSaverPainter.SetPowerSavingEnable(false);
    }
    if (refMsg.message != CONFIG_MSG_BCAST_CHANGED &&
            refMsg.message != BACKLIGHT_LEVEL_CHANGED_MSG &&
            refMsg.message != POWERSAVINGMEG_SCREEN_CLOSE)
    {
        return FALSE;
    }

    IDLEUI_INFO("ScreenSaver Controller OnMessage, refMsg.wParam = %d refMsg.lParam = %d", refMsg.wParam, refMsg.lParam);

    switch (refMsg.wParam)
    {
#if IS_COLOR
    case ST_SETTIME:
        {
            g_ScreenSaverPainter.OnClockFormatChange();
        }
        break;

#if SCREEN_SAVE_FEATRUE_CUSTOM_MODE
    case ST_SCREEN_SAVER_MODE:
        {
            //收到配置变化的消息，从使用系统变为使用自定义或相反
            SCREENSAVER_MODE_CHANGE_TYPE eSSType = (SCREENSAVER_MODE_CHANGE_TYPE)configParser_GetConfigInt(kszScreenSaverType);

            IDLEUI_INFO("ScreenSaver Controller OnMessage, ST_SCREEN_SAVER_MODE, nSSType = %d", eSSType);

            //如果配置改变，则原有的记录上一次index的内容失效
            if (m_eSSType != eSSType)
            {
                m_nLastUsedPicIndex = -1;
                m_eSSType = eSSType;

                if (SCREENSAVER_MODE_CHANGE_TYPE_SYS == eSSType)
                {
                    g_ScreenSaverPainter.OnScreenSaverPicChange(true);
                }
                else if (SCREENSAVER_MODE_CHANGE_TYPE_USER == eSSType)
                {
                    g_ScreenSaverPainter.OnScreenSaverPicChange(false);
                }
                else if (SCREENSAVER_MODE_CHANGE_TYPE_XML == eSSType)
                {
                    g_ScreenSaverPainter.OnScreenSaverPicChange(false);
                }

                CheckXMLScreenSavers();
            }
        }
        break;
#endif
#endif
    case ST_XML_SCREEN_SAVER_URL:
        {
            if (m_eSSType == SCREENSAVER_MODE_CHANGE_TYPE_XML)
            {
                xmlbrowser_DeleteSSData();
                CheckXMLScreenSavers();
            }
        }
        break;
#if IS_COLOR
    case ST_SCREEN_SAVER_PIC:
        {
            ReloadPicFiles();

            if (SCREENSAVER_MODE_CHANGE_TYPE_SYS == m_eSSType)
            {
                break;
            }

            if (0 >= m_listUserPic.size())
            {
                IDLEUI_INFO("ScreenSaver Controller OnMessage, ST_SCREEN_SAVER_PIC, m_listUserPic is NULL");

                //如果获取不到自定义图片则使用系统默认图片
                m_nLastUsedPicIndex = -1;
                //当前模式状态为从用户模式变为缺少用户图片模式
                m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_NO_USER_PIC;
                g_ScreenSaverPainter.OnScreenSaverPicChange(true);
                g_ScreenSaverPainter.OnCheckShowClock();
            }
            //如果获取到图片列表就查找正在使用的图是否还存在
            else
            {
                listScreenSaverPic::iterator iter = m_listUserPic.begin();
                for (/* */; iter != m_listUserPic.end(); iter++)
                {
                    YLSmartPtr<BaseResItem>& pItem = *iter;
                    if (pItem.IsEmpty())
                    {
                        continue;
                    }

                    if (pItem->resPath == m_strCurrentPicPath)
                    {
                        break;
                    }
                }

                // 当前背景被删除则随机使用一个用户背景
                if (iter == m_listUserPic.end())
                {
                    IDLEUI_INFO("ScreenSaver Controller OnMessage, ST_SCREEN_SAVER_PIC, Current Pic Change");

                    m_nLastUsedPicIndex = -1;
                    m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_USER;
                    g_ScreenSaverPainter.OnScreenSaverPicChange(false);
                    g_ScreenSaverPainter.OnCheckShowClock();
                }
                else
                {
                    //如果正在使用的图片没有被删除则不需要变化
                    IDLEUI_INFO("ScreenSaver Controller OnMessage, ST_SCREEN_SAVER_PIC, Current Pic Not Change");
                }
            }
        }
        break;

    case ST_SCREEN_SAVER_BG_CHANGE_TIME:
        {
            m_nBGChangeInterval = configParser_GetConfigInt(kszPicChangeInterval);
            JudgeIntervalValid(m_nBGChangeInterval, SCREENSAVER_BG_CHANGE_MAX_INTERVAL, SCREENSAVER_BG_CHANGE_MIN_INTERVAL, SCREENSAVER_BG_CHANGE_DEFAULT_INTERVAL);
            g_ScreenSaverPainter.OnBgIntervalChange();
        }
        break;
    case ST_SCREEN_SAVER_CLOCK_MOVE_TIME:
        {
            m_nClockMoveInterval = configParser_GetConfigInt(kszClockMoveInterval);
            JudgeIntervalValid(m_nClockMoveInterval, SCREENSAVER_CLOCK_MOVE_MAX_INTERVAL, SCREENSAVER_CLOCK_MOVE_MIN_INTERVAL, SCREENSAVER_CLOCK_MOVE_DEFAULT_INTERVAL);
            g_ScreenSaverPainter.OnMoveTimeIntervalChange();

        }
        break;
    case ST_SCREENSAVER_SHOW_CLOCK:
        {
            m_bIsShowClock = configParser_GetConfigInt(kszScreensaverDisplayClock);
            g_ScreenSaverPainter.OnCheckShowClock();
        }
        break;
#endif
    case BACKLIGHT_LEVEL_CHANGED_MSG:
        {
            if (refMsg.lParam > 0)
            {
                g_ScreenSaverPainter.SetPowerSavingEnable(false);
            }
            else
            {
                g_ScreenSaverPainter.SetPowerSavingEnable(true);
            }
        }
        break;
    case POWERSAVING_MODE_CLOSE:
        {
            g_ScreenSaverPainter.SetPowerSavingEnable(true);
        }
    default:
        {
            return FALSE;
        }
    }

    return TRUE;
}

LRESULT ScreenSaverController::OnMessage(msgObject & refMsg)
{
    return g_ScreenSaverController.OnReceiveMessage(refMsg);
}

#endif
