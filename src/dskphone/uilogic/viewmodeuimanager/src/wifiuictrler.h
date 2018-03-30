#ifdef IF_SUPPORT_WIFI
#ifndef _WIFIUI_CTRLER_H_
#define _WIFIUI_CTRLER_H_

#include "viewmodeuimanager/include/viewmodectrler.h"
#include "wireless/wifi/include/wifidefines.h"
#include "wifiuidata.h"

class CWifiUICtrler : public CViewModeControler
{
public:
    CWifiUICtrler();
    ~CWifiUICtrler();
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
    virtual bool GetDataList(int iUIType, int iPageIndex, VecViewInfo & vecInfo,
                             int iStartPos = 0, int iNumber = -1);
    // 获取softkey列表
    virtual bool GetSoftkeyList(int iUIType, int iDataID, YLVector<int> & vecSoftkey);
    // 获取Option softkey列表
    virtual bool GetOptionSoftkeyList(int iUIType, int iFocusIndex,
                                      YLVector<OPTION_TYPE> & vecOptionSoftkey);
    // 获取数据长度
    // bOnlyData----只获取数据的大小
    virtual int  GetListSize(int iUIType, bool bOnlyData = false);
    // 获取数据在链表的焦点
    virtual int  GetFocusInDataList(int iDataID);
    // 处理页面退出后的业务
    virtual bool PageExit(int iUIType);
    // 校正数据
    virtual bool CorrectViewInfo(int iUIType, const VecItemDataID2Info & vecItemDataInfo,
                                 VecViewInfo & vecInfo);

private:
    // -------------------------处理logic消息-----------------------------
    // 消息接收函数
    bool OnWifiMsg(msgObject & objMsg);
    // 开关结果处理
    bool ProcessSwitchResult(UINT wParam, UINT lParam);
    // 鉴权消息处理
    bool ProcessEnterPsk(int iNetID, bool bEapNetwork);
    // 连接结果处理
    bool ProcessConnectResult();
    // wps连接结果处理
    bool ProcessWpsConnectResult(UINT wParam, UINT lParam);
    // 是否需要更新UI
    bool IsNeedUpdateUI(UINT wParam);
    // -------------------------处理UI操作--------------------------------
    // 开关操作
    bool SwitchOperation(bool bOpen);
    // 扫描操作
    bool ScanOperation(bool bStartScan);
    // 连接操作
    bool ConnectOperation(bool bConnect, int iNetID);
    // wps连接操作
    bool WpsOperation(bool bStartWps, bool bStartWpsPin = false);
    // 确认操作
    bool ConfirmOperation(int iKey, int iDataID,
                          const yl::string & strValue, void * pData = NULL);
    // 返回操作
    bool CancelOperation(int iKey, bool bExitPage = true);
    // 设备位置调整操作
    bool MovePosOperation(bool bUp, int iNetID);
    // 编辑网络信息
    bool EditNetInfoOperation(bool bAdd, VecItemDataID2Info & vecItemData);
    // 鉴权
    bool AuthNetWork(int iNetID, bool bEAPNetwork, const yl::string & strValue);

private:
    CWifiUIData         m_WifiUIData;
    bool                m_bUserCancelScan;
    int                 m_iCurrentOperatingNet;         // 当前正在操作配对/连接的设备
};

#endif // _WIFIUI_CTRLER_H_
#endif // IF_SUPPORT_WIFI