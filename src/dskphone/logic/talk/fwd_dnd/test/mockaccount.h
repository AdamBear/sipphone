#ifndef __MOCK_ACCOUNT_H__
#define __MOCK_ACCOUNT_H__

#include <gmock/gmock.h>

#include <account/include/modaccount.h> // acc_GetAccountList

class MockAccount
{
public:
    MOCK_METHOD1(GetAccountList, void * (int eAccountType));
};

extern MockAccount * g_pMockAccount;

#endif // !__MOCK_ACCOUNT_H__
