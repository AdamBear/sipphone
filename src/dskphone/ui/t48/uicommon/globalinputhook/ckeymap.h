#ifndef __KEY_MAP_H__
#define __KEY_MAP_H__

#include "keymap.h"

class CKeyMap
{
public:
    CKeyMap(void);
    virtual ~CKeyMap(void);
public:
    static CKeyMap * GetKeyMap();

public:
    //映射Qt系统按键至Phone按键
    virtual int MapUiKeyToPhoneKey(int nKeyCode, bool & bPress);
    //映射Phone按键至Qt系统按键
    virtual int MapPhoneKeyToUiKey(int nKeyCode, bool & bPress);
};

#endif //__KEY_MAP_H__
