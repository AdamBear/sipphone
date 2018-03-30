#include "t49test.h"

#include "talk/talklogic/include/modtalklogic.h"
#include "talk/broadsoftsca/include/modbroadsoft_sca.h"
#include "talk/callcompletion/include/modcallcompletion.h"
#include "extended/csta/include/modcsta.h"
#include "extended/actionuri_url/include/modactionuri_url.h"
#include "extended/hotdesking/include/modhotdesking.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "autoset/include/modautoset.h"
#include "lamp/backlight/include/modbacklight.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "lamp/powerled/include/modpowerledlogic.h"
#include "voice/include/voicemanager.h"

T49Test::T49Test()
{

}

T49Test::~T49Test()
{

}

void T49Test::PrintfTestInfo()
{
    printf("talklogic_IsAllSessionRinging[%d]\n", talklogic_IsAllSessionRinging());
}
