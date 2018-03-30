#ifndef __LOGIC_AND_UI_COMMON_H__
#define __LOGIC_AND_UI_COMMON_H__

#include <ylstring.h>
#include "yllist.h"
#include "ylvector.h"
#include "ylalgorithm.h"
#include "utility.hpp"
#include "ETLLib.hpp"
#include <innermsgdefine.h>
#if IF_BUG_31595
#include "dsskey/include/dsskeystructure.h"
#endif
// logic和ui公用部分

#ifdef IF_FEATURE_XMLBROWSER
#define XML_BROWSER_SUPPORT_UI  1
#endif
//
enum MsgBox_Type
{
    MT_NONE = 0,
    MT_YESNO,
    MT_OKCANCEL,
    MT_CANCELONLY,
    MT_RELEASE
};

#define INVALID_ELEMENT_ID  -1

// 每页的softkey个数
#define  SOFTKEY_NUM      4
// 退出界面的默认时间(s)
#define  DEFAULT_TIMEOUT  45

// 一行最多包含多少个字符，要根据最小字体进行评估，相差不大即可，不用精确。
#define MAX_LINE_CHARS    128

// 压缩后的图片路径
#define FILE_COMPRESSED_IMAGE  TEMP_PATH "XMLCompressImage.png"

//// XML Browser模块内部消息
//enum XB_INNER_MSG
//{
//  XB_MSG_FIRST = INNER_MSG_LAST - 50,
//  XB_MSG_UPDATE,   // 更新界面
//  XB_MSG_UPDATE_SOFTKEY, // 只更新softkey
//  XB_MSG_MSGBOX,   // 画提示框
//  XB_MSG_PRESSKEY, // 按键处理
//  XB_MSG_PLAYBEEP,   // 进入界面播放提示音
//  XB_MSG_PROCESSURI, // URI处理放到主线程中
//  XB_MSG_DESTROY_DSSKEY,
//  XB_MSG_LAST
//};

// Element类型
enum XML_ELEMENT_TYPE
{
    XMT_UNKNOWN = 0,            // 未知
    XMT_MENU,                   // 菜单
    XMT_TEXTSCREEN,             // 文本显示
    XMT_INPUT,                  // 输入
    XMT_DIRECTORY,              // 联系人
    XMT_EXECUTE,                // 可执行
    XMT_STATUS,                 // 状态
    XMT_CONFIGURATION,          // 配置
    XMT_IMAGEMENU,              // 图片菜单
    XMT_IMAGESCREEN,            // 图片显示
    XMT_FORMATTEDTEXTSCREEN,    // 按指定格式显示文本

#ifdef IF_FEATURE_GENBAND
    XMT_INTERFACE,              //开放http编辑接口格式解析
#endif
#if IF_FEATURE_XMLB_SCREENSAVER
    XMT_SCREENSAVER,                // 屏保
#endif

};

//解决XMLParseData.pData的内存泄露隐患
struct XMLLogicData
{
    virtual ~XMLLogicData() {};
};

#if IF_BUG_32431
// 自动刷新数据
struct XMLRefreshData
{
    XMLRefreshData()
    {
        Reset();
    }
    void Reset()
    {
        nTimeOut = -1;
        strURL = "";
    }

    int nTimeOut;
    yl::string strURL;
};
#endif

// 对象解析数据定义
struct XMLParseData
{
    XMLParseData()
    {
        pData = NULL;
        eType = XMT_UNKNOWN;
        nDsskeyId = -1;
#if IF_BUG_32431
        sRefreshData.Reset();
#endif
    }

    XML_ELEMENT_TYPE eType;     // 对象类型
    XMLLogicData      *      pData;     // 对象结构体指针
    int nDsskeyId;
#if IF_BUG_32431
    XMLRefreshData sRefreshData;
#endif
};

////----qt
// UI界面类型
enum XB_UI_TYPE
{
    XB_UI_TYPE_NONE = 0,
    XB_UI_TYPE_IDLE,
    XB_UI_TYPE_DLG,
};

struct XMLRegionData
{
    bool             bFull;
    chRect           regionRect;  // region的位置
    XML_ELEMENT_TYPE eElementType;// region的类型
    void      *      pElementData;// region对应的数据

    XMLRegionData()
    {
        eElementType = XMT_UNKNOWN;
        pElementData = NULL;
        bFull        = true;
    }

    ~XMLRegionData()
    {

    }
};
typedef YLList<XMLRegionData> ListRegionData;
typedef ListRegionData::const_iterator IterRegionData;

// 逻辑层传递给UI层的数据
struct XMLLogicToUIData
{
    XMLLogicToUIData()
    {
        eElementType = XMT_UNKNOWN;
        nElementID = INVALID_ELEMENT_ID;
        pElementData = NULL;
    }

    XML_ELEMENT_TYPE eElementType;
    int              nElementID;
    void      *      pElementData;

