#ifndef __CHAT_UI_H__
#define __CHAT_UI_H__

#define BUFF_LEN 1024

namespace m_net {
class ChatUI {
public:
  ChatUI();
  ~ChatUI();
  void start();
  void clean();

private:
  int w_height, w_width;
};
} // namespace m_net

#endif
