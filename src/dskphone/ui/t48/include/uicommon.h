#ifndef DEFINE_UI_COMMON_H_
#define DEFINE_UI_COMMON_H_
#include <QVariant>
#include <QColor>
#include "setting/include/common.h"
#include "directory/dmcommon.h"

//Action Define
#define ACTION_SAVE_DATA    "SaveData"
#define ACTION_QUIT         "Back"
#define ACTION_NEXT_PAGE    "NextPage"
#define ACTION_INITIATION   "initiation"

// LineEdit的透明色,用来在VPStyle中实现LineEdit的透明
const QColor WidgetTransparentColor(1, 2, 3, 4);

// 背景置灰时的背景色
const QColor DeactivateBackGroundColor(0, 0, 0, 60);

//默认CInfoListBox布局大小为5行1列
const int LIST_BOX_DEFAULT_X = 5;
const int LIST_BOX_DEFAULT_Y = 1;
const int LIST_BOX_ITEM_TOTAL = LIST_BOX_DEFAULT_X * LIST_BOX_DEFAULT_Y;

//切除字符串时的预留偏移量
const int CUTOffTEXT_OFFSET = 3;

//界面响应操作类型
enum DM_UI_OPERATE_TYPE
{
    UI_OPERATE_NULL,    // 错误的类型
    UI_OPERATE_CLICK_SOFTKEY, // 点击softkey操作,参数softkeyAction | string |
    UI_OPERATE_CLICK_MENU,    // 点击Menu操作证,参数Menu Action | string |
    UI_OPERATE_CLICK_INFO_ITEM,         // 点击联系人Item,参数Item DisplayName | string |
    UI_OPERATE_LONGPRESS_INFO_ITEM,      // 长按联系人Item,参数Item Index | int |
    UI_OPERATE_CLICK_FAVORITE,// 点击favorite按钮,参数NULL
    UI_OPERATE_CLICK_SEARCH_FRAME,// 点击搜索框,参数NULL
    UI_OPERATE_CLICK_OPTION_ITEM, //点击Option菜单项
    UI_OPERATE_CLICK_NULL,    // 点击空白的地方,参数NULL
    UI_OPERATE_SEARCH,     // 搜索请求,参数SearchString | string |
    UI_OPERATE_PAGE_CHANGED,    // 页面切换联系人数据,参数 | int |
    UI_OPERATE_EDITLINE_FOCUS_CHANGED,  //编辑框焦点变化 参数 | bool |
    UI_OPERATE_COMBOBOX_FOCUS_CHANGED, //下拉框焦点变化 参数 | bool |
    UI_OPERATE_CONTACT_EXIT,    //联系人退出界面信号 参数NULL
    UI_OPERATE_CONTACT_SHOW_EVENT,  //联系界面显示信号 参数NULL
};

// UI操作类型
struct UIOperatorData
{
    DM_UI_OPERATE_TYPE eType;      // UI操作的类型
    QString         strAction; // 附加的Action指令
    QVariant        appendData; // 附加数据的指针
    int                iAppendSearchData; //附加搜索数据

    UIOperatorData(DM_UI_OPERATE_TYPE eType = UI_OPERATE_NULL
                   , const QString & strAction = "", const QVariant & var = QVariant(), int iAppendSearchData = -1)
        : eType(eType)
        , strAction(strAction)
        , appendData(var)
        , iAppendSearchData(iAppendSearchData)
    {

    }

    void Reset()
    {
        eType = UI_OPERATE_NULL;
        strAction.clear();
        appendData.clear();
        iAppendSearchData = -1;
    }
};

#define IME_ACTION        "IME"
#define IME_DEL_ACTION    "IMEDELETE"
#define IME_DELETE_TITLE  "Delete"
#define COMBOBOX_SWITCH_ACTION "Switch"

// Directory模块softkey的配置
const char * const DM_SOFTKEY_NEW_GROUP = "NewGroup";                           // 新建组
const char * const DM_SOFTKEY_ENTER_GROUP_MANAGER = "EnterGroupManager";    // 进入组管理
const char * const DM_SOFTKEY_DELETE_GROUP = "DeleteGroup";                 // 删除组
const char * const DM_SOFTKEY_DELETE_ALL_GROUP =
    "DeleteAllGroup";                  // 删除所有组
