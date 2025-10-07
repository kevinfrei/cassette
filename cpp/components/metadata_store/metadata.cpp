#include <boost/regex.hpp>
#include <charconv>
#include <filesystem>
#include <optional>
#include <string>
#include <taglib/fileref.h>
#include <taglib/tag.h>

// Initial implementation: Just read all the files in the directory
// and produce the music-db map.

#include "CommonTypes.hpp"
#include "metadata.hpp"

namespace fs = std::filesystem;

namespace metadata {

// File-local stuff here:
namespace {

struct RegexPattern {
  Shared::VAType va;
  boost::regex rgx;
};

RegexPattern make(Shared::VAType va, const char* pattern) {
  return {va,
          boost::regex(pattern, boost::regex::icase | boost::regex::optimize)};
}

// Remove the suffix from a path
std::string get_no_suffix(const fs::path& p) {
  std::string s = p.generic_string();
  auto pos = s.find_last_of('.');
  return (pos == std::string_view::npos) ? s : s.substr(0, pos);
}

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

store::store(const fs::path& dir)
    : content_cache(),
      specific_overrides(),
      cache_file(dir / "metadata_cache.json"),
      override_file(dir / "metadata_overrides.json") {
  // NYI: Load any existing cache from disk.
}

// Get the metadata for a song from the relative path. (underlying
// implementation for the public interface of "fs::path").
std::optional<Shared::FullMetadata> store::read(const fs::path& item) {
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

// Get the metadata for a song from the file path only.
std::optional<Shared::FullMetadata> store::read_path(const fs::path& item) {
  std::string noSuffix{get_no_suffix(item)};
  for (const RegexPattern& pattern : patterns) {
    // Match the pattern against the relPath.
    // If it matches, extract the metadata and return it.
    boost::cmatch match;
    if (!boost::regex_match(noSuffix.c_str(), match, pattern.rgx)) {
      continue;
    }
    Shared::FullMetadata metadata;
    metadata.originalPath = item.generic_string();
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
std::optional<Shared::FullMetadata> store::read_override(const fs::path& item) {
  auto it = content_cache.find(item.generic_string());
  if (it != content_cache.end()) {
    return it->second; // Return the cached metadata.
  }
  return std::nullopt; // No cached metadata found.
}

std::optional<Shared::FullMetadata> store::read_content(const fs::path& item) {
  if (item.empty()) {
    return std::nullopt; // No item provided.
  }
  TagLib::FileRef f(item.c_str());
  if (!f.isNull() && f.tag()) {
    Shared::FullMetadata metadata;
    metadata.originalPath = item.generic_string();
    TagLib::Tag* tag = f.tag();
    if (!tag->artist().isEmpty()) {
      metadata.artist.push_back(tag->artist().to8Bit(true));
    }
    if (!tag->album().isEmpty()) {
      metadata.album = tag->album().to8Bit(true);
    }
    if (!tag->title().isEmpty()) {
      metadata.title = tag->title().to8Bit(true);
    }
    if (tag->year() != 0) {
      metadata.year = static_cast<std::int16_t>(tag->year());
    }
    if (tag->track() != 0) {
      metadata.track = static_cast<std::int16_t>(tag->track());
    }
    metadata.vaType = Shared::VAType::none; // NYI: Determine VAType
    return metadata; // Return the extracted metadata.
  }
  return std::nullopt; // NYI: Read from file content.
}

// Clear the *entire* local metadata cache (but maintain any overrides).
void store::clear_metadata_cache() {
  content_cache.clear();
}

void store::clear_metadata_cache(const fs::path& item) {
  content_cache.erase(item.generic_string()); // Remove the specific item from
                                              // the cache.
}

// Clear all overrides.
void store::clear_metadata_override() {
  specific_overrides.clear();
}

// Clear a specific override.
void store::clear_metadata_override(const fs::path& item) {
  specific_overrides.erase(item.generic_string()); // Remove the specific
                                                   // override.
}

void store::clear() {
  clear_metadata_cache();
  clear_metadata_override();
}

void store::clear(const fs::path& item) {
  clear_metadata_cache(item);
  clear_metadata_override(item);
}

} // namespace metadata