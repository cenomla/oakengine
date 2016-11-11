#include "socket_listener.h"

#include <iostream>

namespace oak::network {

	void SocketListener::wait(const std::vector<int> &sockets, std::chrono::seconds timeout) {
		if (sockets.empty()) { return; }
		FD_ZERO(&fset_);
		int maxFd = -1;
		for (auto fd : sockets) {
			FD_SET(fd, &fset_);
			maxFd = fd > maxFd ? fd : maxFd;

			std::cout << fd << std::endl;
		}
		timeval to;
		to.tv_sec = timeout.count();
		to.tv_usec = 0;

		if (maxFd == -1) {
			std::cout << "for some reason the socket max fd is -1" << std::endl;
		}

		int ret = select(maxFd + 1, &fset_, nullptr, nullptr, timeout.count() == 0 ? nullptr : &to);
		if (ret == -1) {
			std::cout << "select triggered an error" << std::endl;
		}
	}

	bool SocketListener::hasEvent(const Socket& socket) {
		return FD_ISSET(socket.getFd(), &fset_) == 1;
	}

}