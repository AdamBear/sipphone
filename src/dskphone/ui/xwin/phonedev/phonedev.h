#ifndef __XPHONEDEV_XPHONEDEV_H__
#define __XPHONEDEV_XPHONEDEV_H__

/****************************
话机设备信息，用于和设备交互和区分话机功能
*****************************/
#include <ETLLib.hpp>
#include "keymap.h"
#include "ylhashmap.h"

enum PhoneEventType
{
    PET_UNKNOW = 0,     //
    PET_KEY_EVENT,          // wParam: key code   lParam:0-key release 1-key-pressed
};
struct PhoneEvent
{
    PhoneEventType eEventType;      // 0 - device connect
    int wParam;             // wParam: key code
    int lParam;             // lParam:0-key release 1-key-pressed
    PhoneEvent()
    {
        eEventType = PET_UNKNOW;
        wParam = -1;
        lParam = -1;
    }
};

struct PhoneKey
{
    int     nPressCode;
    int     nReleaseCode;

    PhoneKey()
    {
        nPressCode = PHONE_KEY_NONE;
        nReleaseCode = PHONE_KEY_NONE;
    }
};

typedef YLHashMap<int, int>             PHONE_KEY_MAP;
typedef YLHashMap<int, PhoneKey>    PHONE_DOUTBL_KEY_MAP;

class CPhoneDev
{
public:
    CPhoneDev();
    virtual ~CPhoneDev();

    //按键
    virtual int GetKey(int nKeyCode, bool bDown);
    virtual int GetKeypadHandle() const;
    virtual PhoneEvent readEvent();
    //灯
    //pass

    //EXP
    //pass

public:
    void readKeyMap(yl::string strMapPath);

protected:
    int m_hKeypad;

    PHONE_KEY_MAP                       m_mapKey;
    PHONE_DOUTBL_KEY_MAP        m_mapDoubleKey;
};

#endif //__XPHONEDEV_XPHONEDEV_H__
