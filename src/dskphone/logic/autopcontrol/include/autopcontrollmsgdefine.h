#ifndef __AUTOPMSG__EFINE_H__
#define __AUTOPMSG__EFINE_H__
#include "innermsgdefine.h"

enum AutopMessageDefine
{
    AUTOP_MESSAGE_BEG = INNER_MSG_AUTOP_MESSAGE_BEGIN,

    //  @wparam : 0 - 更新失败，1 - 更新成功
    //  @lparam : handset id
    AUTOP_AUTOP_FINISH,

    //  @wparam : 0 - 更新中，1 - 更新成功 , 2 - 更新失败, 3 - 鉴权
    //  @lparam : AUTOP_UPDATE_TYPE
    AUTOP_AUTOP_STATUS,

    AUTOP_MESSAGE_END = INNER_MSG_AUTOP_MESSAGE_END
};

#endif //__AUTOPMSG__EFINE_H__
