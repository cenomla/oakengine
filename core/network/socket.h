#pragma once

#include <string>

#include "socket_address.h"

namespace oak::network {

	enum class SocketType {
		TCP,
		UDP
	};

	class Socket {
	public:
		Socket(SocketType type, SocketAddress address);
		~Socket();

		Socket(Socket &&other);
		void operator=(Socket &&other);

		Socket(const Socket &other) = delete;
		void operator=(const Socket &other) = delete;

		void init();
		void destroy();

		void bind();
		void listen();
		Socket accept();

		void connect();
		
		int write(const char *buffer, size_t size);
		int read(char *buffer, size_t size);

		int send(const char *buffer, size_t size, const SocketAddress &address);
		int send(const char *buffer, size_t size);
		int recieve(char *buffer, size_t size, SocketAddress &address);
		int recieve(char *buffer, size_t size);

		inline int getFd() const { return sockfd_; }
		inline bool isOpen() const { return sockfd_ != -1; }

		inline const SocketAddress& getAddress() const { return address_; }

		inline bool operator==(const Socket &other) const { return sockfd_ == other.sockfd_ && type_ == other.type_ && address_ == other.address_; }
		inline bool operator!=(const Socket &other) const { return !operator==(other); }
	private:
		Socket(int sockfd, SocketType type, SocketAddress address);

		int sockfd_;
		SocketType type_;
		SocketAddress address_;

	};

}