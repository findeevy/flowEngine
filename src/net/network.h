#define ASIO_STANDALONE

#include "../events/events.h"

#include <asio.hpp>
#include <string>
#include <vector>

class networkManager {
public:
  const std::string detectHostIp();
  void startClient();
  void startServer();
  std::vector<Event> poll();

private:
  bool isClient = false;
};
