// Generated from www/Shared/Enums.ts by scripts/gencpp.ts

#ifndef SHARED_CONSTANTS_HPP
#define SHARED_CONSTANTS_HPP

#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace Shared {
template <typename T>
constexpr std::optional<T> from_string(const std::string_view& str);

// Keys
enum class Keys {
  AddFileLocation,
  Albums,
  Artists,
  Backward10s,
  Find,
  Forward10s,
  NextTrack,
  NowPlaying,
  Play,
  Playlists,
  PreviousTrack,
  Repeat,
  SavePlaylist,
  Settings,
  Shuffle,
  Songs,
  ToggleMiniPlayer,
  Tools
};

inline constexpr std::string_view to_string(Keys _value) {
  switch (_value) {
    case Keys::AddFileLocation:
      return "O";
    case Keys::Albums:
      return "2";
    case Keys::Artists:
      return "3";
    case Keys::Backward10s:
      return "[";
    case Keys::Find:
      return "F";
    case Keys::Forward10s:
      return "]";
    case Keys::NextTrack:
      return "Right";
    case Keys::NowPlaying:
      return "1";
    case Keys::Play:
      return "P";
    case Keys::Playlists:
      return "5";
    case Keys::PreviousTrack:
      return "Left";
    case Keys::Repeat:
      return "T";
    case Keys::SavePlaylist:
      return "S";
    case Keys::Settings:
      return ",";
    case Keys::Shuffle:
      return "R";
    case Keys::Songs:
      return "4";
    case Keys::ToggleMiniPlayer:
      return "9";
    case Keys::Tools:
      return "L";
    default:
      return "<unknown>";
  }
}

template <>
inline constexpr std::optional<Keys> from_string<Keys>(
    const std::string_view& str) {
  if (str == "O")
    return Keys::AddFileLocation;
  if (str == "2")
    return Keys::Albums;
  if (str == "3")
    return Keys::Artists;
  if (str == "[")
    return Keys::Backward10s;
  if (str == "F")
    return Keys::Find;
  if (str == "]")
    return Keys::Forward10s;
  if (str == "Right")
    return Keys::NextTrack;
  if (str == "1")
    return Keys::NowPlaying;
  if (str == "P")
    return Keys::Play;
  if (str == "5")
    return Keys::Playlists;
  if (str == "Left")
    return Keys::PreviousTrack;
  if (str == "T")
    return Keys::Repeat;
  if (str == "S")
    return Keys::SavePlaylist;
  if (str == ",")
    return Keys::Settings;
  if (str == "R")
    return Keys::Shuffle;
  if (str == "4")
    return Keys::Songs;
  if (str == "9")
    return Keys::ToggleMiniPlayer;
  if (str == "L")
    return Keys::Tools;
  return std::nullopt;
}

// StrId
enum class StrId {
  Mono,
  Stereo,
  Quadrophonic,
  Channels,
  BitDepth,
  FilePath,
  Duration,
  MDHeaderField,
  MDHeaderValue,
  FilesSelected,
  RawMetadata,
  ChooseCoverArt,
  ImageName,
  ErrNotSingleAndNotMultiple,
  ErrSingleAndMultiple,
  Title,
  ArtistTooltip,
  Artists,
  Album,
  Year,
  TrackNum,
  DiskNum,
  DiskName,
  Compilation,
  Soundtrack,
  AdditionalArtists,
  VariationsTooltip,
  Variations,
  AlbumCover,
  ChooseFile,
  FromClipboard,
  ViewNowPlaying,
  ViewAlbums,
  ViewArtists,
  ViewSongs,
  ViewPlaylists,
  ViewSettings,
  ViewTools,
  ImportFiles
};

