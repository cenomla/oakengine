#pragma once

struct WSADATA;

namespace oak::network {

	class WinSockleton {
	public:
		static void startup();
		static void cleanup();
	private:
		static WSADATA wsa_;
	};

}