#ifndef __IDLE_DATA_DEFINE_H__
#define __IDLE_DATA_DEFINE_H__

// Phone state.
enum PhoneState
{
    PS_STATE_NONE = 0,
    PS_STATE_IDLE = 1,

    // Status
    PS_STATE_DND = 2,
    PS_STATE_AA = 3,
    PS_STATE_FWD = 4,
    PS_STATE_LOCK = 5,
    PS_STATE_MUTE = 6, //only for t20
    PS_STATE_RING_MUTE = 7,
    PS_STATE_VPN = 8,
    PS_STATE_SWITCH_ACCOUNT = 9,  // 切换账号.

    PS_STATE_NETWORK_FAILED = 10,
    PS_STATE_NETWORK_SUCCEED = 11,
    PS_STATE_MISSCALL = 12,
    PS_STATE_VOICEMAIL = 13,
    PS_STATE_TEXTMESSAGE = 14,
    PS_STATE_CALLFORWARD = 15,

    PS_STATE_REFLASH_STATE = 16,
    PS_STATE_HEADSET = 17,

    PS_STATE_REMOTE_CONTACT_DOWNLOAD_FAILED = 18,

    PS_STATE_CALL_COMPLETION = 19,
    PS_STATE_AUTO_PROVISION = 20,
    PS_STATE_AUTO_PROVISION_ERROR = 21,

    PS_STATE_CTR_REQUEST, // Remote control request.

    PS_STATE_INTERNET,
    PS_STATE_CABLE,

    PS_STATE_PICKUP_MENU, // Pickup menu.
    PS_STATE_BLUETOOTH,
    PS_STATE_BLUETOOTH_CONNECT,
    PS_STATE_BLUETOOTH_CONNECTING,
    PS_STATE_BLUETOOTH_MOBILECONNECT,
    PS_STATE_BLUETOOTH_MOBILE_ASK_SYNC_DIRECTORY,

    PS_STATE_WIFI,
    PS_STATE_WIFI_CONNECT,
    PS_STATE_WIFI_CONNECTTING,
    PS_STATE_WIFI_CONNECTOVERTIME,
    PS_STATE_WIFI_CONNECT_FAIL,

    PS_STATE_CABLE_REMOVE,
    PS_STATE_CABLE_ADD,
    PS_STATE_WIFI_ADAPTER_REMOVE,
    PS_STATE_WIFI_ADAPTER_ADD,

    PS_STATE_USB_CONNECT,
    PS_STATE_USB_CONNECTING,
    PS_STATE_USB_DISCONNECT,
    PS_STATE_USB_RECORD_SUCCESS,

    PS_STATE_NOTE_HOTDESK,  // HotDesking base

    // 插入移除adapter的提示
    PS_STATE_ADAPTER_ADD,
    PS_STATE_ADAPTER_REMORE,
    PS_STATE_HOTELING_CHANGE,
    PS_STATE_NETWORK_OK_BUT_CONFLICT,
    PS_STATE_ACD_CHANGE,
    PS_VQ_REPORT_ACTIVE,
    PS_STATE_CALL_COMPLETION_CALLBACK,
    PS_STATE_TALK_MUTE,
    PS_STATE_USBDEVICE,
    PS_STATE_USBDEVICE_DETECTING,
    PS_STATE_USBDEVICE_DETECTFAILED,
    PS_STATE_IDLE_RECORD,
    PS_STATE_IDLE_RECORD_PAUSE,
    PS_STATE_SAVE_RECORD_FILE,

    PS_STATE_CAMERA_DISCONNECT,
    PS_STATE_CAMERA_CONNECT, // 摄像头有连接
    PS_STATE_CAMERA_READY,//摄像头可用

    PS_STATE_HDMI_CONNECT, // 扩展屏已连接
    PS_STATE_HDMI_DISCONNECT,

    PS_STATE_EXP_RECOGNITION,//扩展台识别失败提示

    PS_STATE_ALWAYS_FWD = 500,
    PS_STATE_BUSY_FWD,
    PS_STATE_NOANW_FWD,

    PS_STATE_HOTELING_GUESTOUT,
    PS_STATE_HOTELING_GUESTOUT_CHECK,

    PS_STATE_NATWORK_NAT,   // 启用nat模式
    PS_STATE_PSTN_ACCOUNT_ICON,
    PS_STATE_PSTN_ACCOUNT_NOTE,

    PS_STATE_DATE_TIME_MANUAL, // 手动设置时间日期提示

    PS_STATE_AUTO_PROVISION_UPDATING,       //更新中
    PS_STATE_AUTO_PROVISION_UPDATE_FAILED,  //更新失败
    PS_STATE_AUTO_PROVISION_UPDATE_SUCCESS, //更新成功
    PS_STATE_HOTELING_LOGOUT_PROMPT,
    PS_STATE_HOTELING_LOGIN_SUCCESSFUL,
    PS_STATE_HOTELING_LOGOUT_SUCCESSFUL,
    PS_STATE_USBHEADSET_PLUG_IN,    // usb耳麦插入
    PS_STATE_USBHEADSET_PLUG_OUT,   // usb耳麦拔出
    PS_STATE_E911_ERROR,

    PS_STATE_FLEXIBLE_UPDATE,
    PS_STATE_FLEXIBLE_UPDATE_END,

    PS_STATE_BALANCE_LOADING,
    PS_STATE_BALANCE_RESULT,
    PS_STATE_BALANCE_LOAD_ERROR,

    PS_STATE_VOICE_FORBIDDEN,

    PS_STATE_ACCOUNT,
    PS_STATE_E911_LOCATION,
    PS_STATE_WARNING,

    PS_STATE_MAX
};

