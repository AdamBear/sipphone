#include "logic_inc.h"
#include "xmlbrowseraction.h"
#include "adapterbox/include/modadapterbox.h"

#ifdef IF_FEATURE_XMLBROWSER

//////////////////////////////////////////////////////////////////////////
bool CImageCompressAction::Exec()
{
    // 初始为失败
    m_stImageParam.bCompressSuc = false;

    if (m_stImageParam.nWidth <= 0
            || m_stImageParam.nHeight <= 0)
    {
        XMLBROWSER_INFO("Invalid compress param.");
        m_iErrCode = ERR_PARAM;
        return false;
    }

    // 文件不存在则给出错误提示
    if (!pathFileExist(m_strImageFile.c_str()))
    {
        m_iErrCode = ERR_EXIST;
        return false;
    }


    XMLBROWSER_INFO("src: %s,dst: %s", m_strImageFile.c_str(), m_stImageParam.strDstPath.c_str());


    int iRet = AdapterBox_ImageScale(m_strImageFile, m_stImageParam.strDstPath, WALL_PAPER_IMAGE,
                                     m_stImageParam.nWidth, m_stImageParam.nHeight);

    // 判断压缩结果（相应错误提示由UI给出）
    if (iRet == IMAGE_SIZE_OVER)
    {
        // 分辨率过高的提示信息
        XMLBROWSER_INFO("Too hight of resolution!");
        m_iErrCode = ERR_PIXEL;
    }
    else if (iRet != IMAGE_SCALED_SUCCESS)
    {
        m_iErrCode = ERR_COMPRESS;
    }
    else
    {
        m_iErrCode = ERR_OK;
    }

    m_stImageParam.bCompressSuc = (m_iErrCode == ERR_OK);
    return true;
}

#endif // #ifdef IF_FEATURE_XMLBROWSER



