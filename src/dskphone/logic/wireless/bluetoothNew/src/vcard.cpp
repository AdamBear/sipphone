#include "vcard.h"
#include <cstdlib>
#include <stdarg.h>
#include <stdio.h>
#include <winimp/winimp.h>

/* ********************************************************************** */
/* *************************** Helper methods *************************** */
/* ********************************************************************** */

// 分离字符串
// 输入: strInput 待分离的字符串; Separator 分离字符串的关键字
// 输出: OutPutList 依次存放每个被分离的字段
YLVector<yl::string > & _split(const yl::string & strInput, const char Separator,
                               YLVector<yl::string > & OutPutList)
{
    if (strInput.empty())
    {
        return OutPutList;
    }

    OutPutList.clear();

    yl::string  InputCopy(strInput);
    int nFound = InputCopy.find_first_of(Separator);
    while (nFound != yl::string ::npos)
    {
        yl::string  strSub = InputCopy.substr(0, nFound);
        InputCopy = InputCopy.substr(nFound + 1);

        OutPutList.push_back(strSub);
        nFound = InputCopy.find_first_of(Separator);
    }

    if (!InputCopy.empty())
    {
        OutPutList.push_back(InputCopy);
    }
    return OutPutList;
}

void  _trimstring(yl::string & strIn, const yl::string & strTrim)
{
    strIn.erase(0, strIn.find_first_not_of(strTrim));

    strIn.erase(strIn.find_last_not_of(strTrim) + 1);
}

yl::string  _formatstring(const char * szFormat, ...)
{
    // Use va_list to format.
    va_list vl;
    va_start(vl, szFormat);

    int len = -1;

    yl::string  strRet("");
    len = vsnprintf(NULL, 0, szFormat, vl);

    if (len > 0)
    {
        char * pBuffer = new char[len + 1];
        if (pBuffer)
        {
            memset(pBuffer, 0, len + 1);
            vsnprintf(pBuffer, len + 1, szFormat, vl);
            strRet = pBuffer;
            delete[] pBuffer;
        }
    }

    // Append '\0' to the end of string.
    va_end(vl);

    return strRet;
}
/* ********************************************************************** */
/* ************************ VCard Implementation ************************ */
/* ********************************************************************** */

VCard::VCard()
{
}

bool VCard::exportToVCardlist(LIST_MOBILE_CONTACT & listCard, const char * szPath)
{
    if (NULL == szPath)
    {
        return false;
    }

    FILE * pFile = fopen(szPath, "r");
    if (NULL == pFile)
    {
        return false;
    }

    char * pLine = NULL;
    size_t len = 0;
    size_t read;
    MobileContactInfo mContact;
    yl::string strLine;

    while (-1 != (read = getline(&pLine, &len, pFile)))
    {
        if (len <= 0)
        {
            continue;
        }
        strLine = pLine;

        //先去掉两边的空格
        _trimstring(strLine, "  \n\r\t");
        if (strLine.substr(0, 11) == "BEGIN:VCARD")
        {
            mContact.Reset();
        }
        else if (strLine.substr(0, 8) == "VERSION:")
        {
            continue;
        }
        else if (strLine.substr(0, 9) == "END:VCARD")
        {
            //name为空，使用第一个号码字段填充个
            //http://10.2.1.199/Bug.php?BugID=76896
            if (mContact.strContentName.empty())
            {
                mContact.FormatName();
            }

            //有效联系人
            if (!mContact.IsEmpty())
            {
                listCard.push_back(mContact);
            }
        }
        else
        {
            ContentLine cl(strLine);
            if (!isSupportContenline(cl))
            {
                continue;
            }

            yl::string strValue = cl.propertyValue();

            if (PROPERTY_FN == cl.property())
            {
                mContact.strContentName = strValue;
            }
            else if (PROPERTY_TEL == cl.property())
            {
                // 重复号码检测
                /*MAP_NUMBER_TYPE_ITER iter = mContact.mapNumber.find(strValue);
                if (iter != mContact.mapNumber.end())
                {
                    continue;
                }*/
                mContact.mapNumber.insert(strValue, GetNumberType(cl.GetNumTypeKeyword()));
            }
        }
    }

    if (pLine)
    {
        free(pLine);
    }

    fclose(pFile);

    return listCard.size() > 0;
}

int VCard::GetNumberType(const yl::string & strType)
{
    typeNumType eNumType = NUM_TYPE_OTHER;

    if (strType == "TYPE=WORK")
    {
        eNumType = NUM_TYPE_OFFICE;
    }
    else if (strType == "TYPE=CELL")
    {
        eNumType = NUM_TYPE_PHONE;
    }

    return eNumType;
}

bool VCard::isSupportContenline(const ContentLine & cl)
{
    if (cl.property() == PROPERTY_FN
            //|| cl.property() == PROPERTY_N
            || cl.property() == PROPERTY_TEL
       )
    {
        return true;
    }
    return false;
}

yl::string VCard::stringFromPropertyType(PropertyType p)
{
    switch (p)
    {
    case PROPERTY_FN:
        return "FN";
    case PROPERTY_N:
        return "N";
    case PROPERTY_PHOTO:
        return "PHOTO";
    case PROPERTY_TEL:
        return "TEL";
    case PROPERTY_REV:
        return "REV";
    case PROPERTY_CUSTOM:
    default:
        break;
    }
    return "";
}

