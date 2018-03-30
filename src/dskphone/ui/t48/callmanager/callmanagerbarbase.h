#ifndef _CALL_MANAGER_BAR_BASE_H
#define _CALL_MANAGER_BAR_BASE_H

#include <QtGui/QWidget>
#include "callmanagerstruct.h"

class CCallManagerBarBase: public QWidget
{
public:
    CCallManagerBarBase(QWidget * parent = NULL);
    virtual ~CCallManagerBarBase(void);

public:
    //设置所有 item 信息
    virtual void SetCallMGRListInfo(YLList<CCallMGRInfo *> & listCallMGRInfo);

    //设置焦点 item
    virtual void SetFocusMGRInfo(const yl::string & strUIName);


};

#endif
