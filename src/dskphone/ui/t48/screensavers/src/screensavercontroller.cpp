#include "screensavercontroller.h"

#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "service_config.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "uimanager/moduimanager.h"
#include "dsklog/log.h"
#if IF_FEATURE_XMLB_SCREENSAVER
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#endif

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

#if SCREEN_SAVE_FEATURE_CLOCK_RECT_FROM_FIX_LIST
//刚刚进入屏保时，时钟和状态图标整体的初始位置
#define SCREENSAVER_RECT_DEFAULT        QRect(SCREENSAVER_RECT_LEFT, SCREENSAVER_RECT_TOP, SCREENSAVER_RECT_WIDTH, SCREENSAVER_RECT_HEIGHT)
#endif

#define SCREENSAVER_COLOR_BLACK                 QColor(0, 0, 0)

#if SCREEN_SAVE_FEATRUE_COLORFUL_CHANGE
//用于切换时的过渡颜色
#define SCREENSAVER_COLOR_CYAN                  QColor(137, 157, 192)
#define SCREENSAVER_COLOR_WHITE                 QColor(255, 255, 255)
#define SCREENSAVER_COLOR_DARK_GREEN            QColor(17, 63, 61)
#define SCREENSAVER_COLOR_LIGHT_GREEN           QColor(179, 214, 110)
#define SCREENSAVER_COLOR_MAGENTA               QColor(245, 75, 99)
#define SCREENSAVER_COLOR_GREEN                 QColor(6, 128, 68)
#endif

#if SCREEN_SAVE_FEATURE_CLOCK_RECT_FROM_FIX_LIST
//固定位置跳动的位置定义
#define SCREENSAVER_RECT_1                  QRect(80, 144, SCREENSAVER_RECT_WIDTH, SCREENSAVER_RECT_HEIGHT)
#define SCREENSAVER_RECT_2                  QRect(20, 288, SCREENSAVER_RECT_WIDTH, SCREENSAVER_RECT_HEIGHT)
#define SCREENSAVER_RECT_3                  QRect(80, 432, SCREENSAVER_RECT_WIDTH, SCREENSAVER_RECT_HEIGHT)
#define SCREENSAVER_RECT_4                  QRect(320, 110, SCREENSAVER_RECT_WIDTH, SCREENSAVER_RECT_HEIGHT)
#define SCREENSAVER_RECT_5                  QRect(320, 466, SCREENSAVER_RECT_WIDTH, SCREENSAVER_RECT_HEIGHT)
#define SCREENSAVER_RECT_6                  QRect(560, 144, SCREENSAVER_RECT_WIDTH, SCREENSAVER_RECT_HEIGHT)
#define SCREENSAVER_RECT_7                  QRect(620, 288, SCREENSAVER_RECT_WIDTH, SCREENSAVER_RECT_HEIGHT)
#define SCREENSAVER_RECT_8                  QRect(560, 432, SCREENSAVER_RECT_WIDTH, SCREENSAVER_RECT_HEIGHT)
#endif
}

ScreenSaverController * ScreenSaverController::s_pInstance = NULL;

ScreenSaverController * ScreenSaverController::GetInstance()
{
    if (NULL == s_pInstance)
    {
        s_pInstance = new ScreenSaverController();
    }

    return s_pInstance;
}

void ScreenSaverController::ReleaseInstance()
{
    if (s_pInstance != NULL)
    {
        delete s_pInstance;
        s_pInstance = NULL;
    }
}

ScreenSaverController::ScreenSaverController()
    : QObject(NULL)
    , m_eSSType(SCREENSAVER_MODE_CHANGE_TYPE_SYS)
    , m_strCurrentPicPath("")
    , m_nLastUsedPicIndex(-1)
    , m_nLastUsedColorIndex(0)
#if SCREEN_SAVE_FEATURE_CLOCK_RECT_FROM_FIX_LIST
    , m_nLastUsedRectIndex(0)
#endif
    , m_nBGChangeInterval(0)
    , m_nClockMoveInterval(0)
    , m_bIsShowClock(true)
{
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &ScreenSaverController::OnMessage);
}

ScreenSaverController::~ScreenSaverController()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &ScreenSaverController::OnMessage);
}

