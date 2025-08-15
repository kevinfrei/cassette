#include <charconv>
#include <optional>
#include <string>

#include <boost/regex.hpp>

#if defined(USE_XPRESSIVE)
#include <boost/xpressive/xpressive.hpp>
#endif

// Initial implementation: Just read all the files in the directory
// and produce the music-db map.

#include "CommonTypes.hpp"
#include "metadata.hpp"

using regexp = boost::regex;

#if defined(USE_XPRESSIVE)
using namespace boost::xpressive;
#endif

namespace metadata {

// File-local stuff here:
namespace {

struct RegexPattern {
  Shared::VAType va;
  regexp rgx;
};

RegexPattern make(Shared::VAType va, const char* pattern) {
  return {va, regexp(pattern, regexp::icase | regexp::optimize)};
}

// Remove the suffix from a string(_view).
std::string_view get_no_suffix(const std::string_view& s) {
  auto pos = s.find_last_of('.');
  return (pos == std::string_view::npos) ? s : s.substr(0, pos);
}

std::string_view get_no_suffix(const std::string& s) {
  return get_no_suffix(std::string_view(s));
}

#if defined(USE_XPRESSIVE)
// TODO: It would be fun to use Eric's Xpressive static regex library here,
// instead of Boost's regex. I could start by trying to use the Xpressive
// compiler...
mark_tag year(1), album(2), discNum(3), discName(4), artist(5), track(6),
    title(7), va_type(8);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshift-count-overflow"
#pragma clang diagnostic ignored "-Wint-in-bool-context"
sregex various_1 = icase(
    bos
    << !(*_ << '/')
    << (va_type = (('v' << 'a'
                        << !('r' << 'i' << 'o' << 'u' << 's' << ' ' << 'a'
                                 << 'r' << 't' << 'i' << 's' << 't' << 's')) |
                   ('c' << 'o' << 'm' << 'p' << 'i' << 'l' << 'a' << 't' << 'i'
                        << 'o' << 'n') |
                   ('o' << 's' << 't') |
                   ('s' << 'o' << 'u' << 'n' << 'd' << 't' << 'r' << 'a' << 'c'
                        << 'k')))
    << *(set = ' ', '-', '.', '/')
    << !((year = repeat<4, 4>(_d)) << *(set = ' ', '-', '.', '/'))
    << (album = +~'/')
    << !('/' << (('c' << 'd') | ('d' << 'i' << 's' << ('c' | 'k'))) << !' '
             << (discNum = +_d)
             << !(*(set = ' ', '-', '.') << (discName = +~'/')))
    << '/' << (track = +_d) << *(set = ' ', '-', '.') << (artist = +~'/')
    << !' ' << '-' << ' ' << (title = +~'/') << eos);
#pragma clang diagnostic pop
#endif

std::vector<RegexPattern> patterns{
    // va - (year - )albumTitle/(disc #- disc name/)## - artist - trackTitle
    make(Shared::VAType::va,
         "^(.*\\/)?((va(rious artists)?)|(compilation)) - "
         "((?<year>\\d{4}) - )?(?<album>[^/]+)"
         "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
         "\\/(?<track>\\d+)[-. ]+(?<artist>[^/]+) - (?<title>[^/]+)$"),
    // ost - (year - )albumTitle/(disc #- disc name/)## - artist - trackTitle
    make(Shared::VAType::ost,
         "^(.*\\/)?((ost)|(soundtrack)) - "
         "((?<year>\\d{4}) - )?(?<album>[^/]+)"
         "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
         "\\/(?<track>\\d+)[-. ]+(?<artist>[^/]+) - (?<title>[^/]+)$"),
    // artist - year - album/(disc #- disc name/)## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?(?<artist>[^/]+) - (?<year>\\d{4}) - (?<album>[^/]+)"
         "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
         "\\/(?<track>\\d+)[-. ]+(?<title>[^/]+)$"),
    // va/(year - )albumTitle/(CD # name/)## - artist - trackTitle
    make(Shared::VAType::va,
         "^(.*\\/)?((va(rious artists)?)|(compilation))"
         "\\/((?<year>\\d{4}) - )?(?<album>[^/]+)"
         "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
         "\\/(?<track>\\d+)[-. ]+(?<artist>[^/]+) - (?<title>[^/]+)$"),
    // ost/(year - )albumTitle/(CD # name/)## - artist - trackTitle
    make(Shared::VAType::ost,
         "^(.*\\/)?((ost)|(soundtrack))"
         "\\/((?<year>\\d{4}) - )?(?<album>[^/]+)"
         "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
         "\\/(?<track>\\d+)[-. ]+(?<artist>[^/]+) - (?<title>[^/]+)$"),
    // artist/year - albumTitle/CD # name/## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?(?<artist>[^/]+)\\/(?<year>\\d{4}) - (?<album>[^/]+)"
         "\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?"
         "\\/(?<track>\\d+)[-. ]+ (?<title>[^/]+)$"),
    // artist/year - albumTitle/## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?(?<artist>[^/]+)\\/(?<year>\\d{4}) - (?<album>[^/]+)"
         "\\/(?<track>\\d+)[-. ]+ (?<title>[^/]+)$"),
    // artist - album/CD # name/## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?(?<artist>[^/]+) - (?<album>[^/]+)"
         "\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?"
         "\\/(?<track>\\d+)[-. ]+(?<title>[^/]+)$"),
    // artist/albumTitle/CD # name/## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?(?<artist>[^/]+)\\/(?<album>[^/]+)"
         "\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?"
         "\\/(?<track>\\d+)[-. ]+ (?<title>[^/]+)$"),
    // artist - album/## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?(?<artist>[^/]+) - (?<album>[^/]+)"
         "\\/(?<track>\\d+)[-. ]+(?<title>[^/]+)$"),
    // artist/albumTitle/## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?(?<artist>[^/]+)\\/(?<album>[^/]+)"
         "\\/(?<track>\\d+)[-. ]+ (?<title>[^/]+)$")};

} // namespace

