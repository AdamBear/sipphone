#ifndef __Q_T48_APPLICATION_H__
#define __Q_T48_APPLICATION_H__

#include "qtcommon/qdskapplication.h"

#if defined(PHONE_APP_PROFILER)
#include "appprofiler.h"
#endif

class qT48Application : public QDSKApplication
{
    Q_OBJECT

    // 从主线程MsgQueue取到消息后的回调函数列表
    BEGIN_MSG_MAP(CApplication)
    CHAIN_MSG_ON_METHOD(OnMessage)
    CHAIN_MSG_TO_BASE(CMKThread)
    END_MSG_MAP()

public:
    qT48Application(int & argc, char ** arg);
    virtual ~qT48Application(void);

    void Init();
    void UnInit();

    static LRESULT OnMessageEx(msgObject & objMessage);

protected:
    // 回调函数，遍历消息函数表
    BOOL OnMessage(msgObject & objMessage);

protected:
    void InitAppFont();

    void InitStyleImages();

    void InitUIManager();
    void InitModule();
    void InitDefaultIcon();

private:
    // 设置适配函数
    void SetAdapterBox();

private Q_SLOTS:
    // 延迟初始化定时器操作
    void OnDelayInit();

#if defined(PHONE_APP_PROFILER)
protected:
    CAppProfiler m_prof;
#endif
};

#endif