void ScreenSaverController::ReloadPicFiles()
{
    m_listSysPic.clear();
    res_getResItemsByItemType(m_listSysPic, SCREENSAVER_TYPE, SCREENSAVER_FILE_TYPE_SYSTEM);

#if SCREEN_SAVE_FEATRUE_CUSTOM_MODE
    m_listUserPic.clear();
    res_getResItemsByItemType(m_listUserPic, SCREENSAVER_TYPE, SCREENSAVER_FILE_TYPE_USER);
#endif
}

void ScreenSaverController::LoadDefault()
{
#if SCREEN_SAVE_FEATRUE_CUSTOM_MODE
    m_eSSType = (SCREENSAVER_MODE_CHANGE_TYPE)configParser_GetConfigInt(kszScreenSaverType);
#else
    m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_SYS;
#endif

    m_nBGChangeInterval = configParser_GetConfigInt(kszPicChangeInterval);
    JudgeIntervalValid(m_nBGChangeInterval, SCREENSAVER_BG_CHANGE_MAX_INTERVAL,
                       SCREENSAVER_BG_CHANGE_MIN_INTERVAL, SCREENSAVER_BG_CHANGE_DEFAULT_INTERVAL);

    m_nClockMoveInterval = configParser_GetConfigInt(kszClockMoveInterval);
    JudgeIntervalValid(m_nClockMoveInterval, SCREENSAVER_CLOCK_MOVE_MAX_INTERVAL,
                       SCREENSAVER_CLOCK_MOVE_MIN_INTERVAL, SCREENSAVER_CLOCK_MOVE_DEFAULT_INTERVAL);

    m_bIsShowClock = configParser_GetConfigInt(kszScreensaverDisplayClock);

    qsrand(time(NULL));

    ReloadPicFiles();

#if SCREEN_SAVE_FEATRUE_COLORFUL_CHANGE
    m_listColor.clear();

    m_listColor.push_back(SCREENSAVER_COLOR_WHITE);
    m_listColor.push_back(SCREENSAVER_COLOR_BLACK);
    m_listColor.push_back(SCREENSAVER_COLOR_DARK_GREEN);
    m_listColor.push_back(SCREENSAVER_COLOR_LIGHT_GREEN);
    m_listColor.push_back(SCREENSAVER_COLOR_CYAN);
    m_listColor.push_back(SCREENSAVER_COLOR_MAGENTA);
    m_listColor.push_back(SCREENSAVER_COLOR_GREEN);
#endif

#if SCREEN_SAVE_FEATURE_CLOCK_RECT_FROM_FIX_LIST
    m_listRect.clear();

    m_listRect.push_back(SCREENSAVER_RECT_DEFAULT);
    m_listRect.push_back(SCREENSAVER_RECT_1);
    m_listRect.push_back(SCREENSAVER_RECT_2);
    m_listRect.push_back(SCREENSAVER_RECT_3);
    m_listRect.push_back(SCREENSAVER_RECT_4);
    m_listRect.push_back(SCREENSAVER_RECT_5);
    m_listRect.push_back(SCREENSAVER_RECT_6);
    m_listRect.push_back(SCREENSAVER_RECT_7);
    m_listRect.push_back(SCREENSAVER_RECT_8);
#endif
}

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
    IDLEUI_INFO("ScreenSaverController::GetNextScreenSaverPicPath, nSSType = %d", m_eSSType);

    //如果使用系统默认屏保则返回系统默认图片
    bool bUserSysPic = (SCREENSAVER_MODE_CHANGE_TYPE_SYS == m_eSSType);

#if SCREEN_SAVE_FEATURE_SHOW_PICTURE_BY_RANDOM
    return GetRandomPic(bUserSysPic);
#else
    return GetPictureByOrder(bUserSysPic);
#endif
}

QColor ScreenSaverController::GetNextScreenSaverColor()
{
#if SCREEN_SAVE_FEATRUE_COLORFUL_CHANGE
    //直接返回某个随机颜色
    return GetRandomColor();
#else
    //按设计部要求，只使用黑色
    return SCREENSAVER_COLOR_BLACK;
#endif
}

