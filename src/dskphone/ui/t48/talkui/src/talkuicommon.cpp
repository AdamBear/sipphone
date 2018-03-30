#include "talkuicommon.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "voice/include/modvoice.h"

bool TalkUIlogic_IsAllowSilenceDuringCall()
{
    bool bResult = false;

    if (voice_IsPlayingRing())
    {
        // 铃声音量可以调节为0
        bResult = true;
    }
    else
    {
        // http://192.168.1.99/Bug.php?BugID=19177
        bResult = (configParser_GetConfigInt(kszEnableSilenceMode) == 1);
    }

    return bResult;
}
