#pragma once

#include <string>
#include <cstring>

#ifdef _WIN32
	#include "win_sock.h"
#else
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netinet/in.h>
#endif

namespace oak::network {

	class SocketAddress {
	public:
		SocketAddress();
		SocketAddress(sockaddr_in addr);
		SocketAddress(const std::string &name, int port);
		SocketAddress(int port);

		std::string getName() const;
		int getPort() const;
		const sockaddr* getSockAddr() const;
		sockaddr* getSockAddr();
		size_t getSize() const;

		inline bool operator==(const SocketAddress &other) const { return memcmp(&addr_, &other.addr_, sizeof(addr_)) == 0; }
		inline bool operator!=(const SocketAddress &other) const { return !operator==(other); }
	private:
		sockaddr_in addr_;
	};

}