const char * const DM_SOFTKEY_NEW_CONTACT = "NewContact";                       // 新建联系人
const char * const DM_SOFTKEY_EDIT_CONTACT = "EditContact";                 // 编辑联系人
const char * const DM_SOFTKEY_EXIT_DIRECTORY = "ExitDirectory";             // 退出联系人模块
const char * const DM_SOFTKEY_CLICK_BACK = "ClickBack";                     // 点击返回按钮
const char * const DM_SOFTKEY_CLICK_SAVE = "ClickSave";                     // 点击保存按钮
const char * const DM_SOFTKEY_CLICK_FORWARD = "ClickForward";
const char * const DM_SOFTKEY_CLICK_CANCEL = "ClickCancel"; //点击取消按钮
const char * const DM_SOFTKEY_CLICK_OK = "ClickoK"; //点击oK按钮
const char * const DM_SOFTKEY_CLICK_ENTER = "ClickEnter";   //点击Enter按钮
const char * const DM_SOFTKEY_CLICK_OPTION = "ClickOption"; //点击Option按钮
const char * const DM_SOFTKEY_CLICK_SEARCH = "ClickSearch";     //点击搜索
const char * const DM_SOFTKEY_CLICK_UPDATE = "ClickUpdate";     //点出update
const char * const DM_SOFTKEY_CLICK_BD_ADV_SEARCH =
    "ClickBdAdvSearch"; //弹出Broadsoft精确搜索框
const char * const DM_SOFTKEY_SUBMET_BD_ADV_SEARCH =
    "ExecBdAdvSearch"; // 执行Broadsoft精确搜索
const char * const DM_SOFTKEY_CLICK_SEND = "Send";    //点击Send
const char * const DM_SOFTKEY_CLICK_REPLY = "ClickReply"; //点击Reply
const char * const DM_SOFTKEY_CLICK_VIEW = "ClickView"; //点击View
const char * const DM_SOFTKEY_CLICK_DELETE = "ClickDelete";                 // 点击删除按钮
const char * const DM_SOFTKEY_CLICK_CLEAR = "ClickClear";                   // 点击清空按钮
const char * const DM_SOFTKEY_DELETE_CONTACT = "DeleteContact";             // 删除联系人
const char * const DM_SOFTKEY_DELETE_ALL_CONTACT =
    "DeleteAllContact";          // 删除所有联系人
const char * const DM_SOFTKEY_DELETE_MOVE_CONTACT = "MoveContact";              // 移动联系人
const char * const DM_SOFTKEY_MOVE_CONTACT_TO_BLACKLIST =
    "MoveContactToBlacklist"; // 移动联系人到黑名单中
const char * const DM_SOFTKEY_MOVE_CONTACT_TO_DEFAULT_GROUP =
    "MoveContactToDefaultGroup";               // 移动联系人到默认分组
const char * const DM_SOFTKEY_DELETE_EXIT_EDIT_CONTACT = "ExitEditContact"; // 退出编辑联系人
const char * const DM_SOFTKEY_SWITCH_IME =
    IME_ACTION;                              // 切换输入法
const char * const DM_SOFTKEY_DELETE_IME =
    IME_DEL_ACTION;                      // 删除输入字符
const char * const DM_SOFTKEY_DELETE_FAVORITE =
    "DeleteFavorite";               // 删除收藏的联系人
const char * const DM_SOFTKEY_DELETE_ALL_FAVORITE =
    "DeleteAllFavorite";    // 删除收藏所有的联系人
const char * const DM_SOFTKEY_ENTER_ADD_FAVORITE =
    "EnterAddFavorite";          // 进入添加收藏夹
const char * const DM_SOFTKEY_EXIT_ADD_FAVORITE = "ExitAddFavorite";        // 退出添加收藏夹
const char * const DM_SOFTKEY_ENTER_EDIT_FAVORITE = "EnterEditFavorite";    // 进入编辑收藏夹
const char * const DM_SOFTKEY_EXIT_EDIT_FAVORITE =
    "ExitEditFavorite";          // 退出编辑收藏夹
const char * const DM_SOFTKEY_UP_REMOTE_PHONEBOOK =
    "UpRemotePhonebook";    // 远程地址薄回到上一级
const char * const DM_SOFTKEY_NEXT_REMOTE_PHONEBOOK =
    "NextRemotePhonebook";    // 远程地址薄回到上一级
const char * const DM_SOFTKEY_ADD_LOCAL_REMOTE = "AddRemoteToLocal";        // 添加远程联系人
const char * const DM_SOFTKEY_CANCEL_ADD_LOCAL_REMOTE =
    "CancelAddRemote";  // 取消添加联系人状态
