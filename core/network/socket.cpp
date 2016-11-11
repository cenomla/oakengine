#include "socket.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

namespace oak::network {

	Socket::Socket(SocketType type, SocketAddress address) : sockfd_{ -1 }, type_{ type }, address_{ address } {}

	Socket::Socket(int sockfd, SocketType type, SocketAddress address) : sockfd_{ sockfd }, type_{ type }, address_{ address } {}

	Socket::~Socket() {
		destroy();
	}

	Socket::Socket(Socket &&other) : sockfd_{ other.sockfd_ }, type_{ other.type_ }, address_{ other.address_ } {
		other.sockfd_ = -1;
	}

	void Socket::operator=(Socket &&other) {
		sockfd_ = other.sockfd_;
		type_ = other.type_;
		address_ = other.address_;
		other.sockfd_ = -1;
	}

	void Socket::init() {
		if (sockfd_ >= 0) { return; }
		int st = -1;

		switch (type_) {
		case SocketType::TCP:
			st = SOCK_STREAM;
			break;
		case SocketType::UDP:
			st = SOCK_DGRAM;
			break;
		}

		sockfd_ = socket(AF_INET, st, 0);

		if (sockfd_ < 0) {
			printf("failed to create socket\n");
			exit(-3);
		}

	}

	void Socket::destroy() {
		if (sockfd_ >= 0) {
#ifdef _WIN32
			closesocket(sockfd_);
#else
			close(sockfd_);
#endif
			sockfd_ = -1;
		}
	}

	void Socket::bind() {
		//allow accepting of multiple sockets
		int opt = 1;
		if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&opt), sizeof(opt)) < 0) {
			printf("failed to allow multiple connections\n");
			exit(-3);
		}
		//bind the socket to the address
		if (::bind(sockfd_, address_.getSockAddr(), address_.getSize()) < 0) {
			printf("failed to bind socket\n");
			exit(-3);
		}
	}

	void Socket::listen() {
		::listen(sockfd_, 5);
	}

	Socket Socket::accept() {
		sockaddr_in addr;
		socklen_t len = sizeof(addr);
		int nsockfd = ::accept(sockfd_, reinterpret_cast<sockaddr*>(&addr), &len);
		if (nsockfd < 0) {
			printf("failed to connect to client\n");
			exit(-3);
		}
		Socket socket{ nsockfd, type_, SocketAddress{ addr } };
		return std::move(socket);
	}

	void Socket::connect() {
		if (::connect(sockfd_, address_.getSockAddr(), address_.getSize()) < 0) {
			printf("failed to connect to server\n");
		}
	}

	int Socket::write(const char *buffer, size_t size) {
		return ::send(sockfd_, buffer, size, 0);
	}

	int Socket::read(char *buffer, size_t size) {
		return ::recv(sockfd_, buffer, size, 0);
	}

	int Socket::send(const char *buffer, size_t size, const SocketAddress &address) {
		return sendto(sockfd_, buffer, size, 0, address.getSockAddr(), address.getSize());
	}

	int Socket::send(const char *buffer, size_t size) {
		return sendto(sockfd_, buffer, size, 0, address_.getSockAddr(), address_.getSize());
	}

	int Socket::recieve(char *buffer, size_t size, SocketAddress &address) {
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		socklen_t len = sizeof(addr);
		int n = recvfrom(sockfd_, buffer, size, 0, reinterpret_cast<sockaddr*>(&addr), &len);
		address = SocketAddress{ addr };
		return n;
	}

	int Socket::recieve(char *buffer, size_t size) {
		return recvfrom(sockfd_, buffer, size, 0, nullptr, nullptr);
	}

}