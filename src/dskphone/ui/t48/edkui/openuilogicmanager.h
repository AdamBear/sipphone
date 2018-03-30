#ifndef _OPENUI_LOGIC_MANAGER_H
#define _OPENUI_LOGIC_MANAGER_H

#include "singletonclassdefine.h"
#include <map>

// 判断是否允许进入UI的函数指针
typedef bool (*FunIsAllowEnterUI)();

class EDKUIOpenLogic
{
private:
    EDKUIOpenLogic();

public:
    //静态成员函数获取单例
    static  EDKUIOpenLogic * GetInstance();
    //初始化
    void init();
    //从文件加载映射
    void LoadXMLMapByNodeName(const yl::string & strFile, const yl::string & strNode);
    //设置对应UI的IsAllowEnter函数
    bool SetFuncAllowEnterUI(yl::string strHint, FunIsAllowEnterUI pFunc);
    //是否允许进入UI
    bool IsAllowEnterUI(yl::string strHint);

private:
    std::map<yl::string, FunIsAllowEnterUI>
    m_mapOpenLogic;     //提示命令ID和判断是否允许进入的函数映射
};

#define _EDKUIOpenLogic GET_SINGLETON_INSTANCE(EDKUIOpenLogic)

#endif