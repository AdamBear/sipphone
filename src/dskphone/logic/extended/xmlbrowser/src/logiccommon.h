#ifndef __LOGIC_COMMON_H__
#define __LOGIC_COMMON_H__

#define MAX_TITLE_CHARS 256
// Menu item 最大数目
#define MAX_MENU_ITEM 30

// Text 最大长度(B)
#define MAX_TEXT_LENGTH 2000
// Input Screen 最大数目
#define MAX_INPUT_NUM 999999
// Directory 最大数目
#define MAX_DIRECTORY_NUM 30
// Status 最大数目
#define MAX_STATUS_NUM 10
// Status index最大数目
#define MAX_STATUS_INDEX_NUM 10
// ExecuteItem 最大数目
#define MAX_EXECUTE_ITEM 1000
// ConfigItem 最大数目
#define MAX_CONFIG_ITEM 1000
// 允许的xml文件大小
#define XML_FILE_SIZE 1024*1024 // 1M
// 允许的图片文件大小
#define IMAGE_FILE_SIZE 5*1024*1024 // 5M

// 下载的最大文件数
#define MAX_TEMP_FILE 200

// 下载的xml文件保存的路径
const char  szTempFile[] = "XmlBrowserFile";

// 检查空指针
// #define CHECK_NULL(pointer, ret) do{\
//  if(pointer == NULL){ \
//  XMLBROWSER_INFO("Unexpected NULL pointer ! File:[%s] Line:[%d]", __FILE__, __LINE__);\
//  return ret; \
//  }}while(0)

// 将对象地址作为定时器ID
#define TIMER_ID(timer)      (UINT(&(timer)))

#define FIRST_LINEKEY_INDEX 10


// 释放链表,链表内容必须是动态分配的
#define Delete_Dynamic_List(typename, objname)       \
    do {                                             \
        typename::iterator itBegin = objname.begin();\
        typename::iterator itEnd = objname.end();    \
        while (itBegin != itEnd)                     \
        {                                            \
            if (((*itBegin)) != NULL)      \
            {                                        \
                delete ((*itBegin));         \
                ((*itBegin)) = NULL;         \
            }                                        \
            ++itBegin;                 \
        }                                            \
        objname.clear();                             \
    } while (0)

//指示灯状态
// enum LEDSTATUS_TYPE
// {
//  LST_NONE,
//  LST_OFF,
//  LST_ON,
//  LST_SLOWFLASH,
//  LST_FASTFLASH,
// };

// 下载的文件类型
enum LOAD_FILE_TYPE
{
    XB_FILE_UNKNOWN,
    XB_FILE_XML,
    XB_FILE_IMAGE,
    XB_FILE_WAV,
};

enum DOWNLOAD_STATE_TYPE
{
    STATE_READY,
    STATE_LOADING,
    STATE_CANCELED,
};

//////////////////////////////// 属性名定义 ///////////////////////////////

// Dsskey Define Before T3X/VPX V71
// #define ATTR_DSSKEY_TYPE

// M7 Dsskey Define
#define M7_DSSKEY_TYPE   "type"
#define M7_DSSKEY_LINE   "line"
#define M7_DSSKEY_VALUE  "value"
#define M7_DSSKEY_LABEL  "label"
#define M7_DSSKEY_PK_VAL "pickup_value"


#endif  // __LOGIC_COMMON_H__