PropertyType VCard::enumFromPropertyType(const yl::string & p)
{
    if (p == "FN")
    {
        return PROPERTY_FN;
    }
    else if (p == "N")
    {
        return PROPERTY_N;
    }
    else if (p == "PHOTO")
    {
        return PROPERTY_PHOTO;
    }
    else if (p == "TEL")
    {
        return PROPERTY_TEL;
    }
    else if (p == "REV")
    {
        return PROPERTY_REV;
    }
    else
    {
        return PROPERTY_CUSTOM;
    }
}

/* ********************************************************************** */
/* **************************** Content Line **************************** */
/* ********************************************************************** */
// ContentLine Parser
ContentLine::ContentLine(const yl::string & line) :
    m_ParamAndValue(""),
    m_value(""),
    m_customPropertyName(""),
    m_propertyType(PROPERTY_CUSTOM),
    m_bPraseParam(false)
{
    YLVector<yl::string > nameAndValue;
    _split(line, ':', nameAndValue);

    if (nameAndValue.size() < 2)
    {
        return;
    }

    //Get Property And  Set Param String
    m_customPropertyName = nameAndValue.at(0);
    int iFound = m_customPropertyName.find_first_of(';');
    if (iFound != yl::string ::npos)
    {
        m_customPropertyName = m_customPropertyName.substr(0, iFound);
        m_ParamAndValue = nameAndValue.at(0).substr(iFound + 1);
    }

    iFound = m_customPropertyName.find('.');
    if (m_customPropertyName.find('=') == yl::string::npos
            && iFound != yl::string ::npos)
    {
        m_customPropertyName = m_customPropertyName.substr(iFound + 1);
    }

    m_propertyType = VCard::enumFromPropertyType(m_customPropertyName);

    // Set value
    m_value = nameAndValue.at(1);
    _trimstring(m_value, "  \n\r");
}

//获取类型
PropertyType ContentLine::property() const
{
    return m_propertyType;
}



//获取值
/************************************************************************/
/* 可能存在多个value
* 各组成部分用;隔开
*/
/************************************************************************/
yl::string  ContentLine::propertyValue()
{
    if (m_propertyType == PROPERTY_N
            || m_propertyType == PROPERTY_FN)
    {
        return GeneratrNValue(m_value);
    }
#if 0
    if (m_propertyType == PROPERTY_PHOTO)
    {
        return GeneratePhoto(m_value);
    }
#endif // 0

    return m_value;
}

//生成N/FN值
yl::string  ContentLine::GeneratrNValue(const yl::string & strIn)
{
    if (strIn.empty())
    {
        return "";
    }

    YLVector<yl::string > value;
    _split(strIn, ';', value);

    if (value.size() == 0)
    {
        return "";
    }

    yl::string  strOut = value.at(0);

    if (value.size() == 1)
    {
        return strOut;
    }

    for (int i = 1; i < value.size(); ++i)
    {
        if (value.at(i).empty())
        {
            continue;
        }

        strOut = _formatstring("%s %s", strOut.c_str(), value.at(i).c_str());
    }

    return strOut;
}

#if 0

/************************************************************************/
/* 类型+参数
* 类型;[参数=value];[参数];....
*/
/************************************************************************/
void ContentLine::GenerateParamAndValue(const yl::string & strParamAndValue)
{
    //只解析一次
    m_bPraseParam = true;

    YLVector<yl::string > groupNameParams;
    _split(strParamAndValue, ';', groupNameParams);
    if (groupNameParams.size() == 0)
    {
        return;
    }

    YLVector<yl::string >::iterator itor = groupNameParams.begin();

    while (itor != groupNameParams.end())
    {
        YLVector<yl::string > param_value;
        _split(*itor, '=', param_value);

        if (param_value.size() == 1)
        {
            m_customParameters.push_back(param_value.at(0));
            m_customParameterValues.push_back("");
        }

        // Save the parameters
        if (param_value.size() == 2)
        {
            m_customParameters.push_back(param_value.at(0));
            m_customParameterValues.push_back(param_value.at(1));
        }

        ++itor;
    }
}

//获取类型名
yl::string  ContentLine::propertyName() const
{
    if (m_propertyType == PROPERTY_CUSTOM)
    {
        return m_customPropertyName;
    }
    return VCard::stringFromPropertyType(m_propertyType);
}

//生成photo 路径
yl::string  ContentLine::GeneratePhoto(const yl::string & strIn)
{
    if (strIn.empty())
    {
        return "";
    }

    //照片格式
    yl::string  strPhotoFormat = paramValue("TYPE");
    if (strPhotoFormat.empty())
    {
        return "";
    }

    //ToDo生成文件名及文件
    //需要考虑文件名重复及空间大小
    return "";
}


//获取PARAM值
yl::string  ContentLine::paramValue(const yl::string & strParamName)
{
    if (!m_bPraseParam)
    {
        GenerateParamAndValue(m_ParamAndValue);
    }
    for (int i = 0; i < m_customParameters.size(); ++i)
    {
        if (strParamName == m_customParameters[i])
        {
            if (i >= m_customParameterValues.size())
            {
                break;
            }
            return m_customParameterValues[i];
        }
    }

    return "";
}
#endif // 0

