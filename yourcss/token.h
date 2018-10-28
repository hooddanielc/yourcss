#pragma once

#include <ostream>
#include <string>
#include <utility>
#include <map>
#include <memory>
#include "pos.h"

namespace yourcss {


/**
 * The output of the tokenization step is a stream of zero or more of the
 * following tokens:
 *
 * <ident-token>, <function-token>, <at-keyword-token>, <hash-token>,
 * <string-token>, <bad-string-token>, <url-token>, <bad-url-token>, <delim-token>,
 * <number-token>, <percentage-token>, <dimension-token>, <unicode-range-token>,
 * <include-match-token>, <dash-match-token>, <prefix-match-token>, <suffix-match-token>,
 * <substring-match-token>, <column-token>, <whitespace-token>, <CDO-token>, <CDC-token>,
 * <colon-token>, <semicolon-token>, <comma-token>, <[-token>, <]-token>, <(-token>,
 * <)-token>, <{-token>, and <}-token>.
 */

class token_t {

public:

  enum kind_t {
    IDENT_TOKEN,
    FUNCTION_TOKEN,
    AT_KEYWORD_TOKEN,
    HASH_TOKEN,
    STRING_TOKEN,
    BAD_STRING_TOKEN,
    URL_TOKEN,
    BAD_URL_TOKEN,
    DELIM_TOKEN,
    NUMBER_TOKEN,
    PERCENTAGE_TOKEN,
    DIMENSION_TOKEN,
    UNICODE_RANGE_TOKEN,
    INCLUDE_MATCH_TOKEN,
    DASH_MATCH_TOKEN,
    PREFIX_MATCH_TOKEN,
    SUFFIX_MATCH_TOKEN,
    SUBSTRING_MATCH_TOKEN,
    COLUMN_TOKEN,
    WHITESPACE_TOKEN,
    CDO_TOKEN,
    CDC_TOKEN,
    COLON_TOKEN,
    SEMICOLON_TOKEN,
    COMMA_TOKEN,
    LEFT_BRACKET_TOKEN,
    RIGHT_BRACKET_TOKEN,
    LEFT_PAREN_TOKEN,
    RIGHT_PAREN_TOKEN,
    LEFT_BRACE_TOKEN,
    RIGHT_BRACE_TOKEN,
    COMMENT_TOKEN,
  };  // kind_t

  /* The type flag. */
  enum type_flag_t {
    UNKNOWN,
    UNRESTRICTED,
    ID,
    INTEGER,
    NUMBER,
  };  // type_t

  /* Cache the kind. */
  token_t(kind_t kind);

  /* Cache the position and kind and set the text to the empty string. */
  token_t(const pos_t &pos, kind_t kind);

  /* Cache the position and kind and the text. */
  token_t(const pos_t &pos, kind_t kind, std::string &&text);

  static std::string get_desc(kind_t kind);

  kind_t get_kind() const;

  std::string get_text() const;

  /* Returns kind as a string */
  std::string get_name() const;

  pos_t get_pos() const;

  /* The tokens sub type */
  token_t::type_flag_t get_type_flag() const;

  void set_type_flag(type_flag_t flag);

  /* Returns type_flag as a string */
  std::string get_type_flag_name() const;

  static std::shared_ptr<token_t> make(kind_t kind);

  static std::shared_ptr<token_t> make(const pos_t &pos, kind_t kind);

  static std::shared_ptr<token_t> make(const pos_t &pos, kind_t kind, std::string &&text);

  virtual ~token_t();

protected:

  /* Writes a human-readable dump of the token. */
  friend std::ostream &operator<<(std::ostream &strm, const token_t &that);

  friend std::ostream &operator<<(std::ostream &strm, const token_t *that);

  /* See accessors. */
  pos_t pos;

  /* See accessor. */
  kind_t kind;

  /* See accessor. */
  std::string text;

  /* See accessor */
  type_flag_t type_flag;

};  // token_t

}   // yourcss
