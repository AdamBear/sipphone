#ifndef _FEATURE_CONTROLLER_H_
#define _FEATURE_CONTROLLER_H_

#include "../include/common.h"
//#include "directory/dir_data_define.h"
#include "talk/fwd_dnd/include/modfwddnd.h"

#define kszAFEnable  "AF_bEnable"
#define kszAFTarget  "AF_strTarget"
#define kszAFOnCode  "AF_strOnCode"
#define kszAFOffCode "AF_strOffCode"

#define kszBFEnable  "BF_bEnable"
#define kszBFTarget  "BF_strTarget"
#define kszBFOnCode  "BF_strOnCode"
#define kszBFOffCode "BF_strOffCode"

#define kszTFEnable  "TF_bEnable"
#define kszTFTarget  "TF_strTarget"
#define kszTFOnCode  "TF_strOnCode"
#define kszTFOffCode "TF_strOffCode"

#define kszTFTimeout "Timeout"

bool GetDndData(DndData * pDndData, int iAccountID);
// Set DND data.
// | refDndData | is the DND data to set.
void SetDndData(const DndData & refDndData, int iAccountID);

#if IF_BUG_27045
bool GetAreaCodeData(AreaCodeData * pData);

void SetAreaCodeData(const AreaCodeData & refData);
#endif

bool GetAnonymousCallDataByLineId(AnonymousCallData * pData, int nLineId);
// Set anonymous call and reject anonymous call data by line id.
// | nLineId | is the line id.
// | refData | is the data to set.
void SetAnonymousCallDataByLineId(int nLineId, const AnonymousCallData & refData);

#ifdef IF_FEATURE_AUTO_ANSWER_TIMER
void SetAutoAnswerData(const AutoAnswerTimer & refData);

int GetAutoAnswerData();
#endif

#if 0
// 获取指定序号的dsskey数据
bool GetDssKeyDataByIndex(DssKeyData * pData, int nDssKeyId, int eUIType);

// 保存指定序号的dsskey数据
void SetDssKeyDataByIndex(const DssKeyData & refData, int nDssKeyId, int eUIType);

// Get Soft key data
bool GetProgramKeyDataByIndex(DssKeyData * pData, int nProgramKeyId);

// Set Soft key data
void SetProgramKeyDataByIndex(const DssKeyData & refData, int nProgramKeyId);
#endif

// favorite 结构体声明
struct FavoriteItem
{
    yl::string strId;
    yl::string strDisplayName;
    int iPriority;
    FavoriteItem()
    {
        Reset();
    }
    VOID Reset()
    {
        strId = "";
        strDisplayName = "";
        iPriority = 0;
    }

    typedef bool (*FunCompare)(const FavoriteItem & lhs, const FavoriteItem & rhs);
    // 比较大小.
    static bool Compare(const FavoriteItem & lhs, const FavoriteItem & rhs)
    {
        return lhs.iPriority < rhs.iPriority;
    }
};

typedef YLList<FavoriteItem>  LIST_FAVORITE_ITEM;

// 读取favorite配置信息
BOOL LoadFavoriteData();

// 获取favorite数据指针
LIST_FAVORITE_ITEM* GetFavoriteData();

#endif // _FEATURE_CONTROLLER_H_
