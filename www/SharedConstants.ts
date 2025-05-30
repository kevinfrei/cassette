import {
  chk2TupleOf,
  chkObjectOfExactType,
  hasField,
  isBoolean,
  isNonNullable,
  isObjectNonNull,
  isObjectOfExactType,
  isString,
  typecheck,
} from '@freik/typechk';

export type SharedConstants<T> = {
  description: string;
  typechecker: boolean;
  type: [string, string];
  values: Record<string, T>;
};

function isRecordOf<K extends string | number | symbol, V>(
  obj: unknown,
  keyChk: typecheck<K>,
  valChk: typecheck<V>,
): obj is Record<K, V> {
  if (!isObjectNonNull(obj)) {
    return false;
  }
  const keys = Object.keys(obj);
  let len = keys.length;
  for (const fieldName of keys) {
    if (!hasField(obj, fieldName)) continue;
    const theVal = obj[fieldName];
    if (!keyChk(fieldName)) {
      return false;
    }
    if (!valChk(theVal)) {
      return false;
    }
    len--;
  }
  return len === 0;
}

function chkRecordOf<K extends string | number | symbol, V>(
  keyChk: typecheck<K>,
  valChk: typecheck<V>,
): typecheck<Record<K, V>> {
  return (obj): obj is Record<K, V> => isRecordOf(obj, keyChk, valChk);
}

export function isSharedConstantsOf<T>(
  val: unknown,
  typechecker: typecheck<T>,
): val is SharedConstants<T> {
  return isObjectOfExactType(val, {
    description: isString,
    typechecker: isBoolean,
    type: chk2TupleOf(isString, isString),
    values: chkRecordOf(isString, typechecker),
  });
}

export function chkSharedConstantsOf<T>(
  typechecker: typecheck<T>,
): typecheck<SharedConstants<T>> {
  return (val): val is SharedConstants<T> =>
    isSharedConstantsOf(val, typechecker);
}

export const chkSharedConstants: typecheck<SharedConstants<unknown>> =
  chkObjectOfExactType({
    description: isString,
    typechecker: isBoolean,
    type: chk2TupleOf(isString, isString),
    values: chkRecordOf(isString, isNonNullable),
  });

export function isSharedConstants(
  val: unknown,
): val is SharedConstants<unknown> {
  return chkSharedConstants(val);
}

export const CurrentView_source: SharedConstants<number> = {
  description: 'The current view selected by the user in the UI.',
  typechecker: true,
  type: ['int', 'custom-enum'],
  values: {
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
  },
};

export const StrId_source: SharedConstants<string> = {
  description: 'String IDs for localization and UI strings.',
  typechecker: false,
  type: ['string', 'custom-enum'],
  values: {
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
    // Metadata Editor
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
  },
};

export const Keys_source: SharedConstants<string> = {
  description: 'Keyboard shortcuts for various actions in the UI.',
  typechecker: false,
  type: ['string', 'custom-enum'],
  values: {
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
  },
};

export const IpcId_source: SharedConstants<string> = {
  description: 'IPC IDs for communication between the UI and the backend.',
  typechecker: false,
  type: ['string', 'custom-enum'],
  values: {
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
  },
};

export const IgnoreItemType_source: SharedConstants<string> = {
  description: 'Types of items that can be ignored in the music database.',
  typechecker: false,
  type: ['string', 'custom-enum'],
  values: {
    PathRoot: 'path-root',
    PathKeyword: 'path-keyword',
    DirName: 'dir-name',
  },
};

export const AllConstants: Record<string, SharedConstants<any>> = {
  Keys: Keys_source,
  StrId: StrId_source,
  CurrentView: CurrentView_source,
  IpcId: IpcId_source,
  IgnoreItemType: IgnoreItemType_source,
};
