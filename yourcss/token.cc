#include "token.h"

namespace yourcss {

token_t::token_t(token_t::kind_t kind_):
  kind(kind_),
  type_flag(type_flag_t::UNKNOWN) {}

token_t::token_t(const pos_t &pos_, token_t::kind_t kind_):
  pos(pos_),
  kind(kind_),
  type_flag(type_flag_t::UNKNOWN) {}

token_t::token_t(const pos_t &pos_, token_t::kind_t kind_, std::string &&text_):
  pos(pos_),
  kind(kind_),
  text(std::move(text_)),
  type_flag(type_flag_t::UNKNOWN) {}

token_t::~token_t() = default;

std::string token_t::get_desc(token_t::kind_t kind) {
  switch(kind) {
    case IDENT_TOKEN: return "IDENT_TOKEN";
    case FUNCTION_TOKEN: return "FUNCTION_TOKEN";
    case AT_KEYWORD_TOKEN: return "AT_KEYWORD_TOKEN";
    case HASH_TOKEN: return "HASH_TOKEN";
    case STRING_TOKEN: return "STRING_TOKEN";
    case BAD_STRING_TOKEN: return "BAD_STRING_TOKEN";
    case URL_TOKEN: return "URL_TOKEN";
    case BAD_URL_TOKEN: return "BAD_URL_TOKEN";
    case DELIM_TOKEN: return "DELIM_TOKEN";
    case NUMBER_TOKEN: return "NUMBER_TOKEN";
    case PERCENTAGE_TOKEN: return "PERCENTAGE_TOKEN";
    case DIMENSION_TOKEN: return "DIMENSION_TOKEN";
    case UNICODE_RANGE_TOKEN: return "UNICODE_RANGE_TOKEN";
    case INCLUDE_MATCH_TOKEN: return "INCLUDE_MATCH_TOKEN";
    case DASH_MATCH_TOKEN: return "DASH_MATCH_TOKEN";
    case PREFIX_MATCH_TOKEN: return "PREFIX_MATCH_TOKEN";
    case SUFFIX_MATCH_TOKEN: return "SUFFIX_MATCH_TOKEN";
    case SUBSTRING_MATCH_TOKEN: return "SUBSTRING_MATCH_TOKEN";
    case COLUMN_TOKEN: return "COLUMN_TOKEN";
    case WHITESPACE_TOKEN: return "WHITESPACE_TOKEN";
    case CDO_TOKEN: return "CDO_TOKEN";
    case CDC_TOKEN: return "CDC_TOKEN";
    case COLON_TOKEN: return "COLON_TOKEN";
    case SEMICOLON_TOKEN: return "SEMICOLON_TOKEN";
    case COMMA_TOKEN: return "COMMA_TOKEN";
    case LEFT_BRACKET_TOKEN: return "LEFT_BRACKET_TOKEN";
    case RIGHT_BRACKET_TOKEN: return "RIGHT_BRACKET_TOKEN";
    case LEFT_PAREN_TOKEN: return "LEFT_PAREN_TOKEN";
    case RIGHT_PAREN_TOKEN: return "RIGHT_PAREN_TOKEN";
    case LEFT_BRACE_TOKEN: return "LEFT_BRACE_TOKEN";
    case RIGHT_BRACE_TOKEN: return "RIGHT_BRACE_TOKEN";
    case COMMENT_TOKEN: return "COMMENT_TOKEN";
  }
}

token_t::type_flag_t token_t::get_type_flag() const {
  return type_flag;
}

void token_t::set_type_flag(token_t::type_flag_t type_flag_) {
  type_flag = type_flag_;
}

token_t::kind_t token_t::get_kind() const {
  return kind;
}

std::string token_t::get_text() const {
  return text;
}

std::string token_t::get_name() const {
  return token_t::get_desc(kind);
}

std::string token_t::get_type_flag_name() const {
  switch (type_flag) {
    case UNKNOWN: return "UNKNOWN";
    case UNRESTRICTED: return "UNRESTRICTED";
    case ID: return "ID";
    case INTEGER: return "INTEGER";
    case NUMBER: return "NUMBER";
  }
}

pos_t token_t::get_pos() const {
  return pos;
}

std::shared_ptr<token_t> token_t::make(token_t::kind_t kind) {
  return std::make_shared<token_t>(kind);
}

std::shared_ptr<token_t> token_t::make(const pos_t &pos, token_t::kind_t kind) {
  return std::make_shared<token_t>(pos, kind);
}

std::shared_ptr<token_t> token_t::make(const pos_t &pos, token_t::kind_t kind, std::string &&text) {
  return std::make_shared<token_t>(pos, kind, std::move(text));
}

/* Writes a human-readable dump of the token.  This is for debugging
 purposes only. In production, a user never sees tokens directly. */
std::ostream &operator<<(std::ostream &strm, const token_t &that) {
  strm << that.pos << "; " << token_t::get_desc(that.kind);
  if (!that.text.empty()) {
    strm << "; \"" << that.text << '"';
  }
  return strm;
}

std::ostream &operator<<(std::ostream &strm, const token_t *that) {
  strm << *that;
  return strm;
}

}   // yourcss
