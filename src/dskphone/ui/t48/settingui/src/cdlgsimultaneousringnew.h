#ifndef CDLGSIMULTANEOUSRINGNEW_H
#define CDLGSIMULTANEOUSRINGNEW_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "settinguicommon.h"
#include "uikernel/qcheckbox.h"

class CDlgSimultaneousRingNew : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgSimultaneousRingNew(QWidget * parent = 0);
    ~CDlgSimultaneousRingNew();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgSimultaneousRingNew();
    }

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

public:

    //收到logic层返回的消息
    void OnReceiveMsg(msgObject & objMessage);

    //获取SimultaneousRing号码列表
    void GetSimultaneousRingNumbers();

    virtual bool LoadPageData();

    //iSelectedIndex：CDlgSimultaneousRingNumbers界面进入编辑界面时的选中项数据
    void SetSelectedData(int iSelectedIndex);

    virtual bool IsStatusChanged();


    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid();

    virtual void InitData();

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    // 设置子页面后的操作
    virtual void AfterSetSubPage(void * pData);

    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    virtual bool OnAction(const QString & strAction);

    void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

protected:
    void OnDeleteSimultaneousRingNumber();

private:
    qCheckBox * m_pStatusCheckBox;
    QLineEdit * m_pEditNumber;

    SSimultaneousRing m_RingNumbers;

    //是否Edit界面（同Add界面区分）
    bool        m_bIsEditPage;
    //进入编辑界面前的选中索引
    int         m_iSelectedIndex;
    //该simultaneous Ring对应的账号
    int m_iAccountId;

    //当前弹出类型
    EMessageBoxType m_eMessageBoxType;
};

#endif // CDLGSIMULTANEOUSRINGNEW_H
