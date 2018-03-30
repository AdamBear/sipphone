#ifndef _CALL_MANAGER_PT_STRUCT_H
#define _CALL_MANAGER_PT_STRUCT_H

#include <ylstring.h>

//不同的项目 这个文件是可以修改的

//callmanager对话框名称
#define DLG_CALLMANAGER "CDlgCallManager"

//对应 CCallMGRInfo 类中的iUIFlag 属性
//这边的属性 用属性位 来表示  即是按 位与 或 位或 运算的
enum CallManagerUIFlag
{
    //属性暂时不用  隐藏时要退出 交给外部模块自己退出
//  UIFlagHidePutPag = 0x01,        //0x01 说明是隐藏的时候需要调用退出函数的


};

enum CallManagerEventType
{
    CallManagerEvent_Show = 0x01,
    CallManagerEvent_Hide = 0x02,
};

//子窗口 item 信息结构体
struct tagCallMGRItemInfo
{
    tagCallMGRItemInfo()
    {
        strDisplayName = "";        //显示标题
        strIcon = "";               //图标
        strNormalBKImg = "";        //普通状态下的背景图片
        strSelectBKImg = "";        //选中状态下的背景图片
    }

    yl::string strDisplayName;      //显示标题
    yl::string strIcon;             //图标
    yl::string strNormalBKImg;      //普通状态下的背景图片
    yl::string strSelectBKImg;      //选中状态下的背景图片
};

typedef struct tagCallMGRItemInfo CCallMGRItemInfo;



class CCallManagerPTStruct
{
public:
    CCallManagerPTStruct(void);
    virtual ~CCallManagerPTStruct(void);
};

#endif
