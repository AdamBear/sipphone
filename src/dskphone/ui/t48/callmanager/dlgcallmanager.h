#ifndef _DLG_CALLMANAGER_H
#define _DLG_CALLMANAGER_H

#include "dlgcallmanagerbase.h"

class CDlgCallManager: public CDlgCallManagerBase
{
    Q_OBJECT

public:
    CDlgCallManager(void);
    virtual ~CDlgCallManager(void);

public:
    // 返回该窗口指针
    static CBaseDialog * CreateInstance();

    //创建softkey bar 实例
    virtual void CreateSoftkeyBar();

    //设置窗口位置
    virtual void SetRect(const QRect & rect);

    //设置窗口的提示模式命令
    virtual void SetHintTitle(const QString & strHint);

    //获取窗口提示模式命令
    virtual const QString GetWindowHint() const;

public slots:
    void OnBarClick(const QString & strClickAction);

};


#endif
