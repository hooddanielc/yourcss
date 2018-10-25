#include "token.h"

namespace yourcss {

token_t::token_t(token_t::kind_t kind_): kind(kind_) {}

token_t::token_t(const pos_t &pos_, token_t::kind_t kind_): pos(pos_), kind(kind_) {}

token_t::token_t(const pos_t &pos_, token_t::kind_t kind_, std::string &&text_):
  pos(pos_),
  kind(kind_),
  text(std::move(text_)) {}

std::string token_t::get_desc(token_t::kind_t kind) {
  switch(kind) {
    case PERIOD: return "PERIOD";
    case ANY_VALUE: return "ANY_VALUE";
    case WHITESPACE: return "WHITESPACE";
    case DOLLAR: return "DOLLAR";
    case HASH_TOKEN: return "HASH_TOKEN";
    case COMMENT: return "COMMENT";
    case LEFT_PAREN: return "LEFT_PAREN";
    case CARET: return "CARET";
    case LESSER_THEN: return "LESSER_THEN";
    case NEWLINE: return "NEWLINE";
    case COMMA: return "COMMA";
    case TAB: return "TAB";
    case PLUS: return "PLUS";
    case IDENT_TOKEN: return "IDENT_TOKEN";
    case RIGHT_BRACKET: return "RIGHT_BRACKET";
    case RIGHT_PAREN: return "RIGHT_PAREN";
    case GREATER_THAN: return "GREATER_THAN";
    case COLUMN_TOKEN: return "COLUMN_TOKEN";
    case TILDE: return "TILDE";
    case HEX_DIGIT: return "HEX_DIGIT";
    case CDC_TOKEN: return "CDC_TOKEN";
    case PIPE: return "PIPE";
    case ESCAPE: return "ESCAPE";
    case ASTERISK: return "ASTERISK";
    case STRING_TOKEN: return "STRING_TOKEN";
    case NUMBER_TOKEN: return "NUMBER_TOKEN";
    case UNICODE_RANGE_TOKEN: return "UNICODE_RANGE_TOKEN";
    case INCLUDE_MATCH_TOKEN: return "INCLUDE_MATCH_TOKEN";
    case PREFIX_MATCH_TOKEN: return "PREFIX_MATCH_TOKEN";
    case COLON: return "COLON";
    case DASH_MATCH_TOKEN: return "DASH_MATCH_TOKEN";
    case PERCENT: return "PERCENT";
    case LETTER_I: return "LETTER_I";
    case SUFFIX_MATCH_TOKEN: return "SUFFIX_MATCH_TOKEN";
    case LEFT_BRACKET: return "LEFT_BRACKET";
    case EQUALS: return "EQUALS";
    case CDO_TOKEN: return "CDO_TOKEN";
    case SUBSTRING_MATCH_TOKEN: return "SUBSTRING_MATCH_TOKEN";
    case AT: return "AT";
    case DELIM_TOKEN: return "DELIM_TOKEN";
    case PERCENT_TOKEN: return "PERCENT_TOKEN";
    case DIMENSION_TOKEN: return "DIMENSION_TOKEN";
  }
  return "unknown";
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
