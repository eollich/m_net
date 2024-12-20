#include <net/tcp_server.h>

namespace m_net {

bool TCPServer::setup(const std::string &ip, int port) {
  struct addrinfo hints{}, *ai, *p;
  int yes = 1;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(ip.empty() ? nullptr : ip.c_str(),
                  std::to_string(port).c_str(), &hints, &ai) != 0) {
    perror("getaddrinfo");
    return false;
  }

  for (p = ai; p != nullptr; p = p->ai_next) {
    sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sock_fd < 0)
      continue;

    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(sock_fd, p->ai_addr, p->ai_addrlen) < 0) {
      close(sock_fd);
      continue;
    }

    break;
  }

  freeaddrinfo(ai);

  if (!p)
    return false;
  if (listen(sock_fd, 10) == -1)
    return false;

  return true;
}

void TCPServer::listenForConnections() {
  struct sockaddr_storage client_addr;
  socklen_t addrlen = sizeof(client_addr);

  pollfd listener{};
  listener.fd = sock_fd;
  listener.events = POLLIN;

  clients.push_back(listener);
  fd_count = 1;

  for (;;) {
    int poll_count = poll(clients.data(), fd_count, -1);
    if (poll_count == -1) {
      perror("poll");
      return;
    }

    for (int i = 0; i < fd_count; ++i) {
      if (clients[i].revents & POLLIN) {
        if (clients[i].fd == sock_fd) {
          int new_fd =
              accept(sock_fd, (struct sockaddr *)&client_addr, &addrlen);
          if (new_fd == -1) {
            perror("accept");
          } else {
            addClient(new_fd);
            std::cout << "New connection.\n";
          }
        } else {
          char buf[256];
          int nbytes = recv(clients[i].fd, buf, sizeof(buf), 0);
          if (nbytes <= 0) {
            close(clients[i].fd);
            removeClient(i);
          } else {
            handleClientData(clients[i].fd, (uint8_t *)buf, nbytes);
          }
        }
      }
    }
  }
}

void TCPServer::addClient(int new_fd) {
  pollfd client{};
  client.fd = new_fd;
  client.events = POLLIN;
  clients.push_back(client);
  fd_count++;
}

void TCPServer::removeClient(int index) {
  clients[index] = clients[fd_count - 1];
  clients.pop_back();
  fd_count--;
}

} // namespace m_net
