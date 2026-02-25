# C++ code for the music player

Here's a class diagram for general reference of what I think I'm building:

```mermaid
classDiagram
note for file_index "Responsible for tracking file
addition/changes/deletion"
class file_index {
  path loc
  optional~path~ index_file_path
  unordered_map~string, FileKey~ file_to_key
  unordered_map~FileKey, string~ key_to_file
  time_point~system_clock~ last_scan
  set~[IgnoreItem, string]~ ignore_items

  +(path loc, bool update_index = false) : file_index

  bool -belongs_here(path)
  bool -add_new_file(path)
  bool -remove_file(path)
  string -get_relative_path(path)
  FileKey -make_file_key(relPath)
  bool -read_index_file()
  void -write_index_file()

  path get_location()
  time_point~system_clock~ get_last_scan_time()
  optional~key~ get_file_key(path)
  void foreach_file(path_handler)
  void rescan_files(add_file_handler, del_file_handler)
  void add_ignore_item(IgnoreItemType, value)
  void remove_ignore_item(IgnoreItemType, value)
  set~[IgnoreItemType, value]~ get_ignore_items()
}
class metadata__store {
  unordered_map~string, FullMetadata~ content_cache
  unordered_map~string, FullMetadata~ specific_overrides
  path cache_file
  path override_file

  +(path dir) : metadata__store
  optional~FullMetadata~ read(path item)
  optional~FullMetadata~ read_override(path item)
  std::optional<Shared::FullMetadata> read_content(
      const std::filesystem::path& item);
  std::optional<Shared::FullMetadata> read_path(
      const std::filesystem::path& item);
  void write_partial(const std::filesystem::path& item,
                     const Shared::SimpleMetadata& newMetadata);
  void write_partial(const std::filesystem::path& item,
                     const Shared::FullMetadata& metadata);
  void write_full(const std::filesystem::path& item,
                  const Shared::SimpleMetadata& newMetadata);
  void write_full(const std::filesystem::path& item,
                  const Shared::FullMetadata& metadata);
  void clear_metadata_cache(const std::filesystem::path& item);
  void clear_metadata_cache();
  void reset_all_metadata(const std::filesystem::path& item);
  void reset_all_metadata();
  void clear_metadata_override();
  void clear_metadata_override(const std::filesystem::path& item);
  void clear();
  void clear(const std::filesystem::path& item);

  void set_image(const std::filesystem::path& item,
                 const std::vector<std::uint8_t>& buf);
  bool clear_image(const std::filesystem::path& item);
  optional~vector~uint8_t~~ get_image(path item, bool preferInternal = false);
  void clear_local_image_cache()
}
note for metadata__store "Manages caching of file metadata
and storage of overrides and images"
```
