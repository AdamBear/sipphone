
#ifndef __Q_LINE_EDIT_UTILITY_H__
#define __Q_LINE_EDIT_UTILITY_H__

#include <QLineEdit>
#include <ylstring.h>
#define  DFAULT_T48_EDIT_WITH 246
#define  DFAULT_T48_EDIT_HEIGHT 36

//输入类型
enum EDIT_TYPE
{
    EDIT_TYPE_NAME              = 0,        //包括用户名，显示名等
    EDIT_TYPE_SERVER,                       //server name/url,as time.windows.com
    EDIT_TYPE_PASSWORD,                 //密码
    EDIT_TYPE_UNLOCKPIN,                // 键盘锁解锁密码
    EDIT_TYPE_PHONE_NUMBER,             //本地使用的电话号码，如forward to,family
    EDIT_TYPE_IPV4,                     //Ipv4
    EDIT_TYPE_IPV6,                    //Ipv6
    EDIT_TYPE_IP_PORT,                  //ip商品
    EDIT_TYPE_IPV4_NETMASK,             //ipv4掩码
    EDIT_TYPE_IPV6_NETMASK,             //ipv6掩码
    EDIT_TYPE_IPV6_GATEWAY,             //ipv6网关
    EDIT_TYPE_IP_SERVER,                //lan server?
    EDIT_TYPE_DIAL,                     //Idle界面的拨号
    EDIT_TYPE_MAIL,                     //email
    EDIT_TYPE_NUMBER,                       //数字，一般是时间限制
    EDIT_TYPE_DTMF,                     //DTMF类型，只能输入 数字/*/#
    EDIT_TYPE_URL,                  //url类型，用于输入长度限制
    EDIT_TYPE_AES,                  //AES界面使用，用于输入长度限制
    EDIT_TYPE_DIAL_PLAN,
    EDIT_TYPE_PROXY,
    EDIT_TYPE_NUMBER_SHORT,
    EDIT_TYPE_NAME_SHORT,           //限制32个字符的名字
    EDIT_TYPE_PHONE_NUMBER_LONG,    //最长为99个字符的号码
    EDIT_TYPE_CODE,                 //oncode,offcode
    EDIT_TYPE_ACCOUNT_NAME,      //账号相关的显示名.注册名等
    EDIT_TYPE_CONTACT_NAME,      //联系人的名字
    EDIT_TYPE_CONTACT_NUMBER,    //联系人的号码
    EDIT_TYPE_SIP_SERVER,        //sip服务器，限制!&
    EDIT_TYPE_BSFT_NAME,            //BSFT名字，限制/\<>'"
    EDIT_TYPE_CUSTOM,            //自定义输入法
    EDIT_TYPE_DSSKEY_VALUE,         // DssKey Value限制
    EDIT_TYPE_LOWER,                // 从Passwd细分而来，小写，为了和密码区分
    EDIT_TYPE_NUMBER_PWD,           // 密码的数字输入法
    EDIT_TYPE_NAME_PWD,             // 密码的名字输入法
    EDIT_TYPE_WIFI_SSID_PROFILE,   // 32字符
    EDIT_TYPE_WIFI_PWD,            // 64
    EDIT_TYPE_WIFI_WEP_PWD,        // 16
    EDIT_TYPE_LOCAL_FAVORITE_INDEX,     //favorite功能
};

