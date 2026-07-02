#include "tcp_server.h"
#include "logger.h"

#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

namespace {
constexpr std::size_t kMaxClients = 8;

bool startsWith(const std::string& value, const char* prefix) {
    return value.rfind(prefix, 0) == 0;
}

std::string buildProtocolGreeting() {
    return
        "STKPCONNECT 1\n"
        "STKPCONNECT-VERSION 2020\n"
        "\n"
        "sub sim/time/paused\n"
        "sub sim/time/sim_speed\n"
        "sub sim/flightmodel/position/groundspeed\n"
        "sub sim/flightmodel/position/indicated_airspeed\n"
        "sub sim/flightmodel/position/true_airspeed\n"
        "sub sim/flightmodel/position/latitude\n"
        "sub sim/flightmodel/position/longitude\n"
        "sub sim/flightmodel/position/y_agl\n"
        "sub sim/flightmodel/position/elevation\n"
        "sub sim/flightmodel/failures/onground_any\n"
        "sub sim/flightmodel/failures/onground_all\n"
        "sub sim/flightmodel2/gear/on_ground\n"
        "sub sim/flightmodel/position/mag_psi\n"
        "sub sim/flightmodel/position/theta\n"
        "sub sim/flightmodel/position/phi\n"
        "sub sim/flightmodel/position/psi\n"
        "sub sim/flightmodel/position/local_vy\n"
        "sub sim/flightmodel/forces/g_nrml\n"
        "sub sim/aircraft/view/acf_descrip\n"
        "sub sim/aircraft/parts/acf_gear_deploy\n"
        "sub sim/flightmodel/controls/flaprqst\n"
        "sub sim/aircraft/controls/acf_flap_detents\n"
        "sub sim/operation/override/override_planepath\n"
        "sub sim/flightmodel/position/local_vx\n"
        "sub sim/flightmodel/position/local_vz\n"
        "sub sim/aircraft/parts/acf_gear_deploy\n"
        "sub sim/time/local_time_sec\n"
        "sub sim/time/zulu_time_sec\n"
        "sub sim/time/is_in_replay\n"
        "sub sim/flightmodel/position/magnetic_variation\n"
        "sub sim/cockpit/radios/com1_freq_hz\n"
        "sub sim/flightmodel/weight/m_fuel_total\n"
        "sub sim/flightmodel2/engines/N2_percent\n"
        "sub sim/aircraft/engine/acf_num_engines\n"
        "sub sim/aircraft/view/acf_ICAO\n";
}
}

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
    for (const auto& client : clients_) ::close(client.fd);
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
        const int client_id = next_client_id_++;

#ifdef SO_NOSIGPIPE
        int yes = 1;
        setsockopt(client, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(yes));
#endif

        if (!sendAll(client, buildProtocolGreeting())) {
            ::close(client);
            log("client #" + std::to_string(client_id) + " disconnected before protocol greeting");
            continue;
        }

        std::lock_guard<std::mutex> lock(clients_mutex_);
        while (clients_.size() >= kMaxClients) {
            const int oldest_id = clients_.front().id;
            ::close(clients_.front().fd);
            clients_.erase(clients_.begin());
            log("client #" + std::to_string(oldest_id) + " disconnected (oldest); active clients: " + std::to_string(clients_.size()));
        }
        clients_.push_back(Client{client, client_id});
        ++initial_snapshot_request_count_;
        log("client #" + std::to_string(client_id) + " connected; active clients: " + std::to_string(clients_.size()));
        log("client #" + std::to_string(client_id) + " protocol greeting sent");
    }
}

bool TcpServer::hasClients() const {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    return !clients_.empty();
}

int TcpServer::consumeInitialSnapshotRequestCount() {
    return initial_snapshot_request_count_.exchange(0);
}

void TcpServer::pollClients() {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (auto it = clients_.begin(); it != clients_.end();) {
        if (!readClientInput(*it)) {
            const int client_id = it->id;
            ::close(it->fd);
            it = clients_.erase(it);
            log("client #" + std::to_string(client_id) + " disconnected; active clients: " + std::to_string(clients_.size()));
        } else {
            ++it;
        }
    }
}

bool TcpServer::readClientInput(Client& client) {
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(client.fd, &read_set);

    timeval timeout{};
    const int ready = ::select(client.fd + 1, &read_set, nullptr, nullptr, &timeout);
    if (ready < 0) {
        if (errno == EINTR) return true;
        log("select() failed: " + std::string(std::strerror(errno)));
        return false;
    }
    if (ready == 0 || !FD_ISSET(client.fd, &read_set)) return true;

    char buffer[4096];
    const ssize_t received = ::recv(client.fd, buffer, sizeof(buffer), 0);
    if (received < 0) {
        if (errno == EINTR) return true;
        log("recv() failed: " + std::string(std::strerror(errno)));
        return false;
    }
    if (received == 0) return false;

    client.input_buffer.append(buffer, static_cast<size_t>(received));

    size_t newline = std::string::npos;
    while ((newline = client.input_buffer.find('\n')) != std::string::npos) {
        std::string line = client.input_buffer.substr(0, newline);
        client.input_buffer.erase(0, newline + 1);
        handleClientLine(client, line);
    }

    return true;
}

void TcpServer::handleClientLine(Client& client, std::string line) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (line.empty()) return;

    if (line == "STKPCONNECT 1") {
        if (!client.handshake_logged) {
            client.handshake_logged = true;
            log("client #" + std::to_string(client.id) + " handshake received");
        }
        return;
    }

    if (startsWith(line, "STKPCONNECT-VERSION ")) {
        if (!client.version_logged) {
            client.version_logged = true;
            log("client #" + std::to_string(client.id) + " version received: " + line.substr(std::strlen("STKPCONNECT-VERSION ")));
        }
        return;
    }

    if (startsWith(line, "sub ")) {
        ++client.subscription_count;

        if (!client.has_subscription) {
            client.has_subscription = true;
            ++initial_snapshot_request_count_;
        }
        return;
    }

    log("client #" + std::to_string(client.id) + " command ignored: " + line);
}

bool TcpServer::sendAll(int client, const std::string& payload) {
    const char* data = payload.data();
    size_t remaining = payload.size();

    while (remaining > 0) {
        ssize_t sent = ::send(client, data, remaining, MSG_NOSIGNAL);
        if (sent < 0) {
            if (errno == EINTR) continue;
            return false;
        }
        if (sent == 0) return false;

        data += sent;
        remaining -= static_cast<size_t>(sent);
    }

    return true;
}

void TcpServer::broadcast(const std::string& payload) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (auto it = clients_.begin(); it != clients_.end();) {
        if (!sendAll(it->fd, payload)) {
            const int client_id = it->id;
            ::close(it->fd);
            it = clients_.erase(it);
            log("client #" + std::to_string(client_id) + " disconnected; active clients: " + std::to_string(clients_.size()));
        } else {
            ++it;
        }
    }
}

}
