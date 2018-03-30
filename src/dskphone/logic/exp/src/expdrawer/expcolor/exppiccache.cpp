#include "exppiccache.h"
#include "expdefine.h"
#include "t4xpicpath.h"
#include "common/xwindow/xWindowManager.h"

namespace
{
#define DSSKEY_PATH         "images/dss/"
#define DSSBG_PATH          "images/dssbg/"
#define ICON_PATH               "images/icon/"

enum TransparentType
{
    TRANSPARENT_TYPE_LOW_TRANS = 1,
    TRANSPARENT_TYPE_TWENTY_PERCENT,
    TRANSPARENT_TYPE_FORTY_PERCENT,
    TRANSPARENT_TYPE_SIXTY_PERCENT,
    TRANSPARENT_TYPE_EIGHTY_PERCENT,
    TRANSPARENT_TYPE_HEIGHT_TRANS,
};

}

EXPPicCache::EXPPicCache()
    : m_strBackGround("")
    , m_pButtonLeftBG(NULL)
    , m_pButtonRightBG(NULL)
    , m_nTransparentLevel(0)
    , m_imgBackGround(EXP_SCREEN_WIDTH, EXP_SCREEN_HEIGHT, MYPAINTER_FORMAT_16)
    , m_painterBackGround(m_imgBackGround)
{

}


EXPPicCache::~EXPPicCache()
{

}

void EXPPicCache::Init()
{
#ifndef _XWIN
    LoadIconImage();
#endif // _XWIN

    m_pButtonLeftBG = new CMyImageInfo();
    m_pButtonRightBG = new CMyImageInfo();

    m_imgBtnDown.LoadImage(PIC_DSSKEY_BTN_DOWN);
    m_imgBtnHeight.LoadImage(PIC_DSSKEY_BTN_HEIGHT);
    m_imgBtnUse.LoadImage(PIC_DSSKEY_BTN_USE);
}

void EXPPicCache::Uninit()
{

}

bool EXPPicCache::SetBackGround(const yl::string & strBackGround, int nTransparentLevel)
{
    if (NULL == m_pButtonLeftBG
            || NULL == m_pButtonRightBG
            || nTransparentLevel < TRANSPARENT_TYPE_LOW_TRANS
            || nTransparentLevel > TRANSPARENT_TYPE_HEIGHT_TRANS)
    {
        return false;
    }

    //图片背景和按钮背景，只要有一方发生变化，就需要重绘缓存，否则直接使用缓存
    if (m_strBackGround == strBackGround && m_nTransparentLevel == nTransparentLevel)
    {
        return true;
    }

    EXP_INFO("[Exp-Info] EXPPicCache::SetBackGround Pic[%s]\n", strBackGround.c_str());

    CMyImage imgBackGround(EXP_SCREEN_WIDTH, EXP_SCREEN_HEIGHT, MYPAINTER_FORMAT_16);

    // 加载指定路径壁纸
    if (imgBackGround.LoadImage(strBackGround))
    {
        m_strBackGround = strBackGround;
    }
    else
    {
        // 原壁纸加载失败后加载默认壁纸
        m_strBackGround = PIC_DEFAULT_EXP_BACKGROUND;
        imgBackGround.LoadImage(m_strBackGround);
    }

    yl::string strLeft = "";
    yl::string strRight = "";
    if (m_nTransparentLevel != nTransparentLevel)
    {
        m_nTransparentLevel = nTransparentLevel;

        strLeft = MapDssBG(nTransparentLevel, true);
        strRight = MapDssBG(nTransparentLevel, false);
    }

    if (!strLeft.empty())
    {
        m_pButtonLeftBG->LoadImageInfo(strLeft);
    }

    if (!strRight.empty())
    {
        m_pButtonRightBG->LoadImageInfo(strRight);
    }

    if (imgBackGround.GetIsNull()
            || m_pButtonLeftBG->IsImageNull()
            || m_pButtonRightBG->IsImageNull())
    {
        return false;
    }

    //先将加载进来的背景图整体绘制到缓存上
    m_painterBackGround.DrawColor(CMyRect(0, 0, EXP_SCREEN_WIDTH, EXP_SCREEN_HEIGHT), 0, 0, 0);
    m_painterBackGround.DrawImage(CMyRect(0, 0, EXP_SCREEN_WIDTH, EXP_SCREEN_HEIGHT)
                                  , imgBackGround);

    //绘制左边按钮背景
    m_pButtonLeftBG->DrawPointImage(m_painterBackGround
                                    , CMyRect(EXP_BTN_LEFTBG_LEFT, EXP_BTN_BG_TOP_MARGIN, EXP_BTN_BG_WIDTH, EXP_BTN_BG_HEIGHT));

    //绘制右边按钮背景
    m_pButtonRightBG->DrawPointImage(m_painterBackGround
                                     , CMyRect(EXP_BTN_RIGHTBG_LEFT, EXP_BTN_BG_TOP_MARGIN, EXP_BTN_BG_WIDTH, EXP_BTN_BG_HEIGHT));

    //设置画笔颜色
    m_painterBackGround.SetPenColor(EXP_CUTLINE_COLOR_WHITE
                                    , EXP_CUTLINE_COLOR_WHITE
                                    , EXP_CUTLINE_COLOR_WHITE
                                    , EXP_CUTLINE_COLOR_ALPHA);

    //绘制左边分割线
    for (int i = 0; i < EXP_CUT_LINE_COUNT; ++i)
    {
        m_painterBackGround.DrawLine(EXP_BTN_LEFTBG_CUTLINE_LEFT, s_DsskeyCutLineTop[i]
                                     , EXP_BTN_LEFTBG_CUTLINE_LEFT + EXP_CUTLINE_WIDTH, s_DsskeyCutLineTop[i]);
    }

    //绘制右边分割线
    for (int i = 0; i < EXP_CUT_LINE_COUNT; ++i)
    {
        m_painterBackGround.DrawLine(EXP_BTN_RIGHTBG_CUTLINE_LEFT, s_DsskeyCutLineTop[i]
                                     , EXP_BTN_RIGHTBG_CUTLINE_LEFT + EXP_CUTLINE_WIDTH, s_DsskeyCutLineTop[i]);
    }

    return true;
}


