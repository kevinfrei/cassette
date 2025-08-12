import { ShowOpenDialog } from 'www/Tools/Utilities';
import {
  arr,
  bool,
  Enum,
  enum_lst,
  enum_num,
  enum_str,
  fmap,
  i16,
  i32,
  i8,
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
  tup,
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

const IpcCall: NEnum = enum_num(u8(), {
  Unknown: 0,
  ReadFromStorage: 1,
  WriteToStorage: 2,
  DeleteFromStorage: 3,
  AsyncData: 4,
  IsDev: 5,
  ClearHates: 6,
  ClearLikes: 7,
  ClearLocalOverrides: 8,
  DeletePlaylist: 9,
  FlushImageCache: 10,
  FlushMetadataCache: 11,
  GetHates: 12,
  GetLikes: 13,
  GetMediaInfo: 14,
  GetMusicDatabase: 15,
  GetPlaylists: 16,
  LoadPlaylist: 17,
  MenuAction: 18,
  RenamePlaylist: 19,
  SavePlaylist: 20,
  Search: 21,
  SetHates: 22,
  SetLikes: 23,
  SetMediaInfo: 24,
  SetPlaylists: 25,
  SetSaveMenu: 26,
  ShowFile: 27,
  ShowLocFromKey: 28,
  ShowMenu: 29,
  SubstrSearch: 30,
  TranscodingBegin: 31,
  UploadImage: 32,
  MinimizeWindow: 33,
  MaximizeWindow: 34,
  RestoreWindow: 35,
  CloseWindow: 36,
  GetPicUri: 37,
  GetIgnoreList: 38,
  AddIgnoreItem: 39,
  RemoveIgnoreItem: 40,
  PushIgnoreList: 41,
  IgnoreListId: 42,
  ShowOpenDialog: 43,
});

const SocketMsg: Enum = enum_lst(u8(), [
  'Unknown',
  'TranscodingUpdate',
  'ManualRescan',
  'RescanInProgress',
  'RescanComplete',
  'MusicDBUpdate',
  'ContentLoaded',
  'KeepAlive',
]);

const IgnoreItemType: SEnum = enum_str({
  PathRoot: 'path-root',
  PathKeyword: 'path-keyword',
  DirName: 'dir-name',
});

const IgnoreItemPair: ObjType = obj({
  type: ref('IgnoreItemType'),
  value: str(),
});

const StorageId: SEnum = enum_str({
  CurrentView: 'currentView',
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

const FlatMusicDatabase = obj({
  artists: arr(ref('Artist')),
  albums: arr(ref('Album')),
  songs: arr(ref('Song')),
  playlists: map(PlaylistName, ref('Playlist')),
});

const MusicDatabase = obj({
  artists: fmap(ref('ArtistKey'), ref('Artist')),
  albums: fmap(ref('AlbumKey'), ref('Album')),
  songs: fmap(ref('SongKey'), ref('Song')),
  playlists: map(PlaylistName, ref('Playlist')),
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
  // Is this fo a song from a VA album, soundtrack, or single artist
  compilation: ref('VAType'),
  // The regular expression to match, with named groups (ecma syntax)
  rgx: str(),
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

const SearchResults = obj({
  songs: arr(ref('SongKey')),
  artists: arr(ref('ArtistKey')),
  albums: arr(ref('AlbumKey')),
});

export const TypesToGenerate: Record<string, Types> = {
  Keys,
  StrId,
  CurrentView,
  IpcCall,
  SocketMsg,
  IgnoreItemType,
  IgnoreItemPair,
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
  MimeData,
  MusicDatabase,
  FileFilterItem,
  OpenDialogOptions,
  SearchResults,
};

export const PicklersToGenerate: Record<string, Types> = {};
