import {
  arr,
  bool,
  dbl,
  Enum,
  enum_lst,
  enum_num,
  enum_str,
  fmap,
  i16,
  i32,
  map,
  NEnum,
  num,
  obj,
  ObjType,
  opt,
  ref,
  SEnum,
  str,
  sub,
  Types,
  u8,
  u16,
  u64,
} from 'crow-idl/IDL';

const CurrentView: NEnum = enum_num(num(), {
  disabled: -1,
  none: 0,
  settings: 1,
  tools: 2,
  search: 3,
  // Additional entries after this one:
  __last_cuark_view: 100,
  recent: 101,
  albums: 102,
  artists: 103,
  songs: 104,
  playlists: 105,
  now_playing: 106,
});

const StrId: SEnum = enum_str({
  FilePath: 'File Path',
  FilesSelected: 'Files Selected',
  ErrNotSingleAndNotMultiple: 'Not Single and not Multiple (This is a bug!)',
  ErrSingleAndMultiple: 'Both Single and Multiple (This is a bug!)',
  ViewSettings: 'Settings',
  ViewTools: 'Tools',
  // Other strings after this one:
  __last_cuark_StrId: '',
  Mono: 'mono',
  Stereo: 'stereo',
  Quadrophonic: 'quadrophonic',
  Channels: ' channels',
  BitDepth: 'bit',
  Duration: 'Duration:',
  MDHeaderField: 'Field',
  MDHeaderValue: 'Value',
  RawMetadata: 'Raw Metadata',
  ChooseCoverArt: 'Select Cover Art Image',
  ImageName: 'Images',
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
  ImportFiles: 'Import Files...',
});

const Keys: SEnum = enum_str({
  Find: 'F',
  Settings: ',',
  Next: 'Right',
  Prev: 'Left',
  Tools: 'L',
  // Other Keys after this one:
  __last_cuark_Key: '',
  AddFileLocation: 'O',
  Albums: '2',
  Artists: '3',
  Backward10s: '[',
  Forward10s: ']',
  NextTrack: 'Right',
  NowPlaying: '1',
  Play: 'P',
  Playlists: '5',
  PreviousTrack: 'Left',
  Repeat: 'T',
  SavePlaylist: 'S',
  Shuffle: 'R',
  Songs: '4',
  ToggleMiniPlayer: '9',
});

const IpcCall: NEnum = enum_num(u16(), {
  Unknown: 0,
  ReadFromStorage: 1,
  WriteToStorage: 2,
  DeleteFromStorage: 3,
  MinimizeWindow: 4,
  MaximizeWindow: 5,
  RestoreWindow: 6,
  CloseWindow: 7,
  IsDev: 8,
  AsyncData: 9,
  MenuAction: 10,
  ShowOpenDialog: 11,
  GetFileSystemRoots: 12,
  GetNamedLocations: 13,
  GetFolderContents: 14,
  // Other IpcCall's after this one:
  __last_cuark_IpcCall: 1000,
  GetPlaylists: 1001,
  LoadPlaylist: 1002,
  RenamePlaylist: 1003,
  SavePlaylist: 1004,
  DeletePlaylist: 1005,
  SetPlaylists: 1006,
  ClearHates: 1007,
  ClearLikes: 1008,
  ClearLocalOverrides: 1009,
  FlushImageCache: 1010,
  FlushMetadataCache: 1011,
  GetHates: 1012,
  GetLikes: 1013,
  GetMediaInfo: 1014,
  GetMusicDatabase: 1015,
  Search: 1016,
  SetHates: 1017,
  SetLikes: 1018,
  SetMediaInfo: 1019,
  SetSaveMenu: 1020,
  ShowFile: 1021,
  ShowLocFromKey: 1022,
  ShowMenu: 1023,
  SubstrSearch: 1024,
  TranscodingBegin: 1025,
  UploadImage: 1026,
  GetPicUri: 1027,
  GetIgnoreList: 1028,
  AddIgnoreItem: 1029,
  RemoveIgnoreItem: 1030,
  PushIgnoreList: 1031,
  IgnoreListId: 1032,
});

const SocketMsg: Enum = enum_lst(u8(), [
  'Unknown',
  'ContentLoaded',
  'KeepAlive',
  // Other SocketMsg strings after this one:
  '__last_cuark_SocketMsg',
  'TranscodingUpdate',
  'ManualRescan',
  'RescanInProgress',
  'RescanComplete',
  'MusicDBUpdate',
]);

