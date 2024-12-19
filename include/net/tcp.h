#ifndef __M_NET_TCP_H__
#define __M_NET_TCP_H__

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace m_net {
class TCP {
public:
  TCP();
  virtual ~TCP();

  virtual bool setup(const std::string &ip, int port) = 0;

  void closeSocket() {
    if (sock_fd != -1)
      close(sock_fd);
    sock_fd = -1;
  }

  int getSocket() const { return sock_fd; }

protected:
  typedef struct m_net_tcp_header {
    unsigned char proto_ver;
    unsigned char cmd;
    uint32_t len;
  } m_net_tcp_header_t;

  int port;
  std::string ip;
  int sock_fd;

  void initHeader(m_net_tcp_header_t *header, unsigned char req_cmd,
                  char *req_data);

  int prepareReqPacket(m_net_tcp_header_t *header, uint16_t *payload,
                       uint16_t pay_len, uint16_t *packet, uint16_t packet_len);

  uint32_t processRecvPacket(m_net_tcp_header_t *header, char *buffer,
                             char **msg, uint32_t *msg_len);
};
} // namespace m_net

#endif
