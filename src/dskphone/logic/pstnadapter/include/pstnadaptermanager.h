#ifndef __PSTN_ADAPTER_MANAGER_H__
#define __PSTN_ADAPTER_MANAGER_H__

#include "pstnline.h"

#ifdef IF_FEATURE_PSTN

#if WIN32
#include "win32_adapter.h"
#else
#include <features-define.h>
#include <singletonclassdefine.h>
#endif // WIN32

class CPstnAdapterManager : public CMKThread
{
    DECLARE_SINGLETON_CLASS(CPstnAdapterManager)

public:
    // 线程启动完成后回调(重写基类CMKThread::AfterThreadLaunched)
    virtual BOOL AfterThreadLaunched();

    // 线程关闭前回调(重写基类CMKThread::BeforeThreadShutdown)
    virtual void BeforeThreadShutdown();

    // 消息处理(重写基类CMKThread::TryThreadMessage)
    virtual BOOL TryThreadMessage(msgObject & msg);

    /**
     * @brief 获取当前PSTN Box设备数量
     *
     * @return 当前PSTN Box设备数量
     */
    int GetDeviceCount();

    /**
     * @brief 获取当前PSTN线路数量
     *
     * @return 当前PSTN线路数量
     */
    int GetLineCount();

    /**
     * @brief 设置是否启用
     *
     * @param iDeviceId 设备ID
     * @param bEnable true=启用, false=禁用
     */
    void SetEnable(int iDeviceId, bool bEnable);

    /**
     * @brief 摘机/挂机
     *
     * @param iDeviceId 设备ID
     * @param bOffHook true=摘机, false=挂机
     *
     * @return 操作是否成功
     */
    bool SetHook(int iDeviceId, bool bOffHook);

    /**
     * @brief 拍叉簧
     *
     * @param iDeviceId 设备ID
     *
     * @return 操作是否成功
     */
    bool Flash(int iDeviceId);

    /**
     * @brief 当前是否处于Flash（拍叉簧）状态
     *
     * @param iDeviceId 设备ID
     *
     * @return 当前是否处于Flash状态
     */
    bool IsFlashing(int iDeviceId);

    /**
     * @brief 设置静音/取消静音
     *
     * @param iDeviceId 设备ID
     * @param bMute true=静音, false=取消静音
     *
     * @return 操作是否成功
     */
    bool SetMute(int iDeviceId, bool bMute);

    /**
     * @brief 通话保持/取回
     *
     * @param iDeviceId 设备ID
     * @param bHold true=保持, false=取回
     *
     * @return 操作是否成功
     */
    bool SetHold(int iDeviceId, bool bHold);

    /**
     * @brief 设置免打扰状态
     *
     * @param iDeviceId 设备ID
     * @param bDND true=免打扰, false=退出免打扰
     *
     * @return 操作是否成功
     */
    bool SetDND(int iDeviceId, bool bDND);

    /**
     * @brief 拨号
     *
     * @param iDeviceId 设备ID
     * @param strNumber 拨打号码
     * @param iCallId 呼叫ID
     *
     * @return 操作是否成功
     */
    bool Dial(int iDeviceId, const yl::string & strNumber, int iCallId);

    /**
     * @brief 获取线路ID
     * @details 根据设备ID获取线路ID，用于VPM调用
     *
     * @param iDeviceId 设备ID
     * @return 线路ID
     */
    int GetLineId(int iDeviceId);

    /**
     * @brief 判断PSTN线路是否可用（可以用于呼出）
     * @details 判断PSTN线路状态是否为空闲
     *
     * @param iDeviceId 设备ID
     * @return PSTN线路是否可用
     */
    bool IsAvailable(int iDeviceId);

    /**
     * @brief 获取版本号
     * @details 根据设备ID获取版本号
     *
     * @param iDeviceId 设备ID
     * @return 版本号
     */
    yl::string GetVersion(int iDeviceId);

    /**
     * @brief 获取FXO ID
     * @details 根据设备ID获取FXO ID
     *
     * @param iDeviceId 设备ID
     * @return FXO ID
     */
    yl::string GetFxoId(int iDeviceId);

    /**
     * @brief 升级进度回调函数
     *
     * @param iDeviceId 设备ID
     * @param iProgress 当前升级进度，即当前已成功发送字节数
     * @param iTotal 升级总过程，即固件大小字节数
     * @param pParam 自定义参数
     */
    typedef CPstnLine::UpgradeCallback UpgradeCallback;

    /**
     * @brief 升级PSTN Box固件，同步调用
     *
     * @param iDeviceId 设备ID
     * @param strRomPath 升级固件ROM的路径
     * @param pCallback 升级进度回调函数
     * @param pParam 传递给升级进度回调函数的参数
     *
     * @return 升级是否成功
     */
    bool Upgrade(int iDeviceId, const yl::string & strRomPath, UpgradeCallback pCallback,
                 void * pParam);

private:
    typedef YLHashMap<int, CPstnLine *> DeviceMap;
    DeviceMap m_mapDevice; /// 设备ID-设备信息映射关系(key=device ID, value=device info)

    // PSTN适配模块初始化
    void Init();
    // PSTN适配模块释放
    void Uninit();

    // 热插拔消息处理函数
    static LRESULT ProcessHotplugMsg(msgObject & msg);
    // 配置变更消息处理函数
    static LRESULT ProcessConfigChangedMsg(msgObject & msg);
};

#define _PstnAdapterManager GET_SINGLETON_INSTANCE(CPstnAdapterManager)


#endif // IF_FEATURE_PSTN

#endif // __PSTN_ADAPTER_MANAGER_H__
