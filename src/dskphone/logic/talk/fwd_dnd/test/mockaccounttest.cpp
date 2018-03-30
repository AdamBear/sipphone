#include <gtest/gtest.h>

#include "mockaccount.h"

TEST(MockAccount, GetAccountList)
{
    MockAccount objMockAccount;
    g_pMockAccount = &objMockAccount;

    ListAccounts l;
    for (size_t i = 0; i < 10; ++i)
    {
        l.push_back(i);
    }
    ON_CALL(objMockAccount, GetAccountList(ACCOUNT_TYPE_ALL)).WillByDefault(testing::Return(&l));
    EXPECT_CALL(objMockAccount, GetAccountList(ACCOUNT_TYPE_ALL)).Times(1);
    ListAccounts r = acc_GetAccountList(ACCOUNT_TYPE_ALL);
    EXPECT_EQ(r.size(), l.size());
    for (ListAccounts::iterator itl = l.begin(), itr = r.begin();
            itl != l.end() && itr != r.end(); ++itl, ++itr)
    {
        EXPECT_EQ(*itr, *itl);
    }

    g_pMockAccount = NULL;
}
