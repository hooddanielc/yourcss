#include <gtest/gtest.h>
#include <yourcss/lexer.h>
#include <yourcss/token.h>

using namespace yourcss;

TEST(unicode_range, normal_case) {
  const char *src = R"(
    u+00????
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::UNICODE_RANGE_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("000000-00ffff"));
}

TEST(unicode_range, upper_case) {
  const char *src = R"(
    U+00????
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::UNICODE_RANGE_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("000000-00ffff"));
}

TEST(unicode_range, hex_range_uppercase) {
  const char *src = R"(
    U+00aaaa-ffffff
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::UNICODE_RANGE_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("00aaaa-ffffff"));
}

TEST(unicode_range, hex_range_normal_case) {
  const char *src = R"(
    u+00aaaa-ffffff
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::UNICODE_RANGE_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("00aaaa-ffffff"));
}

TEST(unicode_range, hex_mixed_case) {
  const char *src = R"(
    u+00AAAA-FfFfFf
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::UNICODE_RANGE_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("00AAAA-FfFfFf"));
}

TEST(unicode_range, hex_mixed_case_questions) {
  const char *src = R"(
    u+00AAa?
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::UNICODE_RANGE_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("00AAa0-00AAaf"));
}
