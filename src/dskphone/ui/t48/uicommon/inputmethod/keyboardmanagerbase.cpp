#include "keyboardmanagerbase.h"

CKeyboardManagerBase::CKeyboardManagerBase()
{

}

CKeyboardManagerBase::~CKeyboardManagerBase()
{

}

void CKeyboardManagerBase::LoadXmlFiles(listString& listFile, const yl::string& strPath)
{
    listString::iterator iterListFile = listFile.begin();
    for (; iterListFile != listFile.end(); ++iterListFile)
    {
        yl::string& strFile = *iterListFile;
        LoadXmlFiles(strPath, strFile);
    }
}

// 检查属性,存在就获取,不存在就返回false
bool CKeyboardManagerBase::DetectAttr(const xml_node& xmlNode, const char* lpszAttr, yl::string& strTemp)
{
    if (xmlNode.empty())
    {
        return false;
    }

    strTemp = xmlNode.attribute(lpszAttr).value();

    if (strTemp.empty())
    {
        return false;
    }

    return true;
}

// 获取节点内容,节点不存在返回false
bool CKeyboardManagerBase::GetItemValue(const xml_node& xmlFatherNode, const char* lpszItem, yl::string& strValue)
{
    if (xmlFatherNode.empty())
    {
        return false;
    }

    const char* pValue = xmlFatherNode.child_value(lpszItem);
    if (pValue == NULL)
    {
        return false;
    }

    strValue = pValue;
    return true;
}
