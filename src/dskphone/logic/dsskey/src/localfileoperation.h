#ifndef __LOCALFILE_OPERATION_H__
#define __LOCALFILE_OPERATION_H__

#if 0
#include "threadtask/include/localfileaction.h"
#include <ylstl/ylstring.h>
#include "BLFListParse.h"

class CBLFListParser;

class CLocalFileOperation : public NS_TA::CLocalFileAction
{
public:
    // | strLocalFilePath | 为本地文件路径
    CLocalFileOperation(const yl::string & strLocalData, bool bData = false);
    virtual ~CLocalFileOperation();

    // 重写
    virtual bool Exec();

    // 获取Extra数据
    yl::string GetExtraData();
    // 设置Extra数据
    void SetExtraData(const yl::string & strExtraData);
    // 获取BLFLine账号
    int GetBLfListLine();
    // 设置BLFLine账号
    void SetBLFListLine(int iLine);
    // 返回blf list解析对象
    CBLFListParser * GetBLFListParse();

private:
    // 重写. 读取文件内容
    virtual bool Read();

private:
    bool    m_bData;        // 传过来的参数是否直接是数据
    yl::string m_strExtraData;  // 备用变量,用于保存其他信息
    int     m_iBLFListLine; // 保存BLFList要更新的账号
    CBLFListParser m_blfListParser;// 保存解析blf list数据的对象
};
#endif
#endif //__LOCALFILE_OPERATION_H__
