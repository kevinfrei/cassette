import {
  chk2TupleOf,
  hasFieldType,
  is2TupleOf,
  isBoolean,
  isObjectNonNull,
  isString,
  typecheck,
} from '@freik/typechk';

export type SharedConstants<T> = {
  description: string;
  typechecker: boolean;
  type: [string, string];
  values: Record<string, T>;
};

export function isSharedConstants<T>(
  val: unknown,
  typechecker: typecheck<T>,
): val is SharedConstants<T> {
  return (
    hasFieldType(val, 'description', isString) &&
    hasFieldType(val, 'typechecker', isBoolean) &&
    hasFieldType(val, 'type', chk2TupleOf(isString, isString)) &&
    hasFieldType(val, 'values', isObjectNonNull)
  );
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
