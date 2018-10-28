#include <yourcss/token.h>

namespace yourcss {

class number_token_t: public token_t {

public:

  number_token_t(const pos_t &pos, kind_t kind, double value);

  number_token_t(const pos_t &pos, kind_t kind, std::string &&text, double value);

  double get_value();

  static std::shared_ptr<number_token_t> make(const pos_t &pos, kind_t kind, double value);

  static std::shared_ptr<number_token_t> make(const pos_t &pos, kind_t kind, std::string &&text, double value);

  virtual ~number_token_t();

private:

  double value;

};  // function_token_t

} // yourcss
