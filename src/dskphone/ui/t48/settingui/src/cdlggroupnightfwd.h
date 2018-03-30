#ifndef CDLGGROUPNIGHTFWD_H
#define CDLGGROUPNIGHTFWD_H

#include "setting/include/common.h"
#include "settingui/src/cdlgbasesubpage.h"

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>


class CDlgGroupNightFWD : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgGroupNightFWD(QWidget * parent = 0);
    ~CDlgGroupNightFWD();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgGroupNightFWD();
    }


public:

    // 初始化
    virtual void Init();
    // 反初始化
    virtual void Uninit();
    //收到logic层返回的消息
    void OnReceiveMsg(msgObject & objMessage);

    //获取FWDNotReachData数据
    void GetGroupNightForwardData();

    virtual bool LoadPageData();

    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    virtual void InitData();

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    // 获取softkey数据
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 处理密码弹窗
    bool ProcessMsgBoxPassword(int nLineId);

    virtual void ExtraDialogInitData();

public Q_SLOTS:
    // 密码框提交响应函数
    void OnSubmitXSIPassword(const yl::string & strPswd);

    // 密码框取消响应函数
    void OnCancelInputXSIPassword();

private:
    QLabel   *  m_pLabGroupNightForward;
    QLabel   *  m_pLabBusinessHour;
    QLabel   *  m_pLabHolidaySchedule;
    int        m_iAccountId;

    //表示是加载还是保存
    bool m_bIsInLoad;

};

#endif // CDLGGROUPNIGHTFWD_H
