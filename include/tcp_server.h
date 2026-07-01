#pragma once
#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace ostkp {

class TcpServer {
public:
    explicit TcpServer(int port = 51303);
    ~TcpServer();

    bool start();
    void stop();
    void broadcast(const std::string& payload);
    bool isRunning() const { return running_; }

private:
    void acceptLoop();

    int port_;
    int server_fd_ = -1;
    std::atomic<bool> running_{false};
    std::thread thread_;
    std::mutex clients_mutex_;
    std::vector<int> clients_;
};

}
