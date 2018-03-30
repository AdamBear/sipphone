#include "mockaccount.h"

// 使用时不判断g_pMockAccount是否为空，let it crash
MockAccount * g_pMockAccount = NULL;

ListAccounts acc_GetAccountList(int eAccountType /* = ACCOUNT_TYPE_ALL */,
                                bool bOnlyUser/* = false */)
{
    return *(ListAccounts *)g_pMockAccount->GetAccountList(eAccountType);
}

bool acc_GetFlexibleSeatingSwitch(int iAccount)
{
    return false;
}