inline constexpr std::string_view to_string(StrId _value) {
  switch (_value) {
    case StrId::Mono:
      return "mono";
    case StrId::Stereo:
      return "stereo";
    case StrId::Quadrophonic:
      return "quadrophonic";
    case StrId::Channels:
      return " channels";
    case StrId::BitDepth:
      return "bit";
    case StrId::FilePath:
      return "File Path";
    case StrId::Duration:
      return "Duration:";
    case StrId::MDHeaderField:
      return "Field";
    case StrId::MDHeaderValue:
      return "Value";
    case StrId::FilesSelected:
      return "Files Selected";
    case StrId::RawMetadata:
      return "Raw Metadata";
    case StrId::ChooseCoverArt:
      return "Select Cover Art Image";
    case StrId::ImageName:
      return "Images";
    case StrId::ErrNotSingleAndNotMultiple:
      return "Not Single and not Multiple (This is a bug!)";
    case StrId::ErrSingleAndMultiple:
      return "Both Single and Multiple (This is a bug!)";
    case StrId::Title:
      return "Title";
    case StrId::ArtistTooltip:
      return "Multiple artists are specified like this: 'Artist 1, Artist 2 & "
             "Artist 3'";
    case StrId::Artists:
      return "Artist(s)";
    case StrId::Album:
      return "Album";
    case StrId::Year:
      return "Year";
    case StrId::TrackNum:
      return "Track #";
    case StrId::DiskNum:
      return "Disk #";
    case StrId::DiskName:
      return "Disk Name";
    case StrId::Compilation:
      return "Compilation";
    case StrId::Soundtrack:
      return "Soundtrack";
    case StrId::AdditionalArtists:
      return "Additional Artist(s)";
    case StrId::VariationsTooltip:
      return "Separate vartiations with a semicolon";
    case StrId::Variations:
      return "Variation(s)";
    case StrId::AlbumCover:
      return "Album Cover";
    case StrId::ChooseFile:
      return "Choose File...";
    case StrId::FromClipboard:
      return "From Clipboard";
    case StrId::ViewNowPlaying:
      return "Now Playing";
    case StrId::ViewAlbums:
      return "Albums";
    case StrId::ViewArtists:
      return "Artists";
    case StrId::ViewSongs:
      return "All Songs";
    case StrId::ViewPlaylists:
      return "Playlists";
    case StrId::ViewSettings:
      return "Settings";
    case StrId::ViewTools:
      return "Tools";
    case StrId::ImportFiles:
      return "Import Files...";
    default:
      return "<unknown>";
  }
}

template <>
inline constexpr std::optional<StrId> from_string<StrId>(
    const std::string_view& str) {
  if (str == "mono")
    return StrId::Mono;
  if (str == "stereo")
    return StrId::Stereo;
  if (str == "quadrophonic")
    return StrId::Quadrophonic;
  if (str == " channels")
    return StrId::Channels;
  if (str == "bit")
    return StrId::BitDepth;
  if (str == "File Path")
    return StrId::FilePath;
  if (str == "Duration:")
    return StrId::Duration;
  if (str == "Field")
    return StrId::MDHeaderField;
  if (str == "Value")
    return StrId::MDHeaderValue;
  if (str == "Files Selected")
    return StrId::FilesSelected;
  if (str == "Raw Metadata")
    return StrId::RawMetadata;
  if (str == "Select Cover Art Image")
    return StrId::ChooseCoverArt;
  if (str == "Images")
    return StrId::ImageName;
  if (str == "Not Single and not Multiple (This is a bug!)")
    return StrId::ErrNotSingleAndNotMultiple;
  if (str == "Both Single and Multiple (This is a bug!)")
    return StrId::ErrSingleAndMultiple;
  if (str == "Title")
    return StrId::Title;
  if (str ==
      "Multiple artists are specified like this: 'Artist 1, Artist 2 & Artist "
      "3'")
    return StrId::ArtistTooltip;
  if (str == "Artist(s)")
    return StrId::Artists;
  if (str == "Album")
    return StrId::Album;
  if (str == "Year")
    return StrId::Year;
  if (str == "Track #")
    return StrId::TrackNum;
  if (str == "Disk #")
    return StrId::DiskNum;
  if (str == "Disk Name")
    return StrId::DiskName;
  if (str == "Compilation")
    return StrId::Compilation;
  if (str == "Soundtrack")
    return StrId::Soundtrack;
  if (str == "Additional Artist(s)")
    return StrId::AdditionalArtists;
  if (str == "Separate vartiations with a semicolon")
    return StrId::VariationsTooltip;
  if (str == "Variation(s)")
    return StrId::Variations;
  if (str == "Album Cover")
    return StrId::AlbumCover;
  if (str == "Choose File...")
    return StrId::ChooseFile;
  if (str == "From Clipboard")
    return StrId::FromClipboard;
  if (str == "Now Playing")
    return StrId::ViewNowPlaying;
  if (str == "Albums")
    return StrId::ViewAlbums;
  if (str == "Artists")
    return StrId::ViewArtists;
  if (str == "All Songs")
    return StrId::ViewSongs;
  if (str == "Playlists")
    return StrId::ViewPlaylists;
  if (str == "Settings")
    return StrId::ViewSettings;
  if (str == "Tools")
    return StrId::ViewTools;
  if (str == "Import Files...")
    return StrId::ImportFiles;
  return std::nullopt;
}

