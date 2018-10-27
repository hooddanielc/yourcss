#include <yourcss/token.h>

namespace yourcss {

class function_token_t: public token_t {

public:

  function_token_t(const token_t &identifier_, const token_t &left_paren_);

  token_t get_identifier();

  token_t get_left_paren();

  static std::shared_ptr<function_token_t> make(const token_t &identifier_, const token_t &left_paren_);

private:

  token_t identifier;

  token_t left_paren;

};  // function_token_t

} // yourcss
