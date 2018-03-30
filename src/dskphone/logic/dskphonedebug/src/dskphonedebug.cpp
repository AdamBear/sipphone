#include "msgkit/mkhelper.h"
#include "dskphonedebug.h"
#include "dskphonedebugdefine.h"
#include "commonunits/debugfunc.h"

#ifdef IF_FEATURE_TOOLS

#ifdef __cplusplus
extern "C"
{
#endif

/************************************************************************/
/*                          static function                             */
/************************************************************************/
static int cmd_dump_uptime(dskphone_debug_t * dskphone_debug, int argc, const char * argv[])
{
    debug_PrintUptime(NULL);
    return 0;
}

static int cmd_dump_mem_info(dskphone_debug_t * dskphone_debug, int argc, const char * argv[])
{
    debug_PrintMemStatic();
    return 0;
}

static int cmd_dump_flash_info(dskphone_debug_t * dskphone_debug, int argc, const char * argv[])
{
    debug_PrintFlashStatic();
    return 0;
}

static int cmd_dump_net_info(dskphone_debug_t * dskphone_debug, int argc, const char * argv[])
{
    debug_PrintNetInfo();
    return 0;
}

static int cmd_debug_msg_queue(dskphone_debug_t * dskphone_debug, int argc, const char * argv[])
{
    debug_PrintQueue();
    return 0;
}

static int cmd_debug_msg_timer(dskphone_debug_t * dskphone_debug, int argc, const char * argv[])
{
    debug_PrintTimer();
    return 0;
}

static int cmd_tool_map_msg(dskphone_debug_t * dskphone_debug, int argc, const char * argv[])
{
    debug_Tool_Map_Msg(argv[0]);
    return 0;
}

static int cmd_tool_upgrade_rom(dskphone_debug_t * dskphone_debug, int argc, const char * argv[])
{
    debug_Tool_Upgrade_Rom(argv[0]);
    return 0;
}

static int cmd_tool_get_cfg(dskphone_debug_t * dskphone_debug, int argc, const char * argv[])
{
    debug_Tool_Get_Cfg(argv[0]);
    return 0;
}

static int cmd_dump_logic_info(dskphone_debug_t *dskphone_debug, int argc, const char* argv[])
{
	debug_dump_logic_info(dskphone_debug->handle, argv[0]);
	return 0;
}

/************************************************************************/
/*                          define                                      */
/************************************************************************/
static mkcmd_t cmd_table[] =
{
    /////////////////////////debug/////////////////////////////////////////////////
    // 系统相关信息获取
    // debug
    {
        0, (mkcmd_cb)cmd_dump_uptime,
        "dump_uptime",
        "",
    },

    // debug
    {
        0, (mkcmd_cb)cmd_dump_mem_info,
        "dump_mem",
        "",
    },

    // debug
    {
        0, (mkcmd_cb)cmd_dump_flash_info,
        "dump_flash",
        "",
    },

    {
        0, (mkcmd_cb)cmd_debug_msg_queue,
        "dump_msg",
        "",
    },

    {
        0, (mkcmd_cb)cmd_debug_msg_timer,
        "dump_timer",
        "",
    },

    {
        0, (mkcmd_cb)cmd_dump_net_info,
        "dump_net",
        "",
    },

    /////////////////////////tool/////////////////////////////////////////////////
    // 方便调试、分析的工具接口
    {
        1, (mkcmd_cb)cmd_tool_map_msg,
        "tool_map_msg",
        "input msg value",
    },

    {
        1, (mkcmd_cb)cmd_tool_upgrade_rom,
        "tool_upgrade_rom",
        "input rom path",
    },

    {
        1, (mkcmd_cb)cmd_tool_get_cfg,
        "tool_get_cfg",
        "input configmap key",
    },


	/////////////////////////logic/////////////////////////////////////////////////
	// ...补充业务
	{
		1, (mkcmd_cb)cmd_dump_logic_info,
		"dump_logic",
		"dump logic info \n"
		"dump logic module type flag\n"
		"type	:\n"
		"1 - 99    : app\n"
		"101 - 199   : config\n"
		"500 - 999   : talk\n"
		"1000 - 1499 : contact \n"
		"1500 - 1999 : UI\n"
		,
	},
};

/************************************************************************/
/*                          public function                             */
/************************************************************************/
static int dskphone_cmd_console(dskphone_debug_t * dskphone_debug, const char * cmdline)
{
    // call on command list
    if (NULL != cmdline)
    {
        return mkcon_exe(dskphone_debug, cmdline, cmd_table, ARRAYSIZE(cmd_table));
    }
    else
    {
        return mkcon_run(dskphone_debug, "app$ ", cmd_table, ARRAYSIZE(cmd_table));
    }
}

int dskphone_debug_run(void)
{
    dskphone_debug_t dskphone_debug;
    memset(&dskphone_debug, 0, sizeof(dskphone_debug_t));

    // create service as debug
    if (mkit_create2(&dskphone_debug.handle, NULL, 32, 32 * 1024, 8 * 1024, 4, NULL,
                     MKIT_CFL_EXC_BCAST | MKIT_CFL_EXC_REGS) < 0)
    {
        printf("create failed %m\n");
        return -1;
    }

    // Init service
    if (mkservice_init(NULL, 0) < 0)
    {
        printf("init failed for debuger,%m\n");
        return -1;
    }

    return dskphone_cmd_console(&dskphone_debug, NULL);
}

#ifdef __cplusplus
}
#endif

#endif //#ifdef IF_FEATURE_TOOLS