const char * const DM_SOFTKEY_DO_SEARCH = "DoSearch";       //开始搜索
const char * const DM_SOFTKEY_DIAL_CONTACT = "DialContact"; //呼叫联系人
const char * const DM_SOFTKEY_ENTER_MENU = "EnterMen"; //选中一个菜单点击Enter按钮
const char * const DM_SOFTKEY_REMOTE_UPDATE = "RemoteUpdate"; //刷新远程地址薄
const char * const DM_SOFTKEY_MTSW_CALL_VOICEAMIL = "CallVoicemail"; //呼叫voicemail
const char * const DM_SOFTKEY_MTSW_ADD_TO_LOCAL = "AddToLocal"; //添加到本地联系人
const char * const DM_SOFTKEY_MTSW_ADD_TO_NETWORK = "AddToNetwork"; //添加到metaswitch联系人
const char * const DM_SOFTKEY_MTSW_MARK_AS_NEW = "MarkAsNew"; //标记为未读
const char * const DM_SOFTKEY_MTSW_MARK_AS_HEARD = "MarkAsHeard"; //标记为已读
const char * const DM_SOFTKEY_MTSW_PLAY = "Play"; //播放
const char * const DM_SOFTKEY_MTSW_PAUSE = "Pause"; //暂停
const char * const DM_SOFTKEY_MTSW_RESUME = "Resume"; //继续
const char * const DM_SOFTKEY_MTSW_STOP = "Stop"; //停止
const char * const DM_SOFTKEY_MTSW_MORE = "More"; //选项

// ListWidgetItem的数据
struct InfoItemData
{
    bool bSelect;   // 是否处于选择状态
    int  iIndex;    // 联系人的Index
    yl::string strIcon; // 图片路径
    yl::string strName; // 显示名字
    yl::string strNumber;// 电话号码
    yl::string strTime; //显示时间
    QVariant    qAppendData; // 附加数据

    InfoItemData()
    {
        Reset();
    }

    void Reset()
    {
        bSelect = false;
        iIndex  = 0;
        strIcon.clear();
        strName.clear();
        strNumber.clear();
        strTime.clear();
        qAppendData.clear();
    }
};

struct DirNumberData
{
    int iIndex;
    int  iLine; //帐号ID
    yl::string strDisplayName; // 显示名字
    yl::string strOfficeNumber;
    yl::string strOtherNumber;
    yl::string strMobileNumber;
    yl::string strDepartment; //部门
    DirNumberData()
    {
        Reset();
    }

    void Reset()
    {
        iIndex = 0;
        iLine = -1;
        strDisplayName.clear();
        strOfficeNumber.clear();
        strMobileNumber.clear();
        strOtherNumber.clear();
        strDepartment.clear();
    }

    // 返回号码的数量
    int GetNumberCount() const
    {
        int iNumCount = 0;
        iNumCount += strOfficeNumber.empty() ? 0 : 1;
        iNumCount += strMobileNumber.empty() ? 0 : 1;
        iNumCount += strOtherNumber.empty() ? 0 : 1;
        return iNumCount;
    }

    // 获取唯一的号码
    yl::string GetOnlyNum() const
    {
        if (GetNumberCount() != 1)
        {
            return "";
        }
        else
        {
            yl::string strNum;
            strNum = strOfficeNumber.empty() ? strNum : strOfficeNumber;
            strNum = strOtherNumber.empty() ? strNum : strOtherNumber;
            strNum = strMobileNumber.empty() ? strNum : strMobileNumber;
            return strNum;
        }
    }
};

// 重定义类型
typedef YLList<InfoItemData> ListInfoData;
typedef ListInfoData::iterator  IterInfoData;

//点击样式
enum INFO_ITEM_CLICK_TYPE
{
    CLICK_UNKOWN_TYPE = -1, //未知类型
    CLICK_NORMAL,//普通button按下样式
    CLICK_SELECTED,//可勾选样式
};

enum ItemBackgroundType
{
    ITEM_BACKGROUND_UNKOWN,//未知样式
    ITEM_BACKGROUND_NORMAL,//普通背景样式
    ITEM_BACKGROUND_SELECTED//勾选背景样式
};

enum INFO_LIST_TYPE
{
    ILT_UNKNOWN = -1,
    ILT_LOCAL_DIR,
    ILT_REMOTE_DIR,
    ILT_LDAP_DIR,
    ILT_BROADSOFT_DIR,
    ILT_TEXT_MESSAGE,
    ILT_LOCAL_CALLLOG,
    ILT_BROADSOFT_CALLLOG
};

enum UIRefreshType
{
    UI_REFRESH_UNKONW,
    UI_REFRESH_ALL,
    UI_REFRESH_SOFTKEY,
    UI_REFRESH_MENU_LIST,
    UI_REFRESH_SUB_PAGE
};

