#ifndef __M_NET_TCP_H__
#define __M_NET_TCP_H__

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <net/net_util.h>
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
    uint8_t proto_ver;
    uint8_t cmd;
    uint32_t len;
    uint8_t error_code;
  } m_net_tcp_header_t;

  int port;
  std::string ip;
  int sock_fd;

  void initHeader(m_net_tcp_header_t *header, unsigned char req_cmd,
                  const char *req_data,
                  uint8_t error_code = M_NET_TCP_ERROR_OK);

  int prepareReqPacket(m_net_tcp_header_t *header, uint8_t *payload,
                       uint16_t pay_len, uint8_t *packet, uint16_t packet_len);

  uint32_t processRecvPacket(m_net_tcp_header_t *header, uint8_t *buffer,
                             uint8_t **msg, uint16_t *msg_len);
};
} // namespace m_net

#endif
