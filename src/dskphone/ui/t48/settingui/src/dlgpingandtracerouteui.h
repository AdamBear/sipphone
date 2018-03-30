#ifndef _DLG_PING_AND_TRACEROUTE_UI_H
#define _DLG_PING_AND_TRACEROUTE_UI_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"


class CDlgPingAndTracerouteUI : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    enum NETWORK_CHECK_SHOWINFO_TYPE
    {
        NETWORK_CHECK_SHOWINFO_TYPE_NONE = 0,       //未知类型
        NETWORK_CHECK_SHOWINFO_TYPE_IP,             //显示输入ip
        NETWORK_CHECK_SHOWINFO_TYPE_WAIT,           //显示等待界面
        NETWORK_CHECK_SHOWINFO_TYPE_RESULT,         //显示检测结果

    };

public:
    explicit CDlgPingAndTracerouteUI(QWidget * parent = 0);
    virtual ~CDlgPingAndTracerouteUI();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgPingAndTracerouteUI();
    }
public:
    //窗口显示到界面上时的操作
    virtual void UpdateWnd();

    // 是否跳过Return To Idle，包括通话、Idle
    virtual bool IsRejectReturnToIdle(RETURN_IDLE_TYPE eReturnIdle = RETURN_IDLE_TYPE_NORMAL);

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    //处理action
    virtual bool OnAction(const QString & strAction);

    // 处理弹出框回调
    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

public:
    //初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //通过控制获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过控制获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

public:
    //处理返回消息
    void OnPingAndTracerouteMsg(msgObject & objMessage);

protected:
    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    //开始网络检测
    void StartNetworkCheck();

    //显示检测ip
    void ShowNetworkCheckIP();

    //显示检测结果
    void ShowNetworkCheckResult();

protected:
    //注册检测结果消息
    void RegisterCheckResultMsg();

    //注销检测结果消息
    void UnRegisterCheckResultMsg();

    //返回值是获取到的字符串长度
    int GetStringByWidth(const QString & strInData, const QFontMetrics & fm, int iWidth,
                         QString & strGet);

    //返回值是获取的字符串组数
    int GetSplitStringsByWidth(const QString & strInData, const QFontMetrics & fm, int iWidth,
                               std::vector<QString> & vectorSplitStrings);

    //最后一个参数是表示遇到strSplit 就分段（该字符串不会被包含）
    int SplitStringByFont(const QString & strInData, const QFont & fontSplit, int iWidth,
                          std::vector<QString> & vectorSplitStrings, const QString & strSplit = "\n");

private:
    QLineEdit * m_pEditURL;         //输入URL控件

    NETWORK_CHECK_SHOWINFO_TYPE m_eShowInfoType;        //当前显示的信息 类型

    QString m_strCheckResult;       //检测结果

    std::vector<QString> m_vectorCheckResults;  //检测结果数组

    int m_iMaxGetSplitStringLen;        //获取的最大长度字符串   优化解析字符串函数使用


};

#endif // _DLG_PING_AND_TRACEROUTE_UI_H