void EXPPicCache::ReloadCanvas(CMyPainter & painterCanvas, CMyRect rectCopyPart)
{
    if (rectCopyPart.GetValid())
    {
        painterCanvas.DrawImage(rectCopyPart
                                , m_imgBackGround
                                , rectCopyPart);
    }
    else
    {
        painterCanvas.DrawImage(CMyRect(0
                                        , 0
                                        , m_imgBackGround.GetWidth()
                                        , m_imgBackGround.GetHeight())
                                , m_imgBackGround);
    }
}

void EXPPicCache::PaintDownButton(CMyPainter & painterCanvas, CMyRect rectButton)
{
    painterCanvas.DrawImage(rectButton, m_imgBtnDown);
}

void EXPPicCache::PaintUsingButton(CMyPainter & painterCanvas, CMyRect rectButton)
{
    painterCanvas.DrawImage(rectButton, m_imgBtnUse);
}

void EXPPicCache::PaintHeightLightButton(CMyPainter & painterCanvas, CMyRect rectButton)
{
    painterCanvas.DrawImage(rectButton, m_imgBtnHeight);
}

CMyImage & EXPPicCache::GetIconImage(int nIconType)
{
#ifdef _XWIN
    static CMyImage pixMap;
    g_WorkStation.GetImageByPath(MapIconString(nIconType), pixMap);
    return pixMap;
#else
    MapImageCacheIter iterImage = m_imgIconCache.find(nIconType);
    if (iterImage != m_imgIconCache.end())
    {
        return iterImage->second;
    }

    static CMyImage nullImage;
    return nullImage;
#endif // _XWIN
}

CMyImage & EXPPicCache::GetStatusImage(int nStatus, bool bNewIcon /*= false*/)
{
#ifdef _XWIN
    static CMyImage pixMap;
    g_WorkStation.GetImageByPath(MapStatusString(nStatus, bNewIcon), pixMap);
    return pixMap;
#else
    MapImageCacheIter iterImage = bNewIcon ? m_imgNewStatusCache.find(nStatus) : m_imgStatusCache.find(
                                      nStatus);
    MapImageCacheIter iterImageEnd = bNewIcon ? m_imgNewStatusCache.end() : m_imgStatusCache.end();
    if (iterImage != iterImageEnd)
    {
        return iterImage->second;
    }

    static CMyImage nullImage;
    return nullImage;
#endif // _XWIN
}

