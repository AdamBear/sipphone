#ifndef __HOTDESKING_SOFTKEY_H__
#define __HOTDESKING_SOFTKEY_H__

#ifdef IF_FEATURE_HOTDESKING
#ifdef IF_ADVANCE_HOTDESKING

#define MAXKEYNUM 20
#include "singletonclassdefine.h"
#include "xmlparser/xmlparser.hpp"

struct DeskKey_Info
{
    char * pType;
    char * pLabel;
    char * pValue;
};

class HotDesking_SoftKey
{
    DECLARE_SINGLETON_CLASS(HotDesking_SoftKey)

public :
    VOID LoadSoftKey();
    VOID ArrangeSoftKey();

    VOID On_FuncKeyPress(int iKey);
    VOID ProcessSoftKeyMore();
    VOID ProcessURI(LPCSTR pstr);
    VOID ReplaceURL(yl::string & str);

    VOID AddAllKeyInfo(int iIndex, xml_node nodeXmlNode);
    VOID AddKeyInfo(DeskKey_Info & ToKey, const yl::string & pType, const yl::string & pLabel,
                    const yl::string & pValue);
    BOOL GetValue(xml_node nodeXmlNode, yl::string & str);
    VOID FreeString(DeskKey_Info & ToKey, BOOL bFreeType = TRUE, BOOL bFreeLabel = TRUE,
                    BOOL bFreeValue = TRUE);
    VOID ClearAllKeyInfo();
    VOID ClearFileInfo();
    VOID AddMenuKey();

    BOOL OnSIPMessage(msgObject & objMessage);
    BOOL ProcessNotify(int iLineId = 0);
    BOOL IsLoadAllCfg()
    {
        return m_bIsLoadAllCfg;
    }

    // 获取Softkey文本
    VOID GetSoftKeyTextByIndex(int iIndex, yl::string & strText);
    int GetKeyNum()
    {
        return m_iSoftKeyNum;
    }
public:

    int             m_iSoftKeyNum;
    int             m_iMaxKeyNum;
    int             m_iFirstKeyIndex;
    int             m_iKeyIndex[MAXKEYNUM +
                                          1];     // 除了定义的SoftKey外再人为增加一个Menu键以便进入Menu菜单
    DeskKey_Info    m_pDeskKey[MAXKEYNUM +
                                         1];      // 除了定义的SoftKey外再人为增加一个Menu键以便进入Menu菜单
    BOOL            m_bIsLoadAllCfg;
};

#define _HotDeskingSoftKey GET_SINGLETON_INSTANCE(HotDesking_SoftKey)

#endif  //IF_ADVANCE_HOTDESKING
#endif // #ifdef IF_FEATURE_HOTDESKING
#endif
