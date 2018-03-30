#include "statusmodel.h"

#include <assert.h>

#include <ETLLib/ETLLib.hpp>
#include "interfacedefine.h"
#include "configparser/modconfigparser.h"
#include "setting/include/common.h"
#include <configiddefine.h>
#include "setting/include/messagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "devicelib/phonedevice.h"
#include "commonapi/stringhelperapi.h"
#include "service_system.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
const yl::string CStatusModel::GetModel() const
{
    yl::string strModel;
    char bufProductName[512] = {0};
    sys_replace_wildcards((char *)configParser_GetConfigString(kszProductName).c_str(), 0,
                          bufProductName, sizeof(bufProductName) / sizeof(char));
    strModel = bufProductName;

    return strModel;
}

void CStatusModel::SetModel(const yl::string & strModel)
{
    // Save the value to config parse.
    configParser_SetConfigString(kszProductName, strModel.c_str(), CONFIG_LEVEL_PHONE);

    // Post message.
    etl_NotifyAppEx(false, PHONE_STATUS_MODEL_CHANGED, 0, 0,
                    (int)strModel.length() + 1, strModel.c_str());
}

const yl::string CStatusModel::GetHardWare() const
{
    // Read phone hardware value.
    return devicelib_GetHardwareVersion();
}

const yl::string CStatusModel::GetFirmware() const
{
    // Read phone firmware value.
    char szFirmware[32] = {0};
    yl::string strFirmware = "";
    if (0 == sys_get_sw_version(szFirmware, 32))
    {
        strFirmware = szFirmware;
    }
    return strFirmware;
}

const yl::string CStatusModel::GetProductId() const
{
    // Read phone product Id.
    char buf[128] = {0};
    sys_get_rom_info(SYS_ROM_INFO_PACKED_TIME, buf, 128);
    yl::string strProductId(buf);

    strProductId = commonAPI_TrimString(strProductId, "-");
    strProductId = commonAPI_TrimString(strProductId, ":");
    strProductId = commonAPI_TrimString(strProductId, " ");
    strProductId = commonAPI_TrimString(strProductId, "CST");

    return strProductId;
}

// Set phone product Id value.
void CStatusModel::SetProductId(const yl::string & strProductId)
{
    // Save the value to config parse.
    configParser_SetConfigString(kszPhoneVersion, strProductId.c_str(), CONFIG_LEVEL_PHONE);

    // Post message.
    etl_NotifyAppEx(false, PHONE_STATUS_PRODUCT_ID_CHANGED, 0, 0,
                    (int)strProductId.length() + 1, strProductId.c_str());
}

CStatusModel::CStatusModel()
{

}

CStatusModel::~CStatusModel()
{

}

}  // namespace NS_SETTING
