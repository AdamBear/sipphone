/*
同义词查找器
coder: lusy
2013年10月11日8:53:04

读取关系描述文件中的数据
可以用来判断两个词是否为同义

文件结构,同行表示同义，以第一个词为参考基准，词用","隔开(csv格式),注意逗号前后的空格将会算在词内
word1,word2,word3
word4

"word1","word2","word3"同义，以"word1"为基准词
*/

#ifndef __SETTING_UI_STRING_RELATION_H__
#define __SETTING_UI_STRING_RELATION_H__
#include <ylhashmap.h>
#include <ylstring.h>

class CStringRelation
{
public:
    CStringRelation();
    CStringRelation(const yl::string & strFilename);
    ~CStringRelation();

    // 打开关系文件
    bool OpenRelationFile(const yl::string & strFilename);

    // 获取基准词
    yl::string GetStandardString(const yl::string & str);

    // 比较两个词是否为同义
    bool IsSameMeaning(const yl::string & strLhs, const yl::string & strRhs);

    // 检查词语是否在字典中
    bool Count(const yl::string & strWord);

private:
    // 解析数据
    bool ParseData(const char * szData, unsigned nSize);

    // 获取文件大小
    static unsigned GetFileSize(FILE * file);

private:
    YLHashMap<yl::string, yl::string, STRING_HASH> m_mapRelations;

};


#endif //__SETTING_UI_STRING_RELATION_H__