void EXPPicCache::LoadIconImage()
{
    m_imgIconCache.clear();
    for (int i = 0; i < DK_ICON_CUSTOM; i++)
    {
        m_imgIconCache.insert(i, CMyImage(MapIconString(i)));
    }

    m_imgStatusCache.clear();
    for (int i = 0; i < PRESENCE_STATUS_MAX; i++)
    {
        m_imgStatusCache.insert(i, CMyImage(MapStatusString(i)));
    }

    m_imgNewStatusCache.clear();
    for (int i = 0; i < PRESENCE_STATUS_MAX; i++)
    {
        m_imgNewStatusCache.insert(i, CMyImage(MapStatusString(i, true)));
    }

}

yl::string EXPPicCache::MapIconString(int nIconType)
{
#ifdef _XWIN
    yl::string strPic = DSSKEY_PATH;
#else
    yl::string strPic = CMyImageInfo::GetPicRootPath();
    strPic += DSSKEY_PATH;
#endif // _XWIN

    Dsskey_Icon_Type eType = (Dsskey_Icon_Type)nIconType;
    switch (eType)
    {
    case DK_ICON_UnRegistered:
        strPic += PIC_DSS_LINE_UNREGISTERED;
        break;
    case DK_ICON_Registering:
        strPic += PIC_DSS_LINE_REGISTERING;
        break;
    case DK_ICON_Registered:
        strPic += PIC_DSS_LINE_REGISTERED;
        break;
    case DK_ICON_Using:
        strPic += PIC_DSS_LINE_USING;
        break;

    case DK_ICON_Speeddial:
        strPic += PIC_DSS_SPEED_DIAL;
        break;
    case DK_ICON_Remote_Offline:
        strPic += PIC_DSS_INTERCOM_UNKNOWN;
        break;
    case DK_ICON_Remote_Talking:
        strPic += PIC_DSS_INTERCOM_TALKING;
        break;
    case DK_ICON_Remote_Online:
        strPic += PIC_DSS_INTERCOM;
        break;
    case DK_ICON_Remote_Ringing:
        strPic += PIC_DSS_INTERCOM_RINGING;
        break;

    case DK_ICON_DND:
        strPic += PIC_DSS_DND;
        break;
    case DK_ICON_Forward:
        strPic += PIC_DSS_FORWARD;
        break;
    case DK_ICON_HOLD:
        strPic += PIC_DSS_HOLD;
        break;
    case DK_ICON_SMS:
        strPic += PIC_DSS_SMS;
        break;

    case DK_ICON_BLF_PARK:
        strPic += PIC_DSS_BLF_PARK;
        break;
    case DK_ICON_BLF_CALLOUT:
        strPic += PIC_DSS_BLF_CALLOUT;
        break;
    case DK_ICON_BLF_IDLE:
        strPic += PIC_DSS_BLF_IDLE;
        break;
    case DK_ICON_BLF_UNKNOWN:
        strPic += PIC_DSS_BLF_UNKNOWN;
        break;
    case DK_ICON_BLF_TALK:
        strPic += PIC_DSS_BLF_TALKING;
        break;
    case DK_ICON_BLF_RINGING:
        strPic += PIC_DSS_BLF_RING;
        break;
    case DK_ICON_BLF_HOLD:
        strPic += PIC_DSS_BLF_HOLD;
        break;

    case DK_ICON_BLA_PARK:
        strPic += PIC_DSS_BLA_PARK;
        break;
    case DK_ICON_BLA_UNKNOWN:
        strPic += PIC_DSS_BLA_UNKNOWN;
        break;
    case DK_ICON_BLA_SEIZED:
        strPic += PIC_DSS_BLA_SEIZED;
        break;
    case DK_ICON_BLA_PROGRESSING:
        strPic += PIC_DSS_BLA_PROGRESSING;
        break;
    case DK_ICON_BLA_ACTIVE:
        strPic += PIC_DSS_BLA_ACTIVE;
        break;
    case DK_ICON_BLA_HELD:
        strPic += PIC_DSS_BLA_HELD;
        break;
    case DK_ICON_BLA_HELD_PRIVATE:
        strPic += PIC_DSS_BLA_HELD_PRIVATE;
        break;
    case DK_ICON_BLA_BRIDGE_ACTIVE:
        strPic += PIC_DSS_BLA_BRIDGE_ACTIVE;
        break;
    case DK_ICON_BLA_BRIDGE_HELD:
        strPic += PIC_DSS_BLA_BRIDGE_HELD;
        break;
    case DK_ICON_BLA_IDLE:
        strPic += PIC_DSS_BLA_IDLE;
        break;
    case DK_ICON_BLA_ALERTING:
        strPic += PIC_DSS_BLA_ALERTING;
        break;

    case DK_ICON_INTERCOM:
        strPic += PIC_DSS_INTERCOM;
        break;
    case DK_ICON_INTERCOM_TALKING:
        strPic += PIC_DSS_INTERCOM_TALKING;
        break;
    case DK_ICON_INTERCOM_HOLD:
        strPic += PIC_DSS_INTERCOM_HOLD;
        break;
    case DK_ICON_INTERCOM_RINGING:
        strPic += PIC_DSS_INTERCOM_RINGING;
        break;

    case DK_ICON_LINE_FAIL:
        strPic += PIC_DSS_LINE_FAIL;
        break;
    case DK_ICON_LINE_FORWARD:
        strPic += PIC_DSS_LINE_FORWARD;
        break;
    case DK_ICON_LINE_DND:
        strPic += PIC_DSS_LINE_DND;
        break;

    case DK_ICON_VOICE_MAIL:
        strPic += PIC_DSS_VOICE_MAIL;
        break;

    case DK_ICON_PICK_UP:
        strPic += PIC_DSS_PICKUP;
        break;
    case DK_ICON_GROUP_PICK_UP:
        strPic += PIC_DSS_GROUPPICKUP;
        break;
    case DK_ICON_CALLPARK:
        strPic += PIC_DSS_CALLPARK;
        break;
    case DK_ICON_CALLPARK_FAIL:
        strPic += PIC_DSS_CALLPARK_FAIL;
        break;
    case DK_ICON_CALLPARK_RINGING:
        strPic += PIC_DSS_CALLPARK_RINGING;
        break;
    case DK_ICON_CALLPARK_AUTO_TALK:
        strPic += PIC_DSS_CALLPARK_AUTO_TALK;
        break;
    case DK_ICON_CALLPARK_MUNUAL_TALK:
        strPic += PIC_DSS_CALLPARK_MUNUAL_TALK;
        break;

    case DK_ICON_DTMF:
        strPic += PIC_DSS_DTMF;
        break;
    case DK_ICON_PREFIX_ICON:
        strPic += PIC_DSS_PREFIX_ICON;
        break;

    case DK_ICON_LOCAL_GROUP:
        strPic += PIC_DSS_LOCALGROUP;
        break;
    case DK_ICON_DIRECTORY:
        strPic += PIC_DSS_DIRECTORY;
        break;
    case DK_ICON_XMLBROWSER:
        strPic += PIC_DSS_XML_BROWSER;
        break;

    case DK_ICON_LDAP:
        strPic += PIC_DSS_LDAP;
        break;
    case DK_ICON_FAVORITES:
        strPic += PIC_DSS_FAVORITES;
        break;

    case DK_ICON_CONFERENCE:
        strPic += PIC_DSS_CONFERENCE;
        break;
    case DK_ICON_TRANSFER:
        strPic += PIC_DSS_TRANSFER;
        break;
    case DK_ICON_REDIAL:
        strPic += PIC_DSS_REDIAL;
        break;

    case DK_ICON_TALK:
        strPic += PIC_DSS_TALK;
        break;
    case DK_ICON_CALLRETURN:
        strPic += PIC_DSS_CALLRETURN;
        break;

    case DK_ICON_RECORD:
        strPic += PIC_DSS_RECORD;
        break;
    case DK_ICON_RECORDING:
        strPic += PIC_DSS_RECORDING;
        break;

    case DK_ICON_PAGING:
        strPic += PIC_DSS_PAGING;
        break;
    case DK_ICON_GROUP_LISTENING:
        strPic += PIC_DSS_GROUP_LISTENING;
        break;

    case DK_ICON_COLOR_ACD_AVAILABLE:
        strPic += PIC_DSS_ACD_AVAILABLE;
        break;
    case DK_ICON_COLOR_ACD_UNAVAILABLE:
        strPic += PIC_DSS_ACD_UNAVAILABLE;
        break;
    case DK_ICON_COLOR_ACD_WRAPUP:
        strPic += PIC_DSS_ACD_WARP_UP;
        break;
    case DK_ICON_COLOR_ACD_TRACE:
        strPic += PIC_DSS_ACD_TRACE;
        break;
    case DK_ICON_ACD_LOGOUT:
        strPic += PIC_DSS_ACD_LOGOUT;
        break;
    case DK_ICON_ACD_DEFAULT:
        strPic += PIC_DSS_ACD;
        break;

    case DK_ICON_ZERO_TOUCH:
        strPic += PIC_DSS_ZERO_TOUCH;
        break;
    case DK_ICON_MEET_ME:
        strPic += PIC_DSS_MEET_ME;
        break;
    case DK_ICON_HOTELING_LOGININ:
        strPic += PIC_DSS_HOTELING_LOGININ;
        break;
    case DK_ICON_HOTELING_LOGINOUT:
        strPic += PIC_DSS_HOTELING_LOGINOUT;
        break;

    case DK_ICON_DISPCODE:
        strPic += PIC_DSS_DISPCODE;
        break;
    case DK_ICON_ESCALATE:
        strPic += PIC_DSS_ESCALATE;
        break;

    case DK_ICON_RETRIEVE_PARK:
        strPic += PIC_DSS_RETRIEVE_PARK;
        break;
    case DK_ICON_HOTDESKING:
        strPic += PIC_DSS_HOTDESKING;
        break;
    case DK_ICON_BUDDIES:
        strPic += PIC_DSS_BUDDIES;
        break;
    case DK_ICON_STATUS:
        strPic += PIC_DSS_STATUS;
        break;

    case DK_ICON_EDK:
        strPic += PIC_DSS_EDK;
        break;
    case DK_ICON_KEYPAD_LOCK:
        strPic += PIC_DSS_KEYPAD_LOCK;
        break;
    case DK_ICON_GET_BALANCE:
        strPic += PIC_DSS_GET_BALANCE;
        break;

    case DK_ICON_URL_GREEN:
        strPic += PIC_DSS_URL_GREEN;
        break;
    case DK_ICON_URL_RED:
        strPic += PIC_DSS_URL_RED;
        break;
    case DK_ICON_URL:
        strPic += PIC_DSS_URL;
        break;

    case DK_ICON_MOBILE_BLUE_GREEN:
        strPic += PIC_DSS_MOBILE_BLUE_GREEN;
        break;
    case DK_ICON_MOBILE_BLUE_CONNETING:
        strPic += PIC_DSS_MOBILE_BLUE_CONNETING;
        break;
    case DK_ICON_MOBILE_BLUE_RED:
        strPic += PIC_DSS_MOBILE_BLUE_RED;
        break;
    case DK_ICON_METASWITCH_MESSAGE_LIST:
        strPic += PIC_DSS_MTSW_MESSAGELIST;
        break;
    case DK_ICON_METASWITCH_LOGOUT:
        strPic += PIC_DSS_MTSW_LOGOUT;
        break;

    case DK_ICON_EDITING:
        strPic += PIC_DSS_EDITING;
        break;

    default:
        strPic = "";
        break;
    }

    return strPic;
}

