#ifndef DIRECTORY_CONTACTLOCALSAVEACTION_H_
#define DIRECTORY_CONTACTLOCALSAVEACTION_H_

#include "taskaction/taskaction.h"
#include "threadtask/include/networkfileaction.h"

class CBSFTDirectoryAction : public CNetworkFileAction
{
public:
    // 构造函数.
    // | strLocalFilePath | 是远程文件将要存放在本地的路径.
    // | strRemoteFilePath | 是远程文件的地址.
    CBSFTDirectoryAction(const yl::string & strLocalFilePath, const yl::string & strRemoteFilePath);
    virtual ~CBSFTDirectoryAction();

    void SetBsftSearch(bool isSearch);
    bool IsBsftSearch();

    void SetPreLoad(bool isPreLoad);
    bool IsPreLoad();

private:
    bool m_bIsBsftSearch;

    bool m_bIsPreLoad;
};

#endif  // DIRECTORY_CONTACTLOCALSAVEACTION_H_