const StorageId: SEnum = enum_str({
  CurrentView: 'currentView',
  SettingValue: 'someSetting',
  // Other StorageID's after this one:
  __last_cuark_StorageId: '',
  Shuffle: 'shuffle',
  Repeat: 'repeat',
  CurrentIndex: 'currentIndex',
  Mute: 'mute',
  Volume: 'volume',
  PlaybackOrder: 'playbackOrder',
  NowPlaying: 'nowPlaying',
  CurrentSongList: 'currentSongList',
  Locations: 'locations',
  DefaultLocation: 'defaultLocation',
  OnlyPlayLikes: 'onlyPlayLikes',
  NeverPlayHates: 'neverPlayHates',
  FullAlbumsOnly: 'fullAlbumsOnly',
  MinSongCount: 'minSongCount',
  LikedSongs: 'likedSongs',
  HatedSongs: 'hatedSongs',
  SortWithArticles: 'sortWithArticles',
  DownloadAlbumArtwork: 'downloadAlbumArtwork',
  DownloadArtistArtwork: 'downloadArtistArtwork',
  SaveAlbumArtworkWithMusic: 'saveAlbumArtworkWithMusic',
  AlbumCoverName: 'albumCoverName',
  TranscodingUpdate: 'getTranscodeUpdate',
  TranscodeSrcLocDir: 'transcodeSrcLocDir',
  TranscodeSrcLocPlaylist: 'transcodeSrcLocPlaylist',
  TranscodeSrcLocArtist: 'transcodeSrcLocArtist',
  TranscodeSrcLocAlbum: 'transcodeSrcLocAlbum',
  TranscodeDestLoc: 'transcodeDestLoc',
  TranscodeBitRate: 'transcodeBitRate',
});

const MimeData = obj({
  type: str(),
  data: str(),
});

const FileFilterItem = obj({ name: str(), extensions: arr(str()) });

const OpenDialogOptions = obj({
  folder: opt(bool()),
  title: opt(str()),
  defaultPath: opt(str()),
  buttonLabel: opt(str()),
  multiSelections: opt(bool()),
  filters: opt(arr(ref('FileFilterItem'))),
});

const NamedLocations = map(str(), str());

const FileSystemItem = obj({
  file: str(),
  date: dbl(),
  size: u64(),
  type: str(),
});

const FolderContents = arr(ref('FileSystemItem'));

// Additional types go *below* this point:

const IgnoreItemType: SEnum = enum_str({
  PathRoot: 'path-root',
  PathKeyword: 'path-keyword',
  DirName: 'dir-name',
});

const IgnoreItemPair: ObjType = obj({
  type: ref('IgnoreItemType'),
  value: str(),
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

const VAType = enum_str({ none: 'None', va: 'VA', ost: 'OST' });

const Song = obj({
  key: ref('SongKey'),
  track: i16(),
  title: str(),
  albumId: ref('AlbumKey'),
  artistIds: arr(ref('ArtistKey')),
  secondaryIds: arr(ref('ArtistKey')),
  variations: arr(str()),
});

const SongWithPath = sub('Song', { path: str() });

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
  songs: map(i16(), ref('SongObj')), // ptr
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
  songs: map(i16(), ref('SongKey')),
  diskNames: arr(str()),
});

const FlatMusicDatabase = obj({
  artists: arr(ref('Artist')),
  albums: arr(ref('Album')),
  songs: arr(ref('Song')),
});

const MusicDatabase = obj({
  artists: fmap(ref('ArtistKey'), ref('Artist')),
  albums: fmap(ref('AlbumKey'), ref('Album')),
  songs: fmap(ref('SongKey'), ref('Song')),
});

const MediaInfo = obj({
  general: map(str(), str()),
  audio: map(str(), str()),
});

// This is the most simplistic strongly typed metadata you'll find
const SimpleMetadata = obj({
  artist: str(),
  album: str(),
  year: i16(),
  track: i16(),
  title: str(),
  discNum: i16(),
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
  // Is this fo a song from a VA album, soundtrack, or single artist
  compilation: ref('VAType'),
  // The regular expression to match, with named groups (ecma syntax)
  rgx: str(),
});

const SearchResults = obj({
  songs: arr(ref('SongKey')),
  artists: arr(ref('ArtistKey')),
  albums: arr(ref('AlbumKey')),
});

// Additional types go *above* this point:

export const TypesToGenerate: Record<string, Types> = {
  Keys,
  StrId,
  CurrentView,
  IpcCall,
  SocketMsg,
  StorageId,
  MimeData,
  FileFilterItem,
  OpenDialogOptions,
  NamedLocations,
  FileSystemItem,
  FolderContents,
  // Other TypesToGenerate below this comment:
  IgnoreItemType,
  IgnoreItemPair,
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
  SongWithPath,
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
  MusicDatabase,
  SearchResults,
};

export const PicklersToGenerate: Record<string, Types> = {};
