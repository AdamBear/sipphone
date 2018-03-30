#include "implperformanceucfavorite.h"
#include "baseui/t4xpicpath.h"
#include "dsskeyperformancemanager.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"

#define MAX_BUF_LEN 256

////////////////Dsskey会话相关类型的基类/////////////////////////
ImplDsskeyPerformanceUCFavorite::ImplDsskeyPerformanceUCFavorite()
    : ImplDsskeyPerformanceBase(DT_UC_FAVORITE)
{

}

ImplDsskeyPerformanceUCFavorite::~ImplDsskeyPerformanceUCFavorite()
{

}

// 获取显示内容前的操作
bool ImplDsskeyPerformanceUCFavorite::GetPerformanceBegin(const DssKeyLogicData & logicData)
{
    // 需要先获取UC开关
    m_bEnableUC = (1 == configParser_GetConfigInt(kszEnableUC)
                   && configParser_GetConfigInt(kszBroadsoftActive) == 1);
    return true;
}

// 获取文字显示
bool ImplDsskeyPerformanceUCFavorite::GetLabelPerformance(const DssKeyLogicData & logicData,
        DsskeyLabelData & LabelData)
{
    bool bRet = false;
    if (m_bEnableUC)
    {
        bool bExt = GetExtendFlag();
        LabelData.eTextMode = TEXT_MODE_FIRST_ROW;
        if (logicData.strLabel.empty() && logicData.strValue.empty())
        {
            LabelData.strFirstRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_UC_FAVORITE, bExt)).toUtf8().data();
        }
        else
        {
            // UC自定义label 不必翻译
            char szBuffer[MAX_BUF_LEN] = "";

            if (logicData.strLabel.empty())
            {
                sprintf(szBuffer, "%s:%s", (LANG_TRANSLATE_DSSKEY_LABEL(TRID_UC_FAVORITE,
                                            bExt) + ":").toUtf8().data(), logicData.strValue.c_str());
                // 字符串结尾
                szBuffer[MAX_BUF_LEN - 1] = '\0';
                LabelData.strFirstRowText = szBuffer;
            }
            else
            {
                LabelData.strFirstRowText = logicData.strLabel;
            }

        }
        bRet = true;
    }

    DSSKEYUI_INFO("UC Favorite label(%s) iStatus(%d) bEnableUC(%d)", \
                  LabelData.strFirstRowText.c_str(), logicData.iStatus, m_bEnableUC);
    return bRet;
}

// 获取图标显示
bool ImplDsskeyPerformanceUCFavorite::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    if (m_bEnableUC)
    {
        //是否显示新状态图标
        bool bNewIconMode = configParser_GetConfigInt(kszPresenceIconMode) == 1;

        if (logicData.strExtension.empty())
        {
            //使用默认联系人头像
            IconData.strIconPath = PIC_DSS_FAVORITES;

            if (logicData.iStatus == DS_UC_ONLINE)
            {
                IconData.strIconPath = bNewIconMode ? PIC_DSS_UC_NEW_ONLINE : PIC_DSS_UC_ONLINE;
            }
            else if (logicData.iStatus == DS_UC_CHATTY)
            {
                IconData.strIconPath = bNewIconMode ? PIC_DSS_UC_NEW_ONLINE : PIC_DSS_UC_ONLINE;
            }
            else if (logicData.iStatus == DS_UC_AWAY)
            {
                IconData.strIconPath = bNewIconMode ? PIC_DSS_UC_NEW_AWAY : PIC_DSS_UC_AWAY;
            }
            else if (logicData.iStatus == DS_UC_EXTENDED_AWAY)
            {
                IconData.strIconPath = bNewIconMode ? PIC_DSS_UC_NEW_AWAY : PIC_DSS_UC_AWAY;
            }
            else if (logicData.iStatus == DS_UC_BUSY)
            {
                IconData.strIconPath = bNewIconMode ? PIC_DSS_UC_NEW_BUSY : PIC_DSS_UC_BUSY;
            }
            else if (logicData.iStatus == DS_UC_OFFLINE)
            {
                IconData.strIconPath = bNewIconMode ? PIC_DSS_UC_NEW_OFFLINE : PIC_DSS_UC_OFFLINE;
            }
        }
        else
        {
            //使用用户自定义头像
            IconData.strIconPath = TEMP_PATH + logicData.strExtension;

            if (logicData.iStatus == DS_UC_ONLINE)
            {
                IconData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_ONLINE : PIC_ICON_PRESENCE_ONLINE;
            }
            else if (logicData.iStatus == DS_UC_CHATTY)
            {
                IconData.strPresenceIcon = PIC_ICON_PRESENCE_CHATTY;
            }
            else if (logicData.iStatus == DS_UC_AWAY)
            {
                IconData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_AWAY : PIC_ICON_PRESENCE_AWAY;
            }
            else if (logicData.iStatus == DS_UC_EXTENDED_AWAY)
            {
                IconData.strPresenceIcon = PIC_ICON_PRESENCE_EXTENDEDAWAY;
            }
            else if (logicData.iStatus == DS_UC_BUSY)
            {
                IconData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_BUSY : PIC_ICON_PRESENCE_BUSY;
            }
            else if (logicData.iStatus == DS_UC_OFFLINE)
            {
                IconData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_OFFLINE : PIC_ICON_PRESENCE_OFFLINE;
            }
            else
            {
                //IconData.eGroundPhoto = GROUNDPHOTO_MODE_DISALBE;
                IconData.strPresenceIcon = bNewIconMode ? PIC_ICON_PRESENCE_NEW_OFFLINE : PIC_ICON_PRESENCE_OFFLINE;
            }
        }
    }
    return true;
}
