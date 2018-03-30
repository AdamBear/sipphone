#include "talk/talklogic/include/sessionproxy.h"

#ifdef IF_FEATURE_DSSKEY

void talk_dsskey_init();

CSessionProxy talk_GetSessionByDsskey(int iDsskeyID);

#endif  //IF_FEATURE_DSSKEY
