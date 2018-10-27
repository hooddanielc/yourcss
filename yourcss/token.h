#pragma once

#include <ostream>
#include <string>
#include <utility>
#include <map>
#include <memory>
#include "pos.h"

namespace yourcss {

class token_t {

public:

  enum kind_t {
    PERIOD,
    ANY_VALUE,
    WHITESPACE,
    DOLLAR,
    HASH_TOKEN,
    COMMENT,
    LEFT_PAREN,
    CARET,
    LESSER_THEN,
    NEWLINE,
    COMMA,
    TAB,
    PLUS,
    IDENT_TOKEN,
    RIGHT_BRACKET,
    RIGHT_PAREN,
    GREATER_THAN,
    COLUMN_TOKEN,
    TILDE,
    HEX_DIGIT,
    CDC_TOKEN,
    PIPE,
    ESCAPE,
    ASTERISK,
    STRING_TOKEN,
    NUMBER_TOKEN,
    UNICODE_RANGE_TOKEN,
    INCLUDE_MATCH_TOKEN,
    PREFIX_MATCH_TOKEN,
    COLON,
    DASH_MATCH_TOKEN,
    PERCENT,
    LETTER_I,
    SUFFIX_MATCH_TOKEN,
    LEFT_BRACKET,
    EQUALS,
    CDO_TOKEN,
    SUBSTRING_MATCH_TOKEN,
    AT,
    DELIM_TOKEN,
    PERCENT_TOKEN,
    DIMENSION_TOKEN,
    SEMICOLON,
    AT_KEYWORD_TOKEN,
    LEFT_BRACE,
    RIGHT_BRACE
  };   // kind_t

  /* Cache the kind. */
  token_t(kind_t kind);

  /* Cache the position and kind and set the text to the empty string. */
  token_t(const pos_t &pos, kind_t kind);

  /* Cache the position and kind and the text. */
  token_t(const pos_t &pos, kind_t kind, std::string &&text);

  static std::string get_desc(kind_t kind);

  kind_t get_kind() const;

  std::string get_text() const;

  std::string get_name() const;

  pos_t get_pos() const;

  static std::shared_ptr<token_t> make(kind_t kind);

  static std::shared_ptr<token_t> make(const pos_t &pos, kind_t kind);

  static std::shared_ptr<token_t> make(const pos_t &pos, kind_t kind, std::string &&text);

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

};  // token_t

}   // yourcss
