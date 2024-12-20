#include <net/tcp_echo_server.h>

namespace m_net {
void EchoServer::handleClientData(int client_fd, const uint8_t *buf,
                                  size_t nbytes) {
  // std::cout << "Received from client: " << buf << std::endl;

  // Echo the raw data back to the client
  if (send(client_fd, buf, nbytes, 0) == -1) {
    perror("send");
  }
}

} // namespace m_net
