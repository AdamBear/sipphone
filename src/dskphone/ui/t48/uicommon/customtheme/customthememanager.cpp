#include "customthememanager.h"

#include "resource/modresource.h"
#include "dsklog/log.h"

#include "commonunits/t2xpath.h"

#include "propertyiddefine.h"

namespace
{
#define CUSTOMTHEME_FILE_NAME "customtheme.xml"

#define NODE_CUSTOMTHEME "customtheme"
#define NODE_CUSTOMFONT  "font"
#define NODE_CUSTOM_ITEM "item"

#define FONT_FONT_ID "objectid"
#define FONT_ATTRIBUTE_FONTSIZE "fontpoints"
#define FONT_ATTRIBUTE_NORMAL_COLOR "color"
#define FONT_ATTRIBUTE_ACTIVE_COLOR "activecolor"
#define FONT_ATTRIBUTE_SHOW_SHADOW "showshadow"

#define INVALID_FONT_OBJECT_ID -1

#define DEFAULT_FONT_SIZE 18

#define PREPIX_OF_COLOR "#"
}

CCustomThemeManager * CCustomThemeManager::CreateInstance()
{
    static CCustomThemeManager * pCustomThemeManager = NULL;

    if (NULL == pCustomThemeManager)
    {
        pCustomThemeManager = new CCustomThemeManager();
    }

    return pCustomThemeManager;
}

CCustomThemeManager::CCustomThemeManager()
{
}

CCustomThemeManager::~CCustomThemeManager()
{
}

void CCustomThemeManager::Init()
{
    LoadCustomThemeMap();
}

void CCustomThemeManager::LoadCustomThemeMap()
{
    yl::string strFullXmlPath = res_getPathByName(RESOURCE_PATH_CUSTOMTHEME);
    strFullXmlPath += CUSTOMTHEME_FILE_NAME;

    if (!pathFileExist(strFullXmlPath.c_str()))
    {
        UIKERNEL_ERR("File[%s] is not exist.", strFullXmlPath.c_str());
        return;
    }

    xml_document doc;
    if (!doc.load_file(strFullXmlPath.c_str()))
    {
        UIKERNEL_ERR("xmldocument Load File[%s] failed.", strFullXmlPath.c_str());
        return;
    }

    xml_node nodeCustomTheme = doc.child(NODE_CUSTOMTHEME);

    if (nodeCustomTheme.empty())
    {
        UIKERNEL_ERR("xmldocument File[%s] format error.", strFullXmlPath.c_str());
        return;
    }

    // 加载自定义文本信息
    xml_node nodeFont = nodeCustomTheme.child(NODE_CUSTOMFONT);
    ReloadCustomFontData(nodeFont);
}

void CCustomThemeManager::ReloadCustomFontData(const xml_node & nodeFont)
{
    if (nodeFont.empty())
    {
        UIKERNEL_ERR("xmldocument File font format error.");
        return;
    }

    m_mapCustomThemeData.clear();

    yl::string strFontId;
    yl::string strNormanlColor;
    yl::string strActiveColor;
    CustomFontData stFontData;

    xml_node nodeItem = nodeFont.child(NODE_CUSTOM_ITEM);

    while (!nodeItem.empty())
    {
        stFontData.Reset();

        strFontId = nodeItem.attribute(FONT_FONT_ID).value();

        int nObjectId = GetCustomObjectId(strFontId);

        if (INVALID_FONT_OBJECT_ID == nObjectId)
        {
            nodeItem = nodeItem.next_sibling();
            continue;
        }

        // 解析配置大小
        stFontData.m_nFontSize = atoi(nodeItem.attribute(FONT_ATTRIBUTE_FONTSIZE).value());

        if (stFontData.m_nFontSize <= 0)
        {
            stFontData.m_nFontSize = DEFAULT_FONT_SIZE;
        }

        // 解析字体颜色
        strNormanlColor = nodeItem.attribute(FONT_ATTRIBUTE_NORMAL_COLOR).value();
        GetColorByString(strNormanlColor, stFontData.m_colorNormal);

        // 解析使用态字体颜色
        strActiveColor = nodeItem.attribute(FONT_ATTRIBUTE_ACTIVE_COLOR).value();
        GetColorByString(strActiveColor, stFontData.m_colorActive);

        // 解析是否支持阴影
        stFontData.m_bshowShadow = atoi(nodeItem.attribute(FONT_ATTRIBUTE_SHOW_SHADOW).value()) == 1;

        InsertCustomFontData(nObjectId, stFontData);

        nodeItem = nodeItem.next_sibling();
    }
}

