#if IF_FEATURE_METASWITCH
#include "modmtsw.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"

#include "mtswcontroller.h"
#include "mtswactioncmdbase.h"


void MTSW_PreInit()
{
    if ((configParser_GetConfigInt(kszBroadsoftActive) == 1))
    {
        configParser_SetConfigInt(kszMetaSwitchActive, 0);
    }
}

void MTSW_Init()
{
    MTSW_PreInit();

    _MTSWController.Init();
}

bool MTSW_IsLogined()
{
    return (!_MTSWController.GetSessionId().empty());
}

void MTSW_Commprotal_Login()
{
    _MTSWController.Login();
}


void MTSW_Commprotal_Logout()
{
    _MTSWController.Logout();
}

bool MTSW_ExecPost(CMTSWActionCmdBase * pCallback, yl::string strURL, yl::string strData,
                   MtswActionType nType)
{
    return _MTSWController.ExecPost(pCallback, strURL.c_str(), strData.c_str(), nType);
}
bool MTSW_ExecDownLoad(CMTSWActionCmdBase * pCallback, yl::string strURL, MtswActionType nType,
                       yl::string strFilePath /*= ""*/, int nExParm/* = -1*/)
{
    return _MTSWController.ExecDownLoad(pCallback, strURL.c_str(), nType, strFilePath.c_str(), nExParm);
}

bool MTSW_IsCommportalActive()
{
    return (configParser_GetConfigInt(kszMetaSwitchActive) == 1
            && configParser_GetConfigInt(kszMTSWCommportalEnable) == 1);
}
#endif