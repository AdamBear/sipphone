#ifndef IME_PROXY_H
#define IME_PROXY_H

#include <ETLLib.hpp>
#include <QObject>
using namespace std;

//最小的 保存字符数组 单元个数
#define MIN_IME_ARRAY_COUNT 12

class CIMEProxy
{
public:
    static CIMEProxy& GetInstance();

private:
    CIMEProxy(void);
public:
    virtual ~CIMEProxy(void);

public:
    // 初始化IMEText
    bool InitIMEText();

public:
    QString WCHARStrToQString(WCHAR* wcharData);

protected:
    // 释放读入的IME资源
    void freeIMEText();

protected:
    FILE* OpenAndCopyFactoryFile(std::string strConfigIMEPath, std::string strFactoryIMEPath, std::string strIMEFileName);

public:
    // 定义保存输入法字符串序列的指针数组
    WCHAR* pwText_123[MIN_IME_ARRAY_COUNT];// 保存输入法123内容的指针数组
    WCHAR* pwText_abc[MIN_IME_ARRAY_COUNT];// 保存输入法abc内容的指针数组
    WCHAR* pwText_Abc[MIN_IME_ARRAY_COUNT];// 保存输入法Abc内容的指针数组
    WCHAR* pwText_ABC[MIN_IME_ARRAY_COUNT];// 保存输入法ABC内容的指针数组
    WCHAR* pwText_2aB[MIN_IME_ARRAY_COUNT];// 保存输入法2aB内容的指针数组
    WCHAR* pwText_123_Dial[MIN_IME_ARRAY_COUNT];// 保存输入法123_Dial内容的指针数组
    WCHAR* pwText_123_IP[MIN_IME_ARRAY_COUNT];// 保存输入法123_IP内容的指针数组
    WCHAR* pwText_hebrew[MIN_IME_ARRAY_COUNT];  //保存输入法Hebrew内容的指针数组
    WCHAR* pwText_symbol[MIN_IME_ARRAY_COUNT];  //保存符号输入法内容的指针数组

private:
    //类内部循环使用的序号
    int iIndex;
};

#define _IMEPROXY CIMEProxy::GetInstance()


#endif
