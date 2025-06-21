import { ReadFromStorage } from 'www/Tools/ipc';
import {
  arr,
  bool,
  enum_lst,
  enum_num,
  enum_str,
  i16,
  i32,
  map,
  NEnum,
  num,
  obj,
  ObjType,
  ref,
  SEnum,
  str,
  Types,
  u16,
  u8,
} from './IDL';

const CurrentView: NEnum = enum_num(num(), {
  disabled: -1,
  none: 0,
  recent: 1,
  albums: 2,
  artists: 3,
  songs: 4,
  playlists: 5,
  now_playing: 6,
  settings: 7,
  search: 8,
  tools: 9,
});

const StrId: SEnum = enum_str({
  Mono: 'mono',
  Stereo: 'stereo',
  Quadrophonic: 'quadrophonic',
  Channels: ' channels',
  BitDepth: 'bit',
  FilePath: 'File Path',
  Duration: 'Duration:',
  MDHeaderField: 'Field',
  MDHeaderValue: 'Value',
  FilesSelected: 'Files Selected',
  RawMetadata: 'Raw Metadata',
  ChooseCoverArt: 'Select Cover Art Image',
  ImageName: 'Images',
  ErrNotSingleAndNotMultiple: 'Not Single and not Multiple (This is a bug!)',
  ErrSingleAndMultiple: 'Both Single and Multiple (This is a bug!)',
  Title: 'Title',
  ArtistTooltip:
    "Multiple artists are specified like this: 'Artist 1, Artist 2 & Artist 3'",
  Artists: 'Artist(s)',
  Album: 'Album',
  Year: 'Year',
  TrackNum: 'Track #',
  DiskNum: 'Disk #',
  DiskName: 'Disk Name',
  Compilation: 'Compilation',
  Soundtrack: 'Soundtrack',
  AdditionalArtists: 'Additional Artist(s)',
  VariationsTooltip: 'Separate vartiations with a semicolon',
  Variations: 'Variation(s)',
  AlbumCover: 'Album Cover',
  ChooseFile: 'Choose File...',
  FromClipboard: 'From Clipboard',
  ViewNowPlaying: 'Now Playing',
  ViewAlbums: 'Albums',
  ViewArtists: 'Artists',
  ViewSongs: 'All Songs',
  ViewPlaylists: 'Playlists',
  ViewSettings: 'Settings',
  ViewTools: 'Tools',
  ImportFiles: 'Import Files...',
});

const Keys: SEnum = enum_str({
  AddFileLocation: 'O',
  Albums: '2',
  Artists: '3',
  Backward10s: '[',
  Find: 'F',
  Forward10s: ']',
  NextTrack: 'Right',
  NowPlaying: '1',
  Play: 'P',
  Playlists: '5',
  PreviousTrack: 'Left',
  Repeat: 'T',
  SavePlaylist: 'S',
  Settings: ',',
  Shuffle: 'R',
  Songs: '4',
  ToggleMiniPlayer: '9',
  Tools: 'L',
});

