#ifndef CDLGFORWORDUIFORWARDLOGIC_H
#define CDLGFORWORDUIFORWARDLOGIC_H

#include "config.h"
#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>
#include "etlmsghandler/modetlmsghandler.h"


#ifndef GLOAD_ID
#define GLOAD_ID    -1
#endif

#ifdef IF_FEATURE_FORWARD_OPT
#define FWD_PAGE_SAVE_POP_TIMER   (5*1000)
#define FWD_PAGE_SAVE_POP_ID      (305)
#else
#define FWD_PAGE_SAVE_POP_TIMER   (1000)
#define FWD_PAGE_SAVE_POP_ID      MSG_ID_SHOW_NOTE_BEFORE_BACK
#endif

#define ACTION_COPY_TO_ALL_LINES  "alllines"

class CDlgBaseFwdPageUI
    : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgBaseFwdPageUI(FWD_TYPE iType, QWidget * parent = 0);
    virtual ~CDlgBaseFwdPageUI() {}

    // 返回窗口
    void OnBtnBackClick();

    // 从逻辑层取数据
    bool LoadData();

    // 保存键触发
    bool OnBtnSaveClick();

    // 同步回调接口
    bool OnSyncResult(msgObject & msg);

    int GetLineID()
    {
        return m_iLineID;
    }

    void SetLineID(int iLineId)
    {
        m_iLineID = iLineId;
    }

    const ForwardData & GetFwdData()
    {
        return m_FwdData;
    }

    FWD_TYPE GetUIType()
    {
        return m_eFwdType;
    }

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    virtual bool OnAction(const QString & strAction);

    // 获取softkey数据
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    virtual void Init();
    virtual void Uninit();

    //获取数据状态是否改变
    virtual bool IsStatusChanged();

    //从V层读取当前界面的值
    virtual bool GetPageData(ForwardData * pData, FWD_TYPE eFwdType)
    {
        return false;
    }
    // 消息处理
    static LRESULT OnMessage(msgObject & msg);

    int GetMsgBoxTimer();
private:
    int         m_iLineID;
    FWD_TYPE    m_eFwdType;
    ForwardData m_FwdData;
};



#endif