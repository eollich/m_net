#ifndef __TCP_ECHO_SERVER_H__
#define __TCP_ECHO_SERVER_H__

#include <net/tcp_server.h>
namespace m_net {
class EchoServer : public TCPServer {
protected:
  void handleClientData(int client_fd, const std::string &data) override;
};
} // namespace m_net
#endif
