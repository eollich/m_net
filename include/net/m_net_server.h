#ifndef __M_NET_SERVER_H__
#define __M_NET_SERVER_H__

#include <unordered_map>

#include <net/tcp_server.h>
namespace m_net {
class MNetServer : public TCPServer {
public:
  bool addRoom(std::string name);
  bool removeRoom(std::string name);
  void registerCallbacks();

protected:
  void handleClientData(int client_fd, const uint8_t *buf,
                        size_t nbytes) override;

private:
  void removeClientByFd(int fd);
  // maybe pfd instead of fd
  std::unordered_map<std::string, int> rooms;
};
} // namespace m_net

#endif
