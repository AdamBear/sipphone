#pragma once

#include "language/common/singletonbase.h"
#include "language/common/lang_common.h"
#include "modInput.h"

#define KEY_NUM                 12

// 元輸入法描述
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

    // 加载ini格式的输入法
    bool LoadCustomClassicalIme(vecImeType & vec);
    // 加載Basic輸入法
    bool LoadBasicIme(vecImeType & vec);
    // 加載自定義輸入法
    bool LoadCustomXmlIme(vecImeType & vec);
    // 沒有輸入法文件，默認輸入法
    bool LoadDefaultIme(LPCSTR strIme = NULL);
    // 解析函數
    bool ParseSubItem(const pugi::xml_node & nodeRoot, vecImeType & vec, LPCTSTR lpNode = NULL);
    // 重新加載
    void Reload();
    // 創建邏輯層輸入法
    bool CreatImeImp(vecImeType & vecCustom, vecImeType & vecExtern);
    // 獲取Ime名稱
    LPCTSTR Ime2Name(IME_ITEM_TYPE eType, UINT iIndex = 0);
    // 名稱換磚成Ime
    IME_ITEM_TYPE Name2Ime(const yl::string & strIme);

    // 合法性判断
    bool IsVaild(LPCTSTR lpImeName);
    bool IsVaild(IME_ITEM_TYPE eType, UINT iIndex = 0);
    bool IsVaild(ImeKeyMap * pKeyMap);

    // 按键映射到序号
    static int Key2Index(int iKeyCode);

    // 黑白屏适配接口
    LPCWSTR IMEFromKey(int iKeyCode, IME_ITEM_TYPE eType, UINT iCusIndex = 0);
    // 元輸入法總數
    int GetImeCount()
    {
        return vecBasic.size();
    }
    // 自定義輸入法個數
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
    vecImeType      vecBasic; // 一直存在内存中
    vecImeItemPt    vecDIYIme; // 自定义输入法
};


#define _ImeLogic GET_SINGLETON_INSTANCE(CImeLogic)