#include "modsupersearch.h"
#include "supersearchmanager.h"

void SuperSearch_Init()
{
    g_refSuperSearchManager.Init();
}

bool SuperSearch_ReqSearch(const yl::string & strKey)
{
    return g_refSuperSearchManager.ReqSearch(strKey);
}

bool SuperSearch_GetContactListData(SuperSearchItemListData & itemListData,
                                    const yl::string & strKey)
{
    return g_refSuperSearchManager.GetContactListData(itemListData, strKey);
}

#if IF_FEATURE_GBBOOK
void SuperSearch_SetIsClearLastKey(bool bClearKey)
{
    g_refSuperSearchManager.SetIsClearSearchKey(bClearKey);
}
#endif
