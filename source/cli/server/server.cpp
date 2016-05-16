#include "server.hpp"
#include "client.hpp"
#include "message_handler.hpp"
#include "core/logging/logging.hpp"
#include "base/math/vector.inl"
#include "base/thread/thread_pool.hpp"
#include "miniz/miniz.hpp"

#include <sstream>
#include <iostream>

namespace server {

Server::Server(math::int2 dimensions, Message_handler& message_handler) :
	srgb_(dimensions), message_handler_(message_handler) {
	std::ostringstream stream;
	stream << "{ \"resolution\": [" << dimensions.x << ", " << dimensions.y << "] }";
	introduction_ = stream.str();
}

Server::~Server() {
	for (Client* c : clients_) {
		delete c;
	}

	net::Socket::release();
}

void Server::run() {
	if (!net::Socket::init()) {
		logging::error("Could not start socket environment.");
		return;
	}

	accept_socket_ = net::Socket("8080");

	if (!accept_socket_.is_valid()) {
		logging::error("Could not establish accept socket.");
		return;
	}

	shutdown_ = false;

	accept_thread_ = std::thread(&Server::accept_loop, this);
}

void Server::shutdown() {
	shutdown_ = true;

	accept_socket_.cancel_blocking_accept();

	accept_thread_.join();

	accept_socket_.close();

	for (Client* c : clients_) {
		c->shutdown();
	}
}

void Server::write(const image::Image_float_4& image, uint32_t /*frame*/, thread::Pool& pool) {
	auto d = image.description().dimensions;
	pool.run_range([this, &image](uint32_t begin, uint32_t end) {
		srgb_.to_sRGB(image, begin, end); }, 0, d.x * d.y);

	/*
	size_t buffer_len = 0;
	void* png_buffer = tdefl_write_image_to_png_file_in_memory(srgb_.data(), d.x, d.y,
															   4, &buffer_len);

	if (!png_buffer) {
		return;
	}
	*/

	size_t buffer_len = d.x * d.y * sizeof(math::byte4);

	std::string message;

	for (auto c = clients_.begin(); c != clients_.end();) {
		Client* client = *c;

		// Here we are assuming that the client disconnected if the send fails.
		// The nice solution probably is listening for a close message.
	//	if (!client->send(static_cast<const char*>(png_buffer), buffer_len)) {
		if (!client->send(reinterpret_cast<const char*>(srgb_.data()), buffer_len)) {
			client->shutdown();
			delete client;
			client = nullptr;
			c = clients_.erase(c);
		} else {
			++c;
		}

		// If the client is still active we can process any messages that queued up
		if (client) {
			while (client->pop_message(message)) {
				message_handler_.handle(message);
			}
		}
	}

//	mz_free(png_buffer);
}

void Server::accept_loop() {
	accept_socket_.listen(10);

	for (;;) {
		net::Socket connection_socket = accept_socket_.accept();

		if (!connection_socket.is_valid()) {
			if (shutdown_) {
				return;
			} else {
				continue;
			}
		}

		Client* client = new Client(connection_socket);

		if (!client->run(introduction_)) {
			delete client;
			continue;
		}

		clients_.push_back(client);
	}
}

}