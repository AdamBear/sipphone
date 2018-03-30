#include "phonedev.h"

#include <dsklog/log.h>

#include "xmlparser.hpp"

#ifdef WIN32
#else
#include <linux/input.h>
#endif
#include <devicelib/phonedevice.h>

#if WINDOWS_SYSTEM
#define devicelib_RefreshScreen(...)        0
#define devicelib_Init(...)                 0
#define devicelib_InitScreen(...)           0
#define devicelib_UnInitScreen(...)         0
#endif

#define BITS_IN_BYTE 8

const char cszKeypadDeviceName[] = "/dev/keypad0";

const char cszHardCodeAttr[] = "HardCode";

const char cszPressCodeAttr[] = "PressCode";
const char cszReleaseCodeAttr[] = "ReleaseCode";

CPhoneDev::CPhoneDev()
{
    m_hKeypad = open(cszKeypadDeviceName, O_RDONLY);
}

CPhoneDev::~CPhoneDev()
{
    close(m_hKeypad);
}

int CPhoneDev::GetKey(int nKeyCode, bool bDown)
{
    PHONE_KEY_MAP::iterator iter = m_mapKey.find(nKeyCode);

    if (iter != m_mapKey.end())
    {
        return iter->second;
    }

    PHONE_DOUTBL_KEY_MAP::iterator iterDouble = m_mapDoubleKey.find(nKeyCode);

    if (iterDouble == m_mapDoubleKey.end())
    {
        return PHONE_KEY_NONE;
    }

    const PhoneKey& phoneKey = iterDouble->second;

    if (bDown)
    {
        return phoneKey.nPressCode;
    }
    else
    {
        return phoneKey.nReleaseCode;
    }
}

int CPhoneDev::GetKeypadHandle() const
{
    return m_hKeypad;
}

PhoneEvent CPhoneDev::readEvent()
{
    PhoneEvent resultEvent; // return struct
#if LINUX_SYSTEM
    struct input_event event;
    size_t tSize = sizeof(event);
    if (m_hKeypad != -1 && read(m_hKeypad, &event, tSize))
    {
        if (event.code != 0)
        {
            resultEvent.eEventType = PET_KEY_EVENT;
            resultEvent.wParam = event.code;
            resultEvent.lParam = event.value;
        }
    }
#endif
    return resultEvent;
}

void CPhoneDev::readKeyMap(yl::string strMapPath)
{
    m_mapKey.clear();
    m_mapDoubleKey.clear();

    xml_document doc;

    xml_parse_result docRet = doc.load_file(strMapPath.c_str());
    // 解析失败
    if (status_ok != docRet.status)
    {
        return;
    }

    xml_node nodeRoot = doc.first_child();

    if (nodeRoot.empty())
    {
        return;
    }

    xml_node nodeChild = nodeRoot.first_child();

    int nHardCode = PHONE_KEY_NONE;
    int nPressCode = PHONE_KEY_NONE;
    int nReleaseCode = PHONE_KEY_NONE;

    while (!nodeChild.empty())
    {
        bool bHaveReleaseCode = false;
        yl::string strTemp = "";

        nHardCode = PHONE_KEY_NONE;
        nPressCode = PHONE_KEY_NONE;
        nReleaseCode = PHONE_KEY_NONE;

        //Get Hard Code
        strTemp = nodeChild.attribute(cszHardCodeAttr).value();
        if (strTemp.empty())
        {
            continue;
        }
        nHardCode = atoi(strTemp.c_str());

        //Get Press Code
        strTemp = nodeChild.attribute(cszPressCodeAttr).value();
        if (!strTemp.empty())
        {
            nPressCode = atoi(strTemp.c_str());
        }

        //Get Release Code
        strTemp = nodeChild.attribute(cszReleaseCodeAttr).value();
        if (!strTemp.empty())
        {
            bHaveReleaseCode = true;
            nReleaseCode = atoi(strTemp.c_str());
        }

        if (bHaveReleaseCode)
        {
            m_mapDoubleKey[nHardCode].nPressCode = nPressCode;
            m_mapDoubleKey[nHardCode].nReleaseCode = nReleaseCode;
        }
        else
        {
            m_mapKey[nHardCode] = nPressCode;
        }

        nodeChild = nodeChild.next_sibling();
    }
}