yl::string EXPPicCache::MapStatusString(int nStatus, bool bNewIcon /*= false*/)
{

#ifdef _XWIN
    yl::string strPic = ICON_PATH;
#else
    yl::string strPic = CMyImageInfo::GetPicRootPath();
    strPic += ICON_PATH;
#endif // _XWIN


    PRESENCE_STATUS eStatus = (PRESENCE_STATUS)nStatus;
    switch (eStatus)
    {
    case PRESENCE_STATUS_ONLINE:
        strPic += bNewIcon ?  PIC_ICON_PRESENCE_NEW_ONLINE : PIC_ICON_PRESENCE_ONLINE;
        break;
    case PRESENCE_STATUS_CHATTY:
        strPic += PIC_ICON_PRESENCE_CHATTY;
        break;
    case PRESENCE_STATUS_AWAY:
        strPic += bNewIcon ? PIC_ICON_PRESENCE_NEW_AWAY : PIC_ICON_PRESENCE_AWAY;
        break;
    case PRESENCE_STATUS_EXTENDEDAWAY:
        strPic += PIC_ICON_PRESENCE_EXTENDEDAWAY;
        break;
    case PRESENCE_STATUS_BUSY:
        strPic += bNewIcon ? PIC_ICON_PRESENCE_NEW_BUSY : PIC_ICON_PRESENCE_BUSY;
        break;
    case PRESENCE_STATUS_OFFLINE:
        strPic += bNewIcon ? PIC_ICON_PRESENCE_NEW_OFFLINE : PIC_ICON_PRESENCE_OFFLINE;
        break;
    case PRESENCE_STATUS_UNKNOWN:
        strPic += bNewIcon ? PIC_ICON_PRESENCE_NEW_UNKNOWN : PIC_ICON_PRESENCE_UNKNOWN;
        break;

    default:
        strPic = "";
        break;
    }

    return strPic;
}

