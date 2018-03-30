#include "modcalllogui.h"
#include "baseui/t4xpicpath.h"


yl::string GetCallTypeIcon(int nCallType)
{
    switch (nCallType)
    {
    case CT_OUTGOING:
        return yl::string(PIC_ICON_CALL_LOG_OUTING);
        break;
    case CT_INCOMING:
        return yl::string(PIC_ICON_CALL_LOG_INCOMING);
        break;
    case CT_MISSCALL:
        return yl::string(PIC_ICON_CALL_LOG_MISS);
        break;
    case CT_FORWARDCALL:
        return yl::string(PIC_ICON_CALL_LOG_FORWARD);
        break;
    default:
        break;
    }

    return yl::string("");
}

yl::string GetCallTypeSelectIcon(int nCallType)
{
    switch (nCallType)
    {
    case CT_OUTGOING:
        return yl::string(PIC_ICON_CALL_LOG_OUTING_SELECT);
        break;
    case CT_INCOMING:
        return yl::string(PIC_ICON_CALL_LOG_INCOMING_SELECT);
        break;
    case CT_MISSCALL:
        return yl::string(PIC_ICON_CALL_LOG_MISS_SELECT);
        break;
    case CT_FORWARDCALL:
        return yl::string(PIC_ICON_CALL_LOG_FORWARD_SELECT);
        break;
    default:
        break;
    }

    return yl::string("");
}
