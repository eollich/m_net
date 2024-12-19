#include <memory>
#include <net/net_util.h>
#include <net/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

namespace m_net {

TCP::TCP() : sock_fd(-1), port(0) {}
TCP::~TCP() {
  if (sock_fd != -1)
    close(sock_fd);
}

void TCP::initHeader(m_net_tcp_header_t *header, unsigned char req_cmd,
                     char *req_data) {
  memset(header, 0, sizeof(m_net_tcp_header_t));
  header->proto_ver = M_NET_TCP_PROTO_VER_1;
  header->cmd = req_cmd;

  switch (req_cmd) {
  case M_NET_TCP_CMD_INIT:
    header->len = sizeof(m_net_tcp_header_t);
  default:
    header->len = sizeof(m_net_tcp_header_t) + strlen(req_data) + 1;
  }
}

int TCP::prepareReqPacket(m_net_tcp_header_t *header, uint16_t *payload,
                          uint16_t pay_len, uint16_t *packet,
                          uint16_t packet_len) {

  uint16_t packet_sz = sizeof(m_net_tcp_header_t) + pay_len;
  if (packet_sz < packet_len)
    return -1;

  bzero(packet, packet_len);
  memcpy(packet, header, sizeof(m_net_tcp_header_t));
  memcpy(packet + sizeof(m_net_tcp_header_t), payload, pay_len);
  header->len = packet_sz;
  return packet_sz;
}

uint32_t TCP::processRecvPacket(m_net_tcp_header_t *header, char *buffer,
                                char **msg, uint32_t *msg_len) {
  *msg = buffer + sizeof(m_net_tcp_header_t);
  *msg_len = header->len - sizeof(m_net_tcp_header_t);
  return *msg_len;
}

} // namespace m_net
