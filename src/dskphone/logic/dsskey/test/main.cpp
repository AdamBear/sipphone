#include "../include/moddsskey.h"
#include "../include/dsskeystructure.h"
#if LINUX_SYSTEM
#include <gtest/gtest.h>
#include <dsklog/log.h>
bool g_bAfterInit = false;
int main(int argc, char * argv[])
{
	devicelib_Init(false);
	testing::InitGoogleTest(&argc, argv);
	LOG_INIT("LOG", DBG_LOG_MODE_TIME | DBG_LOG_MODE_SYS | DBG_LOG_MODE_CONS, NULL);
	LOG_SET(DBG_LOG_ALL, DBG_LOG_LEVEL_INFO);
	RUN_ALL_TESTS();
	printf("test done\n");
	return 1;
}
#endif
