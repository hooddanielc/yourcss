#include <yourcss/token.h>

namespace yourcss {

class dimension_token_t: public token_t {

public:

  dimension_token_t(const token_t &number_, const token_t &identifier_);

  token_t get_number();

  token_t get_identifier();

  static std::shared_ptr<dimension_token_t> make(const token_t &number, const token_t &identifier);

  virtual ~dimension_token_t();

private:

  token_t number;

  token_t identifier;

};  // dimension_token_t

} // yourcss
