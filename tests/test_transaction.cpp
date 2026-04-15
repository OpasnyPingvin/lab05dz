#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "banking/Transaction.h"
#include "banking/Account.h"

using ::testing::_;
using ::testing::Return;

class MockAccount : public Account
{
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

class TestTransaction : public Transaction
{
public:
    MOCK_METHOD(void, SaveToDataBase, (Account& from, Account& to, int sum), (override));
};

TEST(Transaction, Credit_CallsChangeBalance)
{
    MockAccount account(1, 1000);
    Transaction transaction;
    
    EXPECT_CALL(account, ChangeBalance(100)).Times(1);
    
    transaction.Credit(account, 100);
}

TEST(Transaction, Credit_WithZeroSum_Assert)
{
    MockAccount account(1, 1000);
    Transaction transaction;
    
    EXPECT_DEATH(transaction.Credit(account, 0), ".*");
}

TEST(Transaction, Debit_WhenBalanceSufficient_ReturnsTrue)
{
    MockAccount account(1, 1000);
    Transaction transaction;
    
    EXPECT_CALL(account, GetBalance()).WillOnce(Return(1000));
    EXPECT_CALL(account, ChangeBalance(-100)).Times(1);
    
    bool result = transaction.Debit(account, 100);
    
    EXPECT_TRUE(result);
}

TEST(Transaction, Debit_WhenBalanceInsufficient_ReturnsFalse)
{
    MockAccount account(1, 50);
    Transaction transaction;
    
    EXPECT_CALL(account, GetBalance()).WillOnce(Return(50));
    EXPECT_CALL(account, ChangeBalance(_)).Times(0);
    
    bool result = transaction.Debit(account, 100);
    
    EXPECT_FALSE(result);
}

TEST(Transaction, Debit_WhenBalanceEqualsSum_ReturnsTrue)
{
    MockAccount account(1, 100);
    Transaction transaction;
    
    EXPECT_CALL(account, GetBalance()).WillOnce(Return(100));
    EXPECT_CALL(account, ChangeBalance(-100)).Times(1);
    
    bool result = transaction.Debit(account, 100);
    
    EXPECT_TRUE(result);
}

TEST(Transaction, Debit_WithZeroSum_Assert)
{
    MockAccount account(1, 1000);
    Transaction transaction;
    
    EXPECT_DEATH(transaction.Debit(account, 0), ".*");
}

TEST(Transaction, Make_WhenSameAccount_Throws)
{
    Transaction transaction;
    MockAccount account(1, 1000);
    
    EXPECT_THROW(transaction.Make(account, account, 100), std::logic_error);
}

TEST(Transaction, Make_WhenNegativeSum_Throws)
{
    Transaction transaction;
    MockAccount from(1, 1000);
    MockAccount to(2, 500);
    
    EXPECT_THROW(transaction.Make(from, to, -50), std::invalid_argument);
}

TEST(Transaction, Make_WhenSumTooSmall_Throws)
{
    Transaction transaction;
    MockAccount from(1, 1000);
    MockAccount to(2, 500);
    
    EXPECT_THROW(transaction.Make(from, to, 50), std::logic_error);
}

TEST(Transaction, Make_WhenFeeTooHigh_ReturnsFalse)
{
    Transaction transaction;
    transaction.set_fee(60);
    MockAccount from(1, 1000);
    MockAccount to(2, 500);
    
    EXPECT_CALL(from, Lock()).Times(0);
    EXPECT_CALL(to, Lock()).Times(0);
    
    bool result = transaction.Make(from, to, 100);
    
    EXPECT_FALSE(result);
}

TEST(Transaction, Make_WhenInsufficientBalance_ReturnsFalse)
{
    Transaction transaction;
    transaction.set_fee(1);
    MockAccount from(1, 50);
    MockAccount to(2, 500);
    
    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);
    EXPECT_CALL(from, GetBalance())
        .WillOnce(Return(50))
        .WillOnce(Return(50));
    EXPECT_CALL(to, GetBalance())
        .WillOnce(Return(500));
    EXPECT_CALL(from, ChangeBalance(_)).Times(0);
    EXPECT_CALL(to, ChangeBalance(100)).Times(1);
    EXPECT_CALL(to, ChangeBalance(-100)).Times(1);
    
    bool result = transaction.Make(from, to, 100);
    
    EXPECT_FALSE(result);
}

TEST(Transaction, Make_SuccessfulTransfer)
{
    Transaction transaction;
    transaction.set_fee(10);
    MockAccount from(1, 1000);
    MockAccount to(2, 500);
    
    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);
    EXPECT_CALL(from, GetBalance())
        .WillOnce(Return(1000))
        .WillOnce(Return(890));
    EXPECT_CALL(to, GetBalance())
        .WillOnce(Return(600));
    EXPECT_CALL(from, ChangeBalance(-110)).Times(1);
    EXPECT_CALL(to, ChangeBalance(100)).Times(1);
    
    bool result = transaction.Make(from, to, 100);
    
    EXPECT_TRUE(result);
}

TEST(Transaction, Make_WhenBalanceExactlyEqualsSumPlusFee_Success)
{
    Transaction transaction;
    transaction.set_fee(10);
    MockAccount from(1, 110);
    MockAccount to(2, 500);
    
    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);
    EXPECT_CALL(from, GetBalance())
        .WillOnce(Return(110))
        .WillOnce(Return(0));
    EXPECT_CALL(to, GetBalance())
        .WillOnce(Return(600));
    EXPECT_CALL(from, ChangeBalance(-110)).Times(1);
    EXPECT_CALL(to, ChangeBalance(100)).Times(1);
    
    bool result = transaction.Make(from, to, 100);
    
    EXPECT_TRUE(result);
}

TEST(Transaction, SaveToDataBase_IsVirtualAndCalled)
{
    TestTransaction transaction;
    transaction.set_fee(10);
    MockAccount from(1, 1000);
    MockAccount to(2, 500);
    
    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(1000));
    EXPECT_CALL(from, ChangeBalance(-110)).Times(1);
    EXPECT_CALL(to, ChangeBalance(100)).Times(1);
    EXPECT_CALL(transaction, SaveToDataBase(_, _, 100)).Times(1);
    
    transaction.Make(from, to, 100);
}

TEST(Transaction, SaveToDataBase_OutputsCorrectInfo)
{
    testing::internal::CaptureStdout();
    
    Account from(1, 890);
    Account to(2, 600);
    Transaction transaction;
    
    transaction.SaveToDataBase(from, to, 100);
    
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("1 send to 2 $100"), std::string::npos);
    EXPECT_NE(output.find("Balance 1 is 890"), std::string::npos);
    EXPECT_NE(output.find("Balance 2 is 600"), std::string::npos);
}

TEST(Transaction, Fee_DefaultValue)
{
    Transaction transaction;
    EXPECT_EQ(transaction.fee(), 1);
}

TEST(Transaction, SetFee_ChangesValue)
{
    Transaction transaction;
    transaction.set_fee(20);
    EXPECT_EQ(transaction.fee(), 20);
}
