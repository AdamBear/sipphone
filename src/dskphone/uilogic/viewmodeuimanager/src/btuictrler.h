#ifdef IF_SUPPORT_BLUETOOTH
#ifndef _BTUI_CTRLER_H_
#define _BTUI_CTRLER_H_

#include "viewmodeuimanager/include/viewmodectrler.h"
#include "wireless/bluetoothNew/include/btdefines.h"

class CBtUICtrler : public CViewModeControler
{
public:
    CBtUICtrler();
    ~CBtUICtrler();
    // 初始化
    bool Init();
    // 注销
    bool UnInit();

    // softkey处理
    virtual bool OnSoftKeyEvent(int iKey, int iDataID,
                                const yl::string & strValue, void * pData = NULL);
    // funckey处理
    virtual bool OnFunKeyEvent(int iKey, int iDataID,
                               yl::string & strValue, void * pData = NULL);
    // digitkey处理
    virtual bool OnDigitKeyEvent(int iKey, int iDataID,
                                 yl::string & strValue, void * pData = NULL);
    // option处理
    virtual bool OnOptionEvent(int iKey, int iDataID,
                               const yl::string & strValue, void * pData = NULL);
    // 弹框结果处理
    virtual bool OnMsgBoxEvent(int iID, int iKey);
    // 获取title
    virtual yl::string GetTitle(int iUIType);
    // 获取数据列表
    virtual bool GetDataList(int iUIType, int iPageIndex,
                             VecViewInfo & vecInfo, int iStartPos = 0, int iNumber = -1);
    // 获取softkey列表
    virtual bool GetSoftkeyList(int iUIType, int iDataID, YLVector<int> & vecSoftkey);
    // 获取Option softkey列表
    virtual bool GetOptionSoftkeyList(int iUIType, int iFocusIndex,
                                      YLVector<OPTION_TYPE> & vecOptionSoftkey);
    // 获取数据长度
    // bOnlyData----只获取数据的大小
    virtual int  GetListSize(int iUITypebool, bool bOnlyData = false);
    // 获取数据在链表的焦点
    virtual int  GetFocusInDataList(int iDataID);
    // 获取动画的信息
    // [strPic]:图片名称前缀 [uNum]:图片数量 [uTime]:变换时间
    virtual bool GetAnimationInfo(AnimationType eType,
                                  yl::string & strPic, UINT & uNum, UINT & uTime);
    // 处理页面退出后的业务
    virtual bool PageExit(int iUIType);
    // 是否需要显示动画
    virtual bool IsNeedShowAnimation(int iUIType, AnimationType eType);

    // 处理初始化结果
    void ProcessPowerResult(bool bEnable);
    // 处理开关结果
    void ProcessSwitchResult(bool bEnable);
    // 处理Pin码请求
    void ProcessRequestPin(UINT uMsg, UINT wParam, UINT lParam, void * pData);
    // 处理列表更新
    void ProcessUpdateList(UINT wParam, UINT lParam, void * pData);

private:
    // 消息接收函数
    bool OnBluetoothtMsg(msgObject & objMsg);
    // 获取设备列表
    bool GetDevList(UI_LIST_BT_DEVICE & listDev,
                    DEVICE_LIST_TYPE eType, int iStartPos = 0, int iNumber = 911);
    // 获取设备列表
    bool GetDevList(VecViewInfo & vecInfo,
                    DEVICE_LIST_TYPE eType, int iStartPos = 0, int iNumber = 911);
    // Logic数据转换为UI显示数据
    void LogicData2UI(DeviceInfo & devInfo, ViewInfo & devViewInfo);
    // 设备是否在列表中
    int GetDevIndexInList(int iHandle, DEVICE_LIST_TYPE eType = DEVICE_LIST_ALL);
    // 通过handle获取mac
    yl::string GetDevMacByHandle(int iHandle);
    // 获取设备列表大小
    int GetDevListSize(DEVICE_LIST_TYPE eType = DEVICE_LIST_ALL);
    // 扫描操作
    bool ScanOperation(bool bStartScan);
    // 连接操作
    bool ConnectOperation(bool bConnnect, int iHandle);
    // 确认操作
    bool ConfirmOperation(int iKey, int iDataID,
                          const yl::string & strValue, void * pData = NULL);
    // 返回操作
    bool CancelOperation(int iKey, bool bExitPage = true);
    // 通过Data ID获取UI item对应的value
    bool GetItemValueByDataID(int iDataID,
                              VecItemDataID2Info & vecItemDataID2Info, yl::string & strValue);
    // 获取icon全名
    yl::string GetFullIconName(yl::string strIconPrefix);

private:
    int                 m_iCurrentOperatingDev;     // 当前正在操作的设备handle
};

#endif // _BTUI_CTRLER_H_
#endif // IF_SUPPORT_BLUETOOTH
