#include "network_manager.h"

#include <algorithm>
#include <cstring>

#include "engine.h"
#include "events.h"
#include "log.h"

namespace oak::network {

	NetworkManager::NetworkManager(Engine &engine) : System{ engine, "network_manager" }, host_{ nullptr }, running_{ false }, packetBuffer_{ 1024 } {}


	void NetworkManager::init() {
		//start up enet
		if (enet_initialize() != 0) {
			log_print_err("failed to startup enet");
			abort();
		}

		ENetAddress address;
		address.port = 4099;
		address.host = ENET_HOST_ANY;

		log_print_out("network manager creating host...");
		host_ = enet_host_create(&address, 32, 2, 0, 0);
		if (host_ == nullptr) {
			log_print_err("failed to create host");
			abort();
		}

		log_print_out("successfully created host");

		engine_.getEventManager().add<QuitEvent>(std::ref(*this));
		//engine_.getTaskManager().addTask({ [this](){ run(); }, Task::MULTI_THREAD_BIT });
	}

	void NetworkManager::destroy() {
		if (host_ != nullptr) {
			enet_host_destroy(host_);
		}
		enet_deinitialize();
	}

	void NetworkManager::run() {
		ENetEvent event;
		running_ = true;
		while (running_ && enet_host_service(host_, &event, 1000) >= 0) {
			switch(event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				log_print_out("peer connected");
				{
					std::lock_guard<std::mutex> lock{ peerLock_ };
					peers_.push_back(event.peer);
				}
			break;
			case ENET_EVENT_TYPE_DISCONNECT:
				log_print_out("peer disconnected");
				{
					std::lock_guard<std::mutex> lock{ peerLock_ };
					peers_.erase(std::remove(std::begin(peers_), std::end(peers_), event.peer), std::end(peers_));
				}
			break;
			case ENET_EVENT_TYPE_RECEIVE:
				proccessPacket(event);
				enet_packet_destroy(event.packet);
			break;
			case ENET_EVENT_TYPE_NONE:
			break;
			}
		}
	}

	void NetworkManager::proccessPacket(const ENetEvent &event) {
		util::ByteBuffer buffer{ event.packet->data, event.packet->dataLength };
		uint32_t packetId = buffer.read<uint32_t>();
		log_print_out("packet id: %u, packet size: %u", packetId, buffer.capacity());
	}

}