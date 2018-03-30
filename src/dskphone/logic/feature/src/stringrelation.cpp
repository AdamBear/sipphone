#include "stringrelation.h"

#include <stdio.h>
#include <stdlib.h>

CStringRelation::CStringRelation()
{
}

CStringRelation::CStringRelation(const yl::string & strFilename)
{
    OpenRelationFile(strFilename);
}

CStringRelation::~CStringRelation()
{

}

yl::string CStringRelation::GetStandardString(const yl::string & str)
{
    if (m_mapRelations.count(str))
    {
        return m_mapRelations[str];
    }

    return str;
}

bool CStringRelation::OpenRelationFile(const yl::string & strFilename)
{
    // 清除原数据
    m_mapRelations.clear();

    //打开文件
    FILE * fileRelation = fopen(strFilename.c_str(), "r");
    if (!fileRelation)
    {
        return false;
    }

    // 获取文件大小
    unsigned fileSize = GetFileSize(fileRelation);
    // 分配Buff
    char * szData = (char *)calloc(fileSize + 1, sizeof(char));

    fread(szData, sizeof(char), fileSize, fileRelation);
    fclose(fileRelation);

    ParseData(szData, fileSize);

    free(szData);
    return true;
}

bool CStringRelation::IsSameMeaning(const yl::string & strLhs, const yl::string & strRhs)
{
    return GetStandardString(strLhs) == GetStandardString(strRhs);
}

bool CStringRelation::ParseData(const char * szData, unsigned nSize)
{
    unsigned nBegin = 0;
    unsigned nEnd = 0;
    yl::string strCurrentStandardWord;
    bool bFirstWord = true;

    while (nBegin < nSize)
    {
        //查找下一个换行或者逗号
        while ((szData[nEnd] != '\n'
                && szData[nEnd] != ',')
                && nEnd < nSize)
        {
            ++nEnd;
        }

        //到达数据尾
        if (nEnd >= nSize)
        {
            //确认末尾没有词
            if (nBegin == nEnd)
            {
                break;
            }
        }

        //获取词
        yl::string strWord(szData + nBegin, szData + nEnd);
        //如果是该组词的第一个，则设置为标准词
        if (!strWord.empty())
        {
            if (bFirstWord)
            {
                strCurrentStandardWord = strWord;
                bFirstWord = false;
            }

            //添加到标准词的映射
            m_mapRelations[strWord] = strCurrentStandardWord;
        }

        //如果已换行，则新的一组词开始
        if (nEnd < nSize
                && szData[nEnd] == '\n')
        {
            bFirstWord = true;
        }

        //移动下标到下一个词的开始
        nBegin = ++nEnd;
    }
    return true;
}

bool CStringRelation::Count(const yl::string & strWord)
{
    return (m_mapRelations.count(strWord) > 0);
}

unsigned CStringRelation::GetFileSize(FILE * file)
{
    if (!file)
    {
        return 0;
    }

    fpos_t posOld;
    unsigned posEnd;

    fgetpos(file, &posOld);

    fseek(file, 0, SEEK_END);

    posEnd = ftell(file);

    fsetpos(file, &posOld);

    return posEnd;
}
