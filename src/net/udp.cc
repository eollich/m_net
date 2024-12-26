#include <iostream>
#include <net/udp.h>
#include <unistd.h>
namespace m_net {
UDP::UDP() : conn(nullptr) {
  buff = (char *)calloc(M_NET_UDP_MAX_DGRAM_SZ, sizeof(char));
}
UDP::~UDP() {
  if (conn) {
    closeConnection(conn);
  }
  free(buff);
}

mnet_connp UDP::UDP::init() {
  mnet_connp dpsession = new mnet_conn();
  memset(dpsession, 0, sizeof(mnet_conn));
  dpsession->out_sock_addr.is_addr_init = false;
  dpsession->in_sock_addr.is_addr_init = false;
  dpsession->out_sock_addr.len = sizeof(struct sockaddr_in);
  dpsession->in_sock_addr.len = sizeof(struct sockaddr_in);
  dpsession->seq_num = 0;
  dpsession->is_connected = false;
  return dpsession;
}

bool UDP::setup(const std::string &ip, int port) {
  conn = init();
  if (!conn) {
    perror("UDP connection initialization failed");
    return false;
  }

  conn->udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (conn->udp_sock < 0) {
    perror("Socket creation failed");
    closeConnection(conn);
    return false;
  }

  conn->in_sock_addr.addr.sin_family = AF_INET;
  conn->in_sock_addr.addr.sin_addr.s_addr =
      ip.empty() ? INADDR_ANY : inet_addr(ip.c_str());
  conn->in_sock_addr.addr.sin_port = htons(port);
  conn->in_sock_addr.is_addr_init = true;

  if (bind(conn->udp_sock, (struct sockaddr *)&conn->in_sock_addr.addr,
           conn->in_sock_addr.len) < 0) {
    perror("Bind failed");
    closeConnection(conn);
    return false;
  }

  return true;
}

void *UDP::prepareSend(mnet_udp_pdu *pdu_ptr, void *buff, int buff_sz) {
  if (buff_sz < sizeof(mnet_udp_pdu)) {
    perror("Buffer size too small to prepare PDU");
    return nullptr;
  }

  memset(buff, 0, buff_sz);
  memcpy(buff, pdu_ptr, sizeof(mnet_udp_pdu));

  return (void *)((char *)buff + sizeof(mnet_udp_pdu));
}

int UDP::send(void *sbuff, int sbuff_sz) {
  int total_to_send = sbuff_sz;
  int total_sent = 0;
  char *sptr = (char *)sbuff;
  while (total_to_send > 0) {
    int amount_sent = sendDatagram(sptr, total_to_send);
    if (amount_sent < 0) {
      std::cerr << "UDP send: Error sending fragment, total_sent=" << total_sent
                << std::endl;
      return amount_sent;
    }
    sptr += amount_sent;
    total_to_send -= amount_sent;
    total_sent += amount_sent;
  }
  return total_sent;
}

int UDP::recv(void *buff, int buff_sz) {
  mnet_udp_pdu *in_pdu;
  int total_received = 0;
  char *rPtr = (char *)buff;
  bool is_frag = false;

  do {
    int amountReceived = recvDatagram(buff, sizeof(buff));
    if (amountReceived == M_NET_UDP_CONNECTION_CLOSED)
      return amountReceived;
    if (amountReceived < sizeof(mnet_udp_pdu))
      return M_NET_UDP_ERROR_BAD_DGRAM;

    in_pdu = (mnet_udp_pdu *)buff;

    // overflow check
    if (total_received + in_pdu->dgram_sz > buff_sz) {
      return M_NET_UDP_BUFF_UNDERSIZED;
    }

    // calc size to copy for frag
    int copy_size = (total_received + in_pdu->dgram_sz > buff_sz)
                        ? buff_sz - total_received
                        : in_pdu->dgram_sz;

    // copy cur frag into buff
    memcpy(rPtr, ((char *)buff + sizeof(mnet_udp_pdu)), copy_size);
    rPtr += copy_size;
    total_received += copy_size;

    // check buff perfectly filled
    if (total_received == buff_sz) {
      return total_received;
    }

    is_frag = IS_MT_FRAGMENT(in_pdu->mtype);
  } while (is_frag);

  return total_received;
}

int UDP::listen() {}
int UDP::connect() {}
int UDP::disconnect() {}

void UDP::closeConnection(mnet_connp dpsession) {
  if (dpsession) {
    close(dpsession->udp_sock);
    delete dpsession;
  }
}

int UDP::maxDatagramSize() { return M_NET_UDP_MAX_BUFF_SZ; }

void UDP::initPDU(mnet_udp_pdu *pdu, int mtype, int seqnum, int dgram_sz,
                  int err_num) {}
int UDP::preparePDU(mnet_udp_pdu *pdu, uint8_t *payload, uint16_t payload_len,
                    uint8_t *buffer, uint16_t buffer_len) {}
uint32_t UDP::processPDU(const mnet_udp_pdu *pdu, uint8_t *buffer,
                         uint8_t **msg, uint16_t *msg_len) {}

int UDP::sendRaw(const void *sbuff, int sbuff_sz) {
  int bytes_out = 0;

  if (!conn || !conn->out_sock_addr.is_addr_init) {
    perror("Connection or outbound address not setup properly");
    return -1;
  }

  mnet_udp_pdu *out_pdu = (mnet_udp_pdu *)sbuff;
  bytes_out = sendto(conn->udp_sock, (const char *)sbuff, sbuff_sz, 0,
                     (const struct sockaddr *)&(conn->out_sock_addr.addr),
                     conn->out_sock_addr.len);
  return bytes_out;
}
int UDP::recvRaw(void *buff, int buff_sz) {
  int bytes = 0;
  if (!conn || !conn->in_sock_addr.is_addr_init) {
    perror("Connection or inbound address not setup properly");
    return -1;
  }
  bytes = recvfrom(conn->udp_sock, (char *)buff, buff_sz, MSG_WAITALL,
                   (struct sockaddr *)&(conn->out_sock_addr.addr),
                   &(conn->out_sock_addr.len));
  if (bytes < 0) {
    perror("UDP recv raw: recevied error from recvfrom()");
    return -1;
  }
  conn->out_sock_addr.is_addr_init = true;

  return bytes;
}

int UDP::sendDatagram(void *sbuff, int sbuff_sz) {
  int snd_sz =
      (sbuff_sz > M_NET_UDP_MAX_BUFF_SZ) ? M_NET_UDP_MAX_BUFF_SZ : sbuff_sz;
  mnet_udp_pdu *out_pdu = (mnet_udp_pdu *)buff;
  out_pdu->proto_ver = M_NET_UDP_PROTO_VER_1;
  out_pdu->mtype =
      (sbuff_sz > snd_sz) ? M_NET_UDP_MT_SNDFRAG : M_NET_UDP_MT_SND;
  out_pdu->dgram_sz = snd_sz;
  out_pdu->seqnum = conn->seq_num;

  memcpy((buff + sizeof(mnet_udp_pdu)), sbuff, snd_sz);

  int total_send_sz = out_pdu->dgram_sz + sizeof(mnet_udp_pdu);
  int bytes_out = sendRaw(buff, total_send_sz);

  if (bytes_out != total_send_sz) {
    return M_NET_UDP_ERROR_GENERAL;
  }

  if (out_pdu->dgram_sz == 0) {
    conn->seq_num++;
  } else {
    conn->seq_num += out_pdu->dgram_sz;
  }

  mnet_udp_pdu in_pdu = {0};
  int bytes_in = recvRaw(&in_pdu, sizeof(mnet_udp_pdu));
  if ((bytes_in < sizeof(mnet_udp_pdu)) &&
      (in_pdu.mtype != M_NET_UDP_MT_SNDACK &&
       in_pdu.mtype != M_NET_UDP_MT_SNDFRAGACK)) {
    std::cerr << "Datagram send error: Expected SND/ACK or SNDFRAG/ACK but got "
                 "a different mtype "
              << in_pdu.mtype << std::endl;
  }
  return bytes_out - sizeof(mnet_udp_pdu);
}
int UDP::recvDatagram(void *buff, int buff_sz) {
  int bytes_in = 0;
  int err_code = M_NET_UDP_NO_ERROR;
  if (buff_sz > M_NET_UDP_MAX_DGRAM_SZ) {
    return M_NET_UDP_BUFF_OVERSIZED;
  }
  bytes_in = recvRaw(buff, buff_sz);

  if (bytes_in < sizeof(mnet_udp_pdu)) {
    err_code = M_NET_UDP_ERROR_BAD_DGRAM;
  }

  mnet_udp_pdu in_pdu = {0};
  memcpy(&in_pdu, buff, sizeof(mnet_udp_pdu));
  if (in_pdu.dgram_sz > buff_sz - sizeof(mnet_udp_pdu)) {
    err_code = M_NET_UDP_BUFF_UNDERSIZED;
  }

  if (err_code == M_NET_UDP_NO_ERROR) {
    if (in_pdu.dgram_sz == 0) {
      conn->seq_num++;
    } else {
      conn->seq_num += in_pdu.dgram_sz;
    }
  } else {
    conn->seq_num++;
  }

  mnet_udp_pdu out_pdu = {0};
  out_pdu.proto_ver = M_NET_UDP_PROTO_VER_1;
  out_pdu.dgram_sz = 0;
  out_pdu.seqnum = conn->seq_num;
  out_pdu.err_num = err_code;

  int act_snd_sz = 0;

  if (err_code != M_NET_UDP_NO_ERROR) {
    out_pdu.mtype = M_NET_UDP_MT_ERROR;
    act_snd_sz = sendRaw(&out_pdu, sizeof(mnet_udp_pdu));
    if (act_snd_sz != sizeof(mnet_udp_pdu)) {
      return M_NET_UDP_ERROR_PROTOCOL;
    }
  }

  switch (in_pdu.mtype) {
  case M_NET_UDP_MT_SNDFRAG:
  case M_NET_UDP_MT_SND:
    out_pdu.mtype = IS_MT_FRAGMENT(in_pdu.mtype) ? M_NET_UDP_MT_SNDFRAGACK
                                                 : M_NET_UDP_MT_SNDACK;
    act_snd_sz = sendRaw(&out_pdu, sizeof(mnet_udp_pdu));
    if (act_snd_sz != sizeof(mnet_udp_pdu)) {
      return M_NET_UDP_ERROR_GENERAL;
    }
    break;
  case M_NET_UDP_MT_CLOSE:
    out_pdu.mtype = M_NET_UDP_MT_CLOSEACK;
    act_snd_sz = sendRaw(&out_pdu, sizeof(mnet_udp_pdu));
    if (act_snd_sz != sizeof(mnet_udp_pdu)) {
      return M_NET_UDP_ERROR_PROTOCOL;
    }
    closeConnection(conn);
    return M_NET_UDP_CONNECTION_CLOSED;
  default:
    std::cerr << "recvDgram: Error! Unexpected or bad mtype in header: "
              << in_pdu.mtype << std::endl;
    return M_NET_UDP_ERROR_PROTOCOL;
  }
  return bytes_in;
}

} // namespace m_net
