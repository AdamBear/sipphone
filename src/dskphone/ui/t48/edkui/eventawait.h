#ifndef _EDK_EVENT_WAIT_H
#define _EDK_EVENT_WAIT_H

#include <QObject>
#include "common/common_data_define.h"

// 用于等待EDK某个事件执行完成
class CEDKEventWait : public QObject
{
public:
    explicit CEDKEventWait(const KeyPressCallBackData & callBackData);
    virtual ~CEDKEventWait();

private:
    int                         m_iEdkId;
    FuncCallBackAfterKeyPress   m_pCallBackFun;
};

#endif // _EDK_EVENT_WAIT_H
