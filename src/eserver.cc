#include <net/tcp_echo_server.h>

int main() {
  m_net::EchoServer server;

  // Start the server
  if (!server.setup("", 9034)) {
    std::cerr << "Failed to start server" << std::endl;
    return 1;
  }

  std::cout << "Server is running on port 9034..." << std::endl;

  // Listen for incoming connections
  server.listenForConnections();

  return 0;
}
