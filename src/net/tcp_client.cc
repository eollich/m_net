#include <net/net_util.h>
#include <net/tcp_client.h>

#define BUFF_SZ 512

namespace m_net {

bool TCPClient::setup(const std::string &ip, int port) {
  struct addrinfo hints{}, *ai, *p;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &ai) != 0) {
    perror("getaddrinfo");
    return false;
  }

  for (p = ai; p != nullptr; p = p->ai_next) {
    sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sock_fd < 0)
      continue;

    if (connect(sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sock_fd);
      continue;
    }

    break;
  }

  freeaddrinfo(ai);

  return p != nullptr;
}

bool TCPClient::sendData(int cmd, const std::string &data) {

  m_net_tcp_header_t header;
  uint8_t send_buffer[BUFF_SZ] = {};
  initHeader(&header, cmd, data.c_str());
  prepareReqPacket(&header, (uint8_t *)data.c_str(), data.size(), send_buffer,
                   sizeof(send_buffer));

  // if (send(sock_fd, data.c_str(), data.size(), 0) == -1) {
  int sent = 0;
  if ((sent = send(sock_fd, send_buffer, header.len, 0)) == -1) {
    perror("send");
    return false;
  }
  std::cout << "SENT " << sent << " bytes " << std::endl;
  return true;
}

std::string TCPClient::receiveData() {
  uint8_t recv_buffer[BUFF_SZ];
  // int nbytes = recv(sock_fd, buf, sizeof(buf), 0);
  int nbytes = recv(sock_fd, recv_buffer, sizeof(recv_buffer), 0);
  if (nbytes <= 0) {
    return "";
  }
  std::cout << "RECEIVED " << nbytes << " bytes " << std::endl;
  m_net_tcp_header_t *pckt_pointer = (m_net_tcp_header_t *)recv_buffer;

  uint8_t err_code = pckt_pointer->error_code;
  if (err_code != M_NET_TCP_ERROR_OK) {
    switch (err_code) {
    case M_NET_TCP_ERROR_ROOM_EXISTS:
      return "ERROR ROOM ALREADY EXISTS";
    case M_NET_TCP_ERROR_ROOM_DNE:
      return "ERROR ROOM DOES NOT EXIST YET";
    default:
      return "GENERAL ERROR";
    }
  }

  uint8_t *msg_pointer = NULL;
  uint16_t msg_len = 0;
  processRecvPacket(pckt_pointer, recv_buffer, &msg_pointer, &msg_len);
  return std::string(reinterpret_cast<const char *>(msg_pointer), msg_len);
}

} // namespace m_net