const IpcId: SEnum = enum_str({
  ReadFromStorage: 'read-from-storage',
  WriteToStorage: 'write-to-storage',
  DeleteFromStorage: 'delete-from-storage',
  AsyncData: 'async-data',
  IsDev: 'is-dev',
  ClearHates: 'clear-hates',
  ClearLikes: 'clear-likes',
  ClearLocalOverrides: 'clear-local-overrides',
  DeletePlaylist: 'delete-playlist',
  FlushImageCache: 'flush-image-cache',
  FlushMetadataCache: 'flush-metadata-cache',
  GetHates: 'get-hates',
  GetLikes: 'get-likes',
  GetMediaInfo: 'media-info',
  GetMusicDatabase: 'get-music-database',
  GetPlaylists: 'get-playlists',
  LoadPlaylist: 'load-playlist',
  ManualRescan: 'manual-rescan',
  MenuAction: 'menuAction',
  MusicDBUpdate: 'music-database-update',
  RenamePlaylist: 'rename-playlist',
  SavePlaylist: 'save-playlist',
  Search: 'search',
  SetHates: 'set-hates',
  SetLikes: 'set-likes',
  SetMediaInfo: 'set-media-info',
  SetPlaylists: 'set-playlists',
  SetSaveMenu: 'set-save-menu',
  ShowFile: 'show-file',
  ShowLocFromKey: 'show-location-from-key',
  ShowMenu: 'show-menu',
  SubstrSearch: 'subsearch',
  TranscodingUpdate: 'get-xcode-update',
  TranscodingBegin: 'start-xcode',
  UploadImage: 'upload-image',
  MinimizeWindow: 'minimize-window',
  MaximizeWindow: 'maximize-window',
  RestoreWindow: 'restore-window',
  CloseWindow: 'close-window',
  GetPicUri: 'get-pic-uri',
  GetIgnoreList: 'get-ignore-list',
  AddIgnoreItem: 'add-ignore-item',
  RemoveIgnoreItem: 'del-ignore-item',
  PushIgnoreList: 'push-ignore-list',
  IgnoreListId: 'ignore-list',
  RescanInProgress: 'rescan-in-progress',
});

const IgnoreItemType: SEnum = enum_str({
  PathRoot: 'path-root',
  PathKeyword: 'path-keyword',
  DirName: 'dir-name',
});

const StorageId: SEnum = enum_str({
  CurrentView: 'CurrentView',
  CurrentIndex: 'currentIndex',
  PlaybackOrder: 'playbackOrder',
  NowPlaying: 'nowPlaying',
  CurrentSongList: 'currentSongList',
  Shuffle: 'shuffle',
  Repeat: 'repeat',
  NeverPlayHates: 'neverPlayHates',
  OnlyPlayLikes: 'onlyPlayLikes',
  FullAlbumsOnly: 'FullAlbumsOnly',
  MinSongCount: 'MinSongCount',
  Mute: 'mute',
  LikedSongs: 'likedSongs',
  HatedSongs: 'hatedSongs',
  Volume: 'volume',
  Locations: 'locations',
  DefaultLocation: 'defaultLocation',
  SortWithArticles: 'rSortWithArticles',
  DownloadAlbumArtwork: 'downloadAlbumArtwork',
  DownloadArtistArtwork: 'downloadArtistArtwork',
  SaveAlbumArtworkWithMusic: 'saveAlbumArtworkWithMusic',
  AlbumCoverName: 'albumCoverName',
  TranscodingUpdate: 'get-xcode-update',
  TranscodeSrcLocDir: 'xcodeSrcLocDir',
  TranscodeSrcLocPlaylist: 'xcodeSrcLocPlaylist',
  TranscodeSrcLocArtist: 'xcodeSrcLocArtist',
  TranscodeSrcLocAlbum: 'xcodeSrcLocAlbum',
  TranscodeDestLoc: 'xcodeDestLoc',
  TranscodeBitRate: 'xcodeBitRate',
});

const TranscodeFormatTargetName: SEnum = enum_str({
  m4a: 'm4a',
  mp3: 'mp3',
  aac: 'aac',
});

const TranscodeSource: SEnum = enum_str({
  Playlist: 'p',
  Artist: 'r',
  Album: 'l',
  Disk: 'd',
});

const TranscodeSourceLocation: ObjType = obj({
  type: ref('TranscodeSource'),
  loc: str(),
});

const FileFailure: ObjType = obj({
  file: str(),
  error: str(),
});

const TranscodeState: ObjType = obj({
  curStatus: str(),
  filesTranscoded: arr(str()),
  filesFound: i32(),
  filesPending: i32(),
  filesUntouched: i32(),
  filesFailed: arr(ref('FileFailure')),
  itemsRemoved: arr(str()),
});

