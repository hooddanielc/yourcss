#include <gtest/gtest.h>
#include <yourcss/lexer.h>
#include <yourcss/token.h>

using namespace yourcss;

TEST(simple_token, hash_delimeter_token) {
  const char *src = R"(
    #
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::DELIM_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
}

TEST(simple_token, hash_token) {
  const char *src = R"(
    #\asdf
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::HASH_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("\\asdf"));
}

TEST(simple_token, hash_token_dash) {
  const char *src = R"(
    #-moz-asdf
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::HASH_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("-moz-asdf"));
}

TEST(simple_token, hash_name) {
  const char *src = R"(
    #asdf
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::HASH_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("asdf"));
}

TEST(simple_token, simple_string) {
  const char *src = R"(
    "this is a string?"
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::STRING_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("this is a string?"));
}

TEST(simple_token, single_simple_string) {
  const char *src = R"(
    'this is a string?'
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::STRING_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("this is a string?"));
}

TEST(simple_token, suffix_match_token) {
  const char *src = R"(
    $=
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::SUFFIX_MATCH_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("$="));
}


TEST(simple_token, single_left_paren) {
  const char *src = R"(
    (
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::LEFT_PAREN_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
}

TEST(simple_token, single_right_paren) {
  const char *src = R"(
    )
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::RIGHT_PAREN_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
}

TEST(simple_token, substring_match_token) {
  const char *src = R"(
    *=
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::SUBSTRING_MATCH_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("*="));
}
