#ifndef __M_NET_UDP_H__
#define __M_NET_UDP_H__

#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>

namespace m_net {

#define M_NET_UDP_PROTO_VER_1 1
#define M_NET_UDP_MT_ACK 1       // ACK MSG
#define M_NET_UDP_MT_SND 2       // SND MSG
#define M_NET_UDP_MT_CONNECT 4   // Connect MSG
#define M_NET_UDP_MT_CLOSE 8     // CLOSE MSG
#define M_NET_UDP_MT_NACK 16     // NEG ACK
#define M_NET_UDP_MT_FRAGMENT 32 // DGRAM IS A FRAGMENT
#define M_NET_UDP_MT_ERROR 64    // SIMULATE ERROR

// Message ACKS, ACK OR'ed with Message Type
#define M_NET_UDP_MT_SNDACK (M_NET_UDP_MT_SND | M_NET_UDP_MT_ACK)
#define M_NET_UDP_MT_CNTACK (M_NET_UDP_MT_CONNECT | M_NET_UDP_MT_ACK)
#define M_NET_UDP_MT_CLOSEACK (M_NET_UDP_MT_CLOSE | M_NET_UDP_MT_ACK)

#define M_NET_UDP_MT_SNDFRAG                                                   \
  (M_NET_UDP_MT_SND | M_NET_UDP_MT_FRAGMENT) // fragmented SEND
#define M_NET_UDP_MT_SNDFRAGACK                                                \
  (M_NET_UDP_MT_SNDACK | M_NET_UDP_MT_FRAGMENT) // fragmented SEND ACK

#define IS_MT_FRAGMENT(x)                                                      \
  ((x & M_NET_UDP_MT_FRAGMENT) == M_NET_UDP_MT_FRAGMENT) // checks if fragment

typedef struct {
  int proto_ver;
  int mtype;
  int seqnum;
  int dgram_sz;
  int err_num;
} mnet_udp_pdu;

#define M_NET_UDP_MAX_BUFF_SZ 512
#define M_NET_UDP_MAX_DGRAM_SZ (M_NET_UDP_MAX_BUFF_SZ + sizeof(mnet_udp_pdu))

#define M_NET_UDP_NO_ERROR 0
#define M_NET_UDP_ERROR_GENERAL -1
#define M_NET_UDP_ERROR_PROTOCOL -2
#define M_NET_UDP_BUFF_UNDERSIZED -4
#define M_NET_UDP_BUFF_OVERSIZED -8
#define M_NET_UDP_CONNECTION_CLOSED -16
#define M_NET_UDP_ERROR_BAD_DGRAM -32

struct mnet_sock {
  socklen_t len;
  bool is_addr_init;
  struct sockaddr_in addr;
};

typedef struct mnet_conn {
  unsigned int seq_num;
  int udp_sock;
  bool is_connected;
  struct mnet_sock out_sock_addr;
  struct mnet_sock in_sock_addr;
} mnet_conn;

typedef struct mnet_conn *mnet_connp;

class UDP {
public:
  UDP();
  virtual ~UDP();

  mnet_connp init();
  bool setup(const std::string &ip, int port);
  void *prepareSend(mnet_udp_pdu *pdu_ptr, void *buff, int buff_sz);
  int send(void *sbuff, int sbuff_sz);
  int recv(void *buff, int buff_sz);

  int listen();
  int connect();
  int disconnect();

  void closeConnection(mnet_connp dpsession);
  static int maxDatagramSize();

private:
  void initPDU(mnet_udp_pdu *pdu, int mtype, int seqnum, int dgram_sz,
               int err_num = M_NET_UDP_NO_ERROR);
  int preparePDU(mnet_udp_pdu *pdu, uint8_t *payload, uint16_t payload_len,
                 uint8_t *buffer, uint16_t buffer_len);
  uint32_t processPDU(const mnet_udp_pdu *pdu, uint8_t *buffer, uint8_t **msg,
                      uint16_t *msg_len);

  int sendRaw(const void *sbuff, int sbuff_sz);
  int recvRaw(void *buff, int buff_sz);

  int sendDatagram(void *sbuff, int sbuff_sz);
  int recvDatagram(void *buff, int buff_sz);

  mnet_connp conn;
  char *buff;
};

} // namespace m_net

#endif
