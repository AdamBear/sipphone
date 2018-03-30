#include "modbtucui.h"

#include "directorycommon.h"
#include "btuccontactcontroler.h"
#include "dirctrlfactory.h"
#include "directorymgr.h"
#include "baseui/t4xpicpath.h"
#include "presence/include/modpresence.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

yl::string BTUC_GetPresenceIcon(int nPresence)
{
    //是否显示新状态图标
    bool bNewIconMode = configParser_GetConfigInt(kszPresenceIconMode) == 1;
    switch (nPresence)
    {
    case _PRESENCE_TYPE_ONLINE:
        return bNewIconMode ? yl::string(PIC_ICON_PRESENCE_NEW_ONLINE) : yl::string(
                   PIC_ICON_PRESENCE_ONLINE);
        break;
    case _PRESENCE_TYPE_AWAY:
        return bNewIconMode ? yl::string(PIC_ICON_PRESENCE_NEW_AWAY) : yl::string(PIC_ICON_PRESENCE_AWAY);
        break;
    case _PRESENCE_TYPE_BUSY:
        return bNewIconMode ? yl::string(PIC_ICON_PRESENCE_NEW_BUSY) : yl::string(PIC_ICON_PRESENCE_BUSY);
        break;
    case _PRESENCE_TYPE_OFFLINE:
        return bNewIconMode ? yl::string(PIC_ICON_PRESENCE_NEW_OFFLINE) : yl::string(
                   PIC_ICON_PRESENCE_OFFLINE);
        break;
    default:
        break;
    }

    return bNewIconMode ? yl::string(PIC_ICON_PRESENCE_NEW_UNKNOWN) : yl::string(
               PIC_ICON_PRESENCE_UNKNOWN);
}

bool BTUC_DialFavorite(int nContactId, int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    DIRUI_INFO("Dsskey Press Call BTUC_DialFavorite ContactId[%d], AccountId[%d].", nContactId,
               nAccountId);
    LIST_ATTRIBUTE lsNum;
    yl::string strName;

    if (!CBTUCContactControler::GetDialDetail(nContactId, strName, lsNum))
    {
        return false;
    }

    return CDirUIHelper::Dial(nAccountId, strName, lsNum,
                              g_DirCtrlFactory.GetControler(DIRECTORY_TYPE_BTUC).Get(), NULL);
}
