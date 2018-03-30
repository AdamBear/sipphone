#ifndef __TALK_UI_MANAGER_H__
#define __TALK_UI_MANAGER_H__

//mod-stl
#include <map>
#include <ylstring.h>
using namespace std;

// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talkadapter/include/modtalkadapter.h"

//前置声明
class CBaseDialog;
class CDlgBaseTalkUI;
class CDlgTalkUIFrame;
class CWndTemplate;

//定义TalkUI指针的列表表类型
typedef list<CDlgBaseTalkUI *>       ListTalkUI;

//通话UI管理类
class CTalkUIManager
{
public:
    //TalkUI内部更新类型，主要针对小窗口外部的数据
    enum TALK_UI_UPDATE_TYPE
    {
        TUUT_ALL = 0,   //所有都进行更新
        TUUT_SOFTKEY,   //只更新Softkey
        TUUT_RESET_TYPE,//只进行类型切换
    };
public:
    //构造函数
    CTalkUIManager(void);

    //析构函数
    virtual ~CTalkUIManager(void);

public:
    //初始化管理对象
    bool InitManager();

    //注销管理对象
    void UninitManager();

    //获取通话UI管理类的示例
    static CTalkUIManager & GetInstance();

public:
    //根据SessionId与UI名查找TalkUI指针，当存在多个匹配SessionId的UI时，同时UI名为空或UI名匹配不上时，返回列表中第一个窗口指针
    CDlgBaseTalkUI * FindTalkUIBySessionId(int nSessionId, yl::string strUIName = "");

    //根据SessionId从映射表移除TalkUI
    bool EraseTalkUIFromMap(int nSessionId);

    //清除对应sessionid 的数据
    bool ClearSessionData(int iSessionID);

    //刷新通道图标
    void UpdateChannelIcon();

    //根据逻辑层数据更新UI内容
    void UpdateContentByData(DataLogic2UI & objData);

    //内部刷新界面接口
    void UpdateWndByData(DataLogic2UI & objData, const yl::string & strUIName);

    //根据逻辑层数据更新错误提示界面
    void UpdateErrorInfoByData(const Error_Info & objErrorInfo); ////----qt

    //输入输入框内容，并返回当前输入内容
    bool GetInputContent(yl::string & strNum, yl::string & strDisplayname);

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    //显示ACD Queue Status
    void ShowACDQueueStatus();
#endif

    //更新录音图标
    void UpdateRecordIcon(ICON_TYPE eIconType);

    // 更新蓝牙图标
    void RefreshBlueToothIcon();

    void ExitDialingUI();

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    void OnTalkUIUSBRecordMsg(msgObject & objMessage);
#endif

    int GetInputCallID();

protected:
    //清理TalkUI映射表
    void ClearTalkUIMap();

    //根据逻辑层数据获取TalkUI指针
    CDlgBaseTalkUI * GetTalkUIByData(const DataLogic2UI & objData);

private:

    //void ReleaseTalkUI(CBaseDialog* pBaseDialog, bool bClearList = false);

private:
    CDlgBaseTalkUI   *  m_pCurrentTalkUI;   //当前使用的 talkui 子界面实例指针
    //ListTalkUI            m_listTalkUI;       //TalkUI指针列表
    //CDlgTalkUIFrame*  m_pTalkUIFrame;     //TalkUI框架窗口指针对象
    DataLogic2UI
    m_tLogicData;       //用于切换UI时缓存Logic到UI的数据，模拟新数据给新UI
};

// 获取全局唯一的TalkUI管理对象
#define _TalkUIManager CTalkUIManager::GetInstance()

#endif //__TALK_UI_MANAGER_H__
