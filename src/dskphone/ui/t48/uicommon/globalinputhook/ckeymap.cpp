#include <QtGui>
#include "ckeymap.h"
#include <ETLLib/ETLLib.hpp>

enum DEFAULT_KEY_CODE
{
    //number key
    DEFAULT_KEY_0           = Qt::Key_0,
    DEFAULT_KEY_1           = Qt::Key_1,
    DEFAULT_KEY_2           = Qt::Key_2,
    DEFAULT_KEY_3           = Qt::Key_3,
    DEFAULT_KEY_4           = Qt::Key_4,
    DEFAULT_KEY_5           = Qt::Key_5,
    DEFAULT_KEY_6           = Qt::Key_6,
    DEFAULT_KEY_7           = Qt::Key_7,
    DEFAULT_KEY_8           = Qt::Key_8,
    DEFAULT_KEY_9           = Qt::Key_9,
    DEFAULT_KEY_STAR        = Qt::Key_Asterisk,
    DEFAULT_KEY_POUND       = Qt::Key_NumberSign,

    //function key
    DEFAULT_KEY_FN1     = Qt::Key_F1,
    DEFAULT_KEY_FN2     = Qt::Key_F2,
    DEFAULT_KEY_FN3     = Qt::Key_F3,
    DEFAULT_KEY_FN4     = Qt::Key_F4,

    //cursor key
    DEFAULT_KEY_LEFT        = Qt::Key_Left,
    DEFAULT_KEY_RIGHT       = Qt::Key_Right,
    DEFAULT_KEY_UP      = Qt::Key_Up,
    DEFAULT_KEY_DOWN        = Qt::Key_Down,

    //system key
    DEFAULT_KEY_ENTER       = Qt::Key_Return,
    DEFAULT_KEY_BACKSPACE   = Qt::Key_Backspace,

    //misc key
    DEFAULT_KEY_CAMERA  = Qt::Key_F5,
    DEFAULT_KEY_PHONEBOOK   = Qt::Key_F6,
    DEFAULT_KEY_MUTE        = Qt::Key_F7,
    DEFAULT_KEY_TRANSFER    = Qt::Key_F8,
    DEFAULT_KEY_REDIAL  = Qt::Key_F9,
    DEFAULT_KEY_HANDFREE    = Qt::Key_F10,
    DEFAULT_KEY_HANDSET = Qt::Key_F11,
    DEFAULT_KEY_HEADSET = Qt::Key_F12,
};

CKeyMap::CKeyMap(void)
{
}

CKeyMap::~CKeyMap(void)
{
}

//将QT系统库的按键映射为代码处理的系统按键
int CKeyMap::MapUiKeyToPhoneKey(int nKeyCode, bool & bPress)
{
    switch (nKeyCode)
    {
    case DEFAULT_KEY_0:
    case DEFAULT_KEY_1:
    case DEFAULT_KEY_2:
    case DEFAULT_KEY_3:
    case DEFAULT_KEY_4:
    case DEFAULT_KEY_5:
    case DEFAULT_KEY_6:
    case DEFAULT_KEY_7:
    case DEFAULT_KEY_8:
    case DEFAULT_KEY_9:
        {
            return PHONE_KEY_0 + (nKeyCode - DEFAULT_KEY_0);
        }
        break;
    case DEFAULT_KEY_FN1:
    case DEFAULT_KEY_FN2:
    case DEFAULT_KEY_FN3:
    case DEFAULT_KEY_FN4:
        {
            return PHONE_KEY_FN1 + (nKeyCode - DEFAULT_KEY_FN1);
        }
        break;
    case DEFAULT_KEY_LEFT:
        {
            return PHONE_KEY_LEFT;
        }
        break;
    case DEFAULT_KEY_RIGHT:
        {
            return PHONE_KEY_RIGHT;
        }
        break;
    case DEFAULT_KEY_UP:
        {
            return PHONE_KEY_UP;
        }
        break;
    case DEFAULT_KEY_DOWN:
        {
            return PHONE_KEY_DOWN;
        }
        break;
    case DEFAULT_KEY_ENTER:
        {
            return PHONE_KEY_OK;
        }
        break;
    case DEFAULT_KEY_BACKSPACE:
        {
            return PHONE_KEY_BACKSPACE;
        }
        break;
    case DEFAULT_KEY_CAMERA:
        {
            return PHONE_KEY_CAMERA;
        }
        break;
    case DEFAULT_KEY_PHONEBOOK:
        {
            return PHONE_KEY_PHONEBOOK;
        }
        break;
    case DEFAULT_KEY_MUTE:
        {
            return PHONE_KEY_MUTE;
        }
        break;
    case DEFAULT_KEY_TRANSFER:
        {
            return PHONE_KEY_TRANSFER;
        }
        break;
    case DEFAULT_KEY_REDIAL:
        {
            return PHONE_KEY_REDIAL;
        }
        break;
    case DEFAULT_KEY_HANDFREE:
        {
            return PHONE_KEY_HANDFREE;
        }
        break;
    case DEFAULT_KEY_HANDSET:
        {
            if (bPress)
            {
                bPress = true;
                return PHONE_KEY_HANDSET_ON_HOOK;
            }
            else
            {
                bPress = true;
                return PHONE_KEY_HANDSET_OFF_HOOK;
            }
        }
        break;
    case DEFAULT_KEY_HEADSET:
        {
            if (bPress)
            {
                bPress = true;
                return PHONE_KEY_HEADSET_IN;
            }
            else
            {
                bPress = true;
                return PHONE_KEY_HEADSET_OUT;
            }
        }
        break;
    default:
        break;
    }

    return nKeyCode;
}

