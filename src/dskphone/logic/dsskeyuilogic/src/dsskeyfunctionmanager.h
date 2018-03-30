#ifndef _DSSKEY_FUNCTION_MANAFER_HEADER_
#define _DSSKEY_FUNCTION_MANAFER_HEADER_

#include <ylstring.h>
#include <ylvector.h>
#include <ylhashmap.h>
#include "dsskeyfunction.h"

enum FunctionAttr
{
    DA_NONE = 0x00,
    DA_LOAD = (0x01 << 0),
    DA_SAVE = (0x01 << 1),
    DA_STATIC  = (0x01 << 7),
    DA_DYNAMIC = (0x01 << 8),
    DA_REGEXP  = (0x01 << 9),
};

struct CFunDetails
{
    yl::string  strID;       // 函数ID
    yl::string  strFunction;  // 函数数据
    int         iTipsID;    // 错误提示
    int         iAttr;      // 函数属性
    bool        bResult;    // 静态函数缓存的结果

    CFunDetails() : iAttr(DA_NONE), iTipsID(INVALID_INDEX), bResult(true) {}
    CFunDetails(int i) : iAttr(i), iTipsID(INVALID_INDEX), bResult(true) {}
};

// 函数操作
class CFunctionManager
{
public:
    CFunctionManager();

public:

    // 保存提示语
    void CreateTips(int iTipsID, const yl::string & strTips);

    // 获取函数信息
    bool CreateFuncData(yl::string & strID, LPCSTR lpType, yl::string & strValue, int iTipsID);

    // 保存函数触发动作
    bool OnSaveAction(const yl::string & strID, const yl::string & strValue = "");

    // 加载函数触发动作
    bool OnLoadAction(const yl::string & strID);

    // 释放
    void UnInit();

    // 获取错误信息
    yl::string GetErrorString();

    // 创建函数结果
    bool CreateFuctionResult();

    // 保存xml
    bool SaveRegexp(LPCSTR lpFileName);

private:
    // 解析函数
    bool ParseFunction(const yl::string & strFunction);

    // 解析多重函数
    bool OnMultipleParse(const yl::string & strID, const yl::string & strValue, bool bSaveData);

    // 解析单个函数
    bool OnSingleParse(const yl::string & strID, const yl::string & strValue, bool bSaveData);

    // 获取翻译
    CDsskeyPair * GetPairByIndex(int iIndex);

    // 获取函数体
    CFunDetails * GetFunDataByID(const yl::string & strID);

private:
    typedef YLVector<CFunDetails>                       vecFunDetails;
    typedef YLVector<CCfgStaticFunctionBase *>           vecFunctionImp;

    vecFunctionImp              m_vecFunImp;    // 函数指针数组
    vecFunDetails               m_vecFunData;   // 函数操作数据集合
    VecCandidateValue           m_vecTips;      // 提示语信息

    int                         m_iLastTipsID;  // 上一次错误信息
};

#endif