    /*  ListRegionData listRegionData;
        int            iCurrentRegion;
        int            iTimeOut;
        bool           bFullScreen;

        XMLLogicToUIData()
        {
            iCurrentRegion = 0;
            iTimeOut = DEFAULT_TIMEOUT;
            bFullScreen = false;
        }

        ~XMLLogicToUIData()
        {

        }
        */
};

// Action类型
enum XB_ACTION_TYPE
{
    XB_ACTION_NONE = -1,
    XB_ACTION_SOFTKEY = 99,  // softkey处理 参数：softkey index, focuseditem index

    XB_ACTION_EXIT,          // 退出当前UI  参数：none
    XB_ACTION_SELECT,        // 选择事件    参数：选择序号

    XB_ACTION_URI,           // 执行URI，参数为URI的字符串const char*
    XB_ACTION_DIAL,          // 参数为焦点序号

    XB_ACTION_LINEKEY_CALLOUT,  // Linekey呼出, wParam: KeyIndex, lParam: FocusIndex，pExtraData: NULL

    XB_ACTION_PREVIOUS,      // 上一层级
    XB_ACTION_NEXT,          // 下一层级

    XB_ACTION_DIGITKEY,      // 数字键事件  参数：按键字符0-9,* and #
    XB_ACTION_FUNCKEY,       // 功能键
    XB_ACTION_EXIT_ALL,      // 退出所有UI  参数：none

    XB_ACTION_FOCUSCHANGE,   // input界面焦点改变  参数：改变后的焦点（从0开始）

    XB_ACTION_CONFIRM,       // ok键处理    参数：focuseditem index, iuput界面传满屏的数据
    XB_ACTION_CANCEL,        // cancel键处理

    XB_ACTION_DOWNLOAD_DONE, // 下载完成

    XB_ACTION_COMPRESS,      // 图片解压     参数：
    XB_ACTION_COMPRESS_DONE, // 图片解压完成 参数：

    XB_ACTION_CURRENT_REGION_CHANGE,////----add new type
    XB_ACTION_HOOK_ON,      // 挂机
    XB_ACTION_HOOK_OFF,     // 摘机
    XB_ACTION_PRESS_HANDFREE,
    XB_ACTION_PRESS_STAR_AND_POUND,

    XB_ACTION_ST_EXIT = 0,     // SoftKey:Exit
    XB_ACTION_ST_SELECT,       // SoftKey:Select
    XB_ACTION_ST_DIAL,         // SoftKey:Dial
    XB_ACTION_ST_SUBMIT,       // SoftKey:Submit

    XB_ACTION_ST_PREVIOUS,     // SoftKey:Previous
    XB_ACTION_ST_NEXT,         // SoftKey:Next

    XB_ACTION_ST_MORE,         // SoftKey:More
    XB_ACTION_ST_DOT,          // SoftKey:Dot         UI处理
    XB_ACTION_ST_BACKSPACE,    // SoftKey:BackSpace   UI处理
    XB_ACTION_ST_NEXTSPACE,    // SoftKey:NextSpace   UI处理
    XB_ACTION_ST_CHANGEMODE,   // SoftKey:ChangeMode  UI处理
    XB_ACTION_ST_EMPTY,        // 空
    XB_ACTION_ST_FUNCTION,     // 其他功能http://、Dial:、Command等
#if IF_FEATURE_XMLDIRECTORY
    XB_ACTION_SEARCH_PHONEBOOK,// 联系人搜索
#endif
};

// 提示框文本类型
enum XB_TIP_TYPE
{
    XB_TIP_UNKOWN,

    XB_TIP_LOADING, // cancel可以取消下载
    XB_TIP_LOADING_IMAGE,

    XB_TIP_LOAD_FAIL,
    XB_TIP_LOAD_IMAGE_FAIL,
    XB_TIP_NOT_EXIST,
    XB_TIP_PARSE_FAIL,
    XB_TIP_INVAILID_URI,
    XB_TIP_LARGE_FILE,
    XB_TIP_LARGE_IMAGE,
    XB_TIP_NET_UNAVAILABLE,
    XB_TIP_ITEMS_OUT_OF_LIMIT,
    XB_TIP_NO_SUBMIT_URI,
    XB_TIP_INPUT_ERROR,

    XB_TIP_RESET,
    XB_TIP_REBOOT,

    XB_TIP_INVALID_IP,
    XB_TIP_INVALID_DATE_INT,
    XB_TIP_INVALID_DATE_US,
    XB_TIP_INVALID_TIME_US,
    XB_TIP_INVALID_TIME_INT,
    XB_TIP_INVALID_NUMBER,
    XB_TIP_COMPRESS_FAIL,

#ifdef IF_FEATURE_GENBAND
    XB_TIP_ACTIVATED,
    XB_TIP_NOT_ACTIVATED,
#endif
    XB_TIP_HOTELING_LOGIN_FAIL,
    XB_TIP_HOTELING_LOGOUT_FAIL,
    XB_TIP_CONFIG_TIPS,
};

