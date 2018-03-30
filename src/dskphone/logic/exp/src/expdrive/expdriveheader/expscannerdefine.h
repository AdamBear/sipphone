#ifndef _EXP_SCANNER_DEFINE_H__
#define _EXP_SCANNER_DEFINE_H__

#ifndef EXP_INFO
#define EXP_INFO DSSKEY_INFO
#define EXP_ERR DSSKEY_ERR
#define EXP_WARN DSSKEY_WARN
#endif // !EXP_INFO

#include "innermsgdefine.h"
#include "interfacedefine.h"
#include "ETLLib.hpp"
#include "dsklog/log.h"
typedef int USBHandle;
typedef int DsskeyID;
typedef int ExpID;
typedef int KeyID;

#define EXP_KEY_STATUS_PRESS    1
#define EXP_KEY_STATUS_RELEASE  0
#define EXP_KEY_STATUS_TEST     2


enum EXP_SCANNER_MESSAGE
{
    EXP_BEG = INNER_MSG_EXP_SCANNER_MESSAGE_BEGIN,
    EXP_IN,
    EXP_OUT,
    EXP_KEY_DOWN,
    EXP_KEY_UP,
    EXP_END = INNER_MSG_EXP_SCANNER_MESSAGE_END,
};


inline bool PostMsgToApp(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), uMsg, wParam, lParam);
}

inline bool PostMsgToApp(UINT uMsg, WPARAM wParam, LPARAM lParam, int nExtraSize,
                         LPCVOID lpExtraData)
{
    return msgPostMsgToThreadEx(msgGetThreadByName(VP_PHONE_THREAD_NAME), uMsg, wParam, lParam,
                                nExtraSize, lpExtraData);
}

//inline bool PostMsgToDrawer(UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//  return msgPostMsgToThread("", uMsg, wParam, lParam);
//}
//
//inline bool PostMsgToDrawer(UINT uMsg, WPARAM wParam, LPARAM lParam, int nExtraSize, LPCVOID lpExtraData)
//{
//  return msgPostMsgToThreadEx("", uMsg, wParam, lParam, nExtraSize, lpExtraData);
//}


#endif //_EXP_SCANNER_DEFINE_H__