enum class CurrentView {
  disabled = -1,
  none = 0,
  recent = 1,
  albums = 2,
  artists = 3,
  songs = 4,
  playlists = 5,
  now_playing = 6,
  settings = 7,
  search = 8,
  tools = 9,
};
// IpcId
enum class IpcId {
  ClearHates,
  ClearLikes,
  ClearLocalOverrides,
  DeletePlaylist,
  FlushImageCache,
  FlushMetadataCache,
  GetHates,
  GetLikes,
  GetMediaInfo,
  GetMusicDatabase,
  GetPlaylists,
  LoadPlaylist,
  ManualRescan,
  MenuAction,
  MusicDBUpdate,
  RenamePlaylist,
  SavePlaylist,
  Search,
  SetHates,
  SetLikes,
  SetMediaInfo,
  SetPlaylists,
  SetSaveMenu,
  ShowFile,
  ShowLocFromKey,
  ShowMenu,
  SubstrSearch,
  TranscodingUpdate,
  TranscodingBegin,
  UploadImage,
  MinimizeWindow,
  MaximizeWindow,
  RestoreWindow,
  CloseWindow,
  GetPicUri,
  GetIgnoreList,
  AddIgnoreItem,
  RemoveIgnoreItem,
  PushIgnoreList,
  IgnoreListId,
  RescanInProgress
};

inline constexpr std::string_view to_string(IpcId _value) {
  switch (_value) {
    case IpcId::ClearHates:
      return "clear-hates";
    case IpcId::ClearLikes:
      return "clear-likes";
    case IpcId::ClearLocalOverrides:
      return "clear-local-overrides";
    case IpcId::DeletePlaylist:
      return "delete-playlist";
    case IpcId::FlushImageCache:
      return "flush-image-cache";
    case IpcId::FlushMetadataCache:
      return "flush-metadata-cache";
    case IpcId::GetHates:
      return "get-hates";
    case IpcId::GetLikes:
      return "get-likes";
    case IpcId::GetMediaInfo:
      return "media-info";
    case IpcId::GetMusicDatabase:
      return "get-music-database";
    case IpcId::GetPlaylists:
      return "get-playlists";
    case IpcId::LoadPlaylist:
      return "load-playlist";
    case IpcId::ManualRescan:
      return "manual-rescan";
    case IpcId::MenuAction:
      return "menuAction";
    case IpcId::MusicDBUpdate:
      return "music-database-update";
    case IpcId::RenamePlaylist:
      return "rename-playlist";
    case IpcId::SavePlaylist:
      return "save-playlist";
    case IpcId::Search:
      return "search";
    case IpcId::SetHates:
      return "set-hates";
    case IpcId::SetLikes:
      return "set-likes";
    case IpcId::SetMediaInfo:
      return "set-media-info";
    case IpcId::SetPlaylists:
      return "set-playlists";
    case IpcId::SetSaveMenu:
      return "set-save-menu";
    case IpcId::ShowFile:
      return "show-file";
    case IpcId::ShowLocFromKey:
      return "show-location-from-key";
    case IpcId::ShowMenu:
      return "show-menu";
    case IpcId::SubstrSearch:
      return "subsearch";
    case IpcId::TranscodingUpdate:
      return "get-xcode-update";
    case IpcId::TranscodingBegin:
      return "start-xcode";
    case IpcId::UploadImage:
      return "upload-image";
    case IpcId::MinimizeWindow:
      return "minimize-window";
    case IpcId::MaximizeWindow:
      return "maximize-window";
    case IpcId::RestoreWindow:
      return "restore-window";
    case IpcId::CloseWindow:
      return "close-window";
    case IpcId::GetPicUri:
      return "get-pic-uri";
    case IpcId::GetIgnoreList:
      return "get-ignore-list";
    case IpcId::AddIgnoreItem:
      return "add-ignore-item";
    case IpcId::RemoveIgnoreItem:
      return "del-ignore-item";
    case IpcId::PushIgnoreList:
      return "push-ignore-list";
    case IpcId::IgnoreListId:
      return "ignore-list";
    case IpcId::RescanInProgress:
      return "rescan-in-progress";
    default:
      return "<unknown>";
  }
}