int CCustomThemeManager::GetCustomObjectId(yl::string & strId)
{
    strId.trim_both();

    if (kszFontIdCommonText == strId)
    {
        return CUSTOM_FONT_ID_COMMON_TEXT;
    }
    else if (kszFontIdCommonForgroundText == strId)
    {
        return CUSTOM_FONT_ID_COMMON_FORGROUND_TEXT;
    }
    else if (kszFontIdCommonButton == strId)
    {
        return CUSTOM_FONT_ID_COMMON_BUTTON;
    }
    else if (kszFontIdTitleBarNormal == strId)
    {
        return CUSTOM_FONT_ID_TITLEBAR_NORMAL;
    }
    else if (kszFontIdTitleBarAccount == strId)
    {
        return CUSTOM_FONT_ID_TITLEBAR_ACCOUNT;
    }
    else if (kszFontIdTitleBarTime == strId)
    {
        return CUSTOM_FONT_ID_TITLEBAR_TIME;
    }
    else if (kszFontIdTitleBarTitle == strId)
    {
        return CUSTOM_FONT_ID_TITLEBAR_TITLE;
    }
    else if (kszFontIdTitleBarHint == strId)
    {
        return CUSTOM_FONT_ID_TITLEBAR_HINT;
    }
    else if (kszFontIdTitleBarFocusTitle == strId)
    {
        return CUSTOM_FONT_ID_TITLEBAR_FOCUS_TITLE;
    }
    else if (kszFontIdLineKeyNormal == strId)
    {
        return CUSTOM_FONT_ID_LINEKEY_NORMAL;
    }
    else if (kszFontIdLineKeyHighLight == strId)
    {
        return CUSTOM_FONT_ID_LINEKEY_HIGHLIGHT;
    }
    else if (kszFontIdLineKeyUsing == strId)
    {
        return CUSTOM_FONT_ID_LINEKEY_USING;
    }
    else if (kszFontIdLineKeyDisable == strId)
    {
        return CUSTOM_FONT_ID_LINEKEY_DISABLE;
    }
    else if (kszFontIdSoftKeyNormal == strId)
    {
        return CUSTOM_FONT_ID_SOFAKEY_NORMAL;
    }
    else if (kszFontIdSoftKeyIdle == strId)
    {
        return CUSTOM_FONT_ID_SOFAKEY_IDLE;
    }
    else if (kszFontIdIdleLabel == strId)
    {
        return CUSTOM_FONT_ID_IDLE_LABEL;
    }
    else if (kszFontIdIdleAcdStatus == strId)
    {
        return CUSTOM_FONT_ID_IDLE_ACD_STATUS;
    }
    else if (kszFontIdIdleDND == strId)
    {
        return CUSTOM_FONT_ID_IDLE_DND;
    }
    else if (kszFontIdIdleClockDate == strId)
    {
        return CUSTOM_FONT_ID_IDLE_CLOCK_DATE;
    }
    else if (kszFontIdIdleClockAmPm == strId)
    {
        return CUSTOM_FONT_ID_IDLE_CLOCK_AMPM;
    }
    else if (kszFontIdIdleClockTime == strId)
    {
        return CUSTOM_FONT_ID_IDLE_CLOCK_TIME;
    }
    else if (kszFontIdMenuNormal == strId)
    {
        return CUSTOM_FONT_ID_MENU_NORMAL;
    }
    else if (kszFontIdDirectoryNormal == strId)
    {
        return CUSTOM_FONT_ID_DIRECTORY_NORMAL;
    }
    else if (kszFontIdDirectoryFocus == strId)
    {
        return CUSTOM_FONT_ID_DIRECTORY_FOCUS;
    }
    else if (kszFontIdSettingNormal == strId)
    {
        return CUSTOM_FONT_ID_SETTING_NORMAL;
    }
    else if (kszFontIdSettingFocus == strId)
    {
        return CUSTOM_FONT_ID_SETTING_FOCUS;
    }
    else if (kszFontIdLineSelectNormal == strId)
    {
        return CUSTOM_FONT_ID_LINESELECT_NORMAL;
    }
    else if (kszFontIdLineSelectFocus == strId)
    {
        return CUSTOM_FONT_ID_LINESELECT_FOCUS;
    }
    else if (kszFontIdTalkNormal == strId)
    {
        return CUSTOM_FONT_ID_TALK_NORMAL;
    }
    else if (kszFontIdTalkNormalTimepiece == strId)
    {
        return CUSTOM_FONT_ID_TALK_NORMAL_TIMEPIECE;
    }
    else if (kszFontIdTalkNormalIncommingTip == strId)
    {
        return CUSTOM_FONT_ID_TALK_NORMAL_INCOMMING_TIP;
    }
    else if (kszFontIdTalkNormalHintTip == strId)
    {
        return CUSTOM_FONT_ID_TALK_NORMAL_HINTTIP;
    }
    else if (kszFontIdTalkNormalHoldTipWithoutPhoto == strId)
    {
        return CUSTOM_FONT_ID_TALK_NORMAL_HOLDTIP_WITHOUTPHOTO;
    }
    else if (kszFontIdTalkNormalTransfTipWithoutPhoto == strId)
    {
        return CUSTOM_FONT_ID_TALK_NORMAL_TRANSFTIP_WITHOUTPHOTO;
    }
    else if (kszFontIdTalkNormalHoldTip == strId)
    {
        return CUSTOM_FONT_ID_TALK_NORMAL_HOLDTIP;
    }
    else if (kszFontIdTalkSessionTitleNornal == strId)
    {
        return CUSTOM_FONT_ID_TALK_SESSITON_TITLE_NORMAL;
    }
    else if (kszFontIdTalkSessionTitleFocus == strId)
    {
        return CUSTOM_FONT_ID_TALK_SESSITON_TITLE_FOCUS;
    }
    else if (kszFontIdTalkConferenceLabel == strId)
    {
        return CUSTOM_FONT_ID_TALK_CONFERENCE_LABEL;
    }
    else if (kszFontIdTalkConferenceHintTip == strId)
    {
        return CUSTOM_FONT_ID_TALK_CONFERENCE_HINT_TIP;
    }
    else if (kszFontIdTalkConferenceManagerNormal == strId)
    {
        return CUSTOM_FONT_ID_TALK_CONFERENCEMANAGE_NORMAL;
    }
    else if (kszFontIdTalkSearchNormal == strId)
    {
        return CUSTOM_FONT_ID_TALK_SEARCH_NORMAL;
    }
    else if (kszFontIdTalkDialingErrorInfo == strId)
    {
        return CUSTOM_FONT_ID_TALK_DIALING_ERROR_INFO;
    }
    else if (kszFontIdTalkContactInfoName == strId)
    {
        return CUSTOM_FONT_ID_TALK_CONTACT_INFO_NAME;
    }
    else if (kszFontIdTalkContactInfoNumber == strId)
    {
        return CUSTOM_FONT_ID_TALK_CONTACT_INFO_NUMBER;
    }
    else if (kszFontIdTalkContactInfoInfo == strId)
    {
        return CUSTOM_FONT_ID_TALK_CONTACT_INFO_INFO;
    }

    UIKERNEL_WARN("CCustomThemeManager::GetCustomObjectId(%s) Fail.", strId.c_str());

    return INVALID_FONT_OBJECT_ID;
}

