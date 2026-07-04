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
    void pollClients();
    void broadcast(const std::string& payload);
    bool isRunning() const { return running_; }
    bool hasClients() const;
    int consumeInitialSnapshotRequestCount();

private:
    struct Client {
        int fd = -1;
        int id = 0;
        std::string input_buffer;
        std::string disconnect_reason;
        bool handshake_logged = false;
        bool version_logged = false;
        bool has_subscription = false;
        int subscription_count = 0;
    };

    void acceptLoop();
    bool readClientInput(Client& client);
    void handleClientLine(Client& client, std::string line);
    bool sendAll(int client, const std::string& payload);

    int port_;
    int server_fd_ = -1;
    std::atomic<bool> running_{false};
    std::atomic<int> initial_snapshot_request_count_{0};
    std::atomic<int> next_client_id_{1};
    std::thread thread_;
    mutable std::mutex clients_mutex_;
    std::vector<Client> clients_;
};

}