template <>
inline constexpr std::optional<IpcId> from_string<IpcId>(
    const std::string_view& str) {
  if (str == "clear-hates")
    return IpcId::ClearHates;
  if (str == "clear-likes")
    return IpcId::ClearLikes;
  if (str == "clear-local-overrides")
    return IpcId::ClearLocalOverrides;
  if (str == "delete-playlist")
    return IpcId::DeletePlaylist;
  if (str == "flush-image-cache")
    return IpcId::FlushImageCache;
  if (str == "flush-metadata-cache")
    return IpcId::FlushMetadataCache;
  if (str == "get-hates")
    return IpcId::GetHates;
  if (str == "get-likes")
    return IpcId::GetLikes;
  if (str == "media-info")
    return IpcId::GetMediaInfo;
  if (str == "get-music-database")
    return IpcId::GetMusicDatabase;
  if (str == "get-playlists")
    return IpcId::GetPlaylists;
  if (str == "load-playlist")
    return IpcId::LoadPlaylist;
  if (str == "manual-rescan")
    return IpcId::ManualRescan;
  if (str == "menuAction")
    return IpcId::MenuAction;
  if (str == "music-database-update")
    return IpcId::MusicDBUpdate;
  if (str == "rename-playlist")
    return IpcId::RenamePlaylist;
  if (str == "save-playlist")
    return IpcId::SavePlaylist;
  if (str == "search")
    return IpcId::Search;
  if (str == "set-hates")
    return IpcId::SetHates;
  if (str == "set-likes")
    return IpcId::SetLikes;
  if (str == "set-media-info")
    return IpcId::SetMediaInfo;
  if (str == "set-playlists")
    return IpcId::SetPlaylists;
  if (str == "set-save-menu")
    return IpcId::SetSaveMenu;
  if (str == "show-file")
    return IpcId::ShowFile;
  if (str == "show-location-from-key")
    return IpcId::ShowLocFromKey;
  if (str == "show-menu")
    return IpcId::ShowMenu;
  if (str == "subsearch")
    return IpcId::SubstrSearch;
  if (str == "get-xcode-update")
    return IpcId::TranscodingUpdate;
  if (str == "start-xcode")
    return IpcId::TranscodingBegin;
  if (str == "upload-image")
    return IpcId::UploadImage;
  if (str == "minimize-window")
    return IpcId::MinimizeWindow;
  if (str == "maximize-window")
    return IpcId::MaximizeWindow;
  if (str == "restore-window")
    return IpcId::RestoreWindow;
  if (str == "close-window")
    return IpcId::CloseWindow;
  if (str == "get-pic-uri")
    return IpcId::GetPicUri;
  if (str == "get-ignore-list")
    return IpcId::GetIgnoreList;
  if (str == "add-ignore-item")
    return IpcId::AddIgnoreItem;
  if (str == "del-ignore-item")
    return IpcId::RemoveIgnoreItem;
  if (str == "push-ignore-list")
    return IpcId::PushIgnoreList;
  if (str == "ignore-list")
    return IpcId::IgnoreListId;
  if (str == "rescan-in-progress")
    return IpcId::RescanInProgress;
  return std::nullopt;
}

// IgnoreItemType
enum class IgnoreItemType { PathRoot, PathKeyword, DirName };

inline constexpr std::string_view to_string(IgnoreItemType _value) {
  switch (_value) {
    case IgnoreItemType::PathRoot:
      return "path-root";
    case IgnoreItemType::PathKeyword:
      return "path-keyword";
    case IgnoreItemType::DirName:
      return "dir-name";
    default:
      return "<unknown>";
  }
}

template <>
inline constexpr std::optional<IgnoreItemType> from_string<IgnoreItemType>(
    const std::string_view& str) {
  if (str == "path-root")
    return IgnoreItemType::PathRoot;
  if (str == "path-keyword")
    return IgnoreItemType::PathKeyword;
  if (str == "dir-name")
    return IgnoreItemType::DirName;
  return std::nullopt;
}

} // namespace Shared

#endif // SHARED_CONSTANTS_HPP
