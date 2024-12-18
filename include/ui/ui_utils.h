#ifndef __UI_UTILS_H__
#define __UI_UTILS_H__

#include <mutex>
#include <ncurses.h>

namespace m_net {

extern std::mutex ncc_mutex;
template <typename Func> void safeClear(Func func) {
  std::lock_guard<std::mutex> lock(ncc_mutex);
  func();
}
} // namespace m_net

#endif
