#ifndef MTSW_MESSAGE_H_
#define MTSW_MESSAGE_H_

#include "innermsgdefine.h"

enum MTSWMessageDefine
{
    MTSW_MESSAGE_BEGIN = INNER_MSG_MTSW_MESSAGE_BEGIN,

    MTSW_MESSAGE_LOGIN, //登陆

    MTSW_MESSAGE_AUTH_ERR, //登陆失败

    MTSW_MESSAGE_DOWNLOADED, //语音文件下载完成
    MTSW_MESSAGE_DOWNLOADFAILED, //语音文件下载失败

    MTSW_MESSAGE_VMLIST_RESULT, //message list下载结果


    MTSW_MESSAGE_END = INNER_MSG_MTSW_MESSAGE_END,
};

#endif  // MTSW_MESSAGE_H_
