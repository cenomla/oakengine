#pragma once

#include <mutex>
#include <atomic>
#include <enet/enet.h>

#include "container.h"
#include "util/byte_buffer.h"
#include "util/buffer_puper.h"
#include "system.h"
#include "events.h"
#include "pup.h"

namespace oak::network {

	class NetworkManager : public System {
	public:
		NetworkManager(Engine &engine);

		void init() override;
		void destroy() override;

		void run();

		template<class T>
		void broadcast(uint32_t packetId, const T& data) {
			//create a byte buffer puper to serialize the data
			BufferPuper puper{ &packetBuffer_ };
			//write the packet id
			packetBuffer_.write(packetId);
			//call serialization function
			pup(puper, data, {});

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
		oak::vector<ENetPeer*> peers_;
		std::mutex peerLock_;
		std::atomic<bool> running_;
		util::ByteBuffer packetBuffer_;

		void proccessPacket(const ENetEvent &event);
	};
	
}