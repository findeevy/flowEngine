#include "network.h"
#include "../utils/logger.h"

const std::string networkManager::detectHostIp() {
  asio::io_context io;

  asio::ip::udp::resolver resolver(io);
  asio::ip::udp::endpoint endpoint =
      *resolver.resolve(asio::ip::udp::v4(), "8.8.8.8", "80").begin();

  asio::ip::udp::socket socket(io);
  socket.connect(endpoint);

  auto local_ep = socket.local_endpoint();
  LOG_EVENT(local_ep.address().to_string());
  return local_ep.address().to_string();
}

void startClient() {}
void startServer() {}
std::vector<Event> poll() { return {}; }
