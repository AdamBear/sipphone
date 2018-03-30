// Main dialog of directory module to show all contacts.
// Created by yyj.

#ifndef __DLG_DIRECTORY_H__
#define __DLG_DIRECTORY_H__

#include "uimanager/basedialog.h"

class CFrameList;
class CTreeList;

class CDlgDirectory : public CBaseDialog
{
    Q_OBJECT

public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgDirectory();
    }

public:
    CDlgDirectory(QWidget * parent = NULL);
    virtual ~CDlgDirectory();

public:
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    virtual void UpdateWnd();

    // 焦点改变.
    virtual void OnFocusChanged(bool bFocused);

    // 获取联系人群组树形列表
    CTreeList * GetTreeList() const;

    // 获取联系人列表框
    CFrameList * GetFrameList() const;

    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

    // 获取页面默认图片路径
    virtual QPixmap GetCustomBackGround();

protected:
    virtual bool OnAction(const QString & strAction);

private:
    // 群组树形控件
    CTreeList * m_pTreeList;
    // 联系人列表框控件
    CFrameList * m_pFrameList;
};

#endif // DLGDIRECTORY_H
