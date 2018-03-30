#ifndef _DLG_MULTICAST_PAGING_LIST_H_
#define _DLG_MULTICAST_PAGING_LIST_H_

#include "cdlgbasesubpage.h"
#include "setting/src/multicastpaginglistcontroller.h"
class QWidget;

class CDlgMulticastPaingList : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgMulticastPaingList(QWidget * parent = 0);
    ~CDlgMulticastPaingList();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgMulticastPaingList();
    }

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

public:
    virtual bool LoadPageData();    // 读取页面数据
    virtual bool IsStatusChanged()
    {
        return false;   // 查看页面状态是否改变
    }
    virtual bool SavePageData()
    {
        return true;   // 保存页面数
    }

    virtual bool OnAction(const QString & strAction);
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);
    virtual bool GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData);

    static LRESULT OnMessage(msgObject & refObj);
    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    virtual void ExtraDialogInitData();

    virtual void BeforeSetSubPage(void * pData);

protected:
    //点击Option按钮
    void OnOptionClick();
    void OnEditClicked();
    void OnDeleteClicked();
    void OnDeleteAllClicked();

    //paging到序号为index的组，序号从1开始
    bool OnPaging(int iIndex);

    bool eventFilter(QObject * pObj, QEvent * pEvt);

    void showEvent(QShowEvent * pEvent);

protected:
    PagingListVec m_pagelist;
    int             m_CurSelect;
    bool            m_bShowPassword;
};


#endif  //_DLG_MULTICAST_PAGING_LIST_H_
