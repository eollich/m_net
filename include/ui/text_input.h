#ifndef __TEXT_INPUT_H__
#define __TEXT_INPUT_H__

#include <functional>
#include <ncurses.h>
#include <tuple>

namespace m_net {
class TextInput {
public:
  TextInput(const char *placeholder, int nlines, int ncols, int begin_y,
            int begin_x,
            std::function<bool(const char *, const char *)> validator);
  ~TextInput();
  void draw();
  std::tuple<bool, const char *> getInput();

private:
  const char *placeholder;
  size_t max_buffer_sz;
  char *buffer;
  WINDOW *input_win;
  std::function<bool(const char *, const char *)> validator;
};
} // namespace m_net

#endif
