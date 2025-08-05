#include <string>
#include <string_view>

namespace txtnorm {

// Normalize a string to diacritic post-modifiers form. This is a very
// simplistic implementation that only handles a few common cases. A full
// implementation would use ICU or similar library.
std::string normalize_latin_to_utf8(std::string_view input);

// This is the most likely entrypoint for callers. It will take a string that
// may be in UTF-8 or Latin-1, and return a normalized UTF-8 string with
// diacritics as combining characters.
std::string normalize_utf8_or_latin(std::string_view input);

} // namespace txtnorm