const TranscodeInfo: ObjType = obj({
  source: ref('TranscodeSourceLocation'),
  dest: str(),
  artwork: bool(),
  mirror: bool(),
  format: ref('TranscodeFormatTargetName'),
  bitrate: u16(),
});

const SongKey = str();
const AlbumKey = str();
const ArtistKey = str();
const MediaKey = str();
const PlaylistName = str();
const Playlist = arr(ref('SongKey'));

const VAType = enum_lst(u8(), ['None', 'VA', 'OST']);

const Song = obj({
  key: ref('SongKey'),
  track: i16(),
  title: str(),
  albumId: ref('AlbumKey'),
  artistIds: arr(ref('ArtistKey')),
  secondaryIds: arr(ref('ArtistKey')),
  variations: arr(str()),
});

const SongObj = obj({
  track: i16(),
  title: str(),
  album: ref('AlbumObj'), // ptr
  artists: arr(ref('ArtistObj')), // ptr
  secondaryArtists: arr(ref('ArtistObj')), // ptr
  variations: arr(str()),
});

const ArtistObj = obj({
  name: str(),
  albums: arr(ref('AlbumObj')), // ptr
  songs: arr(ref('SongObj')), // ptr
});

const AlbumObj = obj({
  title: str(),
  year: i16(),
  vatype: ref('VAType'),
  primaryArtists: arr(ref('ArtistObj')), // ptr
  songs: arr(ref('SongObj')), // ptr
  diskNames: arr(str()),
});

const Artist = obj({
  key: ref('ArtistKey'),
  name: str(),
  albums: arr(ref('AlbumKey')),
  songs: arr(ref('SongKey')),
});

const Album = obj({
  key: ref('AlbumKey'),
  year: i16(),
  title: str(),
  vatype: ref('VAType'),
  primaryArtists: arr(ref('ArtistKey')),
  songs: arr(ref('SongKey')),
  diskNames: arr(str()),
});

const MediaInfo = obj({
  general: map(str(), str()),
  audio: map(str(), str()),
});

// This is the most simplistic strongly typed metadata you'll find
const SimpleMetadata = obj({
  artist: str(),
  album: str(),
  year: str(),
  track: str(),
  title: str(),
  discNum: str(),
  discName: str(),
  compilation: ref('VAType'),
});

// This is a more robust metadata type, meant to be used in,
// among other scenarios, situations where you're moving files around
const FullMetadata = obj({
  originalPath: str(),
  artist: arr(str()),
  album: str(),
  year: i16(),
  track: i16(),
  title: str(),
  vaType: ref('VAType'),
  moreArtists: arr(str()),
  variations: arr(str()),
  disk: i16(),
  diskName: str(),
});

// This is a general mechanism for describing how to extract
// various metadata components out of a file path
const AudioFileRegexPattern = obj({
  // This can be something like "soundtrack"
  // or "true/false" to simply indicate that it's
  // a compilation of works by various artists
  compilation: ref('VAType'),
  // This is the regular expression to match
  rgx: str(),
});

const MimeData = obj({
  type: str(),
  data: str(),
});

export const TypesToGenerate: Record<string, Types> = {
  Keys,
  StrId,
  CurrentView,
  IpcId,
  IgnoreItemType,
  StorageId,
  TranscodeFormatTargetName,
  TranscodeSource,
  TranscodeSourceLocation,
  FileFailure,
  TranscodeState,
  TranscodeInfo,
  SongKey,
  AlbumKey,
  ArtistKey,
  MediaKey,
  PlaylistName,
  Playlist,
  Song,
  VAType,
  /* 
  SongObj,
  ArtistObj,
  AlbumObj,
  */
  Artist,
  Album,
  MediaInfo,
  SimpleMetadata,
  FullMetadata,
  AudioFileRegexPattern,
  MimeData,
};

export const PicklersToGenerate: Record<string, Types> = {};
