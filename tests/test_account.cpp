#include <gtest/gtest.h>
#include <stdexcept>
#include "banking/Account.h"

TEST(AccountTest, ConstructorSetsIdAndBalance) {
    Account account(123, 1000);
    
    EXPECT_EQ(account.id(), 123);
    EXPECT_EQ(account.GetBalance(), 1000);
}

TEST(AccountTest, GetBalanceReturnsCorrectValue) {
    Account account(1, 500);
    EXPECT_EQ(account.GetBalance(), 500);
}

TEST(AccountTest, LockWorksCorrectly) {
    Account account(1, 1000);
    
    EXPECT_NO_THROW(account.Lock());
    EXPECT_THROW(account.Lock(), std::runtime_error);
}

TEST(AccountTest, UnlockWorksCorrectly) {
    Account account(1, 1000);
    
    account.Lock();
    EXPECT_NO_THROW(account.Unlock());
    EXPECT_NO_THROW(account.Lock());
}

TEST(AccountTest, ChangeBalanceWorksWhenLocked) {
    Account account(1, 1000);
    
    account.Lock();
    account.ChangeBalance(500);
    EXPECT_EQ(account.GetBalance(), 1500);
}

TEST(AccountTest, ChangeBalanceThrowsWhenUnlocked) {
    Account account(1, 1000);
    
    EXPECT_THROW(account.ChangeBalance(500), std::runtime_error);
}
