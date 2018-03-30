#ifndef SETTING_COMMONDEFINE_H_
#define SETTING_COMMONDEFINE_H_

namespace NS_SETTING
{
// Disallows copy and assignment.
#define DISALLOW_COPY_AND_ASSIGN(OBJ)  \
    OBJ(const OBJ&);  \
    void operator= (const OBJ&)

// Delete object.
#define DELETE_OBJ(obj)  \
    {  \
        if (obj != NULL)  \
        {  \
            delete obj;  \
            obj = NULL;  \
        }  \
    }

// enum ConfigSwitch, used to indicate if or not the config value is valid.
enum ConfigSwitch
{
    CONFIG_SWITCH_NONE = -1,  // Invalid switch value.
    CONFIG_OFF = 0,  // Switch is off.
    CONFIG_ON = 1,  // Switch is on.
    CONFIG_SWITCH_UNKOWN = 2,  // Unknown switch value.
    CONFIG_SWITH_MAX
};

// Translate integer to config value.
// | nValue | is the integer value.
// Returns the translated config value.
inline ConfigSwitch StrictCheck(int nValue)
{
    ConfigSwitch eRet = CONFIG_SWITCH_UNKOWN;
    switch (nValue)
    {
    case 0:
        eRet = CONFIG_OFF;
        break;
    case 1:
        eRet = CONFIG_ON;
        break;

    default:
        eRet = CONFIG_SWITCH_UNKOWN;
        break;
    }
    return eRet;
}

// Click action mask definition.
// memory key click action mask
const char * const kszCAMaskMemoryKey = "memory_key_item_x";
// Line key click action mask
const char * const kszCAMaskLineKey = "line_key_item_x";
// Expand key click action mask
const char * const kszCAMaskExpandKey = "expand_key_item_x";
// Anonymous call click action mask.
const char * const kszCAMaskAnonymousCall = "line_x";
// BroadSoft directory click action mask.
const char * const kszCAMaskBroadSoftDirectory = "item_x";
// BroadSoft call log click action mask.
const char * const kszCAMaskBroadSoftCallLog = "item_x";
// Video click action mask.
const char * const kszCAMaskVideo = "line_x";
// Auto answer click action mask.
const char * const kszCAAutoAnswer = "line_x";
// Door phone click action.
const char * const kszCADoorPhone = "setting_x";
// IP camera phone click action.
const char * const kszCAIpCamera = "setting_x";
// Account click action.
const char * const kszCAAccount = "account_x";

}  // namespace NS_SETTING

#endif  // SETTING_COMMONDEFINE_H_
