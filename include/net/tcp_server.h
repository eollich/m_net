#ifndef __M_NET_TCP_SERVER_H__
#define __M_NET_TCP_SERVER_H__

#include <net/tcp.h>
#include <poll.h>

namespace m_net {
class TCPServer : public TCP {
public:
  TCPServer() : TCP(), fd_count(0) {}
  ~TCPServer() override = default;

  bool setup(const std::string &ip, int port) override;
  void listenForConnections();
  virtual void handleClientData(int client_fd, const uint8_t *buf,
                                size_t nbytes) = 0;

private:
  std::vector<pollfd> clients;
  int fd_count;

  void addClient(int new_fd);
  void removeClient(int index);
};
} // namespace m_net

#endif
