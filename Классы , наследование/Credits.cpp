#include "pch.h"

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// Base class for loans
class Credit {
private:
    std::string name;
    double interestRate;  // interest rate
    double amount;        // loan amount
    int term;             // term in years

public:
    Credit(const std::string& name, double interestRate, double amount, int term)
        : name(name), interestRate(interestRate), amount(amount), term(term) {}

    // Getters
    std::string getName() const { return name; }
    double getInterestRate() const { return interestRate; }
    double getAmount() const { return amount; }
    int getTerm() const { return term; }

    // Setters
    void setName(const std::string& newName) { name = newName; }
    void setInterestRate(double newRate) { interestRate = newRate; }
    void setAmount(double newAmount) { amount = newAmount; }
    void setTerm(int newTerm) { term = newTerm; }

    virtual double calculateTotalPayment() const {
        return amount * (1 + interestRate / 100 * term);
    }

    virtual void display() const {
        std::cout << "Loan: " << name << ", Rate: " << interestRate
            << "%, Amount: " << amount << ", Term: " << term << " years" << std::endl;
    }
};

// Target loan
class TargetCredit : public Credit {
private:
    bool earlyRepayment;  // availability of early repayment

public:
    TargetCredit(const std::string& name, double interestRate, double amount, int term, bool earlyRepayment)
        : Credit(name, interestRate, amount, term), earlyRepayment(earlyRepayment) {}

    // Getter
    bool isEarlyRepaymentAvailable() const { return earlyRepayment; }

    // Setter
    void setEarlyRepayment(bool newRepayment) { earlyRepayment = newRepayment; }

    void display() const override {
        Credit::display();
        std::cout << "Early repayment: "
            << (earlyRepayment ? "Available" : "Not available") << std::endl;
    }
};

// Bank
class BankService {
private:
    std::string name;
    std::vector<std::shared_ptr<Credit>> credits;

public:
    BankService(const std::string& name) : name(name) {}

    // Getter
    std::string getName() const { return name; }

    void addCredit(const std::shared_ptr<Credit>& credit) {
        credits.push_back(credit);
    }

    void displayCredits() const {
        std::cout << "Bank Loans: " << name << std::endl;
        for (const auto& credit : credits) {
            credit->display();
            std::cout << "--------------------" << std::endl;
        }
    }

    std::shared_ptr<Credit> findCreditByName(const std::string& creditName) const {
        for (const auto& credit : credits) {
            if (credit->getName() == creditName) {
                return credit;
            }
        }
        return nullptr;
    }
};

// Client
class Client {
private:
    int id;                // unique client identifier
    std::string name;     // client name

    void processCreditSelection(const BankService& bank, const std::string& creditName) {
        auto credit = bank.findCreditByName(creditName);
        if (credit) {
            std::cout << "You selected the loan: " << credit->getName() << std::endl;
            credit->display();
        }
        else {
            std::cout << "Loan not found!" << std::endl;
        }
    }

public:
    Client(int id, const std::string& name) : id(id), name(name) {}

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }

    // Setters
    void setName(const std::string& newName) { name = newName; }

    void displayClientInfo() const {
        std::cout << "Client ID: " << id << ", Name: " << name << std::endl;
    }

    void chooseCredit(const BankService& bank) {
        std::string selectedCredit;
        std::cout << "Enter the name of the loan to select: ";
        std::getline(std::cin, selectedCredit);
        processCreditSelection(bank, selectedCredit);
    }

    // New method for testing
    void chooseCreditByName(const BankService& bank, const std::string& creditName) {
        processCreditSelection(bank, creditName);
    }
};

int main(int argc, char** argv) {
    // Creating banks and loans
    BankService bank("Bank A");

    bank.addCredit(std::make_shared<Credit>("Regular Loan", 10, 50000, 5));
    bank.addCredit(std::make_shared<TargetCredit>("Car Loan", 7, 30000, 3, true));
    bank.addCredit(std::make_shared<Credit>("Consumer Loan", 12, 70000, 4));
    bank.addCredit(std::make_shared<TargetCredit>("Mortgage", 6, 100000, 20, false));

    // Creating client and selecting a loan
    Client client(1, "John Doe");
    client.displayClientInfo();

    bank.displayCredits();

    client.chooseCredit(bank);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// Test cases

TEST(CreditTest, CalculateTotalPayment) {
    Credit credit("Regular Loan", 10, 50000, 5);
    EXPECT_NEAR(credit.calculateTotalPayment(), 75000, 0.1);  // Expected: 50000 * (1 + 0.10 * 5) = 75000
}

TEST(CreditTest, ZeroTermPayment) {
    Credit credit("Zero Term Loan", 10, 50000, 0);
    EXPECT_NEAR(credit.calculateTotalPayment(), 50000, 0.1);  // Expected: Only the principal amount
}

TEST(TargetCreditTest, EarlyRepaymentTest) {
    TargetCredit autoCredit("Car Loan", 7, 30000, 3, true);
    EXPECT_TRUE(autoCredit.isEarlyRepaymentAvailable());
}

TEST(TargetCreditTest, EarlyRepaymentNotAvailable) {
    TargetCredit homeLoan("Home Loan", 5, 200000, 15, true);
    EXPECT_FALSE(homeLoan.isEarlyRepaymentAvailable());
}

TEST(BankTest, FindCreditTest) {
    BankService bank("Bank A");
    bank.addCredit(std::make_shared<Credit>("Regular Loan", 10, 50000, 5));
    auto credit = bank.findCreditByName("Regular Loan");
    ASSERT_NE(credit, nullptr);
    EXPECT_EQ(credit->getName(), "Regular Loan");
}

TEST(BankTest, FindNonExistentCreditTest) {
    BankService bank("Bank A");
    bank.addCredit(std::make_shared<Credit>("Regular Loan", 10, 50000, 5));
    auto credit = bank.findCreditByName("Non-Existent Loan");
    EXPECT_EQ(credit, nullptr);
}

TEST(ClientTest, ChooseCreditTest) {
    BankService bank("Bank A");
    bank.addCredit(std::make_shared<Credit>("Regular Loan", 10, 50000, 5));

    Client client(1, "Test Client");
    testing::internal::CaptureStdout(); // Capture console output
    client.chooseCreditByName(bank, "Regular Loan"); // Using the new method
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("You selected the loan"), std::string::npos);
    EXPECT_NE(output.find("Regular Loan"), std::string::npos);
}

TEST(ClientTest, ChooseNonExistentCreditTest) {
    BankService bank("Bank A");
    bank.addCredit(std::make_shared<Credit>("Regular Loan", 10, 50000, 5));

    Client client(1, "Test Client");
    testing::internal::CaptureStdout(); // Capture console output
    client.chooseCreditByName(bank, "Non-Existent Loan"); // Using the new method
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Loan not found!"), std::string::npos);
}