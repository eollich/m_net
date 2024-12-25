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

  void setClientDisconnectedCallback(const std::function<void(int)> &callback) {
    clientDisconnectedCallback = callback;
  }

private:
  void addClient(int new_fd);
  void removeClient(int index);
  void removeClientByFd(int fd);
  std::function<void(int)> clientDisconnectedCallback;

protected:
  void notifyClientDisconnected(int fd);
  std::vector<pollfd> clients;
  int fd_count;
};
} // namespace m_net

#endif
