#include "client.hpp"

namespace server {

Client::Client(net::Socket socket) : websocket_(socket) {}

Client::~Client() {}

bool Client::run(std::string const& introduction) {
    if (!websocket_.handshake()) {
        return false;
    }

    websocket_.send(introduction);

    thread_ = std::thread(&Client::loop, this);

    return true;
}

void Client::shutdown() {
    websocket_.shutdown();

    thread_.join();
}

bool Client::send(std::string const& text) {
    return websocket_.send(text);
}

bool Client::send(char const* data, size_t size) {
    return websocket_.send(data, size);
}

bool Client::pop_message(std::string& message) {
    if (messages_.empty()) {
        return false;
    }

    message = messages_.front();
    messages_.pop_front();
    return true;
}

void Client::loop() {
    std::vector<char> buffer(512);

    std::string text;

    for (;;) {
        int read_bytes = websocket_.receive(buffer.data(), buffer.size());

        // read_bytes >  0 means something was received
        // read_bytes == 0 means the socket is done with receiving
        // read_bytes <  0 means there was some kind of error

        if (read_bytes <= 0) {
            return;
        }

        if (Websocket::is_text(buffer.data(), read_bytes)) {
            Websocket::decode_text(buffer.data(), read_bytes, text);

            push_message(text);
        }
    }
}

void Client::push_message(std::string const& message) {
    messages_.push_back(message);
}

}  // namespace server
