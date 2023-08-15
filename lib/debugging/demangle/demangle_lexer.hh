//===------------------------------------------===
// cdi 2023
//
// Identification: lib/debugging/demangle/demangle_lexer.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_DEBUGGING_DEMANGLE_DEMANGLE_LEXER_HH
#define CDI_DEBUGGING_DEMANGLE_DEMANGLE_LEXER_HH

#include <cstddef>
#include <string_view>
namespace cdi::debugging::detail {

class State {
public:
  State(const std::string_view &str) : pos(0), str(str) {}

  [[nodiscard]] auto Peek(std::size_t n = 0) const -> char {
    return pos + n < str.size() ? str[pos + n] : '\0';
  }

  [[nodiscard]] auto Next() -> char {
    return pos < str.size() ? str[pos++] : '\0';
  }

  [[nodiscard]] auto eof() const -> bool { return pos >= str.size(); }

  [[nodiscard]] auto get_pos() const -> std::size_t { return pos; }

private:
  size_t pos;
  const std::string_view &str;
};

class Lexer {
private:
};

} // namespace cdi::debugging::detail

#endif // CDI_DEBUGGING_DEMANGLE_DEMANGLE_LEXER_HH
