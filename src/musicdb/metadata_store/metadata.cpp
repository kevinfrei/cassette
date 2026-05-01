#include <charconv>
#include <filesystem>
#include <map>
#include <optional>
#include <regex>
#include <string>

#include <taglib/fileref.h>
#include <taglib/tag.h>

#include "CommonTypes.hpp"
#include "text_tools.hpp"

#include "metadata.hpp"

namespace fs = std::filesystem;

// Initial implementation: Just read all the files in the directory
// and produce the music-db map.
namespace metadata {

// File-local stuff here:
namespace {

enum class Capture { artist, album, year, track, title, discNum, discName };

struct RegexPattern {
  Shared::VAType va;
  std::regex rgx;
  std::map<Capture, int> captureGroups;
};

RegexPattern make(Shared::VAType va,
                  const char* pattern,
                  std::map<Capture, int> captureGroups) {
  return {va,
          std::regex(pattern, std::regex::icase | std::regex::optimize),
          captureGroups};
}

// Remove the suffix from a path
std::string get_no_suffix(const fs::path& p) {
  std::string s = p.generic_string();
  auto pos = s.find_last_of('.');
  return (pos == std::string_view::npos) ? s : s.substr(0, pos);
}

std::optional<Shared::FullMetadata> make_metadata(
    const fs::path& path,
    Shared::VAType va,
    std::string_view year,
    std::string_view album,
    std::optional<std::array<std::string_view, 3>> maybe_track_artist_title,
    std::optional<std::array<std::string_view, 2>> maybe_disc_num_name) {
  if (!maybe_track_artist_title.has_value()) {
    return std::nullopt;
  }
  Shared::FullMetadata res;
  /*struct FullMetadata {
    std::string originalPath;
    std::vector<std::string> artist;
    std::string album;
    std::int16_t year;
    std::int16_t track;
    std::string title;
    VAType vaType;
    std::vector<std::string> moreArtists;
    std::vector<std::string> variations;
    std::int16_t disk;
    std::string diskName;
  };*/
  auto [track, artist, title] = *maybe_track_artist_title;
  res.originalPath = path.generic_string();
  res.artist = std::vector<std::string>{{std::string{artist}}};
  res.album = album;
  res.year = year.empty() ? 0 : text::from_string<int16_t>(year);
  res.track = text::from_string<int16_t>(track);
  res.title = title;
  res.vaType = va;
  if (maybe_disc_num_name.has_value()) {
    auto [num, name] = *maybe_disc_num_name;
    res.disk = text::from_string<int16_t>(num);
    res.diskName = name;
  }
  return res;
}

std::optional<Shared::FullMetadata> make_metadata(
    const fs::path& path,
    std::string_view artist,
    std::string_view year,
    std::string_view album,
    std::optional<std::array<std::string_view, 2>> maybe_track_title,
    std::optional<std::array<std::string_view, 2>> maybe_disc_num_name) {
  if (!maybe_track_title.has_value()) {
    return std::nullopt;
  }
  Shared::FullMetadata res;
  /*struct FullMetadata {
    std::string originalPath;
    std::vector<std::string> artist;
    std::string album;
    std::int16_t year;
    std::int16_t track;
    std::string title;
    VAType vaType;
    std::vector<std::string> moreArtists;
    std::vector<std::string> variations;
    std::int16_t disk;
    std::string diskName;
  };*/
  auto [track, title] = *maybe_track_title;
  res.originalPath = path.generic_string();
  res.artist = std::vector<std::string>{{std::string{artist}}};
  res.album = album;
  res.year = year.empty() ? 0 : text::from_string<int16_t>(year);
  res.track = text::from_string<int16_t>(track);
  res.title = title;
  res.vaType = Shared::VAType::none;
  if (maybe_disc_num_name.has_value()) {
    auto [num, name] = *maybe_disc_num_name;
    res.disk = text::from_string<int16_t>(num);
    res.diskName = name;
  } else {
    res.disk = 0;
    res.diskName = "";
  }
  return res;
}

std::string_view digits{"0123456789"};
std::string_view sep{" - "};

// Case-insensitive "check for this string" function:
bool is_at(std::string_view sv, std::string_view substr, size_t offset = 0) {
  if (sv.length() - offset < substr.length()) {
    return false;
  }
  for (size_t i = 0; i < substr.length(); i++) {
    if (std::tolower(sv.at(0 + offset)) != substr.at(i)) {
      return false;
    }
  }
  return true;
}

// This is a custom implementation of this regex:
// ^(\d+)[-. ]+(.+) - (.+)$ => [track, artist, title]
std::optional<std::array<std::string_view, 3>> track_artist_title(
    std::string_view sv) {
  // First, scan forward for numbers: We should get at least 1 number
  size_t not_number = sv.find_last_not_of(digits);
  if (not_number == 0 || not_number == sv.npos) {
    return std::nullopt;
  }
  // Okay, now skip whitespace, '.' and '-''s
  size_t not_separator = sv.find_first_not_of(" .-", not_number);
  if (not_separator == not_number) {
    return std::nullopt;
  }
  // Next, search for " - ":
  size_t hyphen = sv.find(sep, not_separator);
  if (hyphen == sv.npos || hyphen + 3 != sv.length()) {
    return std::nullopt;
  }
  // Got all 3 pieces:
  return std::array<std::string_view, 3>{
      {sv.substr(0, not_number),
       sv.substr(not_separator, hyphen - not_separator),
       sv.substr(hyphen + 3)}};
}

// ^(cd|dis[ck]) *(\d+)([-. ]+(.+))?$ => [disc #, <name>]
std::optional<std::array<std::string_view, 2>> disc_num_and_name(
    std::string_view sv) {
  size_t num_start = sv.npos;
  if (is_at(sv, "cd")) {
    num_start = 2;
  } else if (is_at(sv, "dis") && (is_at(sv, "k", 3) || is_at(sv, "c", 3))) {
    num_start = 4;
  } else {
    return std::nullopt;
  }
  // Okay, skip spaces:
  num_start = sv.find_first_not_of(" ", num_start);
  // Now get the disk number
  size_t num_end = sv.find_first_not_of(digits, num_start);
  if (num_end == sv.npos || num_end == num_start) {
    return std::nullopt;
  }
  std::string_view disk_num = sv.substr(num_start, num_end - num_start);
  if (num_end == sv.length()) {
    // Just a disc #, no name:
    return std::array<std::string_view, 2>{{disk_num, ""}};
  }
  // Next, skip the dividor (-, space, .)
  size_t start_of_name = sv.find_first_not_of(" .-", num_end);
  if (start_of_name != sv.npos) {
    return std::array<std::string_view, 2>{
        {disk_num, sv.substr(start_of_name)}};
  }
  return std::nullopt;
}

// ^((\\d{4}) - )?(.+)$" => [<year>, album]
std::array<std::string_view, 2> year_and_album(std::string_view sv) {
  size_t not_digit = sv.find_first_not_of(digits);
  if (not_digit != 4 || sv.find(sep) != 4) {
    return std::array<std::string_view, 2>{{"", sv}};
  }
  return std ::array<std::string_view, 2>{{sv.substr(0, 4), sv.substr(7)}};
}

bool is_va(std::string_view sv) {
  return (sv.length() == 11 && is_at(sv, "compilation")) ||
         (sv.length() == 2 && is_at(sv, "va")) ||
         (sv.length() == 14 && is_at(sv, "various artists"));
}

bool is_st(std::string_view sv) {
  return (sv.length() == 3 && is_at(sv, "ost")) ||
         (sv.length() == 10 && is_at(sv, "soundtrack"));
}

bool has_va(Shared::VAType va) {
  return va != Shared::VAType::none;
}

Shared::VAType get_va_st(std::string_view artist) {
  return is_va(artist)   ? Shared::VAType::va
         : is_st(artist) ? Shared::VAType::ost
                         : Shared::VAType::none;
}

// ^(.+) - (\d{4} - )?(.]+)$ => [artist, <year>, album]
std::optional<std::array<std::string_view, 3>> artist_year_album(
    std::string_view sv) {
  size_t first_sep = sv.find(sep);
  if (first_sep == 0 || first_sep == sv.npos) {
    return std::nullopt;
  }
  auto year_album = year_and_album(sv.substr(first_sep + 3));
  return std::array<std::string_view, 3>{
      {sv.substr(0, first_sep), year_album[0], year_album[1]}};
}

// ^(\d+)[-. ]+(.+)$ => [track, title]
std::optional<std::array<std::string_view, 2>> track_title(
    std::string_view sv) {
  // First, scan forward for numbers: We should get at least 1 number
  size_t not_number = sv.find_first_not_of(digits);
  if (not_number == 0 || not_number == sv.npos) {
    return std::nullopt;
  }
  // Okay, now skip whitespace, '.' and '-''s
  size_t not_separator = sv.find_first_not_of(" .-", not_number);
  if (not_separator == not_number) {
    return std::nullopt;
  }
  // Got all both pieces:
  return std::array<std::string_view, 2>{
      {sv.substr(0, not_number), sv.substr(not_separator)}};
}

std::vector<RegexPattern> patterns{
    // va - (year - )albumTitle/(disc #- disc name/)## - artist - trackTitle
    make(Shared::VAType::va,
         "^(?:.*\\/)?(?:(?:va(?:rious artists)?)|(?:compilation)) - "
         "(?:(\\d{4}) - )?([^/]+)" // year & album
         "(?:\\/(?:cd|dis[ck]) *(\\d+)(?:-? +([^ /][^/]+))?)?" // disk #, name
         "\\/(\\d+)[-. ]+([^/]+) - ([^/]+)$", // track, artist, title
         {{Capture::year, 1},
          {Capture::album, 2},
          {Capture::discNum, 3},
          {Capture::discName, 4},
          {Capture::track, 5},
          {Capture::artist, 6},
          {Capture::title, 7}}),
    // ost - (year - )albumTitle/(disc #- disc name/)## - artist - trackTitle
    make(Shared::VAType::ost,
         "^(?:.*\\/)?(?:(?:ost)|(?:soundtrack)) - "
         "(?:(\\d{4}) - )?([^/]+)" // year & album
         "(?:\\/(?:cd|dis[ck]) *(\\d+)(?:-? +([^ /][^/]+))?)?" // disk #, name
         "\\/(\\d+)[-. ]+([^/]+) - ([^/]+)$", // track, artist, title
         {{Capture::year, 1},
          {Capture::album, 2},
          {Capture::discNum, 3},
          {Capture::discName, 4},
          {Capture::track, 5},
          {Capture::artist, 6},
          {Capture::title, 7}}),
    // artist - year - album/(disc #- disc name/)## - trackTitle
    make(Shared::VAType::none,
         "^(?:.*\\/)?"
         "([^/]+) - (\\d{4}) - ([^/]+)" // artist, year, album
         "(?:\\/(?:cd|dis[ck]) *(\\d+)(-? +([^ /][^/]+))?)?" // disk #, name
         "\\/(\\d+)[-. ]+([^/]+)$", // track, title
         {{Capture::artist, 1},
          {Capture::year, 2},
          {Capture::album, 3},
          {Capture::discNum, 4},
          {Capture::discName, 5},
          {Capture::track, 6},
          {Capture::title, 7}}),
    // va/(year - )albumTitle/(CD # name/)## - artist - trackTitle
    make(Shared::VAType::va,
         "^(?:.*\\/)?((?:va(?:rious artists)?)|(?:compilation))"
         "\\/(?:(\\d{4}) - )?([^/]+)" // year, album
         "(?:\\/(?:cd|dis[ck]) *(\\d+)(?:-? +([^ /][^/]+))?)?" // disk #, name
         "\\/(\\d+)[-. ]+([^/]+) - ([^/]+)$", // track, artist, title
         {{Capture::year, 1},
          {Capture::album, 2},
          {Capture::discNum, 3},
          {Capture::discName, 4},
          {Capture::track, 5},
          {Capture::artist, 6},
          {Capture::title, 7}}),
    // ost/(year - )albumTitle/(CD # name/)## - artist - trackTitle
    make(Shared::VAType::ost,
         "^(?:.*\\/)?(?:(?:ost)|(?:soundtrack))"
         "\\/(?:(\\d{4}) - )?([^/]+)" // year, album
         "(?:\\/(?:cd|dis[ck]) *(\\d+)(?:-? +([^ /][^/]+))?)?" // disk #, name
         "\\/(\\d+)[-. ]+([^/]+) - ([^/]+)$", // track, artist, title
         {{Capture::year, 1},
          {Capture::album, 2},
          {Capture::discNum, 3},
          {Capture::discName, 4},
          {Capture::track, 5},
          {Capture::artist, 6},
          {Capture::title, 7}}),
    // artist/year - albumTitle/CD # name/## - trackTitle
    make(Shared::VAType::none,
         "^(?:.*\\/)?([^/]+)\\/(\\d{4}) - ([^/]+)" // artist, year, album
         "\\/(?:cd|dis[ck]) *(\\d+)(?:-? +([^ /][^/]+))?" // disk #, name
         "\\/(\\d+)[-. ]+([^/]+)$", // track, title
         {{Capture::artist, 1},
          {Capture::year, 2},
          {Capture::album, 3},
          {Capture::discNum, 4},
          {Capture::discName, 5},
          {Capture::track, 6},
          {Capture::title, 7}}),
    // artist/year - albumTitle/## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?([^/]+)\\/(\\d{4}) - ([^/]+)" // artist, year, album
         "\\/(\\d+)[-. ]+([^/]+)$", // track, title
         {{Capture::artist, 1},
          {Capture::year, 2},
          {Capture::album, 3},
          {Capture::track, 4},
          {Capture::title, 5}}),
    // artist - album/CD # name/## - trackTitle
    make(Shared::VAType::none,
         "^(?:.*\\/)?([^/]+) - ([^/]+)" // artist, album
         "\\/(?:cd|dis[ck]) *(\\d+)(?:-? +([^ /][^/]+))?" // disk #, name
         "\\/(\\d+)[-. ]+([^/]+)$", // track, title
         {{Capture::artist, 1},
          {Capture::album, 2},
          {Capture::discNum, 3},
          {Capture::discName, 4},
          {Capture::track, 5},
          {Capture::title, 6}}),
    // artist/albumTitle/CD # name/## - trackTitle
    make(Shared::VAType::none,
         "^(?:.*\\/)?([^/]+)\\/([^/]+)" // artist, album
         "\\/(?:cd|dis[ck]) *(\\d+)(?:-? +([^ /][^/]+))?" // disk #, name
         "\\/(\\d+)[-. ]+([^/]+)$", // track, title
         {{Capture::artist, 1},
          {Capture::album, 2},
          {Capture::discNum, 4},
          {Capture::discName, 6},
          {Capture::track, 7},
          {Capture::title, 8}}),
    // artist - album/## - trackTitle
    make(Shared::VAType::none,
         "^(?:.*\\/)?([^/]+) - ([^/]+)" // artist, album
         "\\/(\\d+)[-. ]+([^/]+)$", // track, title
         {{Capture::artist, 1},
          {Capture::album, 2},
          {Capture::track, 3},
          {Capture::title, 4}}),
    // artist/albumTitle/## - trackTitle
    make(Shared::VAType::none,
         "^(?:.*\\/)?([^/]+)\\/([^/]+)" // artist, album
         "\\/(\\d+)[-. ]+([^/]+)$", // track, title
         {{Capture::artist, 1},
          {Capture::album, 2},
          {Capture::track, 3},
          {Capture::title, 4}})};

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

// Here are the 4 patterns I'm looking for (in priority order):
// va|ost/(year - )albumTitle/(CD #- name/)## - artist - trackTitle
// va|ost - (year - )albumTitle/(CD #- name/)## - artist - trackTitle
// artist/(year - )albumTitle/(CD #- name/)## - trackTitle
// artist - (year - )album/(CD #- name/)## - trackTitle

// First, split the path (and remove the suffix), then check for a CD/Disc
// section Then check for a VA/OST marker, then check for artist/VA subfolder or
// not.

std::optional<Shared::FullMetadata> store::read_path(const fs::path& item) {
  std::string fileName = get_no_suffix(item.filename());
  std::vector<std::string> dirs;
  fs::path dir = fs::canonical(item);
  while (dir.has_parent_path() && dirs.size() < 3) {
    dir = dir.parent_path();
    dirs.push_back(dir.filename().string());
  }
  // I now have the filename, plus no more than 3 containing paths
  if (dirs.empty()) {
    return std::nullopt;
  }
  auto maybe_disc_name = disc_num_and_name(dirs[0]);
  size_t pos = !!maybe_disc_name.has_value();
  // First, look for a "year - album" folder parent:
  auto year_album = year_and_album(dirs[pos]);
  if (year_album[0].empty()) {
    // We didn't get a year, so this could be either:
    // "{artist|ost} - (year - )?album"
    //   OR
    // "{ost|artist}/album"
    // Let's first check the first one:
    auto maybe_artist_year_album = artist_year_album(dirs[pos]);
    if (maybe_artist_year_album.has_value()) {
      auto [artist, year, album] = (*maybe_artist_year_album);
      auto va = get_va_st(artist);
      if (has_va(va)) {
        // This fails if track_artist_title is empty
        return make_metadata(item,
                             va,
                             year,
                             album,
                             track_artist_title(fileName),
                             maybe_disc_name);
      } // else:
      return make_metadata(
          item, artist, year, album, track_title(fileName), maybe_disc_name);
    }
    // last chance: {ost|artist}/album
    if (dirs.size() == pos) {
      // No containing folder: No match
      return std::nullopt;
    }
    if (dirs.size() > pos) {
      auto va_st = get_va_st(dirs[pos + 1]);
      if (has_va(va_st)) {
        return make_metadata(item,
                             va_st,
                             year_album[0],
                             year_album[1],
                             track_artist_title(fileName),
                             maybe_disc_name);
      } // else:
      return make_metadata(item,
                           dirs[pos + 1],
                           year_album[0],
                           year_album[1],
                           track_title(fileName),
                           maybe_disc_name);
    }
    // If we get here, we either don't have a containing folder name,
    // or the file name was malformed: Fail.
    return std::nullopt;
  }
  // At this point, we have a year - album match. Go from there:
  if (dirs.size() == pos) {
    // No containing folder: Fail
    return std::nullopt;
  }
  auto va = get_va_st(dirs[pos + 1]);
  if (has_va(va)) {
    // Look for a track - artist - title match:
    return make_metadata(item,
                         va,
                         year_album[0],
                         year_album[1],
                         track_artist_title(fileName),
                         maybe_disc_name);
  } // else:
  return make_metadata(item,
                       dirs[pos + 1],
                       year_album[0],
                       year_album[1],
                       track_title(fileName),
                       maybe_disc_name);
}

std::optional<std::string> get_capture(
    const std::smatch& match,
    Capture capture,
    const std::map<Capture, int>& captureGroups) {
  auto it = captureGroups.find(capture);
  if (it != captureGroups.end() && match[it->second].matched &&
      match[it->second].length() > 0) {
    return match[it->second].str();
  }
  return std::nullopt;
}

// Get the metadata for a song from the file path only.
std::optional<Shared::FullMetadata> store__read_path(const fs::path& item) {
  std::string noSuffix{get_no_suffix(item)};
  for (const RegexPattern& pattern : patterns) {
    // Match the pattern against the relPath.
    // If it matches, extract the metadata and return it.
    std::smatch match;
    if (!std::regex_search(noSuffix, match, pattern.rgx)) {
      continue;
    }
    Shared::FullMetadata metadata;
    metadata.originalPath = item.generic_string();
    // Extract the metadata based on the pattern.
    auto artist = get_capture(match, Capture::artist, pattern.captureGroups);
    if (artist.has_value()) {
      // TODO: Split artist names up.
      metadata.artist.push_back(*artist);
    }
    auto album = get_capture(match, Capture::album, pattern.captureGroups);
    if (album.has_value()) {
      metadata.album = *album;
    }
    auto year = get_capture(match, Capture::year, pattern.captureGroups);
    if (year.has_value()) {
      metadata.year = text::from_string<std::int16_t>(*year);
    }
    auto track = get_capture(match, Capture::track, pattern.captureGroups);
    if (track.has_value()) {
      metadata.track = text::from_string<std::int16_t>(*track);
    }
    auto title = get_capture(match, Capture::title, pattern.captureGroups);
    if (title.has_value()) {
      // TODO: Split title into additional artists & variations.
      metadata.title = *title;
    }
    auto discNum = get_capture(match, Capture::discNum, pattern.captureGroups);
    metadata.disk =
        discNum.has_value() ? text::from_string<std::int16_t>(*discNum) : 0;
    auto discName =
        get_capture(match, Capture::discName, pattern.captureGroups);
    if (discName.has_value()) {
      metadata.diskName = *discName;
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
    metadata.disk = 0; // NYI: Determine disk number
    metadata.diskName = ""; // NYI: Determine disk name
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
