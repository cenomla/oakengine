#pragma once

#include <mutex>
#include <atomic>
#include <vector>
#include <enet/enet.h>

#include "util/byte_buffer.h"
#include "util/pupper.h"
#include "system.h"
#include "events.h"

namespace oak::network {

	class NetworkManager : public System {
	public:
		NetworkManager(Engine &engine);

		void init() override;
		void destroy() override;

		void run();

		template<class T>
		void broadcast(uint32_t packetId, const T& data) {
			//create a byte buffer pupper to serialize the data
			util::ByteBufferPupper pupper{ packetBuffer_ };
			//write the packet id
			packetBuffer_.write(packetId);
			//call serialization function
			util::pup(pupper, data, util::ObjInfo{});

			//send a packet to every peer
			ENetPacket *packet;
			{
				std::lock_guard<std::mutex> lock{ peerLock_ };
				for (auto peer : peers_) {
					packet = enet_packet_create(packetBuffer_.data(), packetBuffer_.pos(), ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(peer, 0, packet);
				}
			}

			//ensure that the packets get sent
			enet_host_flush(host_);
			//reset buffer to pos 0
			packetBuffer_.rewind();
		}

		inline void operator()(const QuitEvent &evt) { running_ = false; }

	private:
		ENetHost *host_;
		std::vector<ENetPeer*> peers_;
		std::mutex peerLock_;
		std::atomic<bool> running_;
		util::ByteBuffer packetBuffer_;

		void proccessPacket(const ENetEvent &event);
	};
	
}