yourcss lexer
=============

A hand rolled lexer implemented as described in [CSS Syntax Module Level 3](https://www.w3.org/TR/css-syntax-3/#tokenization) Tokenization.

## Building

**Dependencies**

- [clang](https://llvm.org) - c++ compiler
- [ib](https://github.com/JasonL9000/ib) - c++ build tool

```bash
git submodule init
git submodule update

# run all tests
ib --test_all test

# build a single test
ib test/selector-test.cc

# run
../out/debug/test/selector-test
```

## Example

```c++
#include <yourcss/lexer.h>

using namespace yourcss;

int main(int, char*[]) {
  const char *src = R"(
    .cool {
      something: 123;
      asdf: url(http://danielhood.com);
    }
  )";

  auto tokens = lexer_t(src).lex();
  for (const auto &token: tokens) {
    std::cout << "type: " << token->get_name() << std::endl;
    std::cout << "text: '" << token->get_text() << "'" << std::endl;
    std::cout << "---------------------------" << std::endl;
  }

  return 0;
}
```
