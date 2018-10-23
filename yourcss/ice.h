#pragma once

#include "pos.h"
#include "error.h"

namespace yourcss {

/* The dreaded internal compiler error. */
class ice_t final: public error_t {

public:

  /* Report the file and line at which we iced. */
  ice_t(const pos_t &pos, const char *file, int line_number);

};  // ice_t

}  // biglr