// 错误类型
enum XB_ERROR_TYPE
{
    XE_OK = 0,
};


// Softkey类型
enum XB_SOFTKEY_TYPE
{
    XB_ST_UNKNOWN,      // Unknown

    XB_ST_EXIT,         // SoftKey:Exit
    XB_ST_SELECT,       // SoftKey:Select
    XB_ST_DIAL,         // SoftKey:Dial
    XB_ST_SUBMIT,       // SoftKey:Submit
    XB_ST_PREVIOUS,     // SoftKey:Previous
    XB_ST_NEXT,         // SoftKey:Next
    XB_ST_MORE,         // SoftKey:More
    XB_ST_DOT,          // SoftKey:Dot
    XB_ST_BACKSPACE,    // SoftKey:BackSpace
    XB_ST_NEXTSPACE,    // SoftKey:NextSpace
    XB_ST_CHANGEMODE,   // SoftKey:ChangeMode
    XB_ST_EMPTY,        // 空
    XB_ST_FUNCTION,     // 其他功能http://、Dial:、Command等
    XB_ST_SEARCH,       // Search
};

// 菜单项样式
enum MENU_ITEM_STYLE
{
    STYLE_NUMBERED = 0, // 菜单项前显示数字序号
    STYLE_NONE,         // 菜单项前不显示
    STYLE_RADIO,        // 菜单项前用圆圈标明
};

// input单元的输入类型
enum INPUT_TYPE
{
    INPUT_IP = 0,    // ip地址
    INPUT_STRING,    // 字符串
    INPUT_NUMBER,    // 数字
    INPUT_TIME_US,   // 时间 美国 12小时制，用AM,PM表示早上和下午
    INPUT_TIME_INT,  // 时间 国际 24小时制
    INPUT_DATE_US,   // 日期 美国格式 MM/DD/YYYY
    INPUT_DATE_INT,  // 日期 国际格式 DD/MM/YYYY
    INPUT_EMPTY,     // 空行 行数由displayMode确定
};

// input单元的显示模式
enum INPUT_DISPLAY_MODE
{
    MODE_NORMAL = 0, // 提示和输入框分两行显示
    MODE_CONDENSED,  // 提示和输入框在一行
};

// input单元的语言
enum INPUT_LANGUAGE
{
    INPUT_LANG_ENGLISH,
    INPUT_LANG_FRENCH,
    INPUT_LANG_GERMAN,
    INPUT_LANG_ITALIAN,
    INPUT_LANG_SPANISH,
};

// status单元的类型
enum STATUS_TYPE
{
    ST_ALERT = 0,
    ST_MESSAGE_NORMAL,
    ST_MESSAGE_DSSKEY,
};

// formatted text 中line的size
enum TEXT_FONT_SIZE
{
    TEXT_FONT_SIZE_SMALL,       // 小字号
    TEXT_FONT_SIZE_NORMAL,      // 正常字号
    TEXT_FONT_SIZE_DOUBLE,      // 两倍正常字号
    TEXT_FONT_SIZE_LARGE,       // 大字号
};

enum X_FONT_TYPE
{
    X_FONT_SMALL,
    X_FONT_NORMAL,
    X_FONT_DOUBLE,
    X_FONT_LARGE,
};

// formatted text 中line的align
enum TEXT_FONT_ALIGN
{
    TEXT_FONT_ALIGN_LEFT,       // 居左
    TEXT_FONT_ALIGN_CENTER,     // 居中
    TEXT_FONT_ALIGN_RIGHT,      // 居右
};

// formatted text 中line的color
enum TEXT_FONT_COLOR
{
    TEXT_FONT_COLOR_WHITE,
    TEXT_FONT_COLOR_BLACK,
    TEXT_FONT_COLOR_RED,
    TEXT_FONT_COLOR_GREEN,
    TEXT_FONT_COLOR_BROWN,
    TEXT_FONT_COLOR_BLUE,
    TEXT_FONT_COLOR_MAGENTA,        // 洋红色
    TEXT_FONT_COLOR_CYAN,           // 青色
    TEXT_FONT_COLOR_LIGHTGRAY,
    TEXT_FONT_COLOR_DARKGRAY,
    TEXT_FONT_COLOR_LIGHTRED,
    TEXT_FONT_COLOR_LIGHTGREEN,
    TEXT_FONT_COLOR_YELLOW,
    TEXT_FONT_COLOR_LIGHTBLUE,
    TEXT_FONT_COLOR_LIGHTMAGENTA,
    TEXT_FONT_COLOR_LIGHTCYAN,
};

// ImageScreen的显示方式
enum IMAGE_MODE
{
    IMAGE_NONE,
    IMAGE_MODE_REGULAR,     // 正常显示
    IMAGE_MODE_FULLSCREEN,  // 全屏显示
};

