#include <yourcss/token.h>

namespace yourcss {

class unicode_range_token_t: public token_t {

public:

  unicode_range_token_t(const token_t &start_, const token_t &end_);

  token_t get_start();

  token_t get_end();

  static std::shared_ptr<unicode_range_token_t> make(const token_t &start, const token_t &end);

  virtual ~unicode_range_token_t();

private:

  token_t start;

  token_t end;

};  // unicode_range_token_t

} // yourcss
