#include "ice.h"

namespace yourcss {

/* Report the file and line at which we iced. */
ice_t::ice_t(const pos_t &pos, const char *file, int line_number): error_t(pos) {
  get_strm() << "internal compiler error";
  end_section();
  get_strm() << file << ", " << line_number;
}

ice_t::~ice_t() = default;

}
