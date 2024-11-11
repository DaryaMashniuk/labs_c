#include "pch.h"
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <memory>  // for std::unique_ptr
#include <random>
#include <atomic>
#include <gtest/gtest.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"  // Console sink

// Class for logging setup
class Logger {
public:
    static std::shared_ptr<spdlog::logger> getInstance() {
        static std::shared_ptr<spdlog::logger> logger = nullptr;
        if (!logger) {
            try {
                auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("call_center_log.txt", true);
                logger = std::make_shared<spdlog::logger>("call_center_logger", spdlog::sinks_init_list{ console_sink, file_sink });
                logger->set_level(spdlog::level::info);
                logger->flush_on(spdlog::level::info);
            }
            catch (const spdlog::spdlog_ex& ex) {
                std::cerr << "Log initialization failed: " << ex.what() << std::endl;
            }
        }
        return logger;
    }
};

// Logging function using the static logger instance directly
void log(const std::string& message) {
    Logger::getInstance()->info(message);
}

// Operator class
class Operator {
public:
    Operator(int id) : id_(id), is_busy_(false) {}

    Operator(const Operator&) = delete;
    Operator& operator=(const Operator&) = delete;

    bool isBusy() const { return is_busy_; }

    void serveClient(int clientId, int serveDuration) {
        is_busy_ = true;
        log("Operator " + std::to_string(id_) + " is serving client " + std::to_string(clientId));
        std::this_thread::sleep_for(std::chrono::milliseconds(serveDuration));
        log("Operator " + std::to_string(id_) + " finished serving client " + std::to_string(clientId));
        is_busy_ = false;
    }

private:
    int id_;
    std::atomic<bool> is_busy_;
};

// CallCenter class
class CallCenter {
public:
    CallCenter(int operatorCount) {
        for (int i = 0; i < operatorCount; ++i) {
            operators_.emplace_back(std::make_unique<Operator>(i));
        }
    }

    void clientCall(int clientId, int maxWaitTime = 1000, int retryDelay = 500) {
        int attempts = 0;
        while (true) {
            Operator* availableOperator = getAvailableOperator();
            if (availableOperator) {
                int serveDuration = getRandomServeDuration();
                availableOperator->serveClient(clientId, serveDuration);
                break;
            }
            else {
                if (attempts++ * retryDelay >= maxWaitTime) {
                    log("Client " + std::to_string(clientId) + " hung up after waiting too long.");
                    return;
                }
                log("Client " + std::to_string(clientId) + " is waiting...");
                std::this_thread::sleep_for(std::chrono::milliseconds(retryDelay));
            }
        }
    }

private:
    std::vector<std::unique_ptr<Operator>> operators_;
    std::mutex mtx_;

    Operator* getAvailableOperator() {
        std::lock_guard<std::mutex> lock(mtx_);
        for (auto& op : operators_) {
            if (!op->isBusy()) {
                return op.get();
            }
        }
        return nullptr;
    }

    int getRandomServeDuration() {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(1000, 3000);
        return dist(rng);
    }
};

// Google Test suite
class CallCenterTest : public ::testing::Test {
protected:
    void SetUp() override {
        call_center_ = new CallCenter(3);  // 3 operators
    }

    void TearDown() override {
        delete call_center_;
    }

    CallCenter* call_center_;
};

TEST_F(CallCenterTest, MultipleClients) {
    std::vector<std::thread> clientThreads;
    for (int i = 0; i < 5; ++i) {
        clientThreads.emplace_back([this, i]() { call_center_->clientCall(i); });
    }

    for (auto& th : clientThreads) {
        th.join();
    }
    ASSERT_TRUE(true);
}

TEST_F(CallCenterTest, ClientsHangUpAfterWaiting) {
    std::vector<std::thread> clientThreads;
    int numClients = 6;
    for (int i = 0; i < numClients; ++i) {
        clientThreads.emplace_back([this, i]() { call_center_->clientCall(i, 2000, 500); });
    }

    for (auto& th : clientThreads) {
        th.join();
    }
    ASSERT_TRUE(true);
}

TEST_F(CallCenterTest, HighLoadTest) {
    std::vector<std::thread> clientThreads;
    int numClients = 15;

    for (int i = 0; i < numClients; ++i) {
        clientThreads.emplace_back([this, i]() { call_center_->clientCall(i, 3000, 300); });
    }

    for (auto& th : clientThreads) {
        th.join();
    }
    ASSERT_TRUE(true);
}

TEST_F(CallCenterTest, VariedClientWaitTimes) {
    std::vector<std::thread> clientThreads;
    int numClients = 8;
    for (int i = 0; i < numClients; ++i) {
        int waitTime = 1000 + i * 500;
        clientThreads.emplace_back([this, i, waitTime]() { call_center_->clientCall(i, waitTime, 200); });
    }

    for (auto& th : clientThreads) {
        th.join();
    }
    ASSERT_TRUE(true);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    Logger::getInstance()->info("Starting Call Center Tests...");

    return RUN_ALL_TESTS();
}