#if SCREEN_SAVE_FEATURE_CLOCK_RECT_FROM_FIX_LIST
QRect ScreenSaverController::GetNextScreenSaverRect()
{
    //直接返回某个随机位置
    return GetRandomRect();
}
#endif

yl::string ScreenSaverController::GetRandomPic(bool bUseSysPic)
{
    listScreenSaverPic & listPic = bUseSysPic ? m_listSysPic : m_listUserPic;
    int nListSize = listPic.size();

    IDLEUI_INFO("ScreenSaverController::GetRandomPic, bUseSysPic = %d, nListSize = %d", bUseSysPic,
                nListSize);

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

        YLSmartPtr<BaseResItem> & pItem = (iter != listPic.end())
                                          ? *iter
                                          : *(listPic.begin());

        if (pItem.IsEmpty())
        {
            //属于用户到系统的模式
            m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_NO_USER_PIC;
            m_strCurrentPicPath = GetRandomPic(true);
        }
        else
        {
            m_strCurrentPicPath = pItem->resPath;
            IDLEUI_INFO("ScreenSaverController::GetRandomPic, m_strCurrentPicPath = %s",
                        m_strCurrentPicPath.c_str());
        }
    }

    return m_strCurrentPicPath;
}

yl::string ScreenSaverController::GetPictureByOrder(bool bUseSysPic)
{
    yl::string strNextPicture;

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
    listScreenSaverPic & listPic = m_listSysPic;
    int nListSize = listPic.size();

    IDLEUI_INFO("ScreenSaverController::GetSysPicByOrder nListSize = %d", nListSize);

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
    listScreenSaverPic & listPic = m_listUserPic;
    int nListSize = listPic.size();

    yl::string strPicPath;

    IDLEUI_INFO("ScreenSaverController::GetUserPicByOrder nListSize = %d", nListSize);

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

yl::string ScreenSaverController::GetNextPicture(listScreenSaverPic & listPic)
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

    YLSmartPtr<BaseResItem> & pItem = (iter != listPic.end()) ? *iter :
                                      *(listPic.begin());

    if (pItem.IsEmpty())
    {
        return yl::string("");
    }
    else
    {
        return pItem->resPath;
    }
}

#if SCREEN_SAVE_FEATRUE_COLORFUL_CHANGE
QColor ScreenSaverController::GetRandomColor()
{
    //选出一个不重复index
    int nListColorSize = m_listColor.size();
    m_nLastUsedColorIndex = GetNotRepeatIndex(m_nLastUsedColorIndex, nListColorSize);

    //根据index找到对应列表项
    listScreenSaverColor::iterator iter = m_listColor.begin();
    for (int nIndex = 0
                      ; iter != m_listColor.end() && nIndex < nListColorSize && nIndex != m_nLastUsedColorIndex
            ; iter++, ++nIndex)
    {
        //
    }

    return (iter != m_listColor.end()) ? *iter : SCREENSAVER_COLOR_BLACK;
}
#endif

#if SCREEN_SAVE_FEATURE_CLOCK_RECT_FROM_FIX_LIST
QRect ScreenSaverController::GetRandomRect()
{
    //选出一个不重复index
    int nListRectSize = m_listRect.size();
    m_nLastUsedRectIndex = GetNotRepeatIndex(m_nLastUsedRectIndex, nListRectSize);

    //根据index找到对应列表项
    listScreenSaverRect::iterator iter = m_listRect.begin();
    for (int nIndex = 0
                      ; iter != m_listRect.end() && nIndex < nListRectSize && nIndex != m_nLastUsedRectIndex
            ; iter++, ++nIndex)
    {
        //
    }

    return (iter != m_listRect.end())
           ? *iter
           : SCREENSAVER_RECT_DEFAULT;
}
#endif

void ScreenSaverController::JudgeIntervalValid(int & nInterval, const int nMaxInterval,
        const int nMinInterval, const int nDefault)
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
    return (qrand() % nMax);
}