// ListWidget的数据
struct InfoListData
{
    int     iCurrentPage; // 当前的页面数
    int     iTotalPage;   // 总页数
    int     iAllTotalCount;//全部Item的总个数
    INFO_ITEM_CLICK_TYPE  eClickType;   // Item的点击状态
    INFO_LIST_TYPE        eInfoListType;// InfoList的类型
    ListInfoData          listInfoData; // ListWidgetItem的数据

    InfoListData()
    {
        Reset();
    }

    void Reset()
    {
        eClickType = CLICK_NORMAL;
        eInfoListType = ILT_UNKNOWN;
        iCurrentPage = 0;
        iTotalPage = 0;
        iAllTotalCount = 0;
        listInfoData.clear();
    }
};

#ifndef _T48
// ListWidget的数据
struct ContactGRoupPageData
{
    //组名是否可编辑
    bool m_bIsGroupNameCanEdit;
    //组名是存在焦点状态
    bool m_bIsGroupNameFocusIn;
    //铃声是否可编辑
    bool m_bIsRingCanEdit;
    ContactGroupData m_objData;

    ContactGRoupPageData()
    {
        Reset();
    }

    void Reset()
    {
        m_bIsGroupNameCanEdit = true;
        m_bIsRingCanEdit = true;
        m_bIsGroupNameFocusIn = false;
    }
};
#endif

struct UISoftkeyData
{
    yl::string strAction;
    yl::string strTitle;

    UISoftkeyData()
    {
        strAction = "";
        strTitle  = "";
    }

    UISoftkeyData(const yl::string & strParaAction, const yl::string & strParaTitle)
    {
        strAction = strParaAction;
        strTitle  = strParaTitle;
    }

    // 重载等号操作符
    const UISoftkeyData & operator=(const UISoftkeyData & other)
    {
        // 证同测试
        if (this != &other)
        {
            this->strAction = other.strAction;
            this->strTitle  = other.strTitle;
        }

        return *this;
    }

    // 重载比较操作符
    bool operator==(const UISoftkeyData & otherData) const
    {
        return (this->strAction == otherData.strAction
                && this->strTitle == otherData.strTitle);
    }

    bool operator!=(const UISoftkeyData & otherData) const
    {
        return !this->operator==(otherData);
    }
};
typedef std::vector<UISoftkeyData> ListSoftkeyData;

#define  ITEM_ROW_COUNT 4
#define  ITEM_COLUMN_COUNT 3
#define  ITEM_ROW_HEIGHT  64 //72
#define  ITEM_CLOUMN_WIDTH 170
// 注册附加数据类型
Q_DECLARE_METATYPE(yl::string);
Q_DECLARE_METATYPE(InfoListData);
Q_DECLARE_METATYPE(DirNumberData);
#ifndef _T48
Q_DECLARE_METATYPE(ContactGroupData);
Q_DECLARE_METATYPE(ContactGRoupPageData);
#endif

enum EPreprocessEventResult
{
    ePreproResut_Continue = -1, //预处理时，未对此事件做任何处理，当前类要接着处理
    ePreproResut_Ignore  = 0,     //预处理后，当前类忽略此事件，要返回false
    ePreproResut_Accept  = 1      //预处理过程中已处理完此事件，不在传递，要返回true
};

// 子页面背景类型
enum BACKGROUND_LINE_TYPE
{
    BACKGROUND_LINE_TYPE_NORMAL             = 0,        //正常画线
    BACKGROUND_LINE_TYPE_WALLPAPER,                     //壁纸设置界面画线
    BACKGROUND_LINE_TYPE_NONE                           //不画线
};
enum IDLE_TYPE
{
    IDLE_TYPE_GENERAL   = 0,
    IDLE_TYPE_DSSKEY
};

/*#define LANGUAGE_FLAG         "?`"
static const char* LogicTranslateText(const char* lpszText)
{
#if IF_SUPPORT_AUTO_TRANSLATE

    if (lpszText == NULL
        || strcmp(lpszText, "") == 0)
    {
        //字符串为空，返回空
        return "";
    }

    yl::string strTran(lpszText);
    if (NULL == strstr(lpszText, LANGUAGE_FLAG))
    {
        strTran.clear();
        strTran = commonAPI_FormatString("%s%.2d%s", LANGUAGE_FLAG, strlen(lpszText), lpszText);
    }

    return strTran.c_str();
#else
    return lpszText;
#endif
}

#ifdef _T48
#define _LOGIC_LANG_TRAN(lpszText)      LogicTranslateText(lpszText)    //逻辑层翻译宏
#endif*/

#ifdef _T4X
#ifndef _LANG
#define _LANG(str)  str
#endif
#endif

#endif //DEFINE_UI_COMMON_H_