//将代码处理的系统按键映射为QT系统库的按键
int CKeyMap::MapPhoneKeyToUiKey(int nKeyCode, bool & bPress)
{
    switch (nKeyCode)
    {
    case PHONE_KEY_0:
    case PHONE_KEY_1:
    case PHONE_KEY_2:
    case PHONE_KEY_3:
    case PHONE_KEY_4:
    case PHONE_KEY_5:
    case PHONE_KEY_6:
    case PHONE_KEY_7:
    case PHONE_KEY_8:
    case PHONE_KEY_9:
        {
            return DEFAULT_KEY_0 + (nKeyCode - PHONE_KEY_0);
        }
        break;
    case PHONE_KEY_FN1:
    case PHONE_KEY_FN2:
    case PHONE_KEY_FN3:
    case PHONE_KEY_FN4:
        {
            return DEFAULT_KEY_FN1 + (nKeyCode - PHONE_KEY_FN1);
        }
        break;
    case PHONE_KEY_LEFT:
        {
            return DEFAULT_KEY_LEFT;
        }
        break;
    case PHONE_KEY_RIGHT:
        {
            return DEFAULT_KEY_RIGHT;
        }
        break;
    case PHONE_KEY_UP:
        {
            return DEFAULT_KEY_UP;
        }
        break;
    case PHONE_KEY_DOWN:
        {
            return DEFAULT_KEY_DOWN;
        }
        break;
    case PHONE_KEY_OK:
        {
            return DEFAULT_KEY_ENTER;
        }
        break;
    case PHONE_KEY_BACKSPACE:
        {
            return DEFAULT_KEY_BACKSPACE;
        }
        break;
    case PHONE_KEY_CAMERA:
        {
            return DEFAULT_KEY_CAMERA;
        }
        break;
    case PHONE_KEY_PHONEBOOK:
        {
            return DEFAULT_KEY_PHONEBOOK;
        }
        break;
    case PHONE_KEY_MUTE:
        {
            return DEFAULT_KEY_MUTE;
        }
        break;
    case PHONE_KEY_TRANSFER:
        {
            return DEFAULT_KEY_TRANSFER;
        }
        break;
    case PHONE_KEY_REDIAL:
        {
            return DEFAULT_KEY_REDIAL;
        }
        break;
    case PHONE_KEY_HANDFREE:
        {
            return DEFAULT_KEY_HANDFREE;
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            bPress = true;
            return DEFAULT_KEY_HANDSET;
        }
        break;
    case PHONE_KEY_HANDSET_OFF_HOOK:
        {
            bPress = false;
            return DEFAULT_KEY_HANDSET;
        }
        break;
    case PHONE_KEY_HEADSET_IN:
        {
            bPress = true;
            return DEFAULT_KEY_HANDSET;
        }
        break;
    case PHONE_KEY_HEADSET_OUT:
        {
            bPress = false;
            return DEFAULT_KEY_HANDSET;
        }
        break;
    default:
        break;
    }

    return nKeyCode;
}

CKeyMap * CKeyMap::GetKeyMap()
{
    static CKeyMap * pKeyMap = NULL;
    if (pKeyMap == NULL)
    {
        pKeyMap = new CKeyMap();
    }
    return pKeyMap;
}
