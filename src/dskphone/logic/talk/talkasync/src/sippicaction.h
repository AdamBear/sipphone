#ifndef __SIP_PIC_ACTION__
#define __SIP_PIC_ACTION__


#include "taskaction/taskaction.h"
#include "ETLLib.hpp"
#include "threadtask/include/networkfileaction.h"
#include "threadtask/include/common.h"


class CSipPicAction : public CNetworkFileAction
{
public:
    // | strLocalFilePath | 是远程文件将要存放在本地的路径.
    // | strRemoteFilePath | 是远程文件的地址.
    CSipPicAction(const yl::string& strLocalFilePath,
                  const yl::string& strRemoteFilePath);
    virtual ~CSipPicAction();

    // Override.
    virtual bool Exec();
};

#endif  // __SIP_PIC_ACTION__
