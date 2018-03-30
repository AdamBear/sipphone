#ifndef IMAGECACHEMANAGERDEFINE_H
#define IMAGECACHEMANAGERDEFINE_H

#include <string>

enum IMAGE_TYPE
{
    DIT_NONE                = 0x00000000,
    DIT_DIR                 = 0x00000001,       // 同逻辑层图片Type值enum LYNC_PHOTE_TYPE对应
    DIT_TALK                = 0x00000002,
    DIT_ACCOUNT             = 0x00000004,
    DIT_SEARCH              = 0x00000008,
    DIT_FAVORITE            = 0x00000010,
    DIT_THEME               = 0x00000020,
    DIT_SOCKET              = 0x00000040,
    DIT_NOCACHE             = 0x00000080,
    DIT_USB                 = 0x00000100,
    DIT_PROPORTION          = 0x00000200,
    DIT_NOTTHEME            = 0x00000400,

    DIT_MAX,
};

enum IMAGE_RECEIVABLE_TYPE
{
    IRT_RECEIVABLE = 0,
    IRT_ERROR,
    IRT_SIZE_OVER,
};

#endif // IMAGECACHEMANAGERDEFINE_H
