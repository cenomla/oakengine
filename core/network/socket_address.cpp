#include "socket_address.h"

#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>

namespace oak::network {

	SocketAddress::SocketAddress() {
		memset(&addr_, 0, sizeof(addr_));
	}

	SocketAddress::SocketAddress(sockaddr_in addr) : addr_{ addr } {

	}

	SocketAddress::SocketAddress(const std::string &name, int port) {
		addr_.sin_addr.s_addr = inet_addr(name.c_str());
		addr_.sin_family = AF_INET;
		addr_.sin_port = htons(port);
	}

	SocketAddress::SocketAddress(int port) {
		memset(&addr_, 0, sizeof(addr_));
		addr_.sin_family = AF_INET;
		addr_.sin_addr.s_addr = INADDR_ANY;
		addr_.sin_port = htons(port);
	}

	std::string SocketAddress::getName() const {
		return std::string{ inet_ntoa(addr_.sin_addr) };
	}

	int SocketAddress::getPort() const {
		return ntohs(addr_.sin_port);	
	}

	const sockaddr* SocketAddress::getSockAddr() const {
		return reinterpret_cast<const sockaddr*>(&addr_);
	}

	sockaddr* SocketAddress::getSockAddr()  {
		return reinterpret_cast<sockaddr*>(&addr_);
	}

	size_t SocketAddress::getSize() const {
		return sizeof(addr_);
	}




}