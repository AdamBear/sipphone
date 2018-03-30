#include "modcalllogui.h"
#include "interfacedefine.h"
#include "baseui/include/t2xpicpath.h"


yl::string GetCallTypeIcon(int nCallType)
{
    switch (nCallType)
    {
    case CT_OUTGOING:
        return yl::string(PIC_CALL_LIST_OUT);
        break;
    case CT_INCOMING:
        return yl::string(PIC_CALL_LIST_IN);
        break;
    case CT_MISSCALL:
        return yl::string(PIC_CALL_LIST_MISS);
        break;
    case CT_FORWARDCALL:
        return yl::string(PIC_ALL_LIST_FORWARD);
        break;
    default:
        break;
    }

    return yl::string("");
}
