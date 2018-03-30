#include <platform.h>
#include <config.h>
#include <path_define.h>
#include <features-define.h>

// YL
#include <ETLLib.hpp>
#include <sub-interface.h>
#include <configiddefine.h>
#include <yllist.h>
#include <ylvector.h>
#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include <ylstring.h>
#include <ylsmartptr.h>
#include <ylstringalgorithm.h>
#include <dsklog/log.h>
#include <configparser/modconfigparser.h>
#include <etlmsghandler/modetlmsghandler.h>
#include <regexp/modregexp.h>
#include <mkthread.h>
#include <commonapi/stringhelperapi.h>
#include <commonapi/systemhelperapi.h>
#include <configiddefine.h>
#include <devicelib/phonedevice.h>
#include <devicelib/networkserver.h>
#include <xmlparser/xmlparser.h>
#include <pugixml.hpp>
#include <translateiddefine.h>
//#include <resource/modresource.h>


//3rd
#include "service_h323_interface.h"
#include "service_cfg.h"
#include "service_config.h"
#include "service_autop.h"
#include "service_ipvp.h"
#include "service_ipvp_api.h"
#include "service_sip_interface.h"
#include "service_system.h"
#include "dl/dl.h"
#include "service_sip_define.h"
#include "autop_msg_mk.h"
#include "service_dsk_interface.h"

// STD
#include <map>
#include <list>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <exception>
#include <sstream>
#include <algorithm>
#include <assert.h>
#include <fstream>
#include <string.h>
#include <typeinfo.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <functional>
#include <iosfwd>
#include <new>
#include <utility>
#include <memory>
#include <iterator>
#include <locale.h>
#include <tchar.h>
#include <fcntl.h>
#include <direct.h>

// windows
#include <strsafe.h>
#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>