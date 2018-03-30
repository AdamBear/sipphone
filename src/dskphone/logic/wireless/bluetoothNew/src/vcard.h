#ifndef _VCARD__H
#define _VCARD__H

#include "yllist.h"
#include "ylvector.h"
#include "ylstring.h"
#include "ylhashmap.h"

YLVector<yl::string > & _split(const yl::string & s, char delim, YLVector<yl::string > & elems);
void  _trimstring(yl::string & strIn, const yl::string & strTrim);

enum PropertyType
{
    PROPERTY_CUSTOM = 0,            // All custom types are handled separately to
    PROPERTY_FN,                // Text             Formatted String Name (Dr. Jane Doe)
    PROPERTY_N,             // Text             Structured Name String (Doe;Jane;;Dr;)
    PROPERTY_PHOTO,             // URI              Photo of vCard ownder
    PROPERTY_TEL,               // Text/(URI)           Telephone Number - TYPE
    PROPERTY_REV,               // Timestamp            Revision time of vCard
};


// 号码类型
typedef enum NumberType
{
    NUM_TYPE_OTHER = -1,
    NUM_TYPE_OFFICE,
    NUM_TYPE_PHONE,
    NUM_TYPE_HOME,
} typeNumType;

typedef YLHashMap<yl::string, int>  MAP_NUMBER_TYPE;
typedef MAP_NUMBER_TYPE::iterator   MAP_NUMBER_TYPE_ITER;
typedef struct
{
    yl::string strContentName;
    MAP_NUMBER_TYPE mapNumber;

    void Reset()
    {
        strContentName.clear();
        mapNumber.clear();
    }

    bool IsEmpty()
    {
        bool bEmpty = strContentName.empty();
        for (MAP_NUMBER_TYPE_ITER itBeg = mapNumber.begin();
                itBeg != mapNumber.end();
                ++ itBeg)
        {
            const yl::string & strNumber = (itBeg->first);

            bEmpty &= strNumber.empty();
        }

        return bEmpty;
    }

    void FormatName()
    {
        for (MAP_NUMBER_TYPE_ITER itBeg = mapNumber.begin();
                itBeg != mapNumber.end();
                ++itBeg)
        {
            const yl::string & strNumber = (itBeg->first);
            if (!strNumber.empty())
            {
                strContentName = strNumber;
                break;
            }
        }
    }
} MobileContactInfo;

typedef YLList<MobileContactInfo>   LIST_MOBILE_CONTACT;
typedef LIST_MOBILE_CONTACT::iterator IT_MOBILE_CONTACT;

/* -------- Vcard Formate Example-------- *
BEGIN:VCARD
VERSION:3.0
FN:test
N:test
TEL;TYPE=HOME:866XXXXX
TEL;TYPE=WORK:23XX
TEL;TYPE=CELL:158980XXXXXX
UID:74
END:VCARD
BEGIN:VCARD
* ----------------- End -----------------*/
/* Class representing a single line of a vCard */
class ContentLine
{
public:
    ContentLine(const yl::string & line);
    ContentLine() {;}
    PropertyType property() const;
    yl::string  propertyValue();
    yl::string  GetNumTypeKeyword()
    {
        return m_ParamAndValue;
    }
protected:
    yl::string  GeneratrNValue(const yl::string & strIn);
#if 0
    yl::string  propertyName() const;
    void GenerateParamAndValue(const yl::string & strParamAndValue);
    yl::string  paramValue(const yl::string & strParamName);
    yl::string  GeneratePhoto(const yl::string & strIn);
#endif // 0

private:
    yl::string          m_ParamAndValue;
    yl::string          m_value;
    yl::string          m_customPropertyName;
    YLVector<yl::string >   m_customParameters;
    YLVector<yl::string >   m_customParameterValues;
    PropertyType            m_propertyType;
    bool m_bPraseParam;
};

class VCard
{
public:
    VCard();
    static yl::string  stringFromPropertyType(PropertyType p);
    static PropertyType enumFromPropertyType(const yl::string & p);
    static bool exportToVCardlist(LIST_MOBILE_CONTACT & listCard, const char * szPath);
    static int GetNumberType(const yl::string & strType);
protected:
    static bool isSupportContenline(const ContentLine & line);
private:
    //LIST_CONTENTLINE m_fields;
};
#endif // _VCARD__H