// ImageScreen的垂直位置
enum IMAGE_VERTICAL_ALIGN
{
    IMAGE_VERTICAL_ALIGN_TOP,       // 顶端对齐
    IMAGE_VERTICAL_ALIGN_CENTER,    // 垂直居中
    IMAGE_VERTICAL_ALIGN_BOTTOM,    // 底端对齐
};

// ImageScreen的水平位置
enum IMAGE_HORIZONTAL_ALIGN
{
    IMAGE_HORIZONTAL_ALIGN_LEFT,    // 左侧对齐
    IMAGE_HORIZONTAL_ALIGN_CENTER,  // 水平居中
    IMAGE_HORIZONTAL_ALIGN_RIGHT,   // 右侧对齐
};

//////////////////////////////////////////////////////////////////////////

// ui发送给logic的数据
struct XDataUI2Logic
{
    XDataUI2Logic()
    {
        wActionParam = 0;
        lActionParam = 0;
        pData = NULL;
    }

    //第一个Action的参数
    WPARAM wActionParam;
    //第二个Action的参数
    LPARAM lActionParam;
    // 发送给logic的数据
    void * pData;
};

/////////////////////////////Softkey 定义//////////////////////////////
// SoftKey 最大数目
#define MAX_SOFTKEY_NUM 6

// 读取softkey的参数
struct SoftkeyParam
{
    SoftkeyParam()
    {
        Reset();
    }
    void Reset()
    {
        eType = XB_ST_UNKNOWN;
        strName = "";
        strURI = "";
    }

    XB_SOFTKEY_TYPE eType;// 主要给UI用
    yl::string strName; // 显示名
    yl::string strURI;  // 操作
};
typedef YLVector<SoftkeyParam>  VecSoftkeyParam;

///////////////////// ElementTextMenu界面的数据定义 /////////////////////
// MenuItem每一项的数据
struct MenuItem
{
    yl::string strPrompt;     // 提示，用于显示到界面
    yl::string strURI;        // URI，用于跳转
    yl::string strDial;       // 号码，用于摘机(免提、linekey)呼出
    yl::string strSelection;  // 跳转时加上后缀
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam vecSoftkey;
#endif
};

typedef YLList<MenuItem *> List_Menu_Item;

// TextMenu页面的属性
struct TextMenuAttr
{
    TextMenuAttr()
    {
        iDefaultIndex = 0;
        eStyle = STYLE_NUMBERED;
        bBeep = true;
        strCancelAction = "";
        bWrapList = true;
        iTimeOut = DEFAULT_TIMEOUT;
        bLockIn = false;
        bWrapTitle = true;
        bDestroyOnExit = false;
        strTitle = "";
    }

    int    iDefaultIndex;   // 进入页面时的默认焦点项(从0开始)
    MENU_ITEM_STYLE eStyle; // 菜单项样式
    bool   bBeep;           // 进入页面时是否提示音
    yl::string strCancelAction; // 用户从菜单中退出时调用该属性指定的URI
    bool   bWrapList;       // 菜单项内容超过一行时，是否多行显示
    int    iTimeOut;        // 菜单位于焦点且无操作的自动退出时间
    bool   bLockIn;         // 是否锁定按键(除softkey)
    bool   bWrapTitle;      // 标题内容超过一行时，是否多行显示
    bool   bDestroyOnExit;  // 退出时是否销毁
    yl::string strTitle;        // 标题内容
};

// TextMenu页面传给UI的数据
struct XBTextMenuData : public XMLLogicData
{
    TextMenuAttr    stAttr;     // 界面属性
    List_Menu_Item  listItem; // 每一项的链表
    VecSoftkeyParam vecSoftkey;

    ~XBTextMenuData()
    {
        yl::for_each(listItem.begin(), listItem.end(), yl::pointer_deleter<MenuItem>());
    }
};

///////////////////// ElementTextScreen界面的数据定义 /////////////////////
// ElementTextScreen页面的属性
struct TextScreenAttr
{
    TextScreenAttr()
    {
        bBeep = true;
        strCancelAction = "";
        strDoneAction = "";
        bWrapList = true;
        iTimeOut = DEFAULT_TIMEOUT;
        bLockIn = false;
        bWrapTitle = true;
        bDestroyOnExit = false;
        strTitle = "";
    }

    bool   bBeep;           // 进入页面时是否提示音
    yl::string strCancelAction; // 用户从菜单中退出时调用该属性指定的URI
    yl::string strDoneAction;   // 选择“done”功能键时调用该属性指定的URI
    bool   bWrapList;       // 菜单项内容超过一行时，是否多行显示
    int    iTimeOut;        // 菜单位于焦点且无操作的自动退出时间
    bool   bLockIn;         // 是否锁定按键(除softkey)
    bool   bWrapTitle;      // 标题内容超过一行时，是否多行显示
    bool   bDestroyOnExit;  // 退出时是否销毁
    yl::string strTitle;        // 标题内容
};

