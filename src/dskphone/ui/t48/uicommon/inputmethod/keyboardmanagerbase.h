#ifndef CKEYBOARDMANAGERBASE_H
#define CKEYBOARDMANAGERBASE_H

#include "inputmanagerdefine.h"
#include "keyboardxmlreader.h"
#include "keyboardhintmanager.h"
#include <xmlparser/xmlparser.hpp>

class CKeyboardManagerBase
{
public:
    CKeyboardManagerBase();
    virtual ~CKeyboardManagerBase();

public:
    //初始化入口
    virtual void InitKeyboardFromXmlFile() = 0;

protected:
    //根据文件名列表和路径类型获取文件
    void LoadXmlFiles(listString& listFile, const yl::string& strPath);
    //根据路径名和文件名获取文件
    virtual void LoadXmlFiles(const yl::string& strPath, const yl::string& strFile) = 0;

    // 检查属性,存在就获取,不存在就返回false
    bool DetectAttr(const xml_node& xmlNode, const char* lpszAttr, yl::string& strTemp);

    // 获取节点内容,节点不存在返回false
    bool GetItemValue(const xml_node& xmlFatherNode, const char* lpszItem, yl::string& strValue);

};

#endif // CKEYBOARDMANAGERBASE_H
