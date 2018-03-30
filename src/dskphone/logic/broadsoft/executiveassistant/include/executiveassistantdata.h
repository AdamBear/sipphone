#ifndef __EXECUTIVE_ASSISTANT_DATA_H_
#define __EXECUTIVE_ASSISTANT_DATA_H_

#include "execommon.h"
#include "executiveassistantparse.h"
#include "executiveassistantprocess.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
struct ActionItem
{
    ExeAssis_Action  eAction;
    const char   *   strAction;
    int              iAccessMode;
};

typedef YLHashMap<int, ExeAssisData *>           MapExeAssia;
typedef YLVector<yl::string>                    NumberList;

// 该类用于缓存Executive数据，以及对应的操作函数集合、查询和设置
class CExecutiveAssistantData
    : protected CExecutiveAssistantParse
{
public:
    // 依类型获取数据
    bool GetData(CExeAssisInspector * pData, int iAccount);
    // 设置数据，会通过XSI进行同步
    bool SetData(const CExeAssisInspector * pData, int iAccount,
                 ExeAssis_Action eAction = EAA_AutoSelect);
    // 根据角色获取账号个数
    int GetAccountListByType(YLVector<int> & listData, EEA_TYPE eType);
    // 设置对应账号的Initiation Executive号码
    bool SetCallInitiationNumber(const yl::string & strNumber, int iAccount);
    // Assistant对应的Executive的个数
    int GetExecutiveListSize(int iAccount);

    // 设置用户属性，在功能键同步时
    bool SetUserType(EEA_TYPE eType, int iAccount, const CExeAssisInspector * pInfo);

    // 获取用户属性函数
    int GetUserType(int iAccount);
    // 获取对应账号的Initiation Executive号码
    yl::string GetInitiationNumber(int iAccount);

    // 本地配置保存和获取
    bool Save2Local(const CExeAssisInspector * pData, int iAccount);
    bool Local2Map(int iAccount, ExeAssis_Action eActionType, int iIndex = -1);

    LRESULT OnMessage(msgObject & msg);

    bool ParseReponseCmdFromFile(const yl::string & strFilePath, int iAccount);
    // 生成本地XML文件
    bool SaveRequestCmdToFile(const yl::string & strFilePath, ExeAssis_Action eAction, int iAccount);
    // Executive 列表不能作为一个整体推送，必须分片
    bool SaveRequestCmdToFileSplit(const yl::string & strFilePath, int iIndex, int iAccount);

    // 本地权限检查
    bool CheckAction(ExeAssis_Action eActionType, int iAccount);

    bool IsExecutiveNumber(const yl::string & strNumber, int iAccount);

protected:
    void SplitRefOperator(bool bAdd = true, ExeAssis_Action eAction = EAA_AutoSelect);

public:
    void SetLineId(int iAccount)
    {
        m_iLine = iAccount;
    }
    int  GetLineId()
    {
        return m_iLine;
    }

    CExecutiveAssistantData();
    virtual ~CExecutiveAssistantData();

private:
    // XML关键字和Action的对应关系
    static ExeAssis_Action GetActionType(const yl::string & strAction);
    static yl::string GetActionByType(ExeAssis_Action eActionType);

    //static yl::string GetSipNumber(const yl::string& strNum);

    // 类型到权限的映射
    int GetUserType(ExeAssis_Action eActionType);

    // 查找对应账号的配置信息
    ExeAssisData * GetInfoById(int iAccount);

    // 填充XML公共头部
    bool SaveCommonHeader(xml_document & doc, ExeAssis_Action eAction, xml_node & nodeRoot);

private:
    // Debug相关函数，打印Map里面的信息
    void PrintInfo();

    void PrintItem(const char * pDsp, ExeAssisData * pData);

    void PrintFile(const yl::string & strFile, int iAccount);

protected:
    // 分片数
    int                                 m_iSplitCount;
private:
    // 所有数据
    MapExeAssia                         m_mapExeAssisInfo;
    // UI当前操作的账号
    int                                 m_iLine;
    // Initiation 各账号对应的Executive号码
    NumberList                          m_vecExeNumber;
    //  本地缓存
    ExeAssisData                        m_ExeAssInfo;
};

#endif //#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
#endif //__EXECUTIVE_ASSISTANT_DATA_H_
