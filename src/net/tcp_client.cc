#include <net/tcp_client.h>

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

bool TCPClient::sendData(const std::string &data) {
  if (send(sock_fd, data.c_str(), data.size(), 0) == -1) {
    perror("send");
    return false;
  }
  return true;
}

std::string TCPClient::receiveData() {
  char buf[256];
  int nbytes = recv(sock_fd, buf, sizeof(buf), 0);
  if (nbytes <= 0) {
    return "";
  }
  return std::string(buf, nbytes);
}

} // namespace m_net
