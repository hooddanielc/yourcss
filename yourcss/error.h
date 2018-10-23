#pragma once

#include <exception>
#include <memory>
#include <sstream>
#include <string>
#include "pos.h"

namespace yourcss {

/* The base for all the kinds of errors we throw. */
class error_t: public std::exception {

public:

  /* Return our diagnostic message. */
  virtual const char *what() const noexcept override final;

  virtual ~error_t() override;

protected:

  /* Do-little. */
  error_t(const pos_t &pos);

  /* Mark the end of a section of a diagnostic message.  The classes which
     inehrit from us will use this in their constructors. */
  void end_section();

  /* The stream to which to write our diagnostic message.  The classes which
     inehrit from us will use this in their constructors. */
  std::ostream &get_strm() const;

private:

  /* A string-builder we use to build our diagnostic message. */
  mutable std::unique_ptr<std::ostringstream> strm;

  /* If true, then the next time we add to the message, we'll insert a
     separator character first. */
  mutable bool sep_needed;

  /* Our diagnostic message. */
  mutable std::string msg;

};  // error_t

}  // yourcss
