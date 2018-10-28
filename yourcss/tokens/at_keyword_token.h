#include <yourcss/token.h>

namespace yourcss {

class at_keyword_token_t: public token_t {

public:

  at_keyword_token_t(const token_t &at_, const token_t &identifier_);

  token_t get_at();

  token_t get_identifier();

  static std::shared_ptr<at_keyword_token_t> make(const token_t &at, const token_t &identifier);

  virtual ~at_keyword_token_t();

private:

  token_t at;

  token_t identifier;

};  // at_keyword_token_t

} // yourcss