yl::string EXPPicCache::MapDssBG(int nTransparentLevel, bool bLeft)
{
    yl::string strPic = CMyImageInfo::GetPicRootPath();

    strPic += DSSBG_PATH;

    if (bLeft)
    {
        switch (nTransparentLevel)
        {
        case TRANSPARENT_TYPE_LOW_TRANS:
            strPic += PIC_DSSBG_FRAME_BG_LEFT_LOW_TRANS;
            break;
        case TRANSPARENT_TYPE_TWENTY_PERCENT:
            strPic += PIC_DSSBG_FRAME_BG_LEFT_20_TRANS;
            break;
        case TRANSPARENT_TYPE_FORTY_PERCENT:
            strPic += PIC_DSSBG_FRAME_BG_LEFT_40_TRANS;
            break;
        case TRANSPARENT_TYPE_SIXTY_PERCENT:
            strPic += PIC_DSSBG_FRAME_BG_LEFT_60_TRANS;
            break;
        case TRANSPARENT_TYPE_EIGHTY_PERCENT:
            strPic += PIC_DSSBG_FRAME_BG_LEFT_80_TRANS;
            break;
        case TRANSPARENT_TYPE_HEIGHT_TRANS:
            strPic += PIC_DSSBG_FRAME_BG_LEFT_HEIGHT_TRANS;
            break;

        default:
            strPic += PIC_DSSBG_FRAME_BG_LEFT_HEIGHT_TRANS;
            break;
        }
    }
    else
    {
        switch (nTransparentLevel)
        {
        case TRANSPARENT_TYPE_LOW_TRANS:
            strPic += PIC_DSSBG_FRAME_BG_RIGHT_LOW_TRANS;
            break;
        case TRANSPARENT_TYPE_TWENTY_PERCENT:
            strPic += PIC_DSSBG_FRAME_BG_RIGHT_20_TRANS;
            break;
        case TRANSPARENT_TYPE_FORTY_PERCENT:
            strPic += PIC_DSSBG_FRAME_BG_RIGHT_40_TRANS;
            break;
        case TRANSPARENT_TYPE_SIXTY_PERCENT:
            strPic += PIC_DSSBG_FRAME_BG_RIGHT_60_TRANS;
            break;
        case TRANSPARENT_TYPE_EIGHTY_PERCENT:
            strPic += PIC_DSSBG_FRAME_BG_RIGHT_80_TRANS;
            break;
        case TRANSPARENT_TYPE_HEIGHT_TRANS:
            strPic += PIC_DSSBG_FRAME_BG_RIGHT_HEIGHT_TRANS;
            break;

        default:
            strPic += PIC_DSSBG_FRAME_BG_RIGHT_HEIGHT_TRANS;
            break;
        }
    }

    return strPic;
}
