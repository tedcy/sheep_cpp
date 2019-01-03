#pragma once 
#include "small_test.h"
#include <string> 

//实现代码
//-------------------
class Account 
{ 
private: 
   std::string accountId_;  //账号
   long balance_;           //存款 呃 还是叫余额吧
public: 
   Account() = default; 
   Account(const std::string& accountId, long initialBalance) :
        accountId_(accountId), balance_(initialBalance){
   }
   //消费
   void debit(long amount) {
       balance_ -= amount;
   }
   //收入
   void credit(long amount) {
       balance_ += amount;
   }
   //获取存款
   long getBalance() const {
       return balance_;
   }
   //获取账号
   std::string getAccountId() const {
       return accountId_;
   }
};

//被测试类需要预留该接口
class AccountManager 
{ 
public: 
    //通过账户号，找到对应的用户
	virtual Account findAccountForUser(const std::string& userId) = 0; 
    //更新用户的信息这里
	virtual void updateAccount(const Account& account) = 0; 
};

//被测试类
class AccountService{
public:
    void setAccountManager(AccountManager *m) {
        m_ = m;
    }
    void transfer(const std::string &A, const std::string &B, int count) {
        auto accountA = m_->findAccountForUser(A);
        accountA.debit(count);
        m_->updateAccount(accountA);

        auto accountB = m_->findAccountForUser(B);
        accountB.credit(count);
        m_->updateAccount(accountB);
    }
private:
    AccountManager *m_;
};

//Mock测试代码
//-------------------
// MockAccountManager, mock AccountManager with googlemock 
class MockAccountManager : public AccountManager 
{ 
public: 
   MOCK_METHOD1(findAccountForUser, Account(const std::string&)); 
   MOCK_METHOD1(updateAccount, void(const Account&)); 
};

//实现mock接口
class AccountHelper 
{ 
public: 
    AccountHelper(std::map<std::string, Account>& mAccount) {
        this->mAccount = mAccount; 
    }
    void updateAccount(const Account& account) {
        this->mAccount[account.getAccountId()] = account; 
    }
    Account findAccountForUser(const std::string& userId) {
        if (this->mAccount.find(userId) != this->mAccount.end()) 
            return this->mAccount[userId]; 
        else 
            return Account();
    }
private: 
    std::map<std::string, Account> mAccount; 
}; 

TEST(AccountServiceTest, transferTest) 
{ 
   std::map<std::string, Account> mAccount; 
   mAccount["A"] = Account("A", 3000); 
   mAccount["B"] = Account("B", 2000); 
   AccountHelper helper(mAccount); 

   MockAccountManager* pManager = new MockAccountManager(); 

   // specify the behavior of MockAccountManager 
   // always invoke AccountHelper::findAccountForUser 
   // when AccountManager::findAccountForUser is invoked 
   EXPECT_CALL(*pManager, findAccountForUser(testing::_)).WillRepeatedly( 
       testing::Invoke(&helper, &AccountHelper::findAccountForUser)); 

   // always invoke AccountHelper::updateAccount 
   //when AccountManager::updateAccount is invoked 
   EXPECT_CALL(*pManager, updateAccount(testing::_)).WillRepeatedly( 
       testing::Invoke(&helper, &AccountHelper::updateAccount)); 

   AccountService as; 
   // inject the MockAccountManager object into AccountService 
   as.setAccountManager(pManager); 

   // operate AccountService 
   as.transfer("A", "B", 1005); 

   // check the balance of Account("A") and Account("B") to 
   //verify that AccountService has done the right job 
   EXPECT_EQ(1995, helper.findAccountForUser("A").getBalance()); 
   EXPECT_EQ(3005, helper.findAccountForUser("B").getBalance()); 

   delete pManager; 
}
