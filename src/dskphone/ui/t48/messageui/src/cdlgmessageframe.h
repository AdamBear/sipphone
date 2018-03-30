#ifndef DLGMESSAGESUBMENU_H
#define DLGMESSAGESUBMENU_H

#include "msgcommondelegate.h"
#include "baseui/framelist.h"
#include "msgdatadelegate.h"

#include "settingui/src/cdlgbasesubpage.h"
#include "uicommon.h"

///////////////////////////////////////////////////////////////////////////////
class CDlgMessageFrame : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgMessageFrame();
    }

public:
    virtual void Init();
    virtual void Uninit();

    virtual bool LoadPageData()
    {
        return false;
    };  // 读取页面数据
    virtual bool IsStatusChanged()
    {
        return false;
    }; // 查看页面状态是否改变
    virtual bool SavePageData()
    {
        return false;
    };  // 保存页面数据

public:
    //设置消息列表
    void SetMessageList(TextMsgSimpListData & sMsgList);

    //清空消息列表
    void ClearListView();

    //获取当前焦点message的id
    int GetCurrentMsgId()
    {
        return m_iCurrentMsgid;
    };

    void SetDelStatus(bool bDel = false);

public:
    //设置子界面的softkey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    //窗口显示到界面上时的操作
    virtual void UpdateWnd();

    //进入其他界面后返回该界面时的操作
    virtual void OnReShow();

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

private Q_SLOTS:
    //切页
    void OnCurrentPageChanged(int page);
    //item被点击
    void OnMyListItemAction(int eActionType, QString strItemAction);

private:
    //具体处理响应事件
    virtual bool OnAction(const QString & strAction);

    //打开查看或编辑消息的界面
    void OpenViewPage();

private:
    //删除所对应的列表所有Message
    void OnMessageDeleteAll();

private:
    void InitData();

    //根据界面内容修正Softkey的显示内容
    void correctSofkeyBarData(CArraySoftKeyBarData & softkeyData);

protected:
    CDlgMessageFrame(QWidget * parent = NULL);
    virtual ~CDlgMessageFrame();

private:
    MsgCommonDelegate   *   m_pMsgListDelegate;
    MsgDataDelegate    *    m_pMsgDataDelegate;
    CArraySoftKeyBarData m_softkeyLst;

    //当前焦点消息的id
    int m_iCurrentMsgid;
};

#endif // DLGMESSAGESUBMENU_H

