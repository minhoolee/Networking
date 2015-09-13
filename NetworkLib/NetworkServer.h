#ifndef NETWORKLIB_NETWORKSERVER
#define NETWORKLIB_NETWORKSERVER

#include "Constants.h"
#include "Statistics.h"

#include "LockedQueue.h"

#include <asio.hpp>

#include <array>
#include <map>
#include <thread>
#include <atomic>
#include <cstdint>
#include "Statistics.h"

using asio::ip::udp;

typedef std::map<uint32_t, udp::endpoint> ClientList;
typedef ClientList::value_type Client;
typedef std::pair<std::string, uint32_t> ClientMessage;

namespace NetworkLib {
	class NetworkServer {
	public:
		explicit NetworkServer(unsigned short local_port);
		~NetworkServer();

		bool HasMessages();
		ClientMessage PopMessage();

		void SendToClient(const std::string& message, uint32_t clientID);
		void SendToAllExcept(const std::string& message, uint32_t clientID);
		void SendToAll(const std::string& message);

		const Statistics& GetStatistics() const { return statistics; };
		std::vector<std::function<void(int32_t)>> clientDisconnectedHandlers;
	private:
		// Network send/receive stuff
		asio::io_service io_service;
		udp::socket socket;
		udp::endpoint server_endpoint;
		udp::endpoint remote_endpoint;
		std::array<char, NetworkBufferSize> recv_buffer;
		std::thread service_thread;

		// Low-level network functions
		void start_receive();
		void handle_remote_error(const std::error_code error_code, const udp::endpoint remote_endpoint);
		void handle_receive(const std::error_code& error, std::size_t bytes_transferred);
		void handle_send(std::string /*message*/, const std::error_code& /*error*/, std::size_t /*bytes_transferred*/)	{}
		void run_service();

		// Client management
		int32_t get_or_create_client_id(udp::endpoint endpoint);
		void on_client_disconnected(int32_t id);

		void send(const std::string& message, udp::endpoint target);

		// Incoming messages queue
		LockedQueue<ClientMessage> incomingMessages;

		// Clients of the server
		ClientList clients;
		std::atomic_int32_t nextClientID;

		NetworkServer(NetworkServer&); // block default copy constructor

		// Statistics
		Statistics statistics;
	};
}
#endif
