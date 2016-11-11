#pragma once

#include <vector>
#include <chrono>

#ifdef _WIN32
	#include "win_sock.h"
#else
	#include <sys/select.h>
#endif

#include "socket.h"

namespace oak::network {


	class SocketListener {
	public:
		void wait(const std::vector<int> &sockets, std::chrono::seconds timeout);
		bool hasEvent(const Socket& socket);
	private:
		fd_set fset_;
	};

}