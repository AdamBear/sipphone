#ifndef __TALK_UI_FRAME_MANAGER_H__
#define __TALK_UI_FRAME_MANAGER_H__


#include <ylstring.h>
using namespace std;

#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"

#include "voice/include/modvoice.h"

class CDlgBaseTalkUI;
class CDlgTalkUIFrame;

//定义是 t46
#ifdef _T4X
#define IS_T46 1
#else
#define IS_T46 0
#endif

//定义 是 t48
#ifdef _T48

#define IF_USE_CALLMANAGER 1

#else

#define IF_USE_CALLMANAGER 0

#endif

//t48才有  CallManager
#if IF_USE_CALLMANAGER

class CDlgCallManager;

#endif

//当前通话框架类型
enum FRAME_TYPE
{
    FRAME_TYPE_NONE = 0,
    FRAME_TYPE_TALKUIFRAME,

#if IF_USE_CALLMANAGER

    FRAME_TYPE_CALLMANAGER,

#endif

};

//CTalkUIFrameManager 会控制每次显示的子窗口只有一个 有新的子窗口设置进来  旧的子窗口会被清除掉  设置
class CTalkUIFrameManager : public QObject
{
    Q_OBJECT

public:
    static CTalkUIFrameManager & GetInstance();

private:
    CTalkUIFrameManager(void);

public:
    virtual ~CTalkUIFrameManager(void);

public:
    // 初始化
    bool Init();

    // 释放数据,将窗口清空回复到初始化状态
    void Uninit();

public:
    // 设置子窗口指针  如果是pBaseTalkUI设置为空 那么只是清除子界面而已 DlgTalkUIFrame 和 DlgCallManager 都还在 窗口堆栈中
    //目前设置为空 只有内部设置调用了  clearsessionData 和 sessionExit 退出不调用该函数  调用SetFocusSessionDataInvalid
    void SetTalkUI(CDlgBaseTalkUI * pBaseTalkUI, void * pData = NULL);
    //http://10.3.5.199/Bug.php?BugID=65217
    //clearsessionData 和 sessionExit 的时候会调用该函数
    //把焦点的session数据设置无效
    void SetFocusSessionDataInvalid();
    //void SetData(void* pData = NULL);
    //清除子窗口 DlgTalkUIFrame 和 DlgCallManager 会从窗口堆栈中清除
    void ClearTalkUI();

    // 获取子窗口指针
    CDlgBaseTalkUI * GetTalkUI() const;

    bool GetInputContent(yl::string & strNum, yl::string & strDisplayname);

    //更新 其他图标  静音、加密、HD、蓝牙、录音
    void AddTitleIcon(const yl::string & strIconPath, bool bAdd);

    int GetInputCallID();

public:

    //这个在t46 中使用   t48 直接在talkuimanager 就刷新了

    //刷新通道图标
    void UpdateChannelIcon();


    //void SendFocusChangeAction(bool bFocus);
    void UpdateRecordIcon(ICON_TYPE eIconType);
    void RefreshBlueToothIcon();
    //DND图标刷新
    void RefreshDNDIcon();

    void UpdateQueueStatus(const yl::string & strQueueStatus);

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    // USB状态改变更改Softkey
    void USBRecordStateChange();
#endif

    bool OnDoorLineMessage(msgObject & refMessage);

public:
    //获取是否显示音量条状态
    bool GetIsShowVolume();
    //设置是否显示音量条状态
    void SetShowVolumeStatus(bool bIsShowVolume);

protected slots:
    //音量值改变
    virtual void OnVolumeChanged(int nValue);

    // 音量条可见状态的槽函数
    void OnVolumeBarVisibleChanged(bool bVisible);

protected:
    bool m_bIsShowVolume;       //是否显示音量条
    // 当前Channel
    CHANNEL_MODE        m_eCurrentChannel;



protected:
    void CreateTalkUIFrame();

public:
    void SetCurrentFrameType(FRAME_TYPE frameType);
    FRAME_TYPE GetCurrentFrameType() const;

    //保存活动的Frame框架指针
    FRAME_TYPE m_frameType;

protected:
    CDlgTalkUIFrame * m_pDlgTalkUIFrame;

};

// 获取全局唯一的TalkUI管理对象
#define _TalkUIFrameMananger CTalkUIFrameManager::GetInstance()

#endif
