#ifndef __DIR_UI_LOGIC_H__
#define __DIR_UI_LOGIC_H__

// 是否以一次加载多个数据的方式加载数据
#ifdef DIR_UI_SUPPORT_LOAD_DATA_LIST
#undef DIR_UI_SUPPORT_LOAD_DATA_LIST
#endif

// 是否支持设置界面
#ifdef DIR_SUPPORT_SETTING
#undef DIR_SUPPORT_SETTING
#endif

// 是否支持编辑界面退出时提示保存
#ifdef DIR_SUPPORT_EXIST_SAVE
#undef DIR_SUPPORT_EXIST_SAVE
#endif

// 是否支持逐个浏览联系人
#ifdef DIR_SUPPORT_SWITCH_DETAIL
#undef DIR_SUPPORT_SWITCH_DETAIL
#endif

// 是否支持批量操作
#ifdef DIR_SUPPORT_BATCH_OPERATE
#undef DIR_SUPPORT_BATCH_OPERATE
#endif

// 是否支持字母索引
#ifdef DIR_SUPPORT_INDICATOR
#undef DIR_SUPPORT_INDICATOR
#endif

// 是否使用左右键切换群组
#define IF_DIR_UI_SWITCH_GROUP_IN_BROWSE 1

// 历史记录是否显示组列表
#define IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST 0

// 是否支持从联系人界面选取一个联系人
#define DIR_SUPPORT_SELECT_CONTACT 1

// 是否支持删除network calllog
#define DIR_SUPPORT_NET_CALLLOG_DELETE 1


#endif
