#pragma once

#include <ostream>

namespace yourcss {

class pos_t final {

public:

  pos_t() noexcept;

  void next_col();

  void next_line();

  int get_line() const;

  int get_col() const;

  friend std::ostream &operator<<(std::ostream &strm, const pos_t &that);

private:

  int line_number, col_number;

};  // pos_t

}