// 弹窗状态.
enum PopupWindowType
{
    POPUP_WINDOW_TYPE_NONE = 0,  // 未定义.
    POPUP_WINDOW_POPUP = 1,  // 弹.
    POPUP_WINDOW_POPUPING = 2,  // 正在弹.
    POPUP_WINDOW_NO_POPUP = 3,  // 不需要弹.
    POPUP_WINDOW_TEXT_NOTIFY = 4,  //idle文本式弹出
    POPUP_WINDOW_TYPE_MAX
};

// 弹窗状态.
enum StateNotifyType
{
    SNT_NONE = 0,                   //未定义
    SNT_TEXT_NOTIFY = 0x1 << 1,     //文本通知类型
    SNT_POPUP = 0x1 << 2,           //弹窗类型
    SNT_SOFTKEY = 0x1 << 3,         //softkey类型
    SNT_ICON = 0x1 << 4,            //图标类型
    SNT_ACCOUNT = 0x1 << 5,         // 账号类型
    SNT_END = 5,
    SNT_ALL = 0xffffffff
};

struct NotifyTypePriority
{
    StateNotifyType m_eNotifyType;  //状态通知类StateNotifyType枚举
    int
    m_Priority;         // 在链表中的位置, 从1开始, 如果为0, 则表示不固定顺序.
    NotifyTypePriority()
    {
        m_eNotifyType = SNT_NONE;
        m_Priority = 0;
    }
};
typedef YLVector<NotifyTypePriority> VecNotifyTypePriority;  //类型优先级

// Shortcut key type
enum ShortcutKeyType
{
    SHORTCUT_KEY_TYPE_NONE = 0,  // No shortcut key type.
    SHORTCUT_KEY_HISTORY = 1,  // history.
    SHORTCUT_KEY_CONTACTS = 2,  // contact.
    SHORTCUT_KEY_MESSAGE = 3,  // message.
    SHORTCUT_KEY_DND = 4,  // DND.
    SHORTCUT_KEY_CREATE_MESSAGE = 5,  // Create message.
    SHORTCUT_KEY_FORWARD = 6,  // Forward.
    SHORTCUT_KEY_SWITCH_ACCOUNT = 7,  // Switch account.
    SHORTCUT_KEY_LOCK = 8,  // Lock
    SHORTCUT_KEY_DAILED_CALLS = 9,  // Dialed calls.
    SHORTCUT_KEY_MENU = 10,  // Menu.
    SHORTCUT_KEY_STATUS = 11,  // Status.
    SHORTCUT_KEY_SPEED_DIAL = 12,  // For t80.
    SHORTCUT_KEY_XML_PHONE_BOOK = 13,  // Xml phone book.
    SHORTCUT_KEY_INTERCOM = 14,  // For t6x.
    SHORTCUT_KEY_SWITCH_TO_NEXT_ACCOUNT = 15,  // Switch to next account.
    SHORTCUT_KEY_SWITCH_TO_LAST_ACCOUNT = 16,  // Switch to last account.
    SHORTCUT_KEY_TYPE_MAX
};

//网络类型
enum NETWORK_STATUS
{
    NS_UNKNOWN = -1,
    NS_CHECKING_MAC,            // 检测MAC中
    NS_INVALID_MAC,             // MAC地址不正确

    NS_UPDATING,                // 网络更新中, 则网络模块来控制的

    NS_CABLE_DISCONNECTED,      // 网线未接

    NS_REQUEST_IP,              // 获取IP

    NS_IP_CONFLICT,             // IP冲突

    NS_ALL_IP_CONFLICT,         // IPV4/IPV6冲突
    NS_IPV4_CONFLICT,           // IPV4冲突
    NS_IPV6_CONFLICT,           // IPV6冲突

    NS_OK,                      // 网络一切正常
    NS_NETWORK_STOP,            //WIFI 网络不可用
};

#ifdef _T49
enum IDLE_FLEXIBLE_MESSAGE
{
    FLEXIBLE_GUESTOUT_MESSAGE = INNER_MSG_FLEXIBLE_MESSAGE_BEGIN,
    FLEXIBLE_GUESTLOCK_MESSAGE,
    FLEXIBLE_UNLOCK_MESSAGE,
    FLEXIBLE_MESSAGE_END = INNER_MSG_FLEXIBLE_MESSAGE_END,
};
#endif

enum IDLE_TEXT_COLOR
{
    IDLE_TEXT_WHITE = 0,
    IDLE_TEXT_BLACK = 1,
};

struct PhoneStateData
{
    PhoneState m_eStatus;  // 状态id.
    yl::string m_strIcon;  // 状态图标.
    yl::string m_strHint;  // 状态提示.
    PopupWindowType m_nPopupType;  // 是否需要弹出.

    PhoneStateData()
    {
        Reset();
    }

    void Reset()
    {
        m_eStatus = PS_STATE_NONE;
        m_strIcon.clear();
        m_strHint.clear();
        m_nPopupType = POPUP_WINDOW_TYPE_NONE;
    }
};

// The max top number of phone states.
static const int knMaxTopPhoneStateNumber = 20;

// struct TopPhoneStateList, top of phone states list.
struct TopPhoneStateList
{
    PhoneStateData m_StatusList[knMaxTopPhoneStateNumber];  // The states get.
    int m_nStateCountToGet;  // The number of state to get.

    TopPhoneStateList()
    {
        Reset();
    }

    void Reset()
    {
        for (int i = 0; i < knMaxTopPhoneStateNumber; ++i)
        {
            m_StatusList[i].Reset();
        }
        m_nStateCountToGet = 0;
    }
};

#endif //__IDLE_DATA_DEFINE_H__