LRESULT ScreenSaverController::OnReceiveMessage(msgObject & refMsg)
{
    if (refMsg.message != CONFIG_MSG_BCAST_CHANGED)
    {
        return FALSE;
    }

    IDLEUI_INFO("ScreenSaverController::OnMessage, refMsg.wParam = %d", refMsg.wParam);

    switch (refMsg.wParam)
    {
    case ST_SETTIME:
    {
        emit ScreenSaverClockFormatChange();
    }
    break;
#if SCREEN_SAVE_FEATRUE_CUSTOM_MODE
#if IF_FEATURE_XMLB_SCREENSAVER
    case ST_XML_SCREEN_SAVER_URL:
    {
        emit ScreenSaverModeOrURLChange();
        SCREENSAVER_MODE_CHANGE_TYPE eSSType = (SCREENSAVER_MODE_CHANGE_TYPE)configParser_GetConfigInt(
                kszScreenSaverType);
        if (eSSType == SCREENSAVER_MODE_CHANGE_TYPE_XMLB)
        {
            //清除xmlScreensaver数据
            xmlbrowser_DeleteSSData();
        }
        emit ScreenSaverClockChange();
    }
    break;
#endif //#if IF_FEATURE_XMLB_SCREENSAVER
    case ST_SCREEN_SAVER_MODE:
    {
        //收到配置变化的消息，从使用系统变为使用自定义或相反
        SCREENSAVER_MODE_CHANGE_TYPE eSSType = (SCREENSAVER_MODE_CHANGE_TYPE)configParser_GetConfigInt(
                kszScreenSaverType);

#if IF_FEATURE_XMLB_SCREENSAVER
        if (SCREENSAVER_MODE_CHANGE_TYPE_XMLB == eSSType
                && configParser_GetConfigString(kszXmlBrowserScreenSaverURL).empty())
        {
            eSSType = SCREENSAVER_MODE_CHANGE_TYPE_SYS;
            configParser_SetConfigInt(kszScreenType, 0, CONFIG_LEVEL_PHONE);
        }
#endif
        IDLEUI_INFO("ScreenSaverController::OnMessage, ST_SCREEN_SAVER_MODE, nSSType = %d", eSSType);

        //如果配置改变，则原有的记录上一次index的内容失效
        if (m_eSSType != eSSType)
        {
#if IF_FEATURE_XMLB_SCREENSAVER
            emit ScreenSaverModeOrURLChange();

            if (SCREENSAVER_MODE_CHANGE_TYPE_SYS == eSSType)
            {
                if (SCREENSAVER_MODE_CHANGE_TYPE_USER == m_eSSType)
                {
                    m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_USER_TO_SYS;
                }
                else
                {
                    m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_XMLB_TO_SYS;
                    //清除xmlScreensaver数据
                    xmlbrowser_DeleteSSData();
                    //重新触发时钟移动定时器，防止定时器stop后不再启动
                    emit signalClockMoveIntervalChange();
                }
                emit ScreenSaverPicChange(true);
                m_nLastUsedPicIndex = -1;
            }
            else if (SCREENSAVER_MODE_CHANGE_TYPE_USER == eSSType)
            {
                if (SCREENSAVER_MODE_CHANGE_TYPE_SYS == m_eSSType)
                {
                    m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_SYS_TO_USER;
                    emit ScreenSaverPicChange(false);
                    m_nLastUsedPicIndex = -1;
                }
                else
                {
                    m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_XMLB_TO_USER;
                    //xml相互切换user不需要变更背景图
                    //清除xmlScreensaver数据
                    xmlbrowser_DeleteSSData();
                    //重新触发时钟移动定时器，防止定时器stop后不再启动
                    emit signalClockMoveIntervalChange();
                }
            }
            else
            {
                if (SCREENSAVER_MODE_CHANGE_TYPE_SYS == m_eSSType)
                {
                    m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_SYS_TO_XMLB;
                    emit ScreenSaverPicChange(false);
                    m_nLastUsedPicIndex = -1;
                }
                else
                {
                    m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_USER_TO_XMLB;
                    //xml切换user不需要变更背景图
                }
            }
#else
            if (SCREENSAVER_MODE_CHANGE_TYPE_SYS == eSSType)
            {
                m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_USER_TO_SYS;
                emit ScreenSaverPicChange(true);
            }
            else
            {
                m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_SYS_TO_USER;
                emit ScreenSaverPicChange(false);
            }

            m_nLastUsedPicIndex = -1;
#endif //#if IF_FEATURE_XMLB_SCREENSAVER

            emit ScreenSaverClockChange();

            m_eSSType = eSSType;
        }
    }
    break;
    case ST_SCREEN_SAVER_PIC:
    {
        ReloadPicFiles();

        if (SCREENSAVER_MODE_CHANGE_TYPE_SYS == m_eSSType)
        {
            break;
        }

        if (0 >= m_listUserPic.size())
        {
            IDLEUI_INFO("ScreenSaverController::OnMessage, ST_SCREEN_SAVER_PIC, m_listUserPic is NULL");

            //如果获取不到自定义图片则使用系统默认图片
            m_nLastUsedPicIndex = -1;
            //当前模式状态为从用户模式变为缺少用户图片模式
            m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_NO_USER_PIC;
            emit ScreenSaverPicChange(true);
            emit ScreenSaverClockChange();
        }
        //如果获取到图片列表就查找正在使用的图是否还存在
        else
        {
            listScreenSaverPic::iterator iter = m_listUserPic.begin();
                for (/* */; iter != m_listUserPic.end(); iter++)
            {
                    YLSmartPtr<BaseResItem> & pItem = *iter;
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
                IDLEUI_INFO("ScreenSaverController::OnMessage, ST_SCREEN_SAVER_PIC, Current Pic Change");

                m_nLastUsedPicIndex = -1;
#if IF_FEATURE_XMLB_SCREENSAVER
                SCREENSAVER_MODE_CHANGE_TYPE eSSType = (SCREENSAVER_MODE_CHANGE_TYPE)configParser_GetConfigInt(
                        kszScreenSaverType);
                if (eSSType == SCREENSAVER_MODE_CHANGE_TYPE_XMLB)
                {
                    m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_XMLB;
                }
                else if (eSSType == SCREENSAVER_MODE_CHANGE_TYPE_USER)
#endif
                {
                    m_eSSType = SCREENSAVER_MODE_CHANGE_TYPE_USER;
                }
                emit ScreenSaverPicChange(false);
                emit ScreenSaverClockChange();
            }
            else
            {
                //如果正在使用的图片没有被删除则不需要变化
                IDLEUI_INFO("ScreenSaverController::OnMessage, ST_SCREEN_SAVER_PIC, Current Pic Not Change");
            }
        }
    }
    break;
#endif
    case ST_SCREEN_SAVER_BG_CHANGE_TIME:
    {
        m_nBGChangeInterval = configParser_GetConfigInt(kszPicChangeInterval);
        JudgeIntervalValid(m_nBGChangeInterval, SCREENSAVER_BG_CHANGE_MAX_INTERVAL,
                           SCREENSAVER_BG_CHANGE_MIN_INTERVAL, SCREENSAVER_BG_CHANGE_DEFAULT_INTERVAL);
        emit signalPicIntervalChange();
    }
    break;
    case ST_SCREEN_SAVER_CLOCK_MOVE_TIME:
    {
        m_nClockMoveInterval = configParser_GetConfigInt(kszClockMoveInterval);
        JudgeIntervalValid(m_nClockMoveInterval, SCREENSAVER_CLOCK_MOVE_MAX_INTERVAL,
                           SCREENSAVER_CLOCK_MOVE_MIN_INTERVAL, SCREENSAVER_CLOCK_MOVE_DEFAULT_INTERVAL);
        emit signalClockMoveIntervalChange();
    }
    break;
    case ST_SCREENSAVER_SHOW_CLOCK:
    {
        m_bIsShowClock = configParser_GetConfigInt(kszScreensaverDisplayClock);
        emit ScreenSaverClockChange();
    }
    break;
    default:
    {
        return FALSE;
    }
    }

    return TRUE;
}

LRESULT ScreenSaverController::OnMessage(msgObject & refMsg)
{
    ScreenSaverController * pScreenSaverControler = g_pScreenSaverController;

    if (NULL == pScreenSaverControler)
    {
        return FALSE;
    }

    return pScreenSaverControler->OnReceiveMessage(refMsg);
}