bool CCustomThemeManager::GetColorByString(yl::string & strColor, QColor & objColor)
{
    strColor.trim_both();

    if (strColor.empty())
    {
        return false;
    }

    strColor.to_upper();

    if (strColor.find(PREPIX_OF_COLOR) != yl::string::npos)
    {
        objColor.setNamedColor(strColor.c_str());
        return true;
    }

    int nRed = 0;
    int nGreen = 0;
    int nBlue = 0;
    int nAlpha = 255;

    if (sscanf(strColor.c_str(), "%d,%d,%d,%d", &nRed, &nGreen, &nBlue, &nAlpha) == 4
            || sscanf(strColor.c_str(), "RGB(%d,%d,%d,%d)", &nRed, &nGreen, &nBlue, &nAlpha) == 4
            || sscanf(strColor.c_str(), "%d,%d,%d", &nRed, &nGreen, &nBlue) == 3
            || sscanf(strColor.c_str(), "RGB(%d,%d,%d)", &nRed, &nGreen, &nBlue) == 3)
    {
        objColor.setRgb(nRed, nGreen, nBlue, nAlpha);
        return true;
    }

    return false;
}

const CustomFontData & CCustomThemeManager::GetCustomFontDataByKey(int nKey)
{
    MAP_CUSTOMTHEME_DATA::iterator iter = m_mapCustomThemeData.find(nKey);

    if (iter == m_mapCustomThemeData.end())
    {
        UIKERNEL_WARN("Get CustomFontData Key[%d] Fail.", nKey);
        return m_themeData;
    }

    return iter->second;
}

int CCustomThemeManager::GetCustomFontSizeByKey(int nFontKey)
{
    const CustomFontData & stCustomFontData = GetCustomFontDataByKey(nFontKey);

    return stCustomFontData.m_nFontSize;
}

QColor CCustomThemeManager::GetCustomFontColorByKey(int nFontKey, int nFontStatus/* = FONT_NORMAL*/)
{
    const CustomFontData & stCustomFontData = GetCustomFontDataByKey(nFontKey);

    if (FONT_ACTIVE == nFontStatus)
    {
        return stCustomFontData.m_colorActive;
    }

    return stCustomFontData.m_colorNormal;
}

bool CCustomThemeManager::IsShowFontShadowByKey(int nFontKey)
{
    const CustomFontData & stCustomFontData = GetCustomFontDataByKey(nFontKey);

    return stCustomFontData.m_bshowShadow;
}

void CCustomThemeManager::InsertCustomFontData(int nFontId, CustomFontData & stCustomFontData)
{
    MAP_CUSTOMTHEME_DATA::iterator iter = m_mapCustomThemeData.find(nFontId);

    // 如果已存在，则保存已存在的项，以最后一次存储的为准
    if (iter != m_mapCustomThemeData.end())
    {
        m_mapCustomThemeData.erase(nFontId);
    }

    m_mapCustomThemeData[nFontId] = stCustomFontData;
}