// TextScreen页面传给UI的数据
struct XBTextScreenData : public XMLLogicData
{
    TextScreenAttr  stAttr;    // 界面属性
    yl::string     strText; // 显示内容
    VecSoftkeyParam vecSoftkey; // softkey内容
};

///////////////////// ElementDirectory界面的数据定义 /////////////////////
// MenuItem每一项的数据
struct DirMenuItem
{
    yl::string strPrompt;     // 提示，用于显示到界面
    yl::string strURI;        // URI，用于跳转
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam vecSoftkey;
#endif
};

typedef YLList<DirMenuItem *> List_Dir_Menu_Item;

///////////////////// ElementInput界面的数据定义 /////////////////////
// InputField每一项的数据
struct InputField
{
    InputField()
    {
        eType = INPUT_STRING;
        bPassword = false;
        bEditable = true;
        strPrompt = "";
        strParam = "";
        strSelection = "";
        strDefault = "";
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
        vecSoftkey.clear();
#endif
    }

    INPUT_TYPE eType;     // 输入类型
    bool   bPassword;     // 是否密码
    bool   bEditable;     // 是否可编辑
    yl::string strPrompt;     // 输入提示
    yl::string
    strParam;      // URL后面所带的参数名，即URL?Parameter设置的名称=用户输入内容
    yl::string strSelection;  // 跳转时加上后缀
    yl::string strDefault;    // 默认的输入内容
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam vecSoftkey;
#endif
};

typedef YLList<InputField *> List_Input_Field;

// ElementInput页面的属性
struct InputAttr
{
    InputAttr()
    {
        iDefaultIndex = 0;
        bBeep = true;
        strCancelAction = "";
        iTimeOut = DEFAULT_TIMEOUT;
        bLockIn = false;
        eType = INPUT_STRING;
        bPassword = false;
        eLanguage = INPUT_LANG_ENGLISH;
        eDisplayMode = MODE_NORMAL;
        bWrapTitle = true;
        bDestroyOnExit = false;
        strTitle = "";
        strURI = "";
    }

    int    iDefaultIndex;   // 进入页面时的默认焦点项(从0开始)
    bool   bBeep;           // 进入页面时是否提示音
    yl::string strCancelAction; // 用户从菜单中退出时调用该属性指定的URI
    int    iTimeOut;        // 菜单位于焦点且无操作的自动退出时间
    bool   bLockIn;         // 是否锁定按键(除softkey)
    INPUT_TYPE eType;       // 输入类型
    bool   bPassword;       // 是否密码
    INPUT_LANGUAGE     eLanguage;     // 语言
    INPUT_DISPLAY_MODE eDisplayMode;  // 显示模式
    bool   bWrapTitle;      // 标题内容超过一行时，是否多行显示
    bool   bDestroyOnExit;  // 退出时是否销毁
    yl::string strTitle;        // 标题内容
    yl::string strURI;
};

// Input页面传给UI的数据
struct XBInputData : public XMLLogicData
{
    InputAttr         stAttr;      // 界面属性
    List_Input_Field  listItem;    // 每一项的链表
    VecSoftkeyParam   vecSoftkey;  // softkey内容

    ~XBInputData()
    {
        yl::for_each(listItem.begin(), listItem.end(), yl::pointer_deleter<InputField>());
    }
};


// ElementDirectory页面的属性
struct DirectoryAttr
{
    DirectoryAttr()
    {
        iDefaultIndex = 0;
        strNext = "";
        strPrevious = "";
        bBeep = true;
        strCancelAction = "";
        iTimeOut = DEFAULT_TIMEOUT;
        bLockIn = false;
        bWrapTitle = true;
        bDestroyOnExit = false;
        strTitle = "";
#if IF_FEATURE_XMLDIRECTORY
        strUrl = "";
#endif
    }

    int    iDefaultIndex;   // 进入页面时的默认焦点项(从0开始)
    yl::string strNext;         // "Next"softkey对应的URI
    yl::string strPrevious;     // "Previous"softkey对应的URI
    bool   bBeep;           // 进入页面时是否提示音
    yl::string strCancelAction; // 用户从菜单中退出时调用该属性指定的URI
    int    iTimeOut;        // 菜单位于焦点且无操作的自动退出时间
    bool   bLockIn;         // 是否锁定按键(除softkey)
    bool   bWrapTitle;      // 标题内容超过一行时，是否多行显示
    bool   bDestroyOnExit;  // 退出时是否销毁
    yl::string strTitle;        // 标题内容
#if IF_FEATURE_XMLDIRECTORY
    yl::string strUrl;
#endif
};

// Directory页面传给UI的数据
struct XBDirectoryData : public XMLLogicData
{
    DirectoryAttr  stAttr;     // 界面属性
    List_Dir_Menu_Item listItem; // 每一项的链表
    VecSoftkeyParam   vecSoftkey;  // softkey内容
#if IF_FEATURE_XMLDIRECTORY
    InputField stInputField;
#endif

