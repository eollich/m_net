#include <arpa/inet.h> // For inet_ntoa, if needed in other parts
#include <iomanip>
#include <iostream>
#include <net/net_util.h>
#include <netdb.h>      // For getaddrinfo and related functions
#include <netinet/in.h> // For sockaddr_in
#include <stdio.h>      // For printf, fprintf, etc.
#include <stdlib.h>     // For exit
#include <string.h>     // For memset
#include <sys/socket.h> // For socket functions
#include <sys/types.h>  // For data types like int
#include <unistd.h>     // For close

namespace m_net {

void *getInAddr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// Return a listening socket
int get_listener_socket(int port) {
  int listener; // Listening socket descriptor
  int yes = 1;  // For setsockopt() SO_REUSEADDR, below
  int rv;

  struct addrinfo hints, *ai, *p;
  char port_str[6];
  snprintf(port_str, sizeof(port_str), "%d", port);

  // Get us a socket and bind it
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if ((rv = getaddrinfo(NULL, port_str, &hints, &ai)) != 0) {
    fprintf(stderr, "pollserver: %s\n", gai_strerror(rv));
    exit(1);
  }
  for (p = ai; p != NULL; p = p->ai_next) {
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0) {
      continue;
    }
    // Lose the pesky "address already in use" error message
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
      close(listener);
      continue;
    }
    break;
  }
  freeaddrinfo(ai); // All done with this
                    // If we got here, it means we didn't get bound
  if (p == NULL) {
    return -1;
  }
  // Listen
  if (listen(listener, 10) == -1) {
    return -1;
  }
  return listener;
}

void hexDump(const uint8_t *buffer, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    // Print byte in hexadecimal format
    std::cout << std::hex << std::setfill('0') << std::setw(2)
              << static_cast<int>(buffer[i]) << " ";

    // Print a new line every 16 bytes for readability
    if ((i + 1) % 16 == 0) {
      std::cout << std::endl;
    }
  }
  // Ensure a newline at the end if the last line isn't complete
  if (length % 16 != 0) {
    std::cout << std::endl;
  }
}

} // namespace m_net
