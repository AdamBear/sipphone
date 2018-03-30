#ifndef _VVM_OPTION_SELECT_DELEGATE_H
#define _VVM_OPTION_SELECT_DELEGATE_H

#include "messagebox/modmessagebox.h"
#include "baseui/framelistdelegate.h"

class CVVMOptionSelectDelegate : public QObject, public CFrameListDelegate,
    public CMsgBoxCallBackAction
{
    Q_OBJECT

public:
    CVVMOptionSelectDelegate(QObject * pParent);
    virtual ~CVVMOptionSelectDelegate();

public:
    //弹出框回调函数
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

public:
    void InitOptionData();

    // IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

    // IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

signals:
    void DialogAction(QString strAction);

protected:
    int m_nSelIndex;

};

#endif //_VVM_OPTION_SELECT_DELEGATE_H