    ~XBDirectoryData()
    {
        yl::for_each(listItem.begin(), listItem.end(), yl::pointer_deleter<DirMenuItem>());
    }
};


///////////////////// ElementStatus界面的数据定义 /////////////////////
// Status页面每一项的数据
struct StatusItem
{
    StatusItem()
    {
        eType = ST_MESSAGE_NORMAL;
        eSize = X_FONT_NORMAL;
        iAlign = DT_LEFT;
        eColor = TEXT_FONT_COLOR_WHITE;
        strAccount = "";
        strMsg = "";
        strIconPath = "";
        strSessionID = "";
#if IF_BUG_31595
        dsskeyData.clear();
#endif
    }

    STATUS_TYPE         eType;      // 状态类型
    X_FONT_TYPE         eSize;      // 字体大小
    int                 iAlign;     // 对齐方式
    TEXT_FONT_COLOR     eColor;     // 字体颜色
    yl::string         strAccount; // 该文本相关的账号: xxx@10.2.1.199
    yl::string          strMsg;     // 文本内容
    yl::string          strIconPath;// 图片的路径
    yl::string         strSessionID;// 标识是重置当前信息还是继续添加
#if IF_BUG_31595
    DssKeyLogicData     dsskeyData;//dsskey数据内容
#endif
};

typedef YLList<StatusItem *> List_Status_Item;

// ElementStatus页面的属性
struct StatusAttr
{
    StatusAttr()
    {
        bBeep = true;
        bWrapList = true;
        iTimeOut = 30;
        strSessionID = "";
    }

    bool   bBeep;           // 进入页面时是否提示音
    bool   bWrapList;       // 状态内容超过一行时，是否多行显示
    int    iTimeOut;        // 显示超时时间
    yl::string strSessionID;    // 标识是重置当前信息还是继续添加
};

// Status页面传给UI的数据
struct XBStatusData : public XMLLogicData
{
    XBStatusData()
    {
        listItem.clear();
    }
    ~XBStatusData()
    {
        yl::for_each(listItem.begin(), listItem.end(), yl::pointer_deleter<StatusItem>());
    }

    StatusAttr        stAttr;   // 界面属性
    List_Status_Item  listItem; // 每一项的链表
};

////////////////// ElementFormattedTextScreen界面的数据定义 //////////////////
// 每一个Line的数据
struct LineData
{
    LineData()
    {
        eFont = X_FONT_NORMAL;
        iAlign = DT_LEFT/* | DT_VCENTER*/;
        eColor = TEXT_FONT_COLOR_WHITE;
        strText = "";
        strRGB = "";
    }

    X_FONT_TYPE         eFont;      // 字体大小
    int                 iAlign;     // 对齐方式
    TEXT_FONT_COLOR     eColor;     // 字体颜色
    yl::string          strText;    // 文本内容
    yl::string          strRGB;     // 字体RGB
};

// Header Zone line的数据
typedef YLList<LineData *> List_Header_Zone;
// Scroll Zone的数据
typedef YLList<LineData *> List_Scroll_Zone;
// Footer Zone的数据
typedef YLList<LineData *> List_Footer_Zone;

// FormattedTextScreen页面的属性
struct FormattedTextAttr
{
    FormattedTextAttr()
    {
        bBeep = true;
        strCancelAction = "";
        strDoneAction = "";
        iTimeOut = DEFAULT_TIMEOUT;
        bLockIn = false;
        bDestroyOnExit = false;
    }

    bool   bBeep;           // 进入页面时是否提示音
    yl::string strCancelAction; // 用户从菜单中退出时调用该属性指定的URI
    yl::string strDoneAction;   // 选择“done”功能键时调用该属性指定的URI
    int    iTimeOut;        // 菜单位于焦点且无操作的自动退出时间
    bool   bLockIn;         // 是否锁定按键(除softkey)
    bool   bDestroyOnExit;  // 退出时是否销毁
};

// FormattedTextScreen页面传给UI的数据
struct XBFormattedTextData : public XMLLogicData
{
    FormattedTextAttr       stAttr;     // 界面属性
    List_Header_Zone        listHeader; // Header Zone每一项的链表
    List_Scroll_Zone        listScroll; // Scroll Zone每一项的链表
    List_Footer_Zone        listFooter; // Footer Zone每一项的链表
    VecSoftkeyParam         vecSoftkey; // softkey内容

    ~XBFormattedTextData()
    {
        yl::for_each(listHeader.begin(), listHeader.end(), yl::pointer_deleter<LineData>());
        yl::for_each(listScroll.begin(), listScroll.end(), yl::pointer_deleter<LineData>());
        yl::for_each(listFooter.begin(), listFooter.end(), yl::pointer_deleter<LineData>());
    }
};

///////////////////// ElementImageScreen界面的数据定义 /////////////////////
enum IMAGE_LOAD_STATUS
{
    ILS_DOWNLOAD,  // 下载中
    ILS_COMPRESS,  // 压缩中
    ILS_LOAD_FAIL, // 加载失败
    ILS_COMPRESS_FAIL,  // 压缩失败
    ILS_LOAD_OK,   // 加载成功
};

