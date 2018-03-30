#ifndef __SET_UI_DATA_DEFINE_H__
#define __SET_UI_DATA_DEFINE_H__

#include "ylstring.h"
#include "innermsgdefine.h"

class xComboBox;
class CSettingUICustomComboBoxItemData;

#define MIN_SETTING_SOFTKEY_NUM         4
#define SETTING_NOTE_MSGBOX_SHOW_SHORT_TIME         1500
#define SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME        2000

#if IF_FEATURE_METASWITCH
typedef void (*FunMTSWLoginReturnCallBack)(bool bLoginSucceed);
#endif

// 界面类型
enum SETTINGUI_PAGE_TYPE
{
    PAGE_TYPE_BASE = 0x0001,
    PAGE_TYPE_SUBMENU = PAGE_TYPE_BASE + 1,         // T48、T49类型界面，左侧列表界面
    PAGE_TYPE_SUBPAGE = PAGE_TYPE_BASE + 2,         // T48、T49类型界面，右侧列表界面
    PAGE_TYPE_TILE = PAGE_TYPE_BASE + 3,            // 平铺界面
    PAGE_TYPE_FULLSCREEN = PAGE_TYPE_BASE + 4,      // 全屏显示界面
    PAGE_TYPE_VC_SUBMENU = PAGE_TYPE_BASE + 5,      // VC11X上Setting和开机引导最左侧界面
};

// Setting界面Action 类型
enum SETTING_ACTION_TYPE
{
    SET_ACTION_TYPE_BASE    = 0x20,
    SET_ACTION_TYPE_CLICK,              // 点击事件
    SET_ACTION_TYPE_FOCUS,              // 焦点事件
    SET_ACTION_TYPE_SAVE,               // 保存
    SET_ACTION_TYPE_BACK,               // 返回
    SET_ACTION_TYPE_OPTION,             // 选项
    SET_ACTION_TYPE_PAGING,             // 组播
    SET_ACTION_TYPE_EDIT,               // 编辑
    SET_ACTION_TYPE_DELETE,             // 删除
    SET_ACTION_TYPE_DELETEALL,          // 删除所有
    SET_ACTION_TYPE_CANCLE,             // 取消
    SET_ACTION_TYPE_START,              // 开始
    SET_ACTION_TYPE_LOGOUT,             // 退出
    SET_ACTION_TYPE_LOGIN,              // 登录
    SET_ACTION_TYPE_SKIP,               // 取消
    SET_ACTION_TYPE_PREVIOUS,           // 开始
    SET_ACTION_TYPE_NEXT,               // 退出
    SET_ACTION_TYPE_FINISH,             // 登录
};

// Setting二级界面
enum SETTING_TWO_LEVEL_AUTTHORITY
{
    SETTING_TWO_LEVEL_AUTTHORITY_BASE   = 0x01,
    SETTING_TWO_LEVEL_AUTTHORITY_ADMIN,
    SETTING_TWO_LEVEL_AUTTHORITY_USER,
};

// 打开Custom ComboBox额外数据结构
struct CustomCmbExtraData
{
public:
    CustomCmbExtraData() : m_pCmb(NULL), m_pCmbData(NULL), m_strPrePageID("") {}
    ~CustomCmbExtraData()
    {
        m_pCmb = NULL;
        m_pCmbData = NULL;
        m_strPrePageID = "";
    }

    void Reset()
    {
        m_pCmb = NULL;
        m_pCmbData = NULL;
        m_strPrePageID = "";
    }

public:
    xComboBox               *               m_pCmb;
    CSettingUICustomComboBoxItemData    *   m_pCmbData;     // Item Data
    yl::string                              m_strPrePageID; // 打开ComboBox界面ID
};

// 弹出框消息ID
enum
{
    MSG_ID_VALID_DATA           = 100,      // 数据校验失败 弹出框ID
    MSG_ID_NETWORK_CHANGED      = 101,      // 网络改变 弹出框ID
    MSG_ID_REBOOT               = 102,      // 确认是否重启 弹出框ID
    MSG_ID_RESET                = 103,      // 确认是否恢复出厂 弹出框ID
    MSG_ID_REBOOTING            = 104,      // 重启中 提示框ID
    MSG_ID_RESETTING            = 105,      // 恢复出厂 提示框ID
    MSG_ID_FIRWARE_UPDATING     = 106,      // 升级中 提示框ID
    MSGBOX_ID_SAVE              = 107,      // 保存 提示框ID
    MSGBOX_ID_SAVE_AND_REFRESH  = 108,      // 保存且重新加载数据 提示框ID
    MSG_ID_UPLOAD_CONFIG        = 109,      // 导入配置
};

// check method type
const char * const kszCheckMethodTypeIP = "ip";
const char * const kszCheckMethodTypeIPv4 = "ipv4";
const char * const kszCheckMethodTypeIPv4NetMask = "ipv4netmask";
const char * const kszCheckMethodTypeIPv6 = "ipv6";
const char * const kszCheckMethodTypeIPv6GateWay = "ipv6gateway";
const char * const kszCheckMethodTypeIPv6Prefix = "ipv6prefix";
const char * const kszCheckMethodTypePort = "port";
const char * const kszCheckMethodTypeDTMF = "dtmf";
const char * const kszCheckMethodTypeNumber = "number";
const char * const kszCheckMethodTypeAES = "aes";
const char * const kszCheckMethodTypeEmpty = "notempty";
const char * const kszCheckMethodTypeURL = "url";
const char * const kszCheckMethodTypeServer = "server";
const char * const kszCheckMethodTypeServerName = "servername";
const char * const kszCheckMethodTypeAccountName = "accountname";
const char * const kszCheckMethodTypePwd = "password";
const char * const kszCheckMethodTypeQos = "qos";
const char * const kszCheckMethodTypeVideoMTU = "videomtu";
const char * const kszCheckMethodTypeVlanVid = "vlanvid";
const char * const kszCheckMethodTypePacketInterval = "packetinterval";
const char * const kszCheckMethodTypeServerExpires = "serverexpires";
const char * const kszCheckMethodTypeUdpPort = "udpport";
const char * const kszCheckMethodTypeConferencePwd = "conferencepwd";

#endif // __SET_UI_DATA_DEFINE_H__

