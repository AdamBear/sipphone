#if 0
#include <ETLLib.hpp>
#include "localfileoperation.h"
#include "../include/dsskeystructure.h"
#include "taskaction/taskactioncallbackbase.h"


CLocalFileOperation::CLocalFileOperation(const yl::string & strLocalData, bool bData/* = false*/)
    : CLocalFileAction(strLocalData), m_bData(bData), m_strExtraData("")
{
    //开启可删除标志.
    SetCreatFlag(true);
}

CLocalFileOperation::~CLocalFileOperation()
{
}

// 获取Extra数据
yl::string CLocalFileOperation::GetExtraData()
{
    return m_strExtraData;
}

// 设置Extra数据
void CLocalFileOperation::SetExtraData(const yl::string & strExtraData)
{
    m_strExtraData = strExtraData;
}

// 获取BLFLine账号
int CLocalFileOperation::GetBLfListLine()
{
    return m_iBLFListLine;
}

// 设置BLFLine账号
void CLocalFileOperation::SetBLFListLine(int iLine)
{
    m_iBLFListLine = iLine;
}

bool CLocalFileOperation::Exec()
{
    // 直接调用基类的Exec().
    return CLocalFileAction::Exec();
}

bool CLocalFileOperation::Read()
{
    if (m_bData)
    {
        // 如果是数据，则路径中保存的会是文件数据
        if (!GetFilePath().empty())
        {
            SetFileContent(GetFilePath());
        }

        SetFilePath("");
    }
    else
    {
        CLocalFileAction::SetRead(true);
        // 调用基类读取文件到内存
        CLocalFileAction::Exec();
    }

//  DSSKEY_INFO("CLocalFileOperation:: Parse BLF List");
    return m_blfListParser.Parse(GetFileContent().c_str());
    // 由回调函数来决定如何读文件
//  if (NULL != GetCallback())
//  {
//      GetCallback()->OnTaskActionCallback(this);
//  }
//  return true;
}

// 返回blf list解析对象
CBLFListParser * CLocalFileOperation::GetBLFListParse()
{
    return &m_blfListParser;
}
#endif

