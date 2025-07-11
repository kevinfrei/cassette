import { isAlbumKey, isArtistKey } from '@freik/media-core';
import { isNumber, isString } from '@freik/typechk';
import { atom } from 'jotai';
import { atomFamily } from 'jotai/utils';
import {
  AlbumKey,
  ArtistKey,
  chkTranscodeState,
  Playlist,
  PlaylistName,
  StorageId,
  TranscodeSource,
  TranscodeState,
} from 'www/Shared/CommonTypes';
import { atomWithMainStorage } from './Storage';

const emptyXcodeInfo: TranscodeState = {
  curStatus: '',
  filesFound: 0,
  filesPending: 0,
  filesUntouched: 0,
  filesTranscoded: [],
  filesFailed: [],
  itemsRemoved: [],
};

export type TranscodeSourceLocation = {
  type: TranscodeSource;
  loc: Playlist | ArtistKey | AlbumKey | string;
};

// const xcodeErr = (curStatus: string) => ({ ...emptyXcodeInfo, curStatus });

// I think this one may be wrong
export const transcodeStatusState = atomWithMainStorage<TranscodeState>(
  StorageId.TranscodingUpdate,
  emptyXcodeInfo,
  chkTranscodeState,
);

export const sourceLocationTypeState = atom<TranscodeSource>(
  TranscodeSource.Playlist,
);

export const sourceLocationDirState = atomWithMainStorage(
  StorageId.TranscodeSrcLocDir,
  '',
  isString,
);

export const sourceLocationPlaylistState = atomWithMainStorage(
  StorageId.TranscodeSrcLocPlaylist,
  '',
  isString,
);

export const sourceLocationArtistState = atomWithMainStorage(
  StorageId.TranscodeSrcLocArtist,
  '',
  isArtistKey,
);

export const sourceLocationAlbumState = atomWithMainStorage(
  StorageId.TranscodeSrcLocAlbum,
  '',
  isAlbumKey,
);

// JODO: Update these once we have good playlist state in Jotai
const validPlaylistFunc = atomFamily((name: string) =>
  atom(() => name.length > 0),
);
const validArtistFunc = atomFamily((rk: ArtistKey) =>
  atom(() => rk.length > 0),
);
const validAlbumFunc = atomFamily((ak: AlbumKey) => atom(() => ak.length > 0));

export const validSourceState = atom(async (get) => {
  const type = get(sourceLocationTypeState);
  switch (type) {
    case TranscodeSource.Playlist:
      return get(validPlaylistFunc(await get(sourceLocationPlaylistState)));
    case TranscodeSource.Artist:
      return get(validArtistFunc(await get(sourceLocationArtistState)));
    case TranscodeSource.Album:
      return get(validAlbumFunc(await get(sourceLocationAlbumState)));
  }
  return false;
});

export const sourceLocationState = atom(async (get) => {
  let loc: PlaylistName | ArtistKey | AlbumKey | string = '';
  const type = await get(sourceLocationTypeState);
  switch (type) {
    case TranscodeSource.Playlist:
      loc = await get(sourceLocationPlaylistState);
      break;
    case TranscodeSource.Artist:
      loc = await get(sourceLocationArtistState);
      break;
    case TranscodeSource.Album:
      loc = await get(sourceLocationAlbumState);
      break;
    case TranscodeSource.Disk:
      loc = await get(sourceLocationDirState);
      break;
    default:
      throw new Error('Invalid Transcode Source Type');
  }
  return { type, loc };
});

export const destLocationState = atomWithMainStorage(
  StorageId.TranscodeDestLoc,
  '',
  isString,
);

export const xcodeBitRateState = atomWithMainStorage(
  StorageId.TranscodeBitRate,
  128,
  isNumber,
);