// Metadata stuff:

// Get the metadata for a song from the relative path. (underlying
// implementation for the public interface of "fs::path" or SongKey).
std::optional<Shared::FullMetadata> cache::read(const std::string& item) {
  auto override = read_override(item);
  if (override.has_value()) {
    return override; // Return the cached metadata.
  }
  auto from_path = read_path(item);
  if (from_path.has_value()) {
    return from_path; // Return the metadata from the path.
  }
  // If we don't have an override or a path match, read the file itself.
  return read_content(item);
}

// Get the metadata for a song from the file path only.i
std::optional<Shared::FullMetadata> cache::read_path(const std::string& item) {
  std::string noSuffix{get_no_suffix(item)};
  for (const RegexPattern& pattern : patterns) {
    // Match the pattern against the relPath.
    // If it matches, extract the metadata and return it.
    boost::cmatch match;
    if (!boost::regex_match(noSuffix.c_str(), match, pattern.rgx)) {
      continue;
    }
    Shared::FullMetadata metadata;
    metadata.originalPath = item;
    // Extract the metadata based on the pattern.
    auto artist = match["artist"];
    if (artist.matched) {
      // TODO: Split artist names up.
      metadata.artist.push_back(artist.str());
    }
    auto album = match["album"];
    if (album.matched) {
      metadata.album = album.str();
    }
    auto year = match["year"];
    if (year.matched) {
      std::from_chars(year.first, year.second, metadata.year);
    }
    auto track = match["track"];
    if (track.matched) {
      std::from_chars(track.first, track.second, metadata.track);
    }
    auto title = match["title"];
    if (title.matched) {
      // TODO: Split title into additional artists & variations.
      metadata.title = title.str();
    }
    auto discNum = match["discNum"];
    if (discNum.matched) {
      std::from_chars(discNum.first, discNum.second, metadata.disk);
    } else {
      metadata.disk = 0; // Default to 0 if not matched.
    }
    auto discName = match["discName"];
    if (discName.matched) {
      metadata.diskName = discName.str();
    }
    metadata.vaType = pattern.va;
    return metadata; // Return the extracted metadata.
  }
  return std::nullopt; // No match found.
}

// Get the metadata for a song from the file's metadata only.
std::optional<Shared::FullMetadata> cache::read_override(
    const std::string& item) {
  auto it = content_cache.find(item);
  if (it != content_cache.end()) {
    return it->second; // Return the cached metadata.
  }
  return std::nullopt; // No cached metadata found.
}

std::optional<Shared::FullMetadata> cache::read_content(
    const std::string& item) {
  if (item.empty()) {
    return std::nullopt; // No item provided.
  }
  return std::nullopt; // NYI: Read from file content.
}

// Clear the *entire* local metadata cache (but maintain any overrides).
void cache::clear_metadata_cache() {
  content_cache.clear();
}

void cache::clear_metadata_cache(const std::string& item) {
  content_cache.erase(item); // Remove the specific item from the cache.
}

// Clear all overrides.
void cache::clear_metadata_override() {
  specific_overrides.clear();
}

// Clear a specific override.
void cache::clear_metadata_override(const std::string& item) {
  specific_overrides.erase(item); // Remove the specific override.
}

void cache::clear() {
  clear_metadata_cache();
  clear_metadata_override();
}

void cache::clear(const std::string& item) {
  clear_metadata_cache(item);
  clear_metadata_override(item);
}

} // namespace metadata