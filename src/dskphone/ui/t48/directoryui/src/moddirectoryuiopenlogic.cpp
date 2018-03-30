#include "moddirectoryuiopenlogic.h"
#include "contacts/directory/include/moddirinterface.h"
#include "baselibs/include/configparser/modconfigparser.h"
#include "commondirctrl.h"
#include "btuccontactcontroler.h"
#include <configiddefine.h>

bool DirUILogic_IsRemoteEnable()
{
    return CCommonDirCtrl::IsDirEnable(DIRECTORY_TYPE_REMOTE)
           && CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_REMOTE);
}

bool DirUILogic_IsButcEnable()
{
    return CBTUCContactControler::IsConfigure()
           && CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_BTUC);
}

bool DirUILogic_IsBSFTEnable()
{
    YLVector<int> vecGroupId;

    return CCommonDirCtrl::IsDirEnable(DIRECTORY_TYPE_BROADSOFT) && \
           CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_BROADSOFT) && \
           Dir_GetConfigMenuIdList(vecGroupId, BSFT_DIR);
}

bool DirUILogic_IsPlcmEnable()
{
    return configParser_GetConfigInt(kszPlcmPhoneEnable) != 0;
}

bool DirUILogic_IsBlacklistEnable()
{
    return CDirectoryControler::IsUserLevelShow(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId);
}

bool DirUILogic_IsLogHistoryChooseEnable()
{
    return BsftLog_IsEnable();
}

bool DirUILogic_IsLogBSFTEnable()
{
    YLVector<int> vecGroupId;

    return BsftLog_IsEnable() && CDirectoryControler::IsUserLevelShow(CALL_LOG_TYPE_BROADSOFT) && \
           Dir_GetConfigMenuIdList(vecGroupId, BSFT_LOG);
}