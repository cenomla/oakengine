#ifdef _WIN32

#include "win_sock.h"

#include <windows.h>
#include <winsock2.h>

namespace oak::network {

	void WinSockleton::startup() {
		if (WSAStartup(MAKEWORD(2, 2), &wsa_) != 0) {
			std::cout << "failed to start windows socket api" << std::endl;
			exit(-3);
		}
	}

	void WinSockleton::cleanup() {
		WSACleanup();
	}

}

#endif