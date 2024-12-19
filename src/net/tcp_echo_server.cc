#include <iostream>
#include <net/tcp_echo_server.h>

namespace m_net {
void EchoServer::handleClientData(int client_fd, const std::string &data) {
  std::cout << "Received from client: " << data << std::endl;

  // echo data back to client
  if (send(client_fd, data.c_str(), data.size(), 0) == -1) {
    perror("send");
  }
}

} // namespace m_net
