#include <gtest/gtest.h>
#include <yourcss/lexer.h>
#include <yourcss/token.h>

using namespace yourcss;

TEST(numeric_token, regular_number) {
  const char *src = R"(
    123
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("123"));
  number_token_t *number = dynamic_cast<number_token_t*>(tokens[1].get());
  EXPECT_TRUE(number->get_value() == double(123.0));
}

TEST(numeric_token, negative_number) {
  const char *src = R"(
    -123
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("-123"));
  number_token_t *number = dynamic_cast<number_token_t*>(tokens[1].get());
  EXPECT_TRUE(number->get_value() == double(-123.0));
}

TEST(numeric_token, positive_number) {
  const char *src = R"(
    +123
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("+123"));
  number_token_t *number = dynamic_cast<number_token_t*>(tokens[1].get());
  EXPECT_TRUE(number->get_value() == double(123.0));
}

TEST(numeric_token, decimal) {
  const char *src = R"(
    123.3
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("123.3"));
  number_token_t *number = dynamic_cast<number_token_t*>(tokens[1].get());
  EXPECT_TRUE(number->get_value() == double(123.3));
}

TEST(numeric_token, neg_decimal) {
  const char *src = R"(
    -123.3
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("-123.3"));
  number_token_t *number = dynamic_cast<number_token_t*>(tokens[1].get());
  EXPECT_TRUE(number->get_value() == double(-123.3));
}

TEST(numeric_token, neg_start_decimal) {
  const char *src = R"(
    -.3
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("-.3"));
  number_token_t *number = dynamic_cast<number_token_t*>(tokens[1].get());
  EXPECT_TRUE(number->get_value() == double(-0.3));
}

TEST(numeric_token, exponent) {
  const char *src = R"(
    3e3
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("3e3"));
  number_token_t *number = dynamic_cast<number_token_t*>(tokens[1].get());
  EXPECT_TRUE(number->get_value() == double(3000.0));
}

TEST(numeric_token, neg_exponent) {
  const char *src = R"(
    -3e3
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("-3e3"));
  number_token_t *number = dynamic_cast<number_token_t*>(tokens[1].get());
  EXPECT_TRUE(number->get_value() == double(-3000.0f));
}

TEST(numeric_token, pos_exponent) {
  const char *src = R"(
    +3e3
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("+3e3"));
  number_token_t *number = dynamic_cast<number_token_t*>(tokens[1].get());
  EXPECT_TRUE(number->get_value() == double(3000.0f));
}

TEST(numeric_token, pos_exponent_pos) {
  const char *src = R"(
    +3e+3
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("+3e+3"));
}

TEST(numeric_token, neg_exponent_pos) {
  const char *src = R"(
    -3e+3
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("-3e+3"));
}

TEST(numeric_token, neg_exponent_neg) {
  const char *src = R"(
    -3e-3
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("-3e-3"));
}

TEST(numeric_token, dec_exponent) {
  const char *src = R"(
    3.3e3
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("3.3e3"));
}

TEST(numeric_token, pos_dec_exponent) {
  const char *src = R"(
    +3.3e3
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("+3.3e3"));
}

TEST(numeric_token, neg_dec_exponent) {
  const char *src = R"(
    -3.3e3
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("-3.3e3"));
}

TEST(numeric_token, dimension_token) {
  const char *src = R"(
    123px
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::DIMENSION_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("123px"));
}

TEST(numeric_token, identifier_token_negative) {
  const char *src = R"(
    -123px
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::DIMENSION_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("-123px"));
}

TEST(numeric_token, identifier_token_positive) {
  const char *src = R"(
    +123px
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::DIMENSION_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("+123px"));
}

TEST(numeric_token, dimension_decimal) {
  const char *src = R"(
    123.3px
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::DIMENSION_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("123.3px"));
}

TEST(numeric_token, neg_dimension_decimal) {
  const char *src = R"(
    -123.3px
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::DIMENSION_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("-123.3px"));
}

TEST(numeric_token, pos_dimension_decimal) {
  const char *src = R"(
    +123.3px
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::DIMENSION_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("+123.3px"));
}

TEST(numeric_token, dimension_token_dash_name) {
  const char *src = R"(
    123-cool-px
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::DIMENSION_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("123-cool-px"));
}

TEST(numeric_token, dimension_weird_escaped_name) {
  const char *src = R"(
    123-moz\*cool
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::DIMENSION_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("123-moz\\*cool"));
}
