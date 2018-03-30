#pragma once

#include "language/common/singletonbase.h"
#include "language/common/lang_common.h"
#include "modInput.h"

#define KEY_NUM                 12

// Ԫݔ�뷨����
struct ImeKeyMap
{
    yl::string                  strName;
    yl::wstring                 keyMap[KEY_NUM];

    ImeKeyMap & operator= (const ImeKeyMap & other);
};

class CImeLogic
    : public SingletonBase<CImeLogic>
{
public:
    typedef YLVector<ImeKeyMap>     vecImeType;
    typedef YLVector<ImeKeyMap *>    vecImeItemPt;

public:

    // ����ini��ʽ�����뷨
    bool LoadCustomClassicalIme(vecImeType & vec);
    // ���dBasicݔ�뷨
    bool LoadBasicIme(vecImeType & vec);
    // ���d�Զ��xݔ�뷨
    bool LoadCustomXmlIme(vecImeType & vec);
    // �]��ݔ�뷨�ļ���Ĭ�Jݔ�뷨
    bool LoadDefaultIme(LPCSTR strIme = NULL);
    // ��������
    bool ParseSubItem(const pugi::xml_node & nodeRoot, vecImeType & vec, LPCTSTR lpNode = NULL);
    // ���¼��d
    void Reload();
    // ����߉݋��ݔ�뷨
    bool CreatImeImp(vecImeType & vecCustom, vecImeType & vecExtern);
    // �@ȡIme���Q
    LPCTSTR Ime2Name(IME_ITEM_TYPE eType, UINT iIndex = 0);
    // ���Q�Q�u��Ime
    IME_ITEM_TYPE Name2Ime(const yl::string & strIme);

    // �Ϸ����ж�
    bool IsVaild(LPCTSTR lpImeName);
    bool IsVaild(IME_ITEM_TYPE eType, UINT iIndex = 0);
    bool IsVaild(ImeKeyMap * pKeyMap);

    // ����ӳ�䵽���
    static int Key2Index(int iKeyCode);

    // �ڰ�������ӿ�
    LPCWSTR IMEFromKey(int iKeyCode, IME_ITEM_TYPE eType, UINT iCusIndex = 0);
    // Ԫݔ�뷨����
    int GetImeCount()
    {
        return vecBasic.size();
    }
    // �Զ��xݔ�뷨����
    int GetCustomImeCount()
    {
        return vecDIYIme.size();
    }

    ImeKeyMap * GetKeyMap(LPCSTR lpszIMEName);

private:
    static bool IsStdIME(const yl::string & strName);

    static yl::string GetLatticeIMEFile();

private:
#ifdef IF_TEST_MODE
    static void PrintIme(const vecImeType & vec, int iLevel, LPCSTR strFun, int iLine);
    static void PrintImeKeyMap(const ImeKeyMap & keyMapPair, int iIndex, int iLevel);
#endif

private:
    vecImeType      vecBasic; // һֱ�����ڴ���
    vecImeItemPt    vecDIYIme; // �Զ������뷨
};


#define _ImeLogic GET_SINGLETON_INSTANCE(CImeLogic)