// 每一张图片的数据
struct ImageItem
{
    ImageItem()
    {
        eVertical = IMAGE_VERTICAL_ALIGN_CENTER;
        eHorizontal = IMAGE_HORIZONTAL_ALIGN_CENTER;
        strImageURL = "";
        strImagePath = "";
        bUrlData = false;
        eLoadStatus = ILS_LOAD_OK;
        uWidth = 0;
        uHeight = 0;
    }

    IMAGE_VERTICAL_ALIGN    eVertical;
    IMAGE_HORIZONTAL_ALIGN  eHorizontal;
    yl::string strImageURL;
    yl::string strImagePath;
    // image data is an url.
    bool bUrlData;
    IMAGE_LOAD_STATUS eLoadStatus;

    UINT uWidth;
    UINT uHeight;
};

typedef YLList<ImageItem *> List_Image_Item;

/*
<URIList base="http://10.3.3.4:9/update">
<URI key="#">/xmlBrowser/TestDirectory.xml</URI>
<URI key="0">/xmlBrowser/TextScreen.xml</URI>
</URIList>
*/
// URI定义
struct XURIItem
{
    yl::string strURI;
    yl::string strKey;
};
typedef YLList<XURIItem *> List_URI_Item;

// URIList
struct XURIList
{
    yl::string      strBase;
    List_URI_Item   listItem;
    ~XURIList()
    {
        yl::for_each(listItem.begin(), listItem.end(), yl::pointer_deleter<XURIItem>());
    }
};

// ImageScreen页面的属性
struct ImageAttr
{
    ImageAttr()
    {
        bBeep = true;
        strCancelAction = "";
        strDoneAction = "";
        iTimeOut = DEFAULT_TIMEOUT;
        bLockIn = false;
        bDestroyOnExit = false;
        eMode = IMAGE_MODE_REGULAR;
    }

    bool   bBeep;           // 进入页面时是否提示音
    yl::string strCancelAction; // 用户从菜单中退出时调用该属性指定的URI
    yl::string strDoneAction;   // 选择“done”功能键时调用该属性指定的URI
    int    iTimeOut;        // 菜单位于焦点且无操作的自动退出时间
    bool   bLockIn;         // 是否锁定按键(除softkey)
    bool   bDestroyOnExit;  // 退出时是否销毁
    IMAGE_MODE eMode;       // 图片的显示方式
};

// ImageScreen页面传给UI的数据
struct XBImageScreenData : public XMLLogicData
{
    ImageAttr               stAttr;     // 界面属性
    List_Image_Item         listImage;  // 图片的链表
    XURIList                stURLList;  // 图片对象可操作的事件
    VecSoftkeyParam         vecSoftkey; // softkey内容
    ~XBImageScreenData()
    {
        yl::for_each(listImage.begin(), listImage.end(), yl::pointer_deleter<ImageItem>());
    }

};

////////////////// ElementExecute的数据定义 //////////////////
// Execute每一项的数据
struct ExecuteItem
{
    yl::string strURI;      // 命令项对应的操作
};

typedef YLList<ExecuteItem *> List_Execute_Item;

// Execute页面的属性
struct ExecuteAttr
{
    ExecuteAttr()
    {
        bBeep = true;
    }
    bool   bBeep;       // 进入页面时是否提示音
};

struct XBExecuteData : public XMLLogicData
{
    ExecuteAttr       stAttr;
    List_Execute_Item listItem;

    ~XBExecuteData()
    {
        yl::for_each(listItem.begin(), listItem.end(), yl::pointer_deleter<ExecuteItem>());
    }
};

////////////////// ElementConfig的数据定义 //////////////////
// 配置版本的兼容
#define XML_CONFIG_COMPATIBLE   1

// Set类型
enum SET_TYPE
{
    SET_CONFIG = 0,         // 更改配置项生效，但话机不重启
    SET_BOOT,               // 更改配置项，且话机重启
    SET_HOTELING,
};

// 配置模板格式
enum X_CFG_MODEL
{
    XCM_M7,                 // 新的M7的格式
    XCM_M1,                 // M1的格式
    XCM_M2,                 // M2(非autop的配置M2)格式，旧的T3X/VPX的格式
};

// Config页面的属性
struct ConfigAttr
{
    ConfigAttr()
    {
        bBeep = true;
        eType = SET_CONFIG;
#if IF_FEATURE_XMLBROWSER_HOTELING
        strMessage = "";
        iMessageTime = 3;
#endif
    }

    bool bBeep;         // 进入页面时是否提示音
    SET_TYPE eType;     // 设置的类型
#if IF_FEATURE_XMLBROWSER_HOTELING
    yl::string strMessage;
    int iMessageTime;
#endif
};

