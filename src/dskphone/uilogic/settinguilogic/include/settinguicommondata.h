#ifndef __SETTINGUI_COMMON_DATA_H__
#define __SETTINGUI_COMMON_DATA_H__

#include "ylstring.h"
#include "yllist.h"
#include "messagebox/messageboxdefine.h"
#include "settinguilogic/include/common.h"

namespace NS_SET
{
extern const char * kszActionSave;
extern const char * kszActionBack;
extern const char * kszActionEnter;
extern const char * kszActionOpenPage;
extern const char * kszActionItemSave;
extern const char * kszActionReboot;
extern const char * kszActionReset;
extern const char * kszActionStart;
extern const char * kszActionSkip;
extern const char * kszActionPrevious;
extern const char * kszActionNext;
extern const char * kszActionFinish;

extern const int g_SetPopNormalTime;
extern yl::string kszEmptyString;
} // namespace NS_SET

// 界面类型
enum SETTING_UI_TYPE
{
    STUI_TYPE_MENU,         //图形菜单
    STUI_TYPE_LIST,         //文本菜单
    STUI_TYPE_ITEM,         //具体配置
    STUI_TYPE_RADIO,        //单选页面
    STUI_TYPE_CUSTOM_BEGIN, //自定义ui
};

// 值类型
enum WIDGET_VALUE_TYPE
{
    WIDGET_VALUE_TYPE_UNKNOW    = 300,
    WIDGET_VALUE_TYPE_INT       = WIDGET_VALUE_TYPE_UNKNOW + 1,
    WIDGET_VALUE_TYPE_STRING    = WIDGET_VALUE_TYPE_UNKNOW + 2,
    WIDGET_VALUE_TYPE_BOOL      = WIDGET_VALUE_TYPE_UNKNOW + 3,
};

// setting中messageBox的类型
enum SETTING_MESSAGEBOX_TYPE
{
    SETTING_MESSAGEBOX_Error       = -1,
    SETTING_MESSAGEBOX_NOTE        =  0,
    SETTING_MESSAGEBOX_Cancel,
    SETTING_MESSAGEBOX_OkCancel,
    SETTING_MESSAGEBOX_SPECIAL,
    SETTING_MESSAGEBOX_PWD,
    SETTING_MESSAGEBOX_UNLOCK,
    SETTING_MESSAGEBOX_EDK,
    SETTING_MESSAGEBOX_END
};

#if IF_MSG_BOX_SHOW_ICON
// setting中messagebox icon类型
enum SETTING_MESSAGEBOX_ICON_TYPE
{
    SETTING_MSG_ICON_TYPE_NONE = -1,
    SETTING_MSG_ICON_TYPE_ERROR,
    SETTING_MSG_ICON_TYPE_WARNING,
    SETTING_MSG_ICON_TYPE_ASK,
    SETTING_MSG_ICON_TYPE_INFO,
    SETTING_MSG_ICON_TYPE_SAVE_SUCCESS,
};
#endif
// setting中弹出框的返回值
enum SETTING_MESSAGEBOX_RESULT
{
    SETTING_MessageBox_ERROR        = -1,
    SETTING_MessageBox_Destory      =  0,       //系统原因导致的非人为关闭
    SETTING_MessageBox_OK           =  1,
    SETTING_MessageBox_Cancel,
    SETTING_MessageBox_TimeOut,
    SETTING_MessageBox_Close,
};

// 静态配置过滤数据结构
struct SettingStaticConfigFilter
{
    yl::string strAuthorityID;
    bool bShow;

    SettingStaticConfigFilter()
        : strAuthorityID("")
        , bShow(true)
    {
    }

    void Reset()
    {
        strAuthorityID = "";
        bShow = true;
    }

    SettingStaticConfigFilter & operator = (SettingStaticConfigFilter & obj)
    {
        if (this != &obj)
        {
            strAuthorityID = obj.strAuthorityID;
            bShow = obj.bShow;
        }

        return (*this);
    }
};

// 动态配置过滤数据结构
enum SETTING_CONFIG_TYPE
{
    SETTING_CONFIG_TYPE_BASE = 100,
    SETTING_CONFIG_TYPE_CONFIG = SETTING_CONFIG_TYPE_BASE + 1,          // config
    SETTING_CONFIG_TYPE_AUTHORITY = SETTING_CONFIG_TYPE_BASE + 2,       // authority
};

//
enum SETTING_FILTER_TYPE
{
    SETTING_FILTER_TYPE_BASE = 0x00,
    SETTING_FILTER_TYPE_VISIABLE = 0x01,        // visiable
    SETTING_FILTER_TYPE_UNVISIABLE = 0x02,      // unvisiable
    SETTING_FILTER_TYPE_UNEDITABLE = 0x04,      // uneditable
};

struct SettingDynamicItem
{
    SETTING_CONFIG_TYPE eType;
    yl::string          strId;
    LIST_STRING         listValue;
    yl::string          strValueType;   // int、string、bool
};

typedef YLList<SettingDynamicItem*>  LIST_DYNAMIC_ITEM_PTR;

struct SettingDynamicConfigFilter
{
    yl::string strAuthorityID;
    SETTING_FILTER_TYPE eFilterType;
    LIST_DYNAMIC_ITEM_PTR listFilter;        // 是否显示
    LIST_DYNAMIC_ITEM_PTR listEditFilter;    // 是否可编辑

    SettingDynamicConfigFilter()
        : strAuthorityID("")
        , eFilterType(SETTING_FILTER_TYPE_BASE)
        , listFilter()
        , listEditFilter()
    {
    }

    void Reset()
    {
        strAuthorityID = "";
        eFilterType = SETTING_FILTER_TYPE_BASE;
        listFilter.clear();
        listEditFilter.clear();
    }

    SettingDynamicConfigFilter & operator = (SettingDynamicConfigFilter & obj)
    {
        if (this != &obj)
        {
            Reset();
            strAuthorityID = obj.strAuthorityID;
            eFilterType = obj.eFilterType;
            listFilter = obj.listFilter;
            listEditFilter = obj.listEditFilter;
        }

        return (*this);
    }
};

// 打开界面携带数据结构
struct PageOpenData
{
    void    *   pExtraData;                 // 设置子页面的附加数据
    yl::string  strPageAction;              // 页面的Action

    // 默认构造函数
    PageOpenData()
    {
        Reset();
    }

    // 带Action的构造函数
    explicit PageOpenData(const yl::string & strAction)
    {
        Reset();
        strPageAction = strAction;
    }

    void Reset()
    {
        pExtraData = NULL;
        strPageAction = "";
    }

    PageOpenData & operator=(PageOpenData & other)
    {
        if (&other == this)
        {
            return *this;
        }

        pExtraData = other.pExtraData;
        strPageAction = other.strPageAction;
    }
};

class CSettingUIPageDataBase;

typedef  bool (*PageLoadFun)(CSettingUIPageDataBase * pConfigInfo, int iLineID);
typedef  bool (*PageSaveFun)(CSettingUIPageDataBase * pConfigInfo);

#endif

