#include "bsftdirectoryaction.h"


CBSFTDirectoryAction::CBSFTDirectoryAction(const yl::string & strLocalFilePath,
        const yl::string & strRemoteFilePath)
    : CNetworkFileAction(strLocalFilePath, strRemoteFilePath)
{
    // 设置TaskAction的类型.
    m_eTARequestType = NS_TA::TA_REQUEST_NETWORK_FILE;

    m_bIsPreLoad = false;
    m_bIsBsftSearch = false;
}

CBSFTDirectoryAction::~CBSFTDirectoryAction()
{
}

void CBSFTDirectoryAction::SetBsftSearch(bool isSearch)
{
    m_bIsBsftSearch = isSearch;
}

bool CBSFTDirectoryAction::IsBsftSearch()
{
    return m_bIsBsftSearch;
}

void CBSFTDirectoryAction::SetPreLoad(bool isPreload)
{
    m_bIsPreLoad = isPreload;
}

bool CBSFTDirectoryAction::IsPreLoad()
{
    return m_bIsPreLoad;
}

