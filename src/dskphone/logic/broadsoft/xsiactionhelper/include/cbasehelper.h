#ifndef __CBASE_HELPER__
#define __CBASE_HELPER__

#include <ylhashmap.h>
#include "taskaction/taskaction.h"
#include "xmlparser/xmlparser.hpp"

typedef YLHashMap<int, bool> XSIAvailableMap;

enum XSI_SEND_TYPE
{
    XSI_UNKNOW_SEND = 0,
    XSI_DIR_SEND,
    XSI_CALLLOD_SEND
};

enum XSI_STATUS
{
    XSI_SIPACCOUNT_NOT_AVAILABLE,
    XSI_AUTH_ERROR,
    XSI_SUCCESS,
};

//初始化接口，程序運行初始化一次
void XSI_BaseHelp_Init();

namespace NS_TA
{
class CTaskAction;
}

class CBaseHelper
{
public:
    CBaseHelper();
    virtual ~CBaseHelper();

    static void SetXSINotif(bool bOn);

    static void ProcessCallback(NS_TA::CTaskAction * pTaskAction, int nLineId = 0,
                                XSI_SEND_TYPE eSend = XSI_UNKNOW_SEND);

    static XSI_STATUS GetXSIStatus(int nLineId = 0);

    static bool IsXsiAvailable(int nLineId = 0);

    static bool SetXsiAvailable(bool bXsiAvailable, int nLine = 0);

    static void SetXsiAllAccountAvailable(bool bXsiAvailable);

private:
    static bool m_bNotifXSIChange;

    static XSIAvailableMap m_map;

    static XSIAvailableMap m_mapAuthSuc;

};

bool GetXmlChildText(const xml_node & xmlRoot, const yl::string & strChildName,
                     yl::string & strOut);

bool GetXmlChildText(const xml_node & xmlRoot, const yl::string & strChildName, bool & bOut);

bool PutXmlChildText(xml_node & xmlRoot, const yl::string & strChildName, const yl::string & strIn);

bool PutXmlChildBool(xml_node & xmlRoot, const yl::string & strChildName, bool bIn);

#endif