// Config每一项的数据
struct ConfigItem
{
    ConfigItem()
    {
#if XML_CONFIG_COMPATIBLE
        strPath = "";
        strSection = "";
        strParam = "";
#endif
        strValue = "";
    }
#if XML_CONFIG_COMPATIBLE
    yl::string strPath;     // 参数存储的文件名
    yl::string strSection;      // 参数存储的节名
    yl::string strParam;        // 参数名
#endif
    yl::string strValue;        // 参数值
};

typedef YLList<ConfigItem *> List_Config_Item;

// 数据结构
struct XBConfigData : public XMLLogicData
{
    X_CFG_MODEL        eCfgModel;
    ConfigAttr         stAttr;
    List_Config_Item   listItem;

    ~XBConfigData()
    {
        yl::for_each(listItem.begin(), listItem.end(), yl::pointer_deleter<ConfigItem>());
    }
};

// config中不同Path的配置项数据
struct PathItem
{
    yl::string strPath;         // 参数存储的路径
    List_Config_Item listConfigItem; //该路径下的配置项链表
};

typedef YLList<PathItem *> List_Path_Item;

//////////////////////////////////////////////////////////////////////////
// soft传回的数据
struct XBSoftkeyTreateData
{
    yl::string _strURI;
    int _nFocusIndex;
};

// XB_ACTION_SOFTKEY : UI和Logic之间的Action数据
struct ActionSoftkeyData
{
    ActionSoftkeyData()
    {
        iFocusedItem = -1;
    }

    yl::string strURI; // 操作
    int         iFocusedItem;   // 当前焦点Item Index，如果没有则为-1
};

// 压缩参数
struct CompressImgParam
{
    CompressImgParam()
    {
        strDstPath.clear();
        nWidth = 0;
        nHeight = 0;
        nMaxSize = 0;
        bCompressSuc = false;
    }
    yl::string strDstPath;
    int nWidth;
    int nHeight;
    int nMaxSize;

    bool bCompressSuc;
};

#ifdef IF_FEATURE_GENBAND
struct InterfaceData : public XMLLogicData
{
    bool bBeep;           // 进入页面时是否提示音
    yl::string strLabel;
    yl::string strMethod;
    yl::string strUrl;
    yl::string strRequestData;
    yl::string strHttpHeaders;

    InterfaceData()
    {
        bBeep = true;
        strLabel.clear();
        strMethod.clear();
        strUrl.clear();
        strRequestData.clear();
        strHttpHeaders.clear();
    }
};
#endif

#if IF_FEATURE_XMLB_SCREENSAVER
// ScreenSaver页面的属性
struct ScreenSaverAttr
{
    ScreenSaverAttr()
    {
        iLineSpacing = 0;
        iInserImageLineNum = 0;
    }

    int    iLineSpacing;    // 行间距
    int    iInserImageLineNum;   // 图片插入的line的行数
};
// SystemTime的数据
struct SysTimeData
{
    SysTimeData()
    {
        eFont = X_FONT_NORMAL;
        eVertical = IMAGE_VERTICAL_ALIGN_TOP;
        eHorizontal = IMAGE_HORIZONTAL_ALIGN_RIGHT;
        eColor = TEXT_FONT_COLOR_WHITE;
        bShow = true;
        strRGB = "";
    }

    X_FONT_TYPE         eFont;      // 字体大小
    IMAGE_VERTICAL_ALIGN    eVertical;
    IMAGE_HORIZONTAL_ALIGN  eHorizontal;
    TEXT_FONT_COLOR     eColor;     // 字体颜色
    bool                bShow;      // 是否显示
    yl::string          strRGB;     // 字体RGB
};

// SystemTime的数据
struct StatusIconData
{
    StatusIconData()
    {
        eVertical = IMAGE_VERTICAL_ALIGN_TOP;
        eHorizontal = IMAGE_HORIZONTAL_ALIGN_CENTER;
        bShow = true;
    }

    IMAGE_VERTICAL_ALIGN    eVertical;
    IMAGE_HORIZONTAL_ALIGN  eHorizontal;
    bool                bShow;      // 是否显示
};
// Line Zone的数据
typedef YLList<LineData *> List_Line_Zone;

// ImageScreen页面传给UI的数据
struct XBScreenSaverData : public XMLLogicData
{
    ScreenSaverAttr         stAttr;     //页面属性
    List_Image_Item         listImage;  // 图片的链表
    List_Line_Zone          listLine;   // Line Zone每一项的链表
    SysTimeData             stSysTime;  // 系统时间
    StatusIconData          stStatusIcon; //状态图标

    ~XBScreenSaverData()
    {
        yl::for_each(listImage.begin(), listImage.end(), yl::pointer_deleter<ImageItem>());
        yl::for_each(listLine.begin(), listLine.end(), yl::pointer_deleter<LineData>());
    }

};
#endif //#if IF_FEATURE_XMLB_SCREENSAVER

#endif // __LOGIC_AND_UI_COMMON_H__
