#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <algorithm>
#include <cstring>

#include "socket.h"
#include "socket_listener.h"

using namespace oak::network;

class Server {
public:
	Server(int portno) : server_{ SocketType::TCP, SocketAddress{ portno } } {
	}

	~Server() {

	}

	void run() {
		server_.init();
		server_.bind();
		std::cout << "server started on port: " << server_.getAddress().getPort() << std::endl;	
		server_.listen();
		SocketListener listener;
		std::vector<int> fds;
		fds.push_back(server_.getFd());
		while (true) {

			listener.wait(fds, std::chrono::seconds{ 0 });

			if (listener.hasEvent(server_)) {
				Socket client = std::move(server_.accept());
				//add the client to the file descriptor list
				fds.push_back(client.getFd());
				clients_.push_back(std::move(client));
				std::cout << "client connected" << std::endl;
			}

			char buffer[2048];

			for (auto& it : clients_) {
				//there is data to be read
				if (listener.hasEvent(it)) {
					memset(buffer, 0, 2048);
					int n = it.read(buffer, 2048);
					if (n == 0) { // client disconnected
						//remove this client from the file descriptor list
						fds.erase(std::remove(std::begin(fds), std::end(fds), it.getFd()), std::end(fds));
						std::cout << "client disconnected: " << it.getFd() << std::endl;
						//close the connection
						it.destroy();
						continue;
					}
					std::cout << "bytes read: " << n << std::endl;
					buffer[n + 1] = '\0';

					std::cout << "msg: " << buffer << std::endl;

					//send data to every client
					for (auto &c : clients_) {
						c.write(buffer, n);
					}
				}
			}
			clients_.erase(std::remove_if(std::begin(clients_), std::end(clients_), [](const Socket &client){ return !client.isOpen(); }), std::end(clients_));
		}
	}

private:
	Socket server_;
	std::vector<Socket> removed_;
	std::vector<Socket> clients_;
};

int start_server(int portno) {
	Server server{ portno };
	server.run();

	return 0;
}

int start_client(const std::string &address) {
	int spos = address.find(':');
	
	Socket client{ SocketType::TCP, SocketAddress{ address.substr(0, spos), std::stoi(address.substr(spos+1)) } };
	client.init();
	client.connect();

	std::thread listener{ [&client](){ 
		char buffer[256];
		while (true) {
			memset(buffer, 0, 256);
			int n = client.read(buffer, 256);
			if (n > 0) {
				std::cout << "msg: " << buffer << std::endl;
			} else {
				std::cout << "server disconnected" << std::endl;
				exit(0);
				break;
			}
		}
	} };

	char buffer[256];
	while (true) {
		memset(buffer, 0, 256);
		std::cin.getline(buffer, 256);

		client.write(buffer, strlen(buffer));
	}
	client.destroy();

	return 0;
}


int network_main(int argc, char **argv) {

	printf("arg count: %i\n", argc);
	//parse args
	//-s runs server on port (eg. -s4099)
	//-c runs client and connects to address (eg. -c192.0.0.1:4099)
	if (strncmp(argv[1], "-s", 2) == 0) {
		start_server(atol(argv[1] + 2));
	} else if (strncmp(argv[1], "-c", 2) == 0) {
		start_client(std::string{argv[1]+2});
	}

	//pause
	printf("press any key to continue...\n");
	getc(stdin);

	return 0; 
} 