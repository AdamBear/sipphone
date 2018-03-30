#ifndef __SYNC_PROCESS_H__
#define __SYNC_PROCESS_H__

#include "xmlparser/xmlparser.hpp"

///////////////////////对Sync的处理基类/////////////////////////
class CSyncBaseProcess
{
public:
    CSyncBaseProcess();
    virtual ~CSyncBaseProcess();

    // 处理服务器的同步返回
    virtual VOID ProcessXMLInfo(xml_node ElementNode, int iAccount) = 0;

    // 获取同步的字符串内容
    yl::string GetXMLExtraData(int iLineID, VOID * pTypeData);

protected:
    // 将要同步到服务器的XML数据填充到 pXmlFile 节点中
    virtual VOID FillXMLExtraData(xml_document & docXmlFile, int iLineID, VOID * pTypeData) = 0;

    // 获取成员内容
    LPCSTR GetElementContent(xml_node nodeElementNode, LPCSTR lpszChildName, yl::string & strContent);

};

///////////////////////对Sync DND的处理类/////////////////////////
class CSyncUpdateProcess : public CSyncBaseProcess
{
public:
    virtual VOID ProcessXMLInfo(xml_node nodeElementNode, int iAccount) {}
protected:
    virtual VOID FillXMLExtraData(xml_document & docXmlFile, int iLineID, VOID * pTypeData) {}
};

///////////////////////对Sync DND的处理类/////////////////////////
class CSyncDNDProcess : public CSyncBaseProcess
{
public:
    CSyncDNDProcess();
    ~CSyncDNDProcess();

    virtual VOID ProcessXMLInfo(xml_node nodeElementNode, int iAccount);

protected:
    virtual VOID FillXMLExtraData(xml_document & docXmlFile, int iLineID, VOID * pTypeData);
};

///////////////////////对Sync FWD的处理类/////////////////////////
class CSyncFWDProcess : public CSyncBaseProcess
{
public:
    CSyncFWDProcess();
    ~CSyncFWDProcess();

    virtual VOID ProcessXMLInfo(xml_node nodeElementNode, int iAccount);

protected:
    virtual VOID FillXMLExtraData(xml_document & docXmlFile, int iLineID, VOID * pTypeData);

    // 获取类型
    yl::string GetFWDTypeString(int iFWDType, int iLineID);
};

///////////////////////对Sync ACD的处理类/////////////////////////
class CSyncACDProcess : public CSyncBaseProcess
{
public:
    CSyncACDProcess();
    ~CSyncACDProcess();

    virtual VOID ProcessXMLInfo(xml_node nodeElementNode, int iAccount);

protected:
    virtual VOID FillXMLExtraData(xml_document & docXmlFile, int iLineID, VOID * pTypeData) {}
};

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
class CSyncCallRecordProcess : public CSyncBaseProcess
{
public:
    CSyncCallRecordProcess();
    ~CSyncCallRecordProcess();

    virtual VOID ProcessXMLInfo(xml_node nodeElementNode, int iAccount);

protected:
    virtual VOID FillXMLExtraData(xml_document & docXmlFile, int iLineID, VOID * pTypeData) {}
};
#endif

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
class CSyncExecutiveAssistantProcess : public CSyncBaseProcess
{
public:
    ~CSyncExecutiveAssistantProcess() {}

    virtual VOID ProcessXMLInfo(xml_node nodeElementNode, int iAccount);

protected:
    virtual VOID FillXMLExtraData(xml_document & docXmlFile, int iLineID, VOID * pTypeData) {}
};
#endif

#endif // __SYNC_PROCESS_H__
