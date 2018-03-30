#include "common.h"
#include "devicelib/phonedevice.h"
#include "service_system.h"

static FuncGetPhoneType pFuncGetPhoneType = NULL;
static FuncGetPhoneName pFuncGetPhoneName = NULL;

void SetGetPhoneTypeCallbackFunc(FuncGetPhoneType pFunc)
{
    pFuncGetPhoneType = pFunc;
}

void SetGetPhoneNameCallbackFunc(FuncGetPhoneName pFunc)
{
    pFuncGetPhoneName = pFunc;
}

int GetPhoneHwVersion()
{
    char buff[64] = {0};
    if (sys_get_hw_version(buff, 64) == 0)
    {
        return GetPhoneHwVersion(buff);
    }
    return PT_UNKNOWN;
}

int GetPhoneHwVersion(const std::string & hw)
{
    return atoi(hw.c_str());
}

void SetPhoneType(int type)
{
    sys_set_hw_version(formatString("%d.0.0.0.0.1", type).c_str());
    sys_set_sw_version(formatString("%d.0.0.1", type).c_str());
    if (pFuncGetPhoneName != NULL)
    {
        sys_set_model_name((*pFuncGetPhoneName)());
    }
}

const char * GetPhoneSwVersion()
{
    static char buffer[64] = {0};
    sys_get_sw_version(buffer, 64);
    return buffer;
}
