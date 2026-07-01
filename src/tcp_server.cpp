#include "tcp_server.h"
#include "logger.h"

#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

namespace ostkp {

TcpServer::TcpServer(int port) : port_(port) {}
TcpServer::~TcpServer() { stop(); }

bool TcpServer::start() {
    if (running_) return true;

    server_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        log("socket() failed: " + std::string(std::strerror(errno)));
        return false;
    }

    int yes = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(static_cast<uint16_t>(port_));

    if (::bind(server_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        log("bind() failed on 127.0.0.1:" + std::to_string(port_) + " - " + std::string(std::strerror(errno)));
        ::close(server_fd_);
        server_fd_ = -1;
        return false;
    }

    if (::listen(server_fd_, 4) < 0) {
        log("listen() failed: " + std::string(std::strerror(errno)));
        ::close(server_fd_);
        server_fd_ = -1;
        return false;
    }

    running_ = true;
    thread_ = std::thread(&TcpServer::acceptLoop, this);
    log("TCP server listening on 127.0.0.1:" + std::to_string(port_));
    return true;
}

void TcpServer::stop() {
    if (!running_) return;
    running_ = false;

    if (server_fd_ >= 0) {
        ::shutdown(server_fd_, SHUT_RDWR);
        ::close(server_fd_);
        server_fd_ = -1;
    }

    if (thread_.joinable()) thread_.join();

    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (int fd : clients_) ::close(fd);
    clients_.clear();
    log("TCP server stopped");
}

void TcpServer::acceptLoop() {
    while (running_) {
        int client = ::accept(server_fd_, nullptr, nullptr);
        if (client < 0) {
            if (running_) log("accept() failed: " + std::string(std::strerror(errno)));
            continue;
        }
        std::lock_guard<std::mutex> lock(clients_mutex_);
        clients_.push_back(client);
        log("client connected");
    }
}

void TcpServer::broadcast(const std::string& payload) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (auto it = clients_.begin(); it != clients_.end();) {
        ssize_t sent = ::send(*it, payload.data(), payload.size(), MSG_NOSIGNAL);
        if (sent < 0) {
            ::close(*it);
            it = clients_.erase(it);
            log("client disconnected");
        } else {
            ++it;
        }
    }
}

}
