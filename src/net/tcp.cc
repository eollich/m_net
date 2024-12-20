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
                     const char *req_data) {
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

// prepareReqPacket(&header, 0, 0, send_buffer, sizeof(send_buffer));
int TCP::prepareReqPacket(m_net_tcp_header_t *header, uint8_t *payload,
                          uint16_t pay_len, uint8_t *packet,
                          uint16_t packet_len) {

  uint16_t packet_sz = sizeof(m_net_tcp_header_t) + pay_len;
  if (packet_sz > packet_len) {
    return -1;
  }

  header->len = packet_sz;

  bzero(packet, packet_len);
  memcpy(packet, header, sizeof(m_net_tcp_header_t));
  memcpy(packet + sizeof(m_net_tcp_header_t), payload, pay_len);
  return packet_sz;
}

uint32_t TCP::processRecvPacket(m_net_tcp_header_t *header, uint8_t *buffer,
                                uint8_t **msg, uint16_t *msg_len) {
  *msg = buffer + sizeof(m_net_tcp_header_t);
  *msg_len = header->len - sizeof(m_net_tcp_header_t);
  return *msg_len;
}

} // namespace m_net
