#ifndef __MSG_BOX_H__
#define __MSG_BOX_H__

#include <condition_variable>
#include <mutex>
#include <ncurses.h>
#include <string>
#include <vector>

namespace m_net {
class MsgBox {
public:
  MsgBox(int nlines, int ncols, int begin_y, int begin_x);
  ~MsgBox();
  void draw();
  void addMsg(const char *msg);
  void start();
  void stop();

private:
  WINDOW *msg_window;
  std::vector<std::string> messages;
  std::mutex mutex;
  std::condition_variable cv;
  bool newMsg = {false};
  bool stopped = {false};
  int nlines; // rows

  void checkForMessages();
  void mockMsgCheck();
};
} // namespace m_net

#endif