#define MAX_LEN_WIFI_SSID       32  // WIFI SSID Profile
#define MAX_LEN_WIFI_PWD        64  // WIFI Pwd
#define MAX_LEN_WIFI_WEP_PWD    15  // WIFI Wep Pwd
#define MAX_LEN_URL             511 //url，最大长度512
#define MAX_LEN_DISPLAY_NAME    99  // 用户名、显示名最大长度
#define MAX_LEN_DISPLAY_NAME_SHORT      32  // 显示名
#define MAX_LEN_SERVER_NAME     99 //server name,as time.windows.com
#define MAX_LEN_PASSWORD        32  // 密码相关最大长度
#define MAX_LEN_UNLOCKPIN       15  // 键盘锁相关最大长度
#define MAX_LEN_PHONE_NUMBER    32  //存储的号码长度
#define MAX_LEN_PHONE_NUMBER_LONG   99  //存储的号码长度
#define MAX_LEN_IP              99  // IP相关最大长度，port除外（增加了IPv6，参照T2X,设置为99）
#define MAX_LEN_PORT            5   // 端口最大长度
#define MAX_LEN_DIAL            32  // Idle拨号最大长度
#define MAX_LEN_MAIL            50  // email地址
#define MAX_LEN_NUMBER          3   //输入整数，一般为时间设置
#define MAX_LEN_NUMBER_SHORT    2   //两位整数，如hotlinedelay
#define MAX_LEN_AES             16  //AES界面的输入框限制16位
#define MAX_LEN_DTMF            32  //DTMF类型使用
#define MAX_LEN_PROXY           255 //OutboundProxy
#define MAX_LEN_BSFT_NAME       50  //BSFT Personal名字最大长度
#define MAX_LEN_BSFT_NUMBER     23  //BSFT Number最大长度
#define MAX_LEN_DSSKEY_VALUE    99  //DssKey Value最大长度
#define MAX_LEN_HOTELING_PASSWORD   10 //hoteling or flexible seating pwd lenth
#define MAX_LEN_LOCAL_FAVORITE_INDEX        9   //favorite功能index最大长度

class qLineEditUtility
{
public:
    qLineEditUtility(void);
    ~qLineEditUtility(void);

public:
    /************************************************************************/
    /*                          Set QLineEdit for T3X                          */
    /************************************************************************/
    static void SetLineEditStyle(QLineEdit * pEdit, const QColor & color/* = Qt::black */,
                                 const QColor & clrHightLight = Qt::darkBlue,
                                 const QColor & clrHightLightText = Qt::white);

    /************************************************************************/
    /*                            Set QLineEditTransparent                  */
    /************************************************************************/
    static void SetLineEditTransparent(QLineEdit * pEdit);

    /********************************************************
    设置QlineEdit的显示效果，这里只是设置其disable时的字体颜色
    *********************************************************/
    static void SetLineEditStype(QLineEdit * pEdit);

    /********************************************************
    函数名称：SetCurrentInputMethod
    函数意义：设置LineEdit载入界面时的首选输入法
    返回值：无返回值
    *********************************************************/
    static void SetCurrentInputMethod(QLineEdit * pEdit, EDIT_TYPE eType);

    //根据输入框的类型设置初始输入法
    //根据输入框的类型进行输入长度的限制
    static void SetMaxLength(QLineEdit * pEdit, const EDIT_TYPE & eType);

    //根据输入框类型判断输入是否合法
    static bool IsFormatValid(const QString & strQInput, const EDIT_TYPE & eType);

private:
    //判断各种类型的输入是否合法
    static bool IsValidIPv4(const yl::string & strValue);
    static bool IsValidIPv6(const yl::string & strValue);
    static bool IsValidPort(const yl::string & strValue);
    static bool IsValidIPv4NetMask(const yl::string & strValue);
    static bool IsValidIPv6NetMask(const yl::string & strValue);
    static bool IsValidIPv6GateWay(const yl::string & strValue);
    static bool IsValidServer(const yl::string & strValue);
    static bool IsValidSipServer(const yl::string & strValue);

    static bool IsValidName(const yl::string & strValue);

    static bool IsValidServerName(const yl::string & strValue);

    static bool IsValidPassword(const yl::string & strValue);

    static bool IsValidPhoneNumber(const yl::string & strValue);

    static bool IsValidDial(const yl::string & strValue);

    static bool IsValidMail(const yl::string & strValue);

    static bool IsValidDTMF(const yl::string & strValue);

    static bool IsValidNumber(const yl::string & strValue);
    static bool IsValidURL(const yl::string & strValue);
    static bool IsValidAES(const yl::string & strValue);
    static bool IsValidOnOffCode(const yl::string & strValue);
    static bool IsValidAccountName(const yl::string & strValue);
    static bool IsValidContactName(const yl::string & strValue);
    static bool IsValidContactNumber(const yl::string & strValue);
    static bool IsValidBSFTName(const yl::string & strValue);
};

#endif
