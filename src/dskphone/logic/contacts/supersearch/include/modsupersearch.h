#ifndef __MOD_SUPERSEARCH_H__
#define __MOD_SUPERSEARCH_H__

#include "ylstring.h"

struct SuperSearchItemListData;

void SuperSearch_Init();

bool SuperSearch_ReqSearch(const yl::string & strKey);


bool SuperSearch_GetContactListData(SuperSearchItemListData & itemListData,
                                    const yl::string & strKey);

#if IF_FEATURE_GBBOOK
void SuperSearch_SetIsClearLastKey(bool bClearKey);
#endif

#endif

