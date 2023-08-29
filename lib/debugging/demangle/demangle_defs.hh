//===------------------------------------------===
// cdi 2023
//
// Identification: lib/debugging/demangle/demangle_defs.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_DEBUGGING_DEMANGLE_DEMANGLE_DEFS_HH
#define CDI_DEBUGGING_DEMANGLE_DEMANGLE_DEFS_HH

#include "container/array.hh"
#include <string_view>

namespace cdi::debugging::detail {

struct AbbrevPair {
  const std::string_view abbrev;
  const std::string_view real_name;
  // Number of arguments in <expression> context, or 0 if disallowed.
  int arity;
};

// List of operators from Itanium C++ ABI.
static const auto kOperatorList = container::to_array<AbbrevPair>({
    // New has special syntax (not currently supported).
    {"nw", "new", 0},
    {"na", "new[]", 0},

    // Works except that the 'gs' prefix is not supported.
    {"dl", "delete", 1},
    {"da", "delete[]", 1},

    {"ps", "+", 1}, // "positive"
    {"ng", "-", 1}, // "negative"
    {"ad", "&", 1}, // "address-of"
    {"de", "*", 1}, // "dereference"
    {"co", "~", 1},

    {"pl", "+", 2},
    {"mi", "-", 2},
    {"ml", "*", 2},
    {"dv", "/", 2},
    {"rm", "%", 2},
    {"an", "&", 2},
    {"or", "|", 2},
    {"eo", "^", 2},
    {"aS", "=", 2},
    {"pL", "+=", 2},
    {"mI", "-=", 2},
    {"mL", "*=", 2},
    {"dV", "/=", 2},
    {"rM", "%=", 2},
    {"aN", "&=", 2},
    {"oR", "|=", 2},
    {"eO", "^=", 2},
    {"ls", "<<", 2},
    {"rs", ">>", 2},
    {"lS", "<<=", 2},
    {"rS", ">>=", 2},
    {"eq", "==", 2},
    {"ne", "!=", 2},
    {"lt", "<", 2},
    {"gt", ">", 2},
    {"le", "<=", 2},
    {"ge", ">=", 2},
    {"nt", "!", 1},
    {"aa", "&&", 2},
    {"oo", "||", 2},
    {"pp", "++", 1},
    {"mm", "--", 1},
    {"cm", ",", 2},
    {"pm", "->*", 2},
    {"pt", "->", 0}, // Special syntax
    {"cl", "()", 0}, // Special syntax
    {"ix", "[]", 2},
    {"qu", "?", 3},
    {"st", "sizeof", 0}, // Special syntax
    {"sz", "sizeof", 1}, // Not a real operator name, but used in expressions.
});

// List of builtin types from Itanium C++ ABI.
//
// Invariant: only one- or two-character type abbreviations here.
static const auto kBuiltinTypeList = container::to_array<AbbrevPair>({
    {"v", "void", 0},
    {"w", "wchar_t", 0},
    {"b", "bool", 0},
    {"c", "char", 0},
    {"a", "signed char", 0},
    {"h", "unsigned char", 0},
    {"s", "short", 0},
    {"t", "unsigned short", 0},
    {"i", "int", 0},
    {"j", "unsigned int", 0},
    {"l", "long", 0},
    {"m", "unsigned long", 0},
    {"x", "long long", 0},
    {"y", "unsigned long long", 0},
    {"n", "__int128", 0},
    {"o", "unsigned __int128", 0},
    {"f", "float", 0},
    {"d", "double", 0},
    {"e", "long double", 0},
    {"g", "__float128", 0},
    {"z", "ellipsis", 0},

    {"De", "decimal128", 0}, // IEEE 754r decimal floating point (128 bits)
    {"Dd", "decimal64", 0},  // IEEE 754r decimal floating point (64 bits)
    {"Dc", "decltype(auto)", 0},
    {"Da", "auto", 0},
    {"Dn", "std::nullptr_t", 0}, // i.e., decltype(nullptr)
    {"Df", "decimal32", 0},      // IEEE 754r decimal floating point (32 bits)
    {"Di", "char32_t", 0},
    {"Du", "char8_t", 0},
    {"Ds", "char16_t", 0},
    {"Dh", "float16", 0}, // IEEE 754r half-precision float (16 bits)
});

// List of substitutions Itanium C++ ABI.
static const auto kSubstitutionList = container::to_array<AbbrevPair>({
    {"St", "", 0},
    {"Sa", "allocator", 0},
    {"Sb", "basic_string", 0},
    // std::basic_string<char, std::char_traits<char>,std::allocator<char> >
    {"Ss", "string", 0},
    // std::basic_istream<char, std::char_traits<char> >
    {"Si", "istream", 0},
    // std::basic_ostream<char, std::char_traits<char> >
    {"So", "ostream", 0},
    // std::basic_iostream<char, std::char_traits<char> >
    {"Sd", "iostream", 0},
});

} // namespace cdi::debugging::detail

#endif // CDI_DEBUGGING_DEMANGLE_DEMANGLE_DEFS_